/*
 * ThreadPool.hpp
 *
 *  Created on: 2011-1-12
 *      Author: wqy
 */

#ifndef NOVA_THREADPOOL_HPP_
#define NOVA_THREADPOOL_HPP_
#include "common/base.hpp"
#include "thread/thread.hpp"
#include "concurrent/thread_mutex_lock.hpp"
//#include <boost/pool/object_pool.hpp>
#include <vector>

using arch::concurrent::ThreadMutexLock;
namespace arch
{
	namespace thread
	{
		class ThreadPool
		{
			protected:
				class ThreadPoolWorker: public Thread
				{
					private:
						ThreadPool* m_pool;
					public:
						ThreadPoolWorker(ThreadPool* pool);
						void Run();
				};

				std::vector<ThreadPoolWorker*> m_pool;
				std::queue<Runnable*> m_taskQueue;
				ThreadMutexLock m_mutex_lock;
				volatile bool m_running;
				uint32_t m_coreSize;
				uint32_t m_maxWaitingTaskQueueSize;
				//void submit(RunnableTask* task);
				virtual void BeforeExecute(Thread* thread, Runnable* task);
				virtual void AfterExecute(Thread* thread,Runnable* task);
			public:
				ThreadPool(uint32_t coreSize, uint32_t maxTaskQueueSize = 0);
				virtual ~ThreadPool();
				void Start();
				bool Submit(Runnable* task);
				//void submitHeapTask(Runnable* task);
				void Shutdown();
		};
	}
}
#endif /* THREADPOOL_HPP_ */
