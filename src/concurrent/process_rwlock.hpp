/*
 * process_mutex_lock.hpp
 *
 *  Created on: 2011-4-4
 *      Author: wqy
 */

#ifndef PROCESS_MUTEX_LOCK_HPP_
#define PROCESS_MUTEX_LOCK_HPP_
#include "common/base.hpp"
#include "concurrent/thread_mutex.hpp"
#include "concurrent/semaphore.hpp"
#include "concurrent/time_unit.hpp"
#include "concurrent/lock_mode.hpp"

namespace arch
{
	namespace concurrent
	{
		/**
		 * This object should be hold on Shared memory between father-children processed
		 */
		class ProcessReadWriteLock
		{
			private:
				uint32 m_reader_count;
				LockMode m_lock_mode;
				SystemVSemaphore m_read_sem;
				SystemVSemaphore m_sem;
			public:
				ProcessReadWriteLock();
				bool Lock(LockMode mode = WRITE_LOCK);
				bool TryLock(LockMode mode = WRITE_LOCK);
				bool Unlock();
				~ProcessReadWriteLock();
		};
	}
}

#endif /* PROCESS_MUTEX_LOCK_HPP_ */
