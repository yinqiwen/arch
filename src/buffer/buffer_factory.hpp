/*
 * ByteBufferFactory.hpp
 * 按照slab内存模型管理的Buffer工厂类
 *  Created on: 2011-1-26
 *      Author: wqy
 */

#ifndef NOVA_BYTEBUFFERFACTORY_HPP_
#define NOVA_BYTEBUFFERFACTORY_HPP_
#include "common/base.hpp"
#include "buffer/buffer.hpp"
#include <map>
#include <deque>

using std::map;
using std::deque;
namespace arch
{
    namespace buffer
    {
        /**
         * 按照slab内存模型管理的Buffer工厂
         */
        class BufferFactory
        {
            private:
                deque<Buffer*> m_buffer_table;
            public:
                BufferFactory();
                /**
                 * 获取指定长度的Buffer，返回buffer长度可能超过指定size
                 */
                Buffer* Acquire(uint32 size);
                /**
                 * 回收Buffer
                 */
                void Release(Buffer* buf);
                ~BufferFactory();
        };
    }
}

#endif /* BYTEBUFFERFACTORY_HPP_ */
