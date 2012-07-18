/*
 * process_rwlock.cpp
 *
 *  Created on: 2010-10-3
 *      Author: wqy
 */
#include "concurrent/process_rwlock.hpp"
#include "exception/api_exception.hpp"

using namespace arch::concurrent;
using namespace arch::exception;

ProcessReadWriteLock::ProcessReadWriteLock() :
	m_reader_count(0), m_lock_mode(INVALID_LOCK), m_read_sem(0), m_sem(0)
{

}

bool ProcessReadWriteLock::Lock(LockMode mode)
{
	switch (mode)
	{
		case READ_LOCK:
		{
			m_read_sem.WaitAndAdd(0, 1);
			m_reader_count++;
			if (m_reader_count == 1)
			{
				m_sem.WaitAndAdd(0, 1);
			}
			m_read_sem.Acquire(1);
			m_lock_mode = READ_LOCK;
			return true;

		}
		case WRITE_LOCK:
		{
		    m_sem.WaitAndAdd(0, 1);
		    m_lock_mode = WRITE_LOCK;
		    return true;
		}
		default:
		{
			return false;
		}
	}
}

bool ProcessReadWriteLock::Unlock()
{
	switch (m_lock_mode)
	{
		case READ_LOCK:
		{
			m_read_sem.WaitAndAdd(0, 1);
			m_reader_count--;
			if (m_reader_count == 0)
			{
				m_sem.Acquire(1);
			}
			m_read_sem.Acquire(1);
			m_reader_count--;
			m_lock_mode = INVALID_LOCK;
			return true;

		}
		case WRITE_LOCK:
		{
			m_lock_mode = INVALID_LOCK;
			m_sem.Acquire(1);
			return true;
		}
		default:
		{
			return false;
		}
	}
}

ProcessReadWriteLock::~ProcessReadWriteLock()
{

}
