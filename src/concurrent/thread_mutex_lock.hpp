/*
 * ThreadMutexLock.hpp
 *
 *  Created on: 2011-1-12
 *      Author: wqy
 */

#ifndef NOVA_THREADMUTEXLOCK_HPP_
#define NOVA_THREADMUTEXLOCK_HPP_
#include "common/base.hpp"
#include "concurrent/thread_mutex.hpp"
#include "concurrent/thread_condition.hpp"
#include "concurrent/time_unit.hpp"
namespace arch
{
	namespace concurrent
	{
		class ThreadMutexLock: public ThreadMutex, public ThreadCondition
		{
			public:
				ThreadMutexLock();
				ThreadMutexLock(ThreadMutex& mutex, ThreadCondition& cond);
				//ThreadMutexLock(ThreadMutexCondition& mutexCond);
				/**
				 * wait for specified timeout value, if timeout < 0, then wait for ever
				 */
				bool Wait(uint64 timeout = 0, TimeUnit unit = MILLIS);
				virtual bool Notify();
				virtual bool NotifyAll();
				bool Lock(uint64 timeout = 0);
				bool Unlock();
				bool TryLock();
				virtual ~ThreadMutexLock();
		};

	}
}

#endif /* THREADMUTEXLOCK_HPP_ */
