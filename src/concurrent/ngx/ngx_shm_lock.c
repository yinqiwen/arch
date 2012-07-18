/*
 * ngx_shm_lock.c
 *
 *  Created on: 2011-5-22
 *      Author: wqy
 */
#include "concurrent/ngx/ngx_shm_lock.h"
#include <errno.h>
#if (HAVE_SCHED_YIELD)
#include <sched.h>
#define ngx_sched_yield()  sched_yield()
#else
#define ngx_sched_yield()  usleep(1)
#endif
/*
 * Copyright (C) Igor Sysoev
 */

int ngx_shmtx_create(ngx_shmtx_t *mtx)
{
    mtx->lock = 0;

    if (mtx->spin == (ngx_uint_t) -1)
    {
        return 0;
    }

    mtx->spin = 2048;

    if (sem_init(&mtx->sem, 1, 0) == -1)
    {
        //ngx_log_error(NGX_LOG_ALERT, ngx_cycle->log, ngx_errno,
        //        "sem_init() failed");
        mtx->semaphore = 0;
    }
    else
    {
        mtx->semaphore = 1;
    }

    return 0;
}

void ngx_shmtx_destory(ngx_shmtx_t *mtx)
{
    if (mtx->semaphore)
    {
        if (sem_destroy(&mtx->sem) == -1)
        {
            //ngx_log_error(NGX_LOG_ALERT, ngx_cycle->log, ngx_errno,
            //              "sem_destroy() failed");
        }
    }
}

int ngx_shmtx_trylock(ngx_shmtx_t *mtx)
{
    ngx_atomic_uint_t val;

    val = mtx->lock;

    return ((val & 0x80000000) == 0
            && ngx_atomic_cmp_set(&(mtx->lock), val, val | 0x80000000));
}

void ngx_shmtx_lock(ngx_shmtx_t *mtx)
{
    //ngx_uint_t i, n;
    ngx_atomic_uint_t val;

    //ngx_log_debug0(NGX_LOG_DEBUG_CORE, ngx_cycle->log, 0, "shmtx lock");

    for (;;)
    {

        val = mtx->lock;

        if ((val & 0x80000000) == 0
                && ngx_atomic_cmp_set(&(mtx->lock), val, val | 0x80000000))
        {
            return;
        }

        //            if (ngx_ncpu > 1) {
        //
        //                for (n = 1; n < mtx->spin; n <<= 1) {
        //
        //                    for (i = 0; i < n; i++) {
        //                        ngx_cpu_pause();
        //                    }
        //
        //                    val = *mtx->lock;
        //
        //                    if ((val & 0x80000000) == 0
        //                        && ngx_atomic_cmp_set(mtx->lock, val, val | 0x80000000))
        //                    {
        //                        return;
        //                    }
        //                }
        //            }


        if (mtx->semaphore)
        {
            val = mtx->lock;

            if ((val & 0x80000000)
                    && ngx_atomic_cmp_set(&(mtx->lock), val, val + 1))
            {
                //ngx_log_debug1(NGX_LOG_DEBUG_CORE, ngx_cycle->log, 0,
                //         "shmtx wait %XA", val);

                while (sem_wait(&mtx->sem) == -1)
                {
                    int err;

                    err = errno;

                    if (err != EINTR)
                    {
                        //ngx_log_error(NGX_LOG_ALERT, ngx_cycle->log, err,
                        //        "sem_wait() failed while waiting on shmtx");
                        break;
                    }
                }
            }

            continue;
        }
        ngx_sched_yield();
    }
}

void ngx_shmtx_unlock(ngx_shmtx_t *mtx)
{
    ngx_atomic_uint_t val, old, wait;

    if (mtx->spin != (ngx_uint_t) -1)
    {
        //ngx_log_debug0(NGX_LOG_DEBUG_CORE, ngx_cycle->log, 0, "shmtx unlock");
    }

    for (;;)
    {

        old = mtx->lock;
        wait = old & 0x7fffffff;
        val = wait ? wait - 1 : 0;

        if (ngx_atomic_cmp_set(&(mtx->lock), old, val))
        {
            break;
        }
    }

    if (wait == 0 || !mtx->semaphore)
    {
        return;
    }

    //ngx_log_debug1(NGX_LOG_DEBUG_CORE, ngx_cycle->log, 0,
    //               "shmtx wake %XA", old);

    if (sem_post(&mtx->sem) == -1)
    {
        //ngx_log_error(NGX_LOG_ALERT, ngx_cycle->log, ngx_errno,
        //              "sem_post() failed while wake shmtx");
    }

}
