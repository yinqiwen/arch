/*
 * thread_rwlock.cpp
 *
 *  Created on: 2011-4-4
 *      Author: wqy
 */
#include "concurrent/thread_rwlock.hpp"
#include "exception/api_exception.hpp"
#include "util/time_helper.hpp"

using namespace arch::concurrent;
using namespace arch::exception;
using namespace arch::util;

ThreadReadWriteLock::ThreadReadWriteLock()
{
    if (pthread_rwlock_init(&m_rwlock, NULL) < 0)
    {
        throw APIException("Failed to init thread read_write lock.");
    }
}

bool ThreadReadWriteLock::Lock(LockMode mode, uint64 timeout)
{
    switch (mode)
    {
        case READ_LOCK:
        {
            if (0 == timeout)
            {
                return 0 == pthread_rwlock_rdlock(&m_rwlock);
            }
            else
            {
                struct timespec next;
                get_current_epoch_time(next);
                uint64 inc = nanostime(timeout, MILLIS);
                add_nanos(next, inc);
                return 0 == pthread_rwlock_timedrdlock(&m_rwlock, &next);
            }
        }
        case WRITE_LOCK:
        {
            if (0 == timeout)
            {
                return 0 == pthread_rwlock_wrlock(&m_rwlock);
            }
            else
            {
                struct timespec next;
                get_current_epoch_time(next);
                uint64 inc = nanostime(timeout, MILLIS);
                add_nanos(next, inc);
                return 0 == pthread_rwlock_timedwrlock(&m_rwlock, &next);
            }
        }
        default:
        {
            return false;
        }
    }
}

bool ThreadReadWriteLock::Unlock()
{
    return 0 == pthread_rwlock_unlock(&m_rwlock);
}

bool ThreadReadWriteLock::TryLock(LockMode mode)
{
    switch (mode)
    {
        case READ_LOCK:
        {
            return 0 == pthread_rwlock_tryrdlock(&m_rwlock);
        }
        case WRITE_LOCK:
        {
            return 0 == pthread_rwlock_trywrlock(&m_rwlock);
        }
        default:
        {
            return false;
        }
    }
}

ThreadReadWriteLock::~ThreadReadWriteLock()
{
    pthread_rwlock_destroy(&m_rwlock);
}
