/*
 * shm_byte_array.hpp
 *
 *  Created on: 2011-3-24
 *      Author: qiyingwang
 */

#ifndef SHM_BYTE_ARRAY_HPP_
#define SHM_BYTE_ARRAY_HPP_
#include "buffer/buffer.hpp"
#include "ipc/shm_pool.hpp"
using arch::ipc::SharedMemoryPool;
namespace arch
{
    namespace buffer
    {
        /**
         * 共享内存中的char数组
         */
        class SHMByteArray
        {
            private:
                char* m_content;
                size_t m_len;
                SharedMemoryPool* m_pool;
            public:
                SHMByteArray(SharedMemoryPool* mm, size_t size);
                uint32 Read(Buffer* buf);
                uint32 Write(Buffer* buf);
                ~SHMByteArray();
        };

        struct SHMByteArrayNode
        {
                SHMByteArray* array;
                SHMByteArrayNode* next;
        };

        /**
         * 共享内存中的char数组FIFO
         */
        struct SHMByteArrayFIFO
        {
                SharedMemoryPool* m_pool;
                SHMByteArrayNode* head;
                SHMByteArrayNode* tail;
                size_t m_size;
                SHMByteArrayFIFO(SharedMemoryPool* mm) :
                    m_pool(mm), head(NULL), tail(NULL), m_size(0)
                {
                }
                SHMByteArray* Take();
                bool Offer(SHMByteArray* array);
                void Clear();
                size_t Size()
                {
                    return m_size;
                }
                ~SHMByteArrayFIFO();
        };
    }
}

#endif /* SHM_BYTE_ARRAY_HPP_ */
