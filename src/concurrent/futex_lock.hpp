/*
 * futex_lock.hpp
 *
 *  Created on: 2011-4-14
 *      Author: qiyingwang
 */

#ifndef FUTEX_LOCK_HPP_
#define FUTEX_LOCK_HPP_
#include "common/base.hpp"
#include "concurrent/lock_mode.hpp"
#include "futexlock.h"
#include "util/time_helper.hpp"

using namespace arch::util;
namespace arch
{
    namespace concurrent
    {
        class FutexLock
        {
            private:
                futex_lock_t m_lock;
            public:
                inline FutexLock()
                {
                    futex_init(&m_lock);
                }
                inline bool Lock(uint64 timeout = 0)
                {
                    if(0 == timeout)
                    {
                        futex_get(&m_lock);
                        return true;
                    }
                    else
                    {
                        struct timespec next;
                        int2timespec(timeout, MILLIS, & next);
                        return 0 == futex_get_timeout(&m_lock, &next);
                    }
                }
                inline void Unlock()
                {
                    futex_release(&m_lock);
                }
                inline bool TryLock()
                {
                    return futex_try(&m_lock) == 1;
                }
        };
    }
}

#endif /* FUTEX_LOCK_HPP_ */
