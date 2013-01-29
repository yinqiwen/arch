/*
 * Copyright (C) Igor Sysoev
 */

/**
 * Port from nginx 1.0.0
 */

#ifndef _NGX_SLAB_H_INCLUDED_
#define _NGX_SLAB_H_INCLUDED_

//#include <ngx_config.h>
//#include <ngx_core.h>
#include <stdint.h>
#include "concurrent/ngx/ngx_shm_lock.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct ngx_slab_page_s ngx_slab_page_t;

    struct ngx_slab_page_s
    {
            uintptr_t slab;
            ngx_slab_page_t *next;
            uintptr_t prev;
    };

    typedef unsigned char u_char;

    typedef struct
    {
            //ngx_atomic_t      lock;

            size_t min_size;
            size_t min_shift;

            ngx_slab_page_t *pages;
            ngx_slab_page_t free;

            u_char *start;
            u_char *end;

            ngx_shmtx_t mutex;

            u_char *log_ctx;
            u_char zero;

            void *data;
            void *addr;
    } ngx_slab_pool_t;

    ngx_slab_pool_t* ngx_slab_init(void* addr, unsigned long size);
    void *ngx_slab_alloc(ngx_slab_pool_t *pool, size_t size);
    void *ngx_slab_alloc_locked(ngx_slab_pool_t *pool, size_t size);
    void ngx_slab_free(ngx_slab_pool_t *pool, void *p);
    void ngx_slab_free_locked(ngx_slab_pool_t *pool, void *p);

#ifdef __cplusplus
}
#endif
#endif /* _NGX_SLAB_H_INCLUDED_ */
