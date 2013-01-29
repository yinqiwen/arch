/*
 * thread_rwlock.hpp
 *
 *  Created on: 2011-4-4
 *      Author: wqy
 */

#ifndef THREAD_RWLOCK_HPP_
#define THREAD_RWLOCK_HPP_
#include "common/base.hpp"
#include "concurrent/lock_mode.hpp"
#include <pthread.h>
namespace arch
{
	namespace concurrent
	{
		class ThreadReadWriteLock
		{
			private:
				pthread_rwlock_t m_rwlock;
			public:
				ThreadReadWriteLock();
				bool Lock(LockMode mode = WRITE_LOCK, uint64 timeout = 0);
				bool TryLock(LockMode mode = WRITE_LOCK);
				bool Unlock();
				~ThreadReadWriteLock();
		};

	}
}

#endif /* THREAD_RWLOCK_HPP_ */
