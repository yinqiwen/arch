/*
 * shm_pool.hpp
 *
 *  Created on: 2011-4-7
 *      Author: qiyingwang
 */

#ifndef SHM_POOL_HPP_
#define SHM_POOL_HPP_
#include "ipc/shm.hpp"
#include "mem/shm_malloc.h"
#include "mem/mm/mm.h"
#include <map>
namespace arch
{
    namespace ipc
    {
        struct SharedMemoryPool
        {
                virtual void* Malloc(uint32 size) = 0;
                virtual void Free(void* addr) = 0;
                virtual void* Realloc(void* p, uint32 size) = 0;
                virtual uint32 Avaliable() = 0;
                virtual ~SharedMemoryPool()
                {
                }
        };

        class MMSharedMemoryPool: public SharedMemoryPool
        {
            private:
                MM* m_raw_pool;
                bool m_managed_self;
            public:
                MMSharedMemoryPool(MM* mm, bool delegate = true) :
                    m_raw_pool(mm), m_managed_self(delegate)
                {
                }
                inline MM* GetRawPool()
                {
                    return m_raw_pool;
                }
                inline void* Malloc(uint32 size)
                {
                    return mm_malloc(m_raw_pool, size);
                }
                inline void* Realloc(void* p, uint32 size)
                {
                    return mm_realloc(m_raw_pool, p, size);
                }
                inline void Free(void* addr)
                {
                    mm_free(m_raw_pool, addr);
                }
                inline uint32 Avaliable()
                {
                    return mm_available(m_raw_pool);
                }
                inline ~MMSharedMemoryPool()
                {
                    if (m_managed_self)
                    {
                        mm_destroy(m_raw_pool);
                    }
                    //shm_malloc_destroy( m_mem_block);
                }
        };

        class FastSharedMemoryPool: public SharedMemoryPool
        {
            private:
                struct shm_block* m_mem_block;
                SharedMemory* m_raw_pool;
                bool m_managed_self;
            public:
                inline FastSharedMemoryPool(SharedMemory* pool, bool delegate =
                        true) :
                    m_mem_block(NULL), m_raw_pool(pool), m_managed_self(
                            delegate)
                {
                    m_mem_block = shm_malloc_init(pool->GetRawAddress(),
                            pool->GetMemorySize());
                }
                inline SharedMemory* GetRawPool()
                {
                    return m_raw_pool;
                }
                inline void* Malloc(uint32 size)
                {
                    return shm_malloc(m_mem_block, size);
                }
                inline void* Realloc(void* p, uint32 size)
                {
                    return shm_realloc(m_mem_block, p, size);
                }
                inline void Free(void* addr)
                {
                    shm_free(m_mem_block, addr);
                }
                inline uint32 Avaliable()
                {
                    return (uint32) (-1);
                }
                inline ~FastSharedMemoryPool()
                {
                    shm_malloc_destroy(m_mem_block);
                    if (m_managed_self)
                    {
                        delete m_raw_pool;
                    }
                }
        };

        template<uint32 type>
        class SharedMemoryPoolHolder
        {
            private:
                static SharedMemoryPool* m_raw;
            public:
                static SharedMemoryPool* GetSharedMemoryPool()
                {
                    return m_raw;
                }
                static void DestoryRawMemoryPool()
                {
                    if (NULL != m_raw)
                    {
                        delete m_raw;
                    }
                }
                static bool SetSharedMemoryPool(SharedMemoryPool* pool)
                {
                    RETURN_FALSE_IF_NULL(pool);
                    if (NULL != m_raw)
                    {
                        return false;
                    }
                    m_raw = pool;
                    return true;
                }
        };

        template<uint32 type>
        SharedMemoryPool* SharedMemoryPoolHolder<type>::m_raw = NULL;
    }
}

#endif /* SHM_POOL_HPP_ */
