/*
 * shm.hpp
 *
 *  Created on: 2011-4-5
 *      Author: wqy
 */

#ifndef ARCH_SHM_HPP_
#define ARCH_SHM_HPP_
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>
#include "common/base.hpp"
#include "exception/api_exception.hpp"

namespace arch
{
    namespace ipc
    {
        struct SharedMemory
        {
                virtual void* GetRawAddress() = 0;
                virtual uint32 GetMemorySize() = 0;
                virtual ~SharedMemory()
                {
                }
        };

        class SystemVSharedMemory: public SharedMemory
        {
            private:
                bool m_is_self_created;
                key_t m_key;
                void* m_attached_addr;
                uint32 m_mem_size;
            public:
                SystemVSharedMemory(uint32 size);
                SystemVSharedMemory(key_t* key);
                key_t GetRawKey();
                void* GetRawAddress();
                uint32 GetMemorySize();
                ~SystemVSharedMemory();
        };

        class PosixSharedMemory: public SharedMemory
        {
            private:
                std::string m_name;
                void* m_attached_addr;
                uint32 m_mem_size;
            public:
                PosixSharedMemory(const std::string& name, uint32 size,
                        bool rm_exist = false);
                const std::string& GetName();
                void* GetRawAddress();
                uint32 GetMemorySize();
                ~PosixSharedMemory();
        };

    }
}

#endif /* SHM_HPP_ */
