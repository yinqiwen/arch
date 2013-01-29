/*
 * ngx_shm_lock.h
 *
 *  Created on: 2011-5-22
 *      Author: wqy
 */

#ifndef NGX_SHM_LOCK_H_
#define NGX_SHM_LOCK_H_

#include <stdint.h>
#include <semaphore.h>
#include "concurrent/ngx/ngx_atomic.h"
#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct
	{
			ngx_atomic_t lock;
			ngx_uint_t semaphore;
			sem_t sem;
			ngx_uint_t spin;
	} ngx_shmtx_t;

	int ngx_shmtx_create(ngx_shmtx_t *mtx);
	void ngx_shmtx_destory(ngx_shmtx_t *mtx);
	int ngx_shmtx_trylock(ngx_shmtx_t *mtx);
	void ngx_shmtx_lock(ngx_shmtx_t *mtx);
	void ngx_shmtx_unlock(ngx_shmtx_t *mtx);
#ifdef __cplusplus
}
#endif
#endif /* NGX_SHM_LOCK_H_ */
