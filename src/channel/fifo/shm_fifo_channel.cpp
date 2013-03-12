/*
 * shm_fifo_channel.cpp
 *
 *  Created on: 2011-3-24
 *      Author: qiyingwang
 */

#include "channel/all_includes.hpp"
#include "exception/api_exception.hpp"
#include "util/file_helper.hpp"
#include "logging/logger_macros.hpp"

using namespace arch::channel;
using namespace arch::channel::fifo;
using namespace arch::exception;
using namespace arch::util;
using namespace arch::ipc;

static const uint32 kDefaultByteArrayChunkSize = 8192;

static SHMByteArray* shm_allocate_bytes(size_t size)
{
    SharedMemoryPool* pool =
            SharedMemoryPoolHolder<IPC_FIFO_SHM_POOL>::GetSharedMemoryPool();
    SHMByteArray* array = NULL;
    void* mem = pool->Malloc(sizeof(SHMByteArray));
    RETURN_NULL_IF_NULL(mem);
    try
    {
        array = new (mem) SHMByteArray(pool,
                size < kDefaultByteArrayChunkSize ? size
                        : kDefaultByteArrayChunkSize);
        return array;
    } catch (Exception& e)
    {
        ERROR_LOG("Failed to initialize a shm byte array with size:%u", size);
        //mm_free(pool, mem);
        pool->Free(mem);
        return NULL;
    }
}

static void shm_destroy_bytes(SHMByteArray* array)
{
    array->~SHMByteArray();
    //MM* pool = SharedMemoryPool<IPC_MEM_POOL>::GetSharedMemoryPool();
    SharedMemoryPool* pool =
            SharedMemoryPoolHolder<IPC_FIFO_SHM_POOL>::GetSharedMemoryPool();
    pool->Free(array);
    //mm_free(pool, array);
}

SharedMemoryFIFOChannel::SharedMemoryFIFOChannel(ChannelService& factory) :
    Channel(NULL, factory), m_as_parent(false), m_parent_socket_fd(-1),
            m_child_socket_fd(-1), m_len_limit(2000), m_p2c_queue(NULL),
            m_c2p_queue(NULL)
{

}

bool SharedMemoryFIFOChannel::DoConfigure(const ChannelOptions& options)
{
    if (options.user_write_buffer_flush_timeout_mills == 0)
    {
        ERROR_LOG("'user_write_buffer_flush_timeout_mills' must be non-zero value.");
        return false;
    }
    return true;
}

bool SharedMemoryFIFOChannel::DoOpen()
{

    if (m_parent_socket_fd > 0 || m_child_socket_fd > 0)
    {
        return true;
    }
#ifndef ARCH_HAS_EVENTFD
    int fds[2];
    if (-1 == socketpair(AF_UNIX, SOCK_STREAM, 0, fds))
    {
        ERROR_LOG("socketpair() failed while creating shm fifo!");
        return false;
    }
    m_parent_socket_fd = fds[1];
    m_child_socket_fd = fds[0];
#else
    m_parent_socket_fd = eventfd(0, EFD_NONBLOCK);
    m_child_socket_fd = eventfd(0, EFD_NONBLOCK);
    if (-1 == m_parent_socket_fd || -1 == m_child_socket_fd)
    {
        ERROR_LOG("eventfd() failed while creating shm fifo!");
        return false;
    }
#endif
    //MM* pool = SharedMemoryPool<IPC_MEM_POOL>::GetSharedMemoryPool();
    SharedMemoryPool* pool =
            SharedMemoryPoolHolder<IPC_FIFO_SHM_POOL>::GetSharedMemoryPool();
    if (NULL == m_c2p_sem)
    {
        void* tmp = pool->Malloc(sizeof(PosixSemaphore));
        m_c2p_sem = new (tmp) PosixSemaphore;
    }
    if (NULL == m_p2c_sem)
    {
        void* tmp = pool->Malloc(sizeof(PosixSemaphore));
        m_p2c_sem = new (tmp) PosixSemaphore;
    }
    void* list_mem1 = pool->Malloc(sizeof(SHMFIFO));
    void* list_mem2 = pool->Malloc(sizeof(SHMFIFO));
    if (NULL == list_mem1 || NULL == list_mem2)
    {
        ERROR_LOG("malloc memory from shm pool failed.");
        DoClose();
        return false;
    }
    m_p2c_queue = new (list_mem1) SHMFIFO(pool);
    m_c2p_queue = new (list_mem2) SHMFIFO(pool);
    return true;
}

bool SharedMemoryFIFOChannel::DoBind(Address* local)
{
    return false;
}
bool SharedMemoryFIFOChannel::DoConnect(Address* remote)
{
    return false;
}

bool SharedMemoryFIFOChannel::DoClose()
{
#ifndef ARCH_HAS_EVENTFD
    if (!Channel::DoClose())
    {
        return false;
    }
    m_parent_socket_fd = -1;
    m_child_socket_fd = -1;
    return true;
#else
    if(-1 != m_parent_socket_fd || -1 != m_child_socket_fd)
    {
        ::close(m_parent_socket_fd);
        ::close(m_child_socket_fd);
        m_parent_socket_fd = -1;
        m_child_socket_fd = -1;
        return true;
    }
    return true;
#endif
}

int SharedMemoryFIFOChannel::GetWriteFD()
{
    if (m_as_parent)
    {
#ifndef ARCH_HAS_EVENTFD
        return m_parent_socket_fd;
#else
        return m_parent_socket_fd;
#endif
    }
    else
    {
#ifndef ARCH_HAS_EVENTFD
        return m_child_socket_fd;
#else
        return m_child_socket_fd;
#endif
    }
}

int SharedMemoryFIFOChannel::GetReadFD()
{
    if (m_as_parent)
    {
#ifndef ARCH_HAS_EVENTFD
        return m_parent_socket_fd;
#else
        return m_child_socket_fd;
#endif
    }
    else
    {
#ifndef ARCH_HAS_EVENTFD
        return m_child_socket_fd;
#else
        return m_parent_socket_fd;
#endif
    }
}

void SharedMemoryFIFOChannel::Clone(SharedMemoryFIFOChannel* other)
{
    m_as_parent = other->m_as_parent;
    m_parent_socket_fd = other->m_parent_socket_fd;
    m_child_socket_fd = other->m_child_socket_fd;

    m_p2c_sem = other->m_p2c_sem;
    m_p2c_queue = other->m_p2c_queue;
    m_c2p_sem = other->m_c2p_sem;
    m_c2p_queue = other->m_c2p_queue;
    m_len_limit = other->m_len_limit;
}

size_t SharedMemoryFIFOChannel::GetFIFOSize()
{
    GetWriteSemphore()->Acquire();
    size_t len = GetWriteFIFO()->Size();
    GetWriteSemphore()->Release();
    return len;
}

bool SharedMemoryFIFOChannel::DoFlush()
{
    if (!m_outputBuffer.Readable())
    {
        return true;
    }
    uint32 total = m_outputBuffer.ReadableBytes();
    if (DoWriteBuffer(&m_outputBuffer) != total)
    {
        EnableWriting();
        return true;
    }
    return true;
}

PosixSemaphore* SharedMemoryFIFOChannel::GetWriteSemphore()
{
    if (m_as_parent)
    {
        return m_p2c_sem;
    }
    else
    {
        return m_c2p_sem;
    }
}
PosixSemaphore* SharedMemoryFIFOChannel::GetReadSemphore()
{
    if (m_as_parent)
    {
        return m_c2p_sem;
    }
    else
    {
        return m_p2c_sem;
    }
}

SharedMemoryFIFOChannel::SHMFIFO* SharedMemoryFIFOChannel::GetWriteFIFO()
{
    if (m_as_parent)
    {
        return m_p2c_queue;
    }
    else
    {
        return m_c2p_queue;
    }
}
SharedMemoryFIFOChannel::SHMFIFO* SharedMemoryFIFOChannel::GetReadFIFO()
{
    if (m_as_parent)
    {
        return m_c2p_queue;
    }
    else
    {
        return m_p2c_queue;
    }
}

uint32 SharedMemoryFIFOChannel::DoWriteBuffer(Buffer* buffer)
{
    uint32 left = 0;
    uint32 total = buffer->ReadableBytes();
    uint64 event_value = 1;
    while ((left = buffer->ReadableBytes()) > 0)
    {
        size_t current_fifo_size = GetFIFOSize();
        if (current_fifo_size >= m_len_limit)
        {
            ERROR_LOG("Current FIFO size(%u) exceed limit(%u).", current_fifo_size, m_len_limit);
            break;
        }
        SHMByteArray* array = shm_allocate_bytes(left);
        if (NULL == array)
        {
            ERROR_LOG("Failed to allocate memory from shm.");
            break;
        }

        if (!GetWriteSemphore()->TryAcquire())
        {
            shm_destroy_bytes(array);
            break;
        }
        if (!GetWriteFIFO()->Offer(array))
        {
            ERROR_LOG("Failed to offer array to fifo.");
            break;
        }
        array->Write(buffer);
        GetWriteSemphore()->Release();
    }
    if (::write(GetWriteFD(), &event_value, sizeof(event_value))
            != sizeof(event_value))
    {
        EnableWriting();
    }
    if (left > 0)
    {
        if (buffer != &m_outputBuffer)
        {
            m_outputBuffer.Write(buffer, left);
            left = 0;
        }
    }
    if (m_outputBuffer.Readable())
    {
        EnableWriting();
    }
    return total - left;
}

int32 SharedMemoryFIFOChannel::WriteNow(Buffer* buffer)
{
    uint32 buf_len = NULL != buffer ? buffer->ReadableBytes() : 0;
    if (m_outputBuffer.Readable())
    {
        m_outputBuffer.Write(buffer, buf_len);
        if (m_options.user_write_buffer_water_mark > 0
                && m_outputBuffer.ReadableBytes()
                        < m_options.user_write_buffer_water_mark)
        {
            CreateFlushTimerTask();
        }
        else
        {
            EnableWriting();
        }
        return buf_len;
    }
    else
    {
        if (buf_len < m_options.user_write_buffer_water_mark)
        {
            m_outputBuffer.Write(buffer, buf_len);
            CreateFlushTimerTask();
            return buf_len;
        }
        DoWriteBuffer(buffer);
        return buf_len;
    }
}

int32 SharedMemoryFIFOChannel::ReadNow(Buffer* buffer)
{
    if (NULL == buffer)
    {
        return 0;
    }
    uint64 event_value;
    uint32 total = 0;
    if (::read(GetReadFD(), &event_value, sizeof(event_value))
            != sizeof(event_value))
    {
        ERROR_LOG("Failed to read event notification.");
        return 0;
    }
    bool loop = true;
    while (loop)
    {
        if (!GetReadSemphore()->TryAcquire())
        {
            uint64 event_value = 1;
            ::write(GetReadFD(), &event_value, sizeof(event_value));
            //ERROR_LOG("Failed to ac!");
            loop = false;
            continue;
        }
        SHMByteArray* array = GetReadFIFO()->Take();
        if (NULL != array)
        {
            total += array->Read(buffer);
            shm_destroy_bytes(array);
        }
        else
        {
            loop = false;
        }
        GetReadSemphore()->Release();
    }

    //GetReadFIFO()->clear();

    return total;
}

void SharedMemoryFIFOChannel::SetProcessRole(bool is_parent)
{
    if (is_parent)
    {
#ifndef ARCH_HAS_EVENTFD
        close(m_child_socket_fd);
        m_child_socket_fd = -1;
        m_fd = m_parent_socket_fd;
#endif
        m_as_parent = true;
    }
    else
    {
#ifndef ARCH_HAS_EVENTFD
        close(m_parent_socket_fd);
        m_parent_socket_fd = -1;

        m_fd = m_child_socket_fd;
#endif
        m_as_parent = false;
    }
    make_fd_nonblocking(GetReadFD());
    aeCreateFileEvent(m_service.GetRawEventLoop(), GetReadFD(), AE_READABLE,
            Channel::IOEventCallback, this);
}

void SharedMemoryFIFOChannel::SetFIFOLengthLimit(uint32 limit)
{
    m_len_limit = limit;
}

SharedMemoryFIFOChannel::~SharedMemoryFIFOChannel()
{
    if (m_as_parent)
    {
        ERROR_LOG("Read FIFO size is %u.", GetReadFIFO()->Size());
        ERROR_LOG("Write FIFO size is %u.", GetWriteFIFO()->Size());
        SharedMemoryPool
                * pool =
                        SharedMemoryPoolHolder<IPC_FIFO_SHM_POOL>::GetSharedMemoryPool();
        GetWriteSemphore()->Acquire();
        //        SHMFIFO::iterator it1 = GetWriteFIFO()->begin();
        //        while (it1 != GetWriteFIFO()->end())
        //        {
        //            SHMByteArray* array = *it1;
        //            shm_destroy_bytes(array);
        //            it1++;
        //        }
        GetWriteFIFO()->Clear();
        GetWriteSemphore()->Release();
        GetReadSemphore()->Acquire();
        //        SHMFIFO::iterator it2 = GetReadFIFO()->begin();
        //        while (it2 != GetReadFIFO()->end())
        //        {
        //            SHMByteArray* array = *it2;
        //            shm_destroy_bytes(array);
        //            it2++;
        //        }
        GetReadFIFO()->Clear();
        GetReadSemphore()->Release();
        pool->Free(GetReadFIFO());
        pool->Free(GetWriteFIFO());
        pool->Free(m_p2c_sem);
        pool->Free(m_c2p_sem);
        //mm_free(pool, m_c2p_queue);
        //mm_free(pool, m_p2c_queue);
    }
}

