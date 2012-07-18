/*
 * semaphore.cpp
 *
 *  Created on: 2010-10-3
 *      Author: wqy
 */
#include "concurrent/semaphore.hpp"
#include "exception/api_exception.hpp"
#include "util/time_helper.hpp"
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace arch::exception;
using namespace arch::concurrent;
using namespace arch::util;

//static union semun k_core_semctlarg;
static struct sembuf k_core_dolock[2];
static struct sembuf k_core_dounlock[1];

static void sem_core_init(void)
{
	static bool initialized = false;
	if (!initialized)
	{
		k_core_dolock[0].sem_num = 0;
		k_core_dolock[0].sem_op = 0;
		k_core_dolock[0].sem_flg = 0;
		k_core_dolock[1].sem_num = 0;
		k_core_dolock[1].sem_op = 1;
		k_core_dolock[1].sem_flg = SEM_UNDO;
		k_core_dounlock[0].sem_num = 0;
		k_core_dounlock[0].sem_op = -1;
		k_core_dounlock[0].sem_flg = SEM_UNDO;
		initialized = true;
	}
	return;
}

PosixSemaphore::PosixSemaphore(uint32 initValue, bool shareByProcess)
{
	if (-1 == sem_init(&m_sem, shareByProcess ? 1 : 0, initValue))
	{
		throw APIException(errno);
	}
}

bool PosixSemaphore::Acquire(uint64 timeout)
{
	if (0 == timeout)
	{
		while (-1 == sem_wait(&m_sem))
		{
			int err = errno;
			if (err == EINTR)
			{
				continue;
			}
			throw APIException(err);
		}
		return true;
	}
	else
	{
		struct timespec abstime;
		get_current_epoch_time(abstime);
		add_millis(abstime, timeout);
		//uint64 absmills = getCurrentTimeMillis() + timeout;
		//abstime.tv_sec = absmills / 1000;
		//abstime.tv_nsec = (absmills % 1000) * 1000000;
		int ret = -1;
		while ((ret= sem_timedwait(&m_sem, &abstime)) == -1)
		{
			int err = errno;
			if (err == EINTR)
			{
				continue;
			}
			break;
		}
		if (ret == -1 && errno == ETIMEDOUT)
		{
		    return false;
		}
		return true;
	}
}

bool PosixSemaphore::TryAcquire()
{
	while (-1 == sem_trywait(&m_sem))
	{
		int err = errno;
		if (err == EINTR)
		{
			continue;
		}
		return false;
	}
	return true;
}

void PosixSemaphore::Release()
{
	if (-1 == sem_post(&m_sem))
	{
		throw APIException(errno);
	}
}

uint32 PosixSemaphore::AvailablePermits() const
{
	int semValue;
	if (-1 == sem_getvalue(const_cast<sem_t*> (&m_sem), &semValue))
	{
		throw APIException(errno);
	}
	else
	{
		return (uint32) semValue;
	}
}

sem_t PosixSemaphore::GetRawKey()
{
	return m_sem;
}

PosixSemaphore::~PosixSemaphore()
{
	sem_destroy(&m_sem);
}

SystemVSemaphore::SystemVSemaphore(int32 initValue)
{
	int fdsem =
	        semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	if (fdsem == -1 && errno == EEXIST)
		fdsem = semget(IPC_PRIVATE, 1, IPC_EXCL | S_IRUSR | S_IWUSR);
	if (fdsem == -1)
		throw APIException("failed to acquire semaphore");
	union semun k_core_semctlarg;
	k_core_semctlarg.val = initValue;
	semctl(fdsem, 0, SETVAL, k_core_semctlarg);
	m_key = fdsem;
}

SystemVSemaphore::SystemVSemaphore(key_t* sem_key)
{
	int rc = 0;
	while (((rc = semctl(*sem_key, 0, GETVAL)) < 0) && (errno == EINTR))
		;
	if (-1 == rc)
	{
		int err = errno;
		throw APIException(err);
	}
	m_key = *sem_key;
}

key_t SystemVSemaphore::GetRawKey()
{
	return m_key;
}

bool SystemVSemaphore::WaitAndAdd(int32 expected, uint32 permits)
{
	if (0 == expected)
	{
		sem_core_init();
		k_core_dolock[1].sem_op = permits;
		int rc = 0;
		while (((rc = semop(m_key, k_core_dolock, 2)) < 0) && (errno == EINTR))
			;
		return rc == 0;
	}
	else
	{
		throw APIException("Non-zero expetecd value is not supported now.");
	}
}

bool SystemVSemaphore::Acquire(uint32 permits, uint64 timeout)
{
	struct sembuf temp[1];
	temp[0].sem_num = 0;
	temp[0].sem_op = (0 - permits);
	temp[0].sem_flg = SEM_UNDO;
	int rc = 0;
	if (timeout == 0)
	{
		while (((rc = semop(m_key, temp, 1)) < 0) && (errno == EINTR))
			;
		return true;
	}
	else
	{
		struct timespec timeout_spec;
		timeout_spec.tv_sec = timeout / 1000;
		timeout_spec.tv_nsec = (timeout % 1000) * 1000000;
		while (((rc = semtimedop(m_key, temp, 1, &timeout_spec)) < 0) && (errno
		        == EINTR))
			;
		if(rc < 0 && errno == EAGAIN)
		{
		    return false;
		}
		return true;
	}
}

bool SystemVSemaphore::TryAcquire(uint32 permits)
{
	struct sembuf temp[1];
	temp[0].sem_num = 0;
	temp[0].sem_op = (0 - permits);
	temp[0].sem_flg = 0;
	int rc = 0;
	while (((rc = semop(m_key, temp, 1)) < 0) && (errno == EINTR))
		;
	return rc == 0;
}

void SystemVSemaphore::Release(uint32 permits)
{
	struct sembuf temp[1];
	temp[0].sem_num = 0;
	temp[0].sem_op = permits;
	temp[0].sem_flg = 0;
	int rc;
	while (((rc = semop(m_key, temp, 1)) < 0) && (errno == EINTR))
		;
}

uint32 SystemVSemaphore::AvailablePermits() const
{
	int rc = 0;
	while (((rc = semctl(m_key, 0, GETVAL)) < 0) && (errno == EINTR))
		;
	if (-1 == rc)
	{
		int err = errno;
		throw APIException(err);
	}
	return (uint32) rc;
}

SystemVSemaphore::~SystemVSemaphore()
{
	semctl(m_key, 0, IPC_RMID, 0);
}
