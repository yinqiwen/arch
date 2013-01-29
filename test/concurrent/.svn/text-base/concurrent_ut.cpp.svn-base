/*
 * util_ut.cpp
 *
 *  Created on: 2011-3-14
 *      Author: qiyingwang
 */
#include <gtest/gtest.h>
#include "concurrent/lock_ops.h"
#include "concurrent/semaphore.hpp"
#include "concurrent/thread_mutex_lock.hpp"
#include "util/time_helper.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace arch::concurrent;
using namespace arch::util;

TEST(FastLock, lock)
{
    uint32 loop_count = 10000000;
    gen_lock_t lock;
    lock_init(&lock);
    uint32 i = 0;
    uint64 start = get_current_epoch_millis();
    for(;i< loop_count;i++)
    {
        lock_get(&lock);
        lock_release(&lock);
    }
    uint64 end = get_current_epoch_millis();
    printf("Futex lock/unlock %u times cost %u ms.\n", loop_count, (end - start));

    SystemVSemaphore sem2(1);
    start = get_current_epoch_millis();
    for(i = 0;i< loop_count;i++)
    {
        sem2.Acquire();
        sem2.Release();
    }
    end = get_current_epoch_millis();
    printf("SystemVSemaphore lock/unlock %u times cost %u ms.\n", loop_count, (end - start));

    ThreadMutexLock mutex;
    start = get_current_epoch_millis();
    for(i = 0;i< loop_count;i++)
    {
        mutex.Lock();
        mutex.Unlock();
    }
    end = get_current_epoch_millis();
    printf("ThreadMutexLock lock/unlock %u times cost %u ms.\n", loop_count, (end - start));

    PosixSemaphore sem(1);
    start = get_current_epoch_millis();
    for(i = 0;i< loop_count;i++)
    {
        sem.Acquire();
        sem.Release();
    }
    end = get_current_epoch_millis();
    printf("PosixSemaphore lock/unlock %u times cost %u ms.\n", loop_count, (end - start));
}

