/*
 * ThreadCondition.cpp
 *
 *  Created on: 2011-1-12
 *      Author: wqy
 */
#include "concurrent/thread_condition.hpp"

using namespace arch::concurrent;

ThreadCondition::ThreadCondition()
{
	pthread_cond_init(&m_cond, NULL);
}

ThreadCondition::~ThreadCondition()
{
	pthread_cond_destroy(&m_cond);
}
