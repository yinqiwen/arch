/*
 * ThreadMutex.cpp
 *
 *  Created on: 2011-1-12
 *      Author: wqy
 */
#include "concurrent/thread_mutex.hpp"

using namespace arch::concurrent;

ThreadMutex::ThreadMutex()
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&m_mutex, &attr);
	pthread_mutexattr_destroy(&attr);
}

ThreadMutex::~ThreadMutex()
{
	pthread_mutex_destroy(&m_mutex);
}

pthread_mutex_t& ThreadMutex::GetRawMutex()
{
	return m_mutex;
}
