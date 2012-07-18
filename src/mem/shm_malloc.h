/*
 * shm_mem.h
 *
 *  Created on: 2011-4-6
 *      Author: qiyingwang
 */

#ifndef SHM_MEM_H_
#define SHM_MEM_H_

#include "mem/mem_malloc.h"

#include "concurrent/lock_ops.h"
#include "concurrent/atomic_ops.h"
#include "concurrent/compiler_opt.h"
/* defs*/
#ifdef __cplusplus
extern "C"
{
#endif

#ifdef GEN_LOCK_T_UNLIMITED
#define SFM_LOCK_PER_BUCKET
#else
#define SFM_ONE_LOCK
#endif

#ifdef DBG_SF_MALLOC
#if defined(__CPU_sparc64) || defined(__CPU_sparc)
/* tricky, on sun in 32 bits mode long long must be 64 bits aligned
 * but long can be 32 bits aligned => malloc should return long long
 * aligned memory */
    #define SF_ROUNDTO  sizeof(long long)
#else
    #define SF_ROUNDTO  sizeof(void*) /* size we round to, must be = 2^n, and
                      sizeof(sfm_frag) must be multiple of SF_ROUNDTO !*/
#endif
#else /* DBG_SF_MALLOC */
    #define SF_ROUNDTO 8UL
#endif
#define SF_MIN_FRAG_SIZE    SF_ROUNDTO

#define SFM_POOLS_NO 4U /* the more the better, but higher initial
                            mem. consumption */

#define SF_MALLOC_OPTIMIZE_FACTOR 14UL /*used below */
#define SF_MALLOC_OPTIMIZE  (1UL<<SF_MALLOC_OPTIMIZE_FACTOR)
                                /* size to optimize for,
                                    (most allocs <= this size),
                                    must be 2^k */

#define SF_HASH_POOL_SIZE   (SF_MALLOC_OPTIMIZE/SF_ROUNDTO + 1)
#define SF_POOL_MAX_SIZE    SF_MALLOC_OPTIMIZE

#define SF_HASH_SIZE (SF_MALLOC_OPTIMIZE/SF_ROUNDTO + \
        (sizeof(long)*8-SF_MALLOC_OPTIMIZE_FACTOR)+1)

/* hash structure:
 * 0 .... SF_MALLOC_OPTIMIZE/SF_ROUNDTO  - small buckets, size increases with
 *                            SF_ROUNDTO from bucket to bucket
 * +1 .... end -  size = 2^k, big buckets */

struct shm_frag{
    union{
        struct shm_frag* nxt_free;
        long reserved;
    }u;
    unsigned long size;
    unsigned long id; /* TODO better optimize the size */
    /* pad to SF_ROUNDTO multiple */
    char _pad[((3*sizeof(long)+SF_ROUNDTO-1)&~(SF_ROUNDTO-1))-3*sizeof(long)];
};

struct shm_frag_lnk{
    struct shm_frag* first;
#ifdef SFM_LOCK_PER_BUCKET
    gen_lock_t lock;
#endif
    unsigned long no;
};

struct shm_pool_head{
    struct shm_frag* first;
#ifdef SFM_LOCK_PER_BUCKET
    gen_lock_t lock;
#endif
    unsigned long no;
    unsigned long misses;
};

struct shm_pool{
#ifdef SFM_ONE_LOCK
    gen_lock_t lock;
#endif
    unsigned long missed;
    unsigned long hits; /* debugging only TODO: remove */
    unsigned long bitmap;
    struct shm_pool_head pool_hash[SF_HASH_POOL_SIZE];
};

struct shm_block{
#ifdef SFM_ONE_LOCK
    gen_lock_t lock;
#endif
    atomic_t crt_id; /* current pool */
    unsigned long size; /* total size */
    /* stats are kept now per bucket */
    struct shm_frag* first_frag;
    struct shm_frag* last_frag;
    unsigned long bitmap; /* only up to SF_MALLOC_OPTIMIZE */
    struct shm_frag_lnk free_hash[SF_HASH_SIZE];
    struct shm_pool pool[SFM_POOLS_NO];
    int is_init;
    gen_lock_t get_and_split;
    char _pad[256];
};



struct shm_block* shm_malloc_init(void* address, unsigned long size);
void shm_malloc_destroy(struct shm_block* qm);
int shm_pool_reset();

void* shm_malloc(struct shm_block*, unsigned long size);

void  shm_free(struct shm_block*, void* p);

void*  shm_realloc(struct shm_block*, void* p, unsigned long size);

void  shm_status(struct shm_block*);
void  shm_info(struct shm_block*, struct mem_info*);
unsigned long shm_available(struct shm_block*);
#ifdef __cplusplus
}
#endif
#endif /* SHM_MEM_H_ */
