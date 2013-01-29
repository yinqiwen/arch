/*
 * ChannelBufferPool.hpp
 *
 *  Created on: 2011-1-26
 *      Author: wqy
 */

#ifndef NOVA_CHANNELBUFFERPOOL_HPP_
#define NOVA_CHANNELBUFFERPOOL_HPP_
#include "buffer/buffer_factory.hpp"
using arch::buffer::BufferFactory;
using arch::buffer::Buffer;

namespace arch
{
	namespace channel
	{
		class ChannelBufferPool
		{
			private:
				static BufferFactory g_buffer_factory;
			public:
				static Buffer* Acquire(uint32 size);
				static void Release(Buffer* buf);
		};
	}
}

#endif /* CHANNELBUFFERPOOL_HPP_ */
