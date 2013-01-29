/*
 * util_ut.cpp
 *
 *  Created on: 2011-3-14
 *      Author: qiyingwang
 */
#include <gtest/gtest.h>
#include "concurrent/semaphore.hpp"
#include "ipc/shm_pool.hpp"
#include "util/time_helper.hpp"
#include "mem/mm/mm.h"
using namespace arch::concurrent;
using namespace arch::ipc;
using namespace arch::util;

TEST(Semaphore, Init)
{
    SystemVSemaphore sem(2);
    key_t key = sem.GetRawKey();
    SystemVSemaphore sem2(&key);
}

TEST(SHM, MallocFree)
{
    uint32 loop_count = 10000000;
    uint32 i = 0;
    //SystemVSharedMemory shm1(2000000);
    FastSharedMemoryPool pool1(new SystemVSharedMemory(2000000));
    uint64 start = get_current_epoch_millis();
    for(;i< loop_count;i++)
    {
        void* temp = pool1.Malloc(256);
        memset(temp, 0, 256);
        pool1.Free(temp);
    }
    uint64 end = get_current_epoch_millis();
    printf("SystemV shm pool malloc/memset/free %u times cost %u ms.\n", loop_count, (end - start));

    //PosixSharedMemory shm2("test_shm", 2000000, true);
    FastSharedMemoryPool pool2(new PosixSharedMemory("test_shm", 2000000, true));
    start = get_current_epoch_millis();
    for(i = 0;i< loop_count;i++)
    {
        void* temp = pool2.Malloc(256);
        memset(temp, 0, 256);
        pool2.Free(temp);
    }
    end = get_current_epoch_millis();
    printf("Posix shm pool malloc/memset/free %u times cost %u ms.\n", loop_count, (end - start));

    MM* mm = mm_create(2000000, NULL);
    start = get_current_epoch_millis();
    for(i = 0;i< loop_count;i++)
    {
        void* temp = mm_malloc(mm, 256);
        memset(temp, 0, 256);
        mm_free(mm, temp);
    }
    end = get_current_epoch_millis();
    printf("libmm shm pool malloc/memset/free %u times cost %u ms.\n", loop_count, (end - start));
    mm_destroy(mm);
}

