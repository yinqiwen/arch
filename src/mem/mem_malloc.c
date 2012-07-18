/*
 * mem_malloc.c
 *
 *  Created on: 2011-4-6
 *      Author: qiyingwang
 */
#include "mem/mem_malloc.h"
#include <string.h>
#include <stdlib.h>

#if __GNUC__ >= 3

#define likely(expr)              __builtin_expect(!!(expr), 1)
#define unlikely(expr)            __builtin_expect(!!(expr), 0)

#else /* __GNUC__ */
#define likely(expr) (expr)
#define unlikely(expr) (expr)
#endif /* __GNUC__ */

#define ROUNDTO_MASK    (~((unsigned long)ROUNDTO-1))
#define ROUNDUP(s)      (((s)+(ROUNDTO-1))&ROUNDTO_MASK)
#define ROUNDDOWN(s)    ((s)&ROUNDTO_MASK)

#define INIT_OVERHEAD   \
    (ROUNDUP(sizeof(struct mem_block))+sizeof(struct mem_frag))

#define GET_HASH(s)   ( ((unsigned long)(s)<=MEM_MALLOC_OPTIMIZE)?\
                            (unsigned long)(s)/ROUNDTO: \
                            MEM_MALLOC_OPTIMIZE/ROUNDTO+big_hash_idx((s))- \
                                MEM_MALLOC_OPTIMIZE_FACTOR+1 )

#define UN_HASH(h)  ( ((unsigned long)(h)<=(MEM_MALLOC_OPTIMIZE/ROUNDTO))?\
                        (unsigned long)(h)*ROUNDTO: \
                        1UL<<((unsigned long)(h)-MEM_MALLOC_OPTIMIZE/ROUNDTO+\
                            MEM_MALLOC_OPTIMIZE_FACTOR-1))

#define FRAG_NEXT(f) \
    ((struct mem_frag*)((char*)(f)+sizeof(struct mem_frag)+(f)->size ))
#define FRAG_OVERHEAD   (sizeof(struct mem_frag))

#define big_hash_idx(s) ((unsigned long)bit_scan_reverse((unsigned long)(s)))

#define mem_bmp_reset(qm, b) \
    do{ \
        (qm)->free_bitmap[(b)/MEM_HASH_BMP_BITS] &= \
                                            ~(1UL<<((b)%MEM_HASH_BMP_BITS)); \
    }while(0)

#define mem_bmp_set(qm, b) \
    do{ \
        (qm)->free_bitmap[(b)/MEM_HASH_BMP_BITS] |= \
                                            1UL<<((b)%MEM_HASH_BMP_BITS); \
    }while(0)



static inline void mem_insert_free(struct mem_block* qm, struct mem_frag* frag)
{
    struct mem_frag** f;
    int hash;

    hash = GET_HASH(frag->size);
    f = &(qm->free_hash[hash].first);
    if (frag->size > MEM_MALLOC_OPTIMIZE)
    { /* because of '<=' in GET_HASH,
     (different from 0.8.1[24] on
     purpose --andrei ) */
        for (; *f; f = &((*f)->u.nxt_free))
        {
            if (frag->size <= (*f)->size)
                break;
        }
    }

    /*insert it here*/
    frag->u.nxt_free = *f;
    *f = frag;
    qm->free_hash[hash].no++;
    mem_bmp_set(qm, hash);
}

static inline void mem_split_frag(struct mem_block* qm, struct mem_frag* frag,
        unsigned long size)
{
    unsigned long rest;
    struct mem_frag* n;

    rest = frag->size - size;
    //#ifdef MEM_FRAG_AVOIDANCE
    if ((rest > (FRAG_OVERHEAD + MEM_MALLOC_OPTIMIZE)) || (rest >= (FRAG_OVERHEAD
            + size)))
    { /* the residue fragm. is big enough*/
        //#else
        //if (rest>(FRAG_OVERHEAD+MIN_FRAG_SIZE)){
        //#endif
        frag->size = size;
        /*split the fragment*/
        n = FRAG_NEXT(frag);
        n->size = rest - FRAG_OVERHEAD;
        //FRAG_CLEAR_USED(n); /* never used */

        /* reinsert n in free list*/
        mem_insert_free(qm, n);
    }
    else
    {
        /* we cannot split this fragment any more => alloc all of it*/
    }
}

inline static int mem_bmp_first_set(struct mem_block* qm, int start)
{
    int bmp_idx;
    int bit;
    int r;
    mem_hash_bitmap_t test_val;
    mem_hash_bitmap_t v;

    bmp_idx = start / MEM_HASH_BMP_BITS;
    bit = start % MEM_HASH_BMP_BITS;
    test_val = 1UL << ((unsigned long) bit);
    if (qm->free_bitmap[bmp_idx] & test_val)
        return start;
    else if (qm->free_bitmap[bmp_idx] & ~(test_val - 1))
    {
        v = qm->free_bitmap[bmp_idx] >> (bit + 1);
        return start + 1 + bit_scan_forward((unsigned long) v);
    }
    for (r = bmp_idx + 1; r < MEM_HASH_BMP_SIZE; r++)
    {
        if (qm->free_bitmap[r])
        {
            /* find first set bit */
            return r * MEM_HASH_BMP_BITS + bit_scan_forward(
                    (unsigned long) qm->free_bitmap[r]);
        }
    }
    /* not found, nothing free */
    return -1;
}

/* init malloc and return a fm_block*/
struct mem_block* mem_malloc_init(void* address, unsigned long size)
{
    char* start;
    char* end;
    struct mem_block* qm;
    unsigned long init_overhead;
    char* start_addr = (char*) address;
    /* make address and size multiple of 8*/
    start = (char*) ROUNDUP((unsigned long) start_addr);

    if (size < start - start_addr)
        return 0;
    size -= (start - start_addr);
    if (size < (MIN_FRAG_SIZE + FRAG_OVERHEAD))
        return 0;
    size = ROUNDDOWN(size);

    init_overhead = INIT_OVERHEAD;

    if (size < init_overhead)
    {
        /* not enough mem to create our control structures !!!*/
        return 0;
    }
    end = start + size;
    qm = (struct mem_block*) start;
    memset(qm, 0, sizeof(struct mem_block));
    qm->size = size;
    size -= init_overhead;

    qm->first_frag = (struct mem_frag*) (start
            +ROUNDUP(sizeof(struct mem_block)));
    qm->last_frag = (struct mem_frag*) (end - sizeof(struct mem_frag));
    /* init initial fragment*/
    qm->first_frag->size = size;
    qm->last_frag->size = 0;

    /* link initial fragment into the free list*/

    mem_insert_free(qm, qm->first_frag);

    return qm;
}

void* mem_malloc(struct mem_block* qm, unsigned long size)
{
    struct mem_frag** f;
    struct mem_frag* frag;
    int hash;
    /*size must be a multiple of 8*/
    size = ROUNDUP(size);
    /*  if (size>(qm->size-qm->real_used)) return 0; */

    /*search for a suitable free frag*/
    hash = mem_bmp_first_set(qm, GET_HASH(size));
    if (likely(hash >= 0))
    {
        if (likely(hash <= MEM_MALLOC_OPTIMIZE / ROUNDTO))
        { /* return first match */
            f = &(qm->free_hash[hash].first);
            goto found;
        }
        /* if we are here we are searching for a "big" fragment
         between F_MALLOC_OPTIMIZE/ROUNDTO+1
         and F_MALLOC_OPTIMIZE/ROUNDTO + (32|64) - F_MALLOC_OPTIMIZE_FACTOR
         => 18 hash buckets on 32 bits and 50 buckets on 64 bits
         The free hash bitmap is used to jump directly to non-empty
         hash buckets.
         */
        do
        {
            for (f = &(qm->free_hash[hash].first); (*f); f
                    = &((*f)->u.nxt_free))
                if ((*f)->size >= size)
                    goto found;
            hash++; /* try in next hash cell */
        } while ((hash < MEM_HASH_SIZE)
                && ((hash = mem_bmp_first_set(qm, hash)) >= 0));
    }

    /* not found, bad! */
    return 0;

    found:
    /* we found it!*/
    /* detach it from the free list*/
    frag = *f;
    *f = frag->u.nxt_free;
    frag->u.nxt_free = 0; /* mark it as 'taken' */
    qm->free_hash[hash].no--;
    if (qm->free_hash[hash].no == 0)
        mem_bmp_reset(qm, hash);

    /*see if we'll use full frag, or we'll split it in 2*/

    mem_split_frag(qm, frag, size);
    //FRAG_MARK_USED(frag); /* mark it as used */
    return (char*) frag + sizeof(struct mem_frag);
}

void mem_free(struct mem_block* qm, void* p)
{
    struct mem_frag* f;
    unsigned long size;

    if (p == 0)
    {
        return;
    }
    f = (struct mem_frag*) ((char*) p - sizeof(struct mem_frag));

    size = f->size;

    mem_insert_free(qm, f);
}

void* mem_realloc(struct mem_block* qm, void* p, unsigned long size)

{
    struct mem_frag *f;
    struct mem_frag **pf;
    unsigned long diff;
    unsigned long orig_size;
    struct mem_frag *n;
    void *ptr;
    int hash;

    if (size == 0)
    {
        if (p)
            mem_free(qm, p);
        return 0;
    }
    if (p == 0)
        return mem_malloc(qm, size);
    f = (struct mem_frag*) ((char*) p - sizeof(struct mem_frag));
    size = ROUNDUP(size);
    orig_size = f->size;
    if (f->size > size)
    {
        /* shrink */
        mem_split_frag(qm, f, size);
    }
    else if (f->size < size)
    {
        /* grow */
        diff = size - f->size;
        n = FRAG_NEXT(f);
        if (((char*) n < (char*) qm->last_frag) && (n->u.nxt_free) && ((n->size
                + FRAG_OVERHEAD) >= diff))
        {
            /* join  */
            /* detach n from the free list */
            hash = GET_HASH(n->size);
            pf = &(qm->free_hash[hash].first);
            /* find it */
            for (; (*pf) && (*pf != n); pf = &((*pf)->u.nxt_free))
                ; /*FIXME slow */
            if (*pf == 0)
            {
                /* not found, bad! */
                abort();
            }
            /* detach */
            *pf = n->u.nxt_free;
            qm->free_hash[hash].no--;
            //#ifdef F_MALLOC_HASH_BITMAP
            if (qm->free_hash[hash].no == 0)
                mem_bmp_reset(qm, hash);
            //#endif /* F_MALLOC_HASH_BITMAP */
            /* join */
            f->size += n->size + FRAG_OVERHEAD;
            /* split it if necessary */
            if (f->size > size)
            {

                mem_split_frag(qm, f, size);
            }
        }
        else
        {
            /* could not join => realloc */
            ptr = mem_malloc(qm, size);
            if (ptr)
            {
                /* copy, need by libssl */
                memcpy(ptr, p, orig_size);
                mem_free(qm, p);
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

void mem_status(struct mem_block* qm)
{
    //    struct mem_frag* f;
    //    int i,j;
    //    int h;
    //    int unused;
    //    unsigned long size;
    //    int memlog;
    //
    //    memlog=cfg_get(core, core_cfg, memlog);
    //    //LOG_(DEFAULT_FACILITY, memlog, "fm_status: ", "fm_status (%p):\n", qm);
    //    if (!qm) return;
    //
    //    LOG_(DEFAULT_FACILITY, memlog, "fm_status: ", " heap size= %ld\n",
    //            qm->size);
    //    /*
    //    LOG_(DEFAULT_FACILITY, memlog, "fm_status: ", "dumping all fragments:\n");
    //    for (f=qm->first_frag, i=0;((char*)f<(char*)qm->last_frag) && (i<10);
    //            f=FRAG_NEXT(f), i++){
    //        LOG_(DEFAULT_FACILITY, memlog, "fm_status: ",
    //                "    %3d. %c  address=%x  size=%d\n", i,
    //                (f->u.reserved)?'a':'N',
    //                (char*)f+sizeof(struct fm_frag), f->size);
    //#ifdef DBG_F_MALLOC
    //        LOG_(DEFAULT_FACILITY, memlog, "fm_status: ",
    //                "            %s from %s: %s(%d)\n",
    //                (f->u.is_free)?"freed":"alloc'd", f->file, f->func, f->line);
    //#endif
    //    }
    //*/
    //    //LOG_(DEFAULT_FACILITY, memlog, "fm_status: ", "dumping free list:\n");
    //    for(h=0,i=0,size=0;h<F_HASH_SIZE;h++){
    //        unused=0;
    //        for (f=qm->free_hash[h].first,j=0; f;
    //                size+=f->size,f=f->u.nxt_free,i++,j++){
    //            if (!FRAG_WAS_USED(f)){
    //                unused++;
    //#ifdef DBG_F_MALLOC
    //                LOG_(DEFAULT_FACILITY, memlog, "fm_status: ",
    //                            "unused fragm.: hash = %3d, fragment %p,"
    //                            " address %p size %lu, created from %s: %s(%ld)\n",
    //                            h, f, (char*)f+sizeof(struct fm_frag), f->size,
    //                            f->file, f->func, f->line);
    //#endif
    //            };
    //        }
    //        if (j) LOG_(DEFAULT_FACILITY, memlog, "fm_status: ",
    //                            "hash = %3d fragments no.: %5d, unused: %5d\n\t\t"
    //                            " bucket size: %9lu - %9lu (first %9lu)\n",
    //                            h, j, unused, UN_HASH(h),
    //                        ((h<=F_MALLOC_OPTIMIZE/ROUNDTO)?1:2)* UN_HASH(h),
    //                            qm->free_hash[h].first->size
    //                );
    //        if (j!=qm->free_hash[h].no){
    //            LOG(L_CRIT, "BUG: fm_status: different free frag. count: %d!=%ld"
    //                    " for hash %3d\n", j, qm->free_hash[h].no, h);
    //        }
    //        /*
    //        {
    //            LOG_(DEFAULT_FACILITY, memlog, "fm_status: ",
    //                    "   %5d.[%3d:%3d] %c  address=%x  size=%d(%x)\n",
    //                    i, h, j,
    //                    (f->u.reserved)?'a':'N',
    //                    (char*)f+sizeof(struct fm_frag), f->size, f->size);
    //#ifdef DBG_F_MALLOC
    //            DBG("            %s from %s: %s(%d)\n",
    //                (f->u.reserved)?"freed":"alloc'd", f->file, f->func, f->line);
    //#endif
    //        }
    //    */
    //    }
    //    LOG_(DEFAULT_FACILITY, memlog, "fm_status: ",
    //            "TOTAL: %6d free fragments = %6lu free bytes\n", i, size);
    //    LOG_(DEFAULT_FACILITY, memlog, "fm_status: ",
    //            "-----------------------------\n");
}

void mem_info(struct mem_block* qm, struct mem_info* info)
{
    int r;
    long total_frags;
    //#if !defined(DBG_F_MALLOC) && !defined(MALLOC_STATS)
    struct mem_frag* f;
    //#endif

    memset(info, 0, sizeof(*info));
    total_frags = 0;
    info->total_size = qm->size;
    info->min_frag = MIN_FRAG_SIZE;
    /* we'll have to compute it all */
    for (r = 0; r <= MEM_MALLOC_OPTIMIZE / ROUNDTO; r++)
    {
        info->free += qm->free_hash[r].no * UN_HASH(r);
        total_frags += qm->free_hash[r].no;
    }
    for (; r < MEM_HASH_SIZE; r++)
    {
        total_frags += qm->free_hash[r].no;
        for (f = qm->free_hash[r].first; f; f = f->u.nxt_free)
        {
            info->free += f->size;
        }
    }
    info->real_used = info->total_size - info->free;
    info->used = 0; /* we don't really now */
    info->used = info->real_used - total_frags * FRAG_OVERHEAD - INIT_OVERHEAD
            - FRAG_OVERHEAD;
    info->max_used = 0; /* we don't really now */
    info->total_frags = total_frags;
}

unsigned long mem_available(struct mem_block* qm)
{
    /* we don't know how much free memory we have and it's to expensive
     * to compute it */
    return ((unsigned long) -1);
}

