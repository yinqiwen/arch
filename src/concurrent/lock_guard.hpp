/*
 * auto_lock.hpp
 *
 *  Created on: 2011-4-2
 *      Author: qiyingwang
 */

#ifndef AUTO_LOCK_HPP_
#define AUTO_LOCK_HPP_
#include "concurrent/lock_mode.hpp"
namespace arch
{
	namespace concurrent
	{
		template<typename T>
		class LockGuard
		{
			private:
				T& m_lock_impl;
			public:
				inline LockGuard(T& lock) :
					m_lock_impl(lock)
				{
					m_lock_impl.Lock();
				}
				~LockGuard()
				{
					m_lock_impl.Unlock();
				}
		};

		template<typename T>
		class ReadLockGuard
		{
			private:
				T& m_lock_impl;
			public:
				ReadLockGuard(T& lock) :
					m_lock_impl(lock)
				{
					m_lock_impl.Lock(READ_LOCK);
				}
				~ReadLockGuard()
				{
					m_lock_impl.Unlock();
				}
		};

		template<typename T>
		class WriteLockGuard
		{
			private:
				T& m_lock_impl;
			public:
				WriteLockGuard(T& lock) :
					m_lock_impl(lock)
				{
					m_lock_impl.Lock(WRITE_LOCK);
				}
				~WriteLockGuard()
				{
					m_lock_impl.Unlock();
				}
		};
	}
}

#endif /* AUTO_LOCK_HPP_ */
