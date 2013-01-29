/*
 * ThreadMutexLock.cpp
 *
 *  Created on: 2011-1-12
 *      Author: wqy
 */
#include "concurrent/thread_mutex_lock.hpp"
#include "util/time_helper.hpp"
#include <sys/time.h>

using namespace arch::concurrent;
using namespace arch::util;

ThreadMutexLock::ThreadMutexLock(ThreadMutex& mutex, ThreadCondition& cond) :
	ThreadMutex(mutex), ThreadCondition(cond)
{
	//pthread_mutex_lock(&(m_mutex.getRawMutex()));
}

ThreadMutexLock::ThreadMutexLock()
{

}

ThreadMutexLock::~ThreadMutexLock()
{
	//pthread_mutex_unlock(&(m_mutex.getRawMutex()));
}

bool ThreadMutexLock::Wait(uint64 timeout, TimeUnit unit)
{
	if (timeout <= 0)
	{
		return 0 == pthread_cond_wait(&m_cond, &m_mutex);
	}
	else
	{
		struct timespec next;
		get_current_epoch_time(next);
		uint64 inc = nanostime(timeout, unit);
		add_nanos(next, inc);
		return 0 == pthread_cond_timedwait(&m_cond, &m_mutex, &next);
	}
}

bool ThreadMutexLock::NotifyAll()
{
	return 0 == pthread_cond_broadcast(&m_cond);
}

bool ThreadMutexLock::Notify()
{
	return 0 == pthread_cond_signal(&m_cond);
}

bool ThreadMutexLock::Lock(uint64 timeout)
{
	if (0 == timeout)
	{
		return 0 == pthread_mutex_lock(&m_mutex);
	}
	else
	{
		struct timespec next;
		get_current_epoch_time(next);
		uint64 nanos = nanostime(timeout, MILLIS);
		add_nanos(next, nanos);
		return 0 == pthread_mutex_timedlock(&m_mutex, &next);
	}
}

bool ThreadMutexLock::Unlock()
{
	return 0 == pthread_mutex_unlock(&m_mutex);
}

bool ThreadMutexLock::TryLock()
{
	return 0 == pthread_mutex_trylock(&m_mutex);
}
