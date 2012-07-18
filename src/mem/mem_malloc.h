/*
 * mem_malloc.h
 *
 *  Created on: 2011-4-6
 *      Author: qiyingwang
 */

#ifndef MEM_MALLOC_H_
#define MEM_MALLOC_H_
#include "mem/bit_scan.h"
#ifdef __cplusplus
extern "C"
{
#endif

#define ROUNDTO 8UL
#define MIN_FRAG_SIZE   ROUNDTO

#define MEM_MALLOC_OPTIMIZE_FACTOR 14UL /*used below */
#define MEM_MALLOC_OPTIMIZE  (1UL<<MEM_MALLOC_OPTIMIZE_FACTOR)
#define MEM_HASH_SIZE (MEM_MALLOC_OPTIMIZE/ROUNDTO + \
        (sizeof(long)*8-MEM_MALLOC_OPTIMIZE_FACTOR)+1)
    typedef unsigned long mem_hash_bitmap_t;
#define MEM_HASH_BMP_BITS  (sizeof(mem_hash_bitmap_t)*8)
#define MEM_HASH_BMP_SIZE  \
        ((MEM_HASH_SIZE+MEM_HASH_BMP_BITS-1)/MEM_HASH_BMP_BITS)
#define MEM_HASH_SIZE (MEM_MALLOC_OPTIMIZE/ROUNDTO + \
        (sizeof(long)*8-MEM_MALLOC_OPTIMIZE_FACTOR)+1)

    struct mem_info
    {
            unsigned long total_size;
            unsigned long free;
            unsigned long used;
            unsigned long real_used; /*used + overhead*/
            unsigned long max_used;
            unsigned long min_frag;
            unsigned long total_frags; /* total fragment no */
    };

    struct mem_frag
    {
            unsigned long size;
            union
            {
                    struct mem_frag* nxt_free;
                    long reserved;
            } u;
    };

    struct mem_frag_lnk
    {
            struct mem_frag* first;
            unsigned long no;
    };

    struct mem_block
    {
            unsigned long size; /* total size */

            struct mem_frag* first_frag;
            struct mem_frag* last_frag;
            mem_hash_bitmap_t free_bitmap[MEM_HASH_BMP_SIZE];
            struct mem_frag_lnk free_hash[MEM_HASH_SIZE];
    };

    struct mem_block* mem_malloc_init(void* address, unsigned long size);

    void* mem_malloc(struct mem_block*, unsigned long size);

    void mem_free(struct mem_block*, void* p);

    void* mem_realloc(struct mem_block*, void* p, unsigned long size);

    void mem_status(struct mem_block*);
    void mem_info(struct mem_block*, struct mem_info*);

    unsigned long mem_available(struct mem_block*);

#ifdef __cplusplus
}
#endif

#endif /* MEM_MALLOC_H_ */
