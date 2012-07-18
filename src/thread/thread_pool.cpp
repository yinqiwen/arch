/*
 * ThreadPool.cpp
 *
 *  Created on: 2011-1-12
 *      Author: wqy
 */
#include "thread/thread_pool.hpp"
#include "concurrent/thread_mutex_lock.hpp"
#include "concurrent/lock_guard.hpp"

using namespace arch::thread;
using namespace arch::concurrent;
using std::vector;

ThreadPool::ThreadPoolWorker::ThreadPoolWorker(ThreadPool* pool) :
	Thread(NULL), m_pool(pool)
{

}

void ThreadPool::ThreadPoolWorker::Run()
{
	Runnable* runner = NULL;
	while (m_pool->m_running)
	{
		runner = NULL;
		do
		{
			LockGuard<ThreadMutexLock> guard(m_pool->m_mutex_lock);
			while (m_pool->m_taskQueue.empty() && m_pool->m_running)
			{
				try
				{
					m_pool->m_mutex_lock.Wait();
				}
				catch (...)
				{

				}
			}
			if (!m_pool->m_taskQueue.empty() && m_pool->m_running)
			{
				runner = m_pool->m_taskQueue.front();
				m_pool->m_taskQueue.pop();
			}

		}
		while (0);

		try
		{
			if (NULL != runner)
			{
				m_pool->BeforeExecute(this, runner);
				runner->Run();
				m_pool->AfterExecute(this, runner);
			}
		}
		catch (...)
		{

		}
	}
}

ThreadPool::ThreadPool(uint32_t coreSize, uint32_t maxTaskQueueSize) :
	m_running(false), m_coreSize(coreSize), m_maxWaitingTaskQueueSize(
	        maxTaskQueueSize)
{
	if (coreSize == 0)
	{
		throw coreSize;
	}
}

void ThreadPool::BeforeExecute(Thread* thread, Runnable* task)
{

}

void ThreadPool::AfterExecute(Thread* thread,Runnable* task)
{

}

ThreadPool::~ThreadPool()
{
	Shutdown();
	vector<ThreadPoolWorker*>::iterator it = m_pool.begin();
	while (it != m_pool.end())
	{
		DELETE(*it);
		it++;
	}
}

void ThreadPool::Start()
{
	if (!m_running)
	{
		m_running = true;
		for (uint32_t i = 0; i < m_coreSize; i++)
		{
			ThreadPoolWorker* worker = NULL;
			NEW(worker, ThreadPoolWorker(this));
			if (NULL == worker)
			{
				//do sth.
				continue;
			}
			m_pool.push_back(worker);
			worker->Start();

		}
	}
}


bool ThreadPool::Submit(Runnable* task)
{
	if (NULL == task || !m_running)
	{
		return false;
	}
	m_mutex_lock.Lock();
	if(m_maxWaitingTaskQueueSize > 0 && m_taskQueue.size() >= m_maxWaitingTaskQueueSize)
	{
		return false;
	}
	m_taskQueue.push(task);
	m_mutex_lock.Notify();
	return true;
}

void ThreadPool::Shutdown()
{
	//AutoThreadMutexLock lock(m_mutex_cond);
	m_mutex_lock.Lock();
	m_running = false;
	m_mutex_lock.NotifyAll();
}
