/*
 * ChannelBufferPool.cpp
 *
 *  Created on: 2011-1-26
 *      Author: wqy
 */
#include "channel/all_includes.hpp"

using namespace arch::channel;

BufferFactory ChannelBufferPool::g_buffer_factory;

Buffer* ChannelBufferPool::Acquire(uint32 size)
{
	return g_buffer_factory.Acquire(size);
}

void ChannelBufferPool::Release(Buffer* buf)
{
	g_buffer_factory.Release(buf);
}

