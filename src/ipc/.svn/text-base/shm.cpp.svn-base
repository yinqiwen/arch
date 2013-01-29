/*
 * shm.cpp
 *
 *  Created on: 2011-4-7
 *      Author: qiyingwang
 */
#include "ipc/shm.hpp"
#include <errno.h>

using namespace arch::ipc;
using namespace arch::exception;

SystemVSharedMemory::SystemVSharedMemory(uint32 size) :
    m_is_self_created(true), m_key(-1), m_attached_addr(NULL), m_mem_size(0)
{
    if (-1 == (m_key = shmget(IPC_PRIVATE, size, IPC_CREAT | IPC_EXCL | 0666)))
    {
        throw APIException(errno);
    }
    m_attached_addr = shmat(m_key, NULL, 0);
    if ((void*) -1 == m_attached_addr)
    {
        throw APIException(errno);
    }
    m_mem_size = size;

}

SystemVSharedMemory::SystemVSharedMemory(key_t* key) :
    m_is_self_created(false), m_key(-1), m_attached_addr(NULL), m_mem_size(0)
{
    if (NULL == key)
    {
        throw APIException("NULL key to attach shared memory.");
    }
    struct shmid_ds shmbuf;
    if (shmctl(m_key, IPC_STAT, &shmbuf) == -1)
    {
        throw APIException("failed to get status of shared memory");
    }
    m_attached_addr = shmat(*key, NULL, 0);
    if ((void*) -1 == m_attached_addr)
    {
        throw APIException(errno);
    }
    m_key = *key;
    m_mem_size = shmbuf.shm_segsz;
}

key_t SystemVSharedMemory::GetRawKey()
{
    return m_key;
}
void* SystemVSharedMemory::GetRawAddress()
{
    return m_attached_addr;
}
uint32 SystemVSharedMemory::GetMemorySize()
{
    return m_mem_size;
}

SystemVSharedMemory::~SystemVSharedMemory()
{
    if (NULL != m_attached_addr)
    {
        shmdt(m_attached_addr);
    }

    struct shmid_ds shmbuf;
    if (shmctl(m_key, IPC_STAT, &shmbuf) == -1)
    {
        //do nothing
    }
    else
    {
        //remove shm
        if (m_is_self_created || shmbuf.shm_nattch == 0)
        {
            shmctl(m_key, IPC_RMID, NULL);
        }
    }
}

PosixSharedMemory::PosixSharedMemory(const std::string& name, uint32 size,
        bool rm_exist):m_name(name), m_mem_size(0)
{
    if(rm_exist)
    {
        shm_unlink(name.c_str());
    }
    int fd = shm_open(name.c_str(), O_CREAT |O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    uint32 mem_size = size;
    if(fd < 0)
    {
        int err = errno;
        if(err == EEXIST)
        {
            fd = shm_open(name.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
            if(fd < 0)
            {
                throw APIException(errno);
            }
            struct stat stat_buf;
            fstat(fd, &stat_buf);
            mem_size = stat_buf.st_size;
        }
        else
        {
            throw APIException(err);
        }
    }
    else
    {
        if(ftruncate(fd, size) < 0)
        {
            throw APIException(errno);
        }
    }
    m_attached_addr = mmap(NULL, mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(MAP_FAILED == m_attached_addr)
    {
        int err = errno;
        throw APIException(err);
    }
    m_mem_size = mem_size;
}
const std::string& PosixSharedMemory::GetName()
{
    return m_name;
}
void* PosixSharedMemory::GetRawAddress()
{
    return m_attached_addr;
}
uint32 PosixSharedMemory::GetMemorySize()
{
    return m_mem_size;
}
PosixSharedMemory::~PosixSharedMemory()
{
    munmap(m_attached_addr, m_mem_size);
}
