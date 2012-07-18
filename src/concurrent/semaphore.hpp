/*
 * semaphore.hpp
 *
 *  Created on: 2010-10-3
 *      Author: wqy
 */

#ifndef SEMAPHORE_HPP_
#define SEMAPHORE_HPP_
#include "common/base.hpp"
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

namespace arch
{
	namespace concurrent
	{

		union semun
		{
				int val;
				struct semid_ds *buf;
				u_short *array;
#if defined(__linux__)
				struct seminfo  *__buf;
#endif
		};


		//not threading-safe if initValue>1
		class PosixSemaphore
		{
			private:
				sem_t m_sem;
				//uint32 m_initValue;
				//bool m_shareByProcess;
			public:
				PosixSemaphore(uint32 initValue = 1, bool shareByProcess = true);
				bool Acquire(uint64 timeout = 0);
				void Release();
				bool TryAcquire();
				uint32 AvailablePermits() const;
				sem_t GetRawKey();
				~PosixSemaphore();
		};

		class SystemVSemaphore
		{
			private:
				//uint32 m_init_value;
				key_t m_key;
			public:
				SystemVSemaphore(int32 initValue = 1);
				SystemVSemaphore(key_t* sem_key);
				bool Acquire(uint32 permits = 1, uint64 timeout = 0);
				/**
				 * If current sem value is expected, do the Release operation
				 * If current sem value is not expected, return false immediately.
				 * Currently, only expected value '0' is supported and atomic-safe
				 */
				bool WaitAndAdd(int32 expected = 0, uint32 added = 1);
				void Release(uint32 permits = 1);
				bool TryAcquire(uint32 permits = 1);
				uint32 AvailablePermits() const;
				key_t GetRawKey();
				~SystemVSemaphore();
		};
	}

}

#endif /* SEMAPHORE_HPP_ */
