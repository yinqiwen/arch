/*
 * FIFOChannel.hpp
 *
 *  Created on: 2011-1-28
 *      Author: wqy
 */

#ifndef SHM_FIFOCHANNEL_HPP_
#define SHM_FIFOCHANNEL_HPP_
#include "channel/channel.hpp"
#include "buffer/shm_byte_array.hpp"
#include "ipc/shm_allocator.hpp"
#include "concurrent/semaphore.hpp"
#include <list>

#if defined(__linux__)
# if !defined(ARCH_HAS_EVENTFD)
#  include <linux/version.h>
#  if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,22)
#include <sys/eventfd.h>
# define ARCH_HAS_EVENTFD
#endif // LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,22)
#endif // !defined(ARCH_HAS_EVENTFD)
#endif // defined(linux)
using std::list;
using arch::ipc::SharedMemoryAllocator;
using arch::ipc::SharedMemoryPool;
//using arch::ipc::IPC_MEM_POOL;
using arch::concurrent::PosixSemaphore;
using arch::buffer::SHMByteArray;
using arch::buffer::SHMByteArrayFIFO;
using arch::channel::ChannelOptions;

#define IPC_FIFO_SHM_POOL  0
namespace arch
{
    namespace channel
    {
        namespace fifo
        {
            class SharedMemoryFIFOChannel: public arch::channel::Channel
            {
                private:
                    bool m_as_parent;
                    int m_parent_socket_fd;
                    int m_child_socket_fd;
                    uint32 m_len_limit;

//                    typedef list<SHMByteArray*, SharedMemoryAllocator<
//                            SHMByteArray*, IPC_FIFO_SHM_POOL> > SHMFIFO;
                    typedef SHMByteArrayFIFO SHMFIFO;
                    /**
                     * parent-child queue & child-parent queue
                     */
                    PosixSemaphore* m_p2c_sem;
                    SHMFIFO* m_p2c_queue;
                    PosixSemaphore* m_c2p_sem;
                    SHMFIFO* m_c2p_queue;

                    int GetWriteFD();
                    int GetReadFD();
                    bool DoOpen();
                    bool DoBind(Address* local);
                    bool DoConnect(Address* remote);
                    bool DoClose();
                    //int GetWriteFD();
                    bool DoConfigure(const ChannelOptions& options);
                    uint32 DoWriteBuffer(Buffer* buffer);
                    int32 WriteNow(Buffer* buffer);
                    int32 ReadNow(Buffer* buffer);
                    bool DoFlush();
                    SHMFIFO* GetWriteFIFO();
                    SHMFIFO* GetReadFIFO();
                    PosixSemaphore* GetWriteSemphore();
                    PosixSemaphore* GetReadSemphore();
                public:
                    SharedMemoryFIFOChannel(ChannelService& factory);
                    void Clone(SharedMemoryFIFOChannel* other);
                    void SetProcessRole(bool is_parent);
                    void SetFIFOLengthLimit(uint32 limit);
                    size_t GetFIFOSize();
                    ~SharedMemoryFIFOChannel();
            };
        }
    }
}

#endif /* FIFOCHANNEL_HPP_ */
