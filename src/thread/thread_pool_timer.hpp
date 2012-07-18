/*
 * ScheduledExecutor.hpp
 *
 *  Created on: 2011-2-11
 *      Author: qiyingwang
 */

#ifndef NOVA_SCHEDULEDEXECUTOR_HPP_
#define NOVA_SCHEDULEDEXECUTOR_HPP_
#include "thread/thread_pool.hpp"
#include "concurrent/thread_mutex_lock.hpp"
#include "timer/timer.hpp"
using arch::timer::Timer;
using arch::timer::TimerTask;
namespace arch
{
	namespace thread
	{
		class ThreadPoolTimer: public Thread, public ThreadPool, public Timer
		{
			private:
				ThreadMutexLock m_mutex_lock;
				volatile bool m_running;
				void Run();
				void BeforeScheduled(TimerTask* task);
				void AfterScheduled(TimerTask* task);
				//void OnScheduled(TimerTask* task);
			public:
				ThreadPoolTimer(uint32 coreSize);
				void Start();
				void Stop();
				~ThreadPoolTimer();
		};
	}
}
#endif /* SCHEDULEDEXECUTOR_HPP_ */
