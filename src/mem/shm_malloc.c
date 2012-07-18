/*
 * shm_malloc.c
 *
 *  Created on: 2011-4-6
 *      Author: qiyingwang
 */
#include "mem/shm_malloc.h"
#include <string.h>
#include <stdlib.h>

#define MAX_POOL_FRAGS 10000 /* max fragments per pool hash bucket */
#define MIN_POOL_FRAGS 10    /* min fragments per pool hash bucket */

/*useful macros*/

#define FRAG_NEXT(f) \
	((struct shm_frag*)((char*)(f)+sizeof(struct shm_frag)+(f)->size ))

/* SF_ROUNDTO= 2^k so the following works */
#define ROUNDTO_MASK	(~((unsigned long)SF_ROUNDTO-1))
#define ROUNDUP(s)		(((s)+(SF_ROUNDTO-1))&ROUNDTO_MASK)
#define ROUNDDOWN(s)	((s)&ROUNDTO_MASK)

#define FRAG_OVERHEAD	(sizeof(struct shm_frag))
#define INIT_OVERHEAD	\
	(ROUNDUP(sizeof(struct shm_block))+sizeof(struct shm_frag))

/* finds hash if s <=SF_MALLOC_OPTIMIZE */
#define GET_SMALL_HASH(s) (unsigned long)(s)/SF_ROUNDTO
/* finds hash if s > SF_MALLOC_OPTIMIZE */
#define GET_BIG_HASH(s) \
	(SF_MALLOC_OPTIMIZE/SF_ROUNDTO+big_hash_idx((s))-SF_MALLOC_OPTIMIZE_FACTOR+1)

/* finds the hash value for s, s=SF_ROUNDTO multiple*/
#define GET_HASH(s)   ( ((unsigned long)(s)<=SF_MALLOC_OPTIMIZE)?\
							GET_SMALL_HASH(s): GET_BIG_HASH(s) )

#define UN_HASH_SMALL(h) ((unsigned long)(h)*SF_ROUNDTO)
#define UN_HASH_BIG(h) (1UL<<((unsigned long)(h)-SF_MALLOC_OPTIMIZE/SF_ROUNDTO+\
							SF_MALLOC_OPTIMIZE_FACTOR-1))

#define UN_HASH(h)	( ((unsigned long)(h)<=(SF_MALLOC_OPTIMIZE/SF_ROUNDTO))?\
						UN_HASH_SMALL(h): UN_HASH_BIG(h) )

#define BITMAP_BITS (sizeof(((struct shm_block*)0)->bitmap)*8)
#define BITMAP_BLOCK_SIZE ((SF_MALLOC_OPTIMIZE/SF_ROUNDTO)/ BITMAP_BITS)
/* only for "small" hashes (up to HASH(SF_MALLOC_OPTIMIZE) */
#define HASH_BIT_POS(h) (((unsigned long)(h))/BITMAP_BLOCK_SIZE)
#define HASH_TO_BITMAP(h) (1UL<<HASH_BIT_POS(h))
#define BIT_TO_HASH(b) ((b)*BITMAP_BLOCK_SIZE)

/* mark/test used/unused frags */
#define FRAG_MARK_USED(f)
#define FRAG_CLEAR_USED(f)
#define FRAG_WAS_USED(f)   (1)

/* other frag related defines:
 * MEM_COALESCE_FRAGS
 * MEM_FRAG_AVOIDANCE
 */
#define MEM_FRAG_AVOIDANCE

#define SFM_REALLOC_REMALLOC

/* computes hash number for big buckets*/
inline static unsigned long big_hash_idx(unsigned long s)
{
	unsigned long idx;
	/* s is rounded => s = k*2^n (SF_ROUNDTO=2^n)
	 * index= i such that 2^i > s >= 2^(i-1)
	 *
	 * => index = number of the first non null bit in s*/
	idx = sizeof(long) * 8 - 1;
	for (; !(s & (1UL << (sizeof(long) * 8 - 1))); s <<= 1, idx--)
		;
	return idx;
}
#ifdef SFM_ONE_LOCK

#define SFM_MAIN_HASH_LOCK(qm, hash) lock_get(&(qm)->lock)
#define SFM_MAIN_HASH_UNLOCK(qm, hash) lock_release(&(qm)->lock)
#define SFM_POOL_LOCK(p, hash) lock_get(&(p)->lock)
#define SFM_POOL_UNLOCK(p, hash) lock_release(&(p)->lock)
#elif defined SFM_LOCK_PER_BUCKET

#define SFM_MAIN_HASH_LOCK(qm, hash) \
	lock_get(&(qm)->free_hash[(hash)].lock)
#define SFM_MAIN_HASH_UNLOCK(qm, hash)  \
	lock_release(&(qm)->free_hash[(hash)].lock)
#define SFM_POOL_LOCK(p, hash) lock_get(&(p)->pool_hash[(hash)].lock)
#define SFM_POOL_UNLOCK(p, hash) lock_release(&(p)->pool_hash[(hash)].lock)
#else
#error no locks defined
#endif /* SFM_ONE_LOCK/SFM_LOCK_PER_BUCKET */

#define SFM_BIG_GET_AND_SPLIT_LOCK(qm)   lock_get(&(qm)->get_and_split)
#define SFM_BIG_GET_AND_SPLIT_UNLOCK(qm) lock_release(&(qm)->get_and_split)

static unsigned long shm_max_hash = 0; /* maximum hash value (no point in
 searching further) */
static unsigned long pool_id = (unsigned long) -1;


/* call for each child */
int shm_pool_reset()
{
    pool_id=(unsigned long)-1;
    return 0;
}


#define shm_fix_pool_id(qm) \
    do{ \
        if (unlikely(pool_id>=SFM_POOLS_NO)) \
            pool_id=((unsigned)atomic_add(&(qm)->crt_id, 1))%SFM_POOLS_NO; \
    }while(0)



static inline void frag_push(struct shm_frag** head, struct shm_frag* frag)
{
    register struct shm_frag* old;
    register struct shm_frag* crt;

    crt=(void*)atomic_get_long(head);
    do{
        frag->u.nxt_free=crt;
        old=crt;
        membar_write_atomic_op();
        crt=(void*)atomic_cmpxchg_long((void*)head, (long)old, (long)frag);
    }while(crt!=old);
}


static inline struct shm_frag* frag_pop(struct shm_frag** head)
{
    register struct shm_frag* old;
    register struct shm_frag* crt;
    register struct shm_frag* nxt;

    crt=(void*)atomic_get_long(head);
    do{
        /* if circular list, test not needed */
        nxt=crt?crt->u.nxt_free:0;
        old=crt;
        membar_read_atomic_op();
        crt=(void*)atomic_cmpxchg_long((void*)head, (long)old, (long)nxt);
    }while(crt!=old);
    return crt;
}


static inline void shm_pool_insert (struct shm_pool* pool, int hash,
                                struct shm_frag* frag)
{
    unsigned long hash_bit;

    frag_push(&pool->pool_hash[hash].first, frag);
    atomic_inc_long((long*)&pool->pool_hash[hash].no);
    /* set it only if not already set (avoids an expensive
     * cache trashing atomic write op) */
    hash_bit=HASH_TO_BITMAP(hash);
    if  (!(atomic_get_long((long*)&pool->bitmap) & hash_bit))
        atomic_or_long((long*)&pool->bitmap, hash_bit);
}



/* returns 1 if it's ok to add a fragm. to pool p_id @ hash, 0 otherwise */
static inline int shm_check_pool(struct shm_block* qm, unsigned long p_id,
                                    int hash, int split)
{
    /* TODO: come up with something better
     * if fragment is some  split/rest from an allocation, that is
     *  >= requested size, accept it, else
     *  look at misses and current fragments and decide based on them */
    return (p_id<SFM_POOLS_NO) && (split ||
            ( (qm->pool[p_id].pool_hash[hash].no < MIN_POOL_FRAGS) ||
              ((qm->pool[p_id].pool_hash[hash].misses >
                 qm->pool[p_id].pool_hash[hash].no) &&
                (qm->pool[p_id].pool_hash[hash].no<MAX_POOL_FRAGS) ) ) );
}
/* choose on which pool to add a free'd packet
 * return - pool idx or -1 if it should be added to main*/
static inline unsigned long  shm_choose_pool(struct shm_block* qm,
                                                struct shm_frag* frag,
                                                int hash, int split)
{
    /* check original pool first */
    if (shm_check_pool(qm, frag->id, hash, split))
        return frag->id;
    else{
        /* check if our pool is properly set */
        shm_fix_pool_id(qm);
        /* check if my pool needs some frags */
        if ((pool_id!=frag->id) && (shm_check_pool(qm,  pool_id, hash, 0))){
            frag->id=pool_id;
            return pool_id;
        }
    }
    /* else add it back to main */
    frag->id=(unsigned long)(-1);
    return frag->id;
}

static inline void shm_insert_free(struct shm_block* qm, struct shm_frag* frag,
                                    int split)
{
    struct shm_frag** f;
    unsigned long p_id;
    int hash;
    unsigned long hash_bit;

    if (likely(frag->size<=SF_POOL_MAX_SIZE)){
        hash=GET_SMALL_HASH(frag->size);
        if (unlikely((p_id=shm_choose_pool(qm, frag, hash, split))==
                    (unsigned long)-1)){
            /* add it back to the "main" hash */
                frag->id=(unsigned long)(-1); /* main hash marker */
                /*insert it here*/
                frag_push(&(qm->free_hash[hash].first), frag);
                atomic_inc_long((long*)&qm->free_hash[hash].no);
                /* set it only if not already set (avoids an expensive
                * cache trashing atomic write op) */
                hash_bit=HASH_TO_BITMAP(hash);
                if  (!(atomic_get_long((long*)&qm->bitmap) & hash_bit))
                    atomic_or_long((long*)&qm->bitmap, hash_bit);
        }else{
            /* add it to one of the pools pool */
            shm_pool_insert(&qm->pool[p_id], hash, frag);
        }
    }else{
        hash=GET_BIG_HASH(frag->size);
        SFM_MAIN_HASH_LOCK(qm, hash);
            f=&(qm->free_hash[hash].first);
            for(; *f; f=&((*f)->u.nxt_free))
                if (frag->size <= (*f)->size) break;
            frag->id=(unsigned long)(-1); /* main hash marker */
            /*insert it here*/
            frag->u.nxt_free=*f;
            *f=frag;
            qm->free_hash[hash].no++;
            /* inc. big hash free size ? */
        SFM_MAIN_HASH_UNLOCK(qm, hash);
    }

}

void shm_split_frag(struct shm_block* qm, struct shm_frag* frag,
                    unsigned long size)
{
    unsigned long rest;
    struct shm_frag* n;
    int bigger_rest;

    rest=frag->size-size;
#ifdef MEM_FRAG_AVOIDANCE
    if ((rest> (FRAG_OVERHEAD+SF_MALLOC_OPTIMIZE))||
        (rest>=(FRAG_OVERHEAD+size))){ /* the residue fragm. is big enough*/
        bigger_rest=1;
#else
    if (rest>(FRAG_OVERHEAD+SF_MIN_FRAG_SIZE)){
        bigger_rest=rest>=(size+FRAG_OVERHEAD);
#endif
        frag->size=size;
        /*split the fragment*/
        n=FRAG_NEXT(frag);
        n->size=rest-FRAG_OVERHEAD;
        n->id=pool_id;
        FRAG_CLEAR_USED(n); /* never used */
        /* reinsert n in free list*/
        shm_insert_free(qm, n, bigger_rest);
    }else{
        /* we cannot split this fragment any more => alloc all of it*/
    }
}


struct shm_block* shm_malloc_init(void* addr, unsigned long size)
{
	char* start;
	char* end;
	struct shm_block* qm;
	unsigned long init_overhead;
	char* address = (char*)addr;
	int r;
#ifdef SFM_LOCK_PER_BUCKET
	int i;
#endif


	/* make address and size multiple of 8*/
	start = (char*) ROUNDUP((unsigned long) address);
	if (size < start - address)
		return 0;
	size -= (start - address);
	if (size < (SF_MIN_FRAG_SIZE + FRAG_OVERHEAD))
		return 0;
	size = ROUNDDOWN(size);

	init_overhead = INIT_OVERHEAD;

	if (size < init_overhead)
	{
		/* not enough mem to create our control structures !!!*/
		return 0;
	}
	end = start + size;
	qm = (struct shm_block*) start;
	memset(qm, 0, sizeof(struct shm_block));
	qm->size = size;
	size -= init_overhead;

	qm->first_frag = (struct shm_frag*) (start
	        +ROUNDUP(sizeof(struct shm_block)));
	qm->last_frag = (struct shm_frag*) (end - sizeof(struct shm_frag));
	/* init initial fragment*/
	qm->first_frag->size = size;
	qm->first_frag->id = (unsigned long) -1; /* not in a pool */
	qm->last_frag->size = 0;


	/* link initial fragment into the free list*/

	shm_insert_free(qm, qm->first_frag, 0);
	shm_max_hash = GET_HASH(size);


	/* init locks */
	if (lock_init(&qm->get_and_split) == 0)
		goto error;
#ifdef SFM_ONE_LOCK
	if (lock_init(&qm->lock) == 0)
	{
		lock_destroy(&qm->get_and_split);
		goto error;
	}
	for (r = 0; r < SFM_POOLS_NO; r++)
	{
		if (lock_init(&qm->pool[r].lock) == 0)
		{
			for (; r > 0; r--)
				lock_destroy(&qm->pool[r - 1].lock);
			lock_destroy(&qm->lock);
			lock_destroy(&qm->get_and_split);
			goto error;
		}
	}
#elif defined(SFM_LOCK_PER_BUCKET)
	for (r=0; r<SF_HASH_SIZE; r++)
	if (lock_init(&qm->free_hash[r].lock)==0)
	{
		for(;r>0; r--) lock_destroy(&qm->free_hash[r-1].lock);
		lock_destroy(&qm->get_and_split);
		goto error;
	}
	for (i=0; i<SFM_POOLS_NO; i++)
	{
		for (r=0; r<SF_HASH_POOL_SIZE; r++)
		if (lock_init(&qm->pool[i].pool_hash[r].lock)==0)
		{
			for(;r>0; r--) lock_destroy(&qm->pool[i].poo_hash[r].lock);
			for(; i>0; i--)
			{
				for (r=0; r<SF_HASH_POOL_SIZE; r++)
				lock_destroy(&qm->pool[i].pool_hash[r].lock);
			}
			for (r=0; r<SF_HASH_SIZE; r++)
			lock_destroy(&qm->free_hash[r].lock);
			lock_destroy(&qm->get_and_split);
			goto error;
		}
	}
#endif
	qm->is_init = 1;
	return qm;
	error: return 0;
}

/* cleanup */
void shm_malloc_destroy(struct shm_block* qm)
{
	int r, i;
	/* destroy all the locks */
	if (!qm || !qm->is_init)
		return; /* nothing to do */
	lock_destroy(&qm->get_and_split);
#ifdef SFM_ONE_LOCK
	lock_destroy(&qm->lock);
	for (r = 0; r < SFM_POOLS_NO; r++)
	{
		lock_destroy(&qm->pool[r].lock);
	}
#elif defined(SFM_LOCK_PER_BUCKET)
	for (r=0; r<SF_HASH_SIZE; r++)
	lock_destroy(&qm->free_hash[r].lock);
	for (i=0; i<SFM_POOLS_NO; i++)
	{
		for (r=0; r<SF_HASH_POOL_SIZE; r++)
		lock_destroy(&qm->pool[i].pool_hash[r].lock);
	}
#endif
	qm->is_init = 0;
}

/* returns next set bit in bitmap, starts at b
 * if b is set, returns b
 * if not found returns BITMAP_BITS */
static inline unsigned long _next_set_bit(unsigned long b,
        unsigned long* bitmap)
{
	for (; !((1UL << b) & *bitmap) && b < BITMAP_BITS; b++)
		;
	return b;
}

/* returns start of block b and sets *end
 * (handles also the "rest" block at the end ) */
static inline unsigned long _hash_range(unsigned long b, unsigned long* end)
{
	unsigned long s;

	if ((unlikely(b>=BITMAP_BITS)))
	{
		s = BIT_TO_HASH(BITMAP_BITS);
		*end = SF_HASH_POOL_SIZE; /* last, possible rest block */
	}
	else
	{
		s = BIT_TO_HASH(b);
		*end = s + BITMAP_BLOCK_SIZE;
	}
	return s;
}

static inline struct shm_frag* pool_get_frag(struct shm_block* qm,
        struct shm_pool* pool, int hash, unsigned long size)
{
	int r;
	int next_block;
	struct shm_frag* volatile * f;
	struct shm_frag* frag;
	unsigned long b;
	unsigned long eob;


	/* special case for r=hash */
	r = hash;
	f = &pool->pool_hash[r].first;


	/* detach it from the free list */
	if ((frag = frag_pop((struct shm_frag**) f)) == 0)
		goto not_found;
	found: atomic_dec_long((long*) &pool->pool_hash[r].no);
	frag->u.nxt_free = 0; /* mark it as 'taken' */
	frag->id = pool_id;

	shm_split_frag(qm, frag, size);

	if (&qm->pool[pool_id] == pool)
		atomic_inc_long((long*) &pool->hits);
	return frag;

	not_found: atomic_inc_long((long*) &pool->pool_hash[r].misses);
	r++;
	b = HASH_BIT_POS(r);

	while (r < SF_HASH_POOL_SIZE)
	{
		b = _next_set_bit(b, &pool->bitmap);
		next_block = _hash_range(b, &eob);
		r = (r < next_block) ? next_block : r;
		for (; r < eob; r++)
		{
			f = &pool->pool_hash[r].first;
			if ((frag = frag_pop((struct shm_frag**) f)) != 0)
				goto found;
			atomic_inc_long((long*) &pool->pool_hash[r].misses);
		}
		b++;
	}
	atomic_inc_long((long*) &pool->missed);
	return 0;
}

static inline struct shm_frag* main_get_frag(struct shm_block* qm, int hash,
        unsigned long size)
{
	int r;
	int next_block;
	struct shm_frag* volatile * f;
	struct shm_frag* frag;
	unsigned long b;
	unsigned long eob;

	r = hash;
	b = HASH_BIT_POS(r);
	while (r <= SF_MALLOC_OPTIMIZE / SF_ROUNDTO)
	{
		b = _next_set_bit(b, &qm->bitmap);
		next_block = _hash_range(b, &eob);
		r = (r < next_block) ? next_block : r;
		for (; r < eob; r++)
		{
			f = &qm->free_hash[r].first;
			if ((frag = frag_pop((struct shm_frag**) f)) != 0)
			{
				atomic_dec_long((long*) &qm->free_hash[r].no);
				frag->u.nxt_free = 0; /* mark it as 'taken' */
				frag->id = pool_id;
				shm_split_frag(qm, frag, size);
				return frag;
			}
		}
		b++;
	}
	/* big fragments */
	SFM_BIG_GET_AND_SPLIT_LOCK(qm);
	for (; r <= shm_max_hash; r++)
	{
		f = &qm->free_hash[r].first;
		if (*f)
		{
			SFM_MAIN_HASH_LOCK(qm, r);
			if (unlikely((*f)==0))
			{
				/* not found */
				SFM_MAIN_HASH_UNLOCK(qm, r);
				continue;
			}
			for (; (*f); f = &((*f)->u.nxt_free))
				if ((*f)->size >= size)
				{
					/* found, detach it from the free list*/
					frag = *f;
					*f = frag->u.nxt_free;
					frag->u.nxt_free = 0; /* mark it as 'taken' */
					qm->free_hash[r].no--;
					SFM_MAIN_HASH_UNLOCK(qm, r);
					frag->id = pool_id;
					shm_split_frag(qm, frag, size);
					SFM_BIG_GET_AND_SPLIT_UNLOCK(qm);
					return frag;
				};
			SFM_MAIN_HASH_UNLOCK(qm, r);
			/* try in a bigger bucket */
		}
	}
	SFM_BIG_GET_AND_SPLIT_UNLOCK(qm);
	return 0;
}

void* shm_malloc(struct shm_block* qm, unsigned long size)
{
	struct shm_frag* frag;
	int hash;
	unsigned int i;


	/*size must be a multiple of 8*/
	size = ROUNDUP(size);
	/*	if (size>(qm->size-qm->real_used)) return 0; */

	/* check if our pool id is set */
	shm_fix_pool_id(qm);


	/*search for a suitable free frag*/
	if (likely(size<=SF_POOL_MAX_SIZE))
	{
		hash = GET_SMALL_HASH(size);
		/* try first in our pool */
		if (likely((frag=pool_get_frag(qm, &qm->pool[pool_id], hash, size))
				!=0 ))
			goto found;
		/* try in the "main" free hash, go through all the hash */
		if (likely((frag=main_get_frag(qm, hash, size))!=0))
			goto found;
		/* really low mem , try in other pools */
		for (i = (pool_id + 1); i < (pool_id + SFM_POOLS_NO); i++)
		{
			if ((frag = pool_get_frag(qm, &qm->pool[i % SFM_POOLS_NO], hash,
			        size)) != 0)
				goto found;
		}
		/* not found, bad! */
		return 0;
	}
	else
	{
		hash = GET_BIG_HASH(size);
		if ((frag = main_get_frag(qm, hash, size)) == 0)
			return 0; /* not found, bad! */
	}

	found:
	/* we found it!*/
	FRAG_MARK_USED(frag); /* mark it as used */
	return (char*) frag + sizeof(struct shm_frag);
}

void shm_free(struct shm_block* qm, void* p)
{
	struct shm_frag* f;

	if (unlikely(p==0))
	{
		return;
	}
	f = (struct shm_frag*) ((char*) p - sizeof(struct shm_frag));
	shm_insert_free(qm, f, 0);
}

void* shm_realloc(struct shm_block* qm, void* p, unsigned long size)
{
	struct shm_frag *f;
	unsigned long orig_size;
	void *ptr;
#ifndef SFM_REALLOC_REMALLOC
	struct shm_frag *n;
	struct shm_frag **pf;
	unsigned long diff;
	unsigned long p_id;
	int hash;
	unsigned long n_size;
	struct shm_pool * pool;
#endif

	if (size == 0)
	{
		if (p)
			shm_free(qm, p);
		return 0;
	}
	if (p == 0)
		return shm_malloc(qm, size);
	f = (struct shm_frag*) ((char*) p - sizeof(struct shm_frag));
	size = ROUNDUP(size);
	orig_size = f->size;
	if (f->size > size)
	{
		/* shrink */
		shm_split_frag(qm, f, size);
	}
	else if (f->size < size)
	{
#ifndef SFM_REALLOC_REMALLOC
		/* should set a magic value in list head and in push/pop if magic value =>
		 * lock and wait */
#error LL_MALLOC realloc not finished yet
		diff=size-f->size;
		n=FRAG_NEXT(f);
		if (((char*)n < (char*)qm->last_frag) &&
				(n->u.nxt_free)&&((n->size+FRAG_OVERHEAD)>=diff))
		{
			/* join  */
			/* detach n from the free list */
			try_again:
			p_id=n->id;
			n_size=n->size;
			if ((unlikely(p_id >=SFM_POOLS_NO)))
			{
				hash=GET_HASH(n_size);
				SFM_MAIN_HASH_LOCK(qm, hash);
				if (unlikely((n->u.nxt_free==0) ||
								((n->size+FRAG_OVERHEAD)<diff)))
				{
					SFM_MAIN_HASH_UNLOCK(qm, hash);
					goto not_found;
				}
				if (unlikely((n->id!=p_id) || (n->size!=n_size)))
				{
					/* fragment still free, but changed, either
					 * moved to another pool or has a diff. size */
					SFM_MAIN_HASH_UNLOCK(qm, hash);
					goto try_again;
				}
				pf=&(qm->free_hash[hash].first);
				/* find it */
				for(;(*pf)&&(*pf!=n); pf=&((*pf)->u.nxt_free));/*FIXME slow */
				if (*pf==0)
				{
					SFM_MAIN_HASH_UNLOCK(qm, hash);
					/* not found, bad! */
					LOG(L_WARN, "WARNING: sfm_realloc: could not find %p in "
							"free " "list (hash=%d)\n", n, hash);
					/* somebody is in the process of changing it ? */
					goto not_found;
				}
				/* detach */
				*pf=n->u.nxt_free;
				n->u.nxt_free=0; /* mark it immediately as detached */
				qm->free_hash[hash].no--;
				SFM_MAIN_HASH_UNLOCK(qm, hash);
				/* join */
				f->size+=n->size+FRAG_OVERHEAD;
				/* split it if necessary */
				if (f->size > size)
				{
					shm_split_frag(qm, f, size);
				}
			}
			else
			{ /* p_id < SFM_POOLS_NO (=> in a pool )*/
				hash=GET_SMALL_HASH(n_size);
				pool=&qm->pool[p_id];
				SFM_POOL_LOCK(pool, hash);
				if (unlikely((n->u.nxt_free==0) ||
								((n->size+FRAG_OVERHEAD)<diff)))
				{
					SFM_POOL_UNLOCK(pool, hash);
					goto not_found;
				}
				if (unlikely((n->id!=p_id) || (n->size!=n_size)))
				{
					/* fragment still free, but changed, either
					 * moved to another pool or has a diff. size */
					SFM_POOL_UNLOCK(pool, hash);
					goto try_again;
				}
				pf=&(pool->pool_hash[hash].first);
				/* find it */
				for(;(*pf)&&(*pf!=n); pf=&((*pf)->u.nxt_free));/*FIXME slow */
				if (*pf==0)
				{
					SFM_POOL_UNLOCK(pool, hash);
					/* not found, bad! */
					/* somebody is in the process of changing it ? */
					goto not_found;
				}
				/* detach */
				*pf=n->u.nxt_free;
				n->u.nxt_free=0; /* mark it immediately as detached */
				pool->pool_hash[hash].no--;
				SFM_POOL_UNLOCK(pool, hash);
				/* join */
				f->size+=n->size+FRAG_OVERHEAD;
				/* split it if necessary */
				if (f->size > size)
				{
					shm_split_frag(qm, f, size);
				}
			}
		}
		else
		{
			not_found:
			/* could not join => realloc */
#else/* SFM_REALLOC_REMALLOC */
		{
#endif /* SFM_REALLOC_REMALLOC */
			ptr = shm_malloc(qm, size);
			if (ptr)
			{
				/* copy, need by libssl */
				memcpy(ptr, p, orig_size);
				shm_free( qm, p);
			}
			p = ptr;
		}
	}
	else
	{
		/* do nothing */
	}
	return p;
}

void shm_status(struct shm_block* qm)
{
	//do nothing
}

/* fills a malloc info structure with info about the block
 * if a parameter is not supported, it will be filled with 0 */
void shm_info(struct shm_block* qm, struct mem_info* info)
{
	int r, k;
	unsigned long total_frags;
	struct shm_frag* f;

	memset(info, 0, sizeof(*info));
	total_frags = 0;
	info->total_size = qm->size;
	info->min_frag = SF_MIN_FRAG_SIZE;
	/* we'll have to compute it all */
	for (r = 0; r <= SF_MALLOC_OPTIMIZE / SF_ROUNDTO; r++)
	{
		info->free += qm->free_hash[r].no * UN_HASH(r);
		total_frags += qm->free_hash[r].no;
	}
	for (; r <= shm_max_hash; r++)
	{
		total_frags += qm->free_hash[r].no;
		SFM_MAIN_HASH_LOCK(qm, r);
		for (f = qm->free_hash[r].first; f; f = f->u.nxt_free)
		{
			info->free += f->size;
		}
		SFM_MAIN_HASH_UNLOCK(qm, r);
	}
	for (k = 0; k < SFM_POOLS_NO; k++)
	{
		for (r = 0; r < SF_HASH_POOL_SIZE; r++)
		{
			info->free += qm->pool[k].pool_hash[r].no * UN_HASH(r);
			total_frags += qm->pool[k].pool_hash[r].no;
		}
	}
	info->real_used = info->total_size - info->free;
	info->used = info->real_used - total_frags * FRAG_OVERHEAD - INIT_OVERHEAD
	        -FRAG_OVERHEAD;
	info->max_used = 0; /* we don't really know */
	info->total_frags = total_frags;
}

/* returns how much free memory is available
 * on error (not compiled with bookkeeping code) returns (unsigned long)(-1) */
unsigned long shm_available(struct shm_block* qm)
{
	/* we don't know how much free memory we have and it's to expensive
	 * to compute it */
	return ((unsigned long) -1);
}

