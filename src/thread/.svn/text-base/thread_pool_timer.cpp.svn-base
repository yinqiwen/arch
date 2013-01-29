/*
 * ThreadPoolTimer.cpp
 *
 *  Created on: 2011-2-11
 *      Author: qiyingwang
 */
#include "thread/thread_pool_timer.hpp"
#include "util/time_helper.hpp"
#include "concurrent/lock_guard.hpp"

using namespace arch::thread;
using namespace arch::util;
using namespace arch::concurrent;

ThreadPoolTimer::ThreadPoolTimer(uint32 coreSize) :
	ThreadPool(coreSize, 0), m_running(false)
{

}

void ThreadPoolTimer::Start()
{
	if (!m_running)
	{
		m_running = true;
		ThreadPool::Start();
		Thread::Start();
	}
}

void ThreadPoolTimer::Stop()
{
	LockGuard<ThreadMutexLock> guard(m_mutex_lock);
	m_running = false;
	m_mutex_lock.Notify();
}

void ThreadPoolTimer::BeforeScheduled(TimerTask* task)
{
    m_mutex_lock.Lock();
}

void ThreadPoolTimer::AfterScheduled(TimerTask* task)
{
	int64 nextTime = GetNearestTaskTriggerTime();
	if (nextTime <= 0 || static_cast<uint64>(nextTime) > task->GetNextTriggerTime())
	{
		m_mutex_lock.Notify();
	}
	m_mutex_lock.Unlock();
}

void ThreadPoolTimer::Run()
{
	while (m_running)
	{
		LockGuard<ThreadMutexLock> guard(m_mutex_lock);
		int64 nextTime = GetNearestTaskTriggerTime();
		//TimerTask* task = getNearestTimerTask();
		while (m_running && nextTime <= 0)
		{
			m_mutex_lock.Wait();
		}
		if (nextTime > 0 && m_running)
		{
			uint64 now = get_current_monotonic_millis();
			if (static_cast<uint64>(nextTime) <= now)
			{
				Routine();
			}
			else
			{
				m_mutex_lock.Wait(nextTime - now, MILLIS);
			}
		}
	}
}

ThreadPoolTimer::~ThreadPoolTimer()
{
	Stop();
}
