/*
 * BufferFactory.cpp
 *
 *  Created on: 2011-1-26
 *      Author: wqy
 */
#include "buffer/buffer_factory.hpp"
#include <algorithm>

using namespace arch::buffer;
using std::lower_bound;

static bool LessBufferCapacityCompare(const Buffer* buf1,
        uint32_t buf2Capacity)
{
	return buf1->Capacity() < buf2Capacity;
}

BufferFactory::BufferFactory()
{
	//uint32_t k = upperPowerOfTwo(maxSize);
	//m_maxbufsize = k;
}

Buffer* BufferFactory::Acquire(uint32 size)
{
	if (m_buffer_table.empty())
	{
		Buffer* buf = NULL;
		NEW(buf, Buffer(size));
		return buf;
	}
	deque<Buffer*>::iterator it = lower_bound(m_buffer_table.begin(),
	        m_buffer_table.end(), size, LessBufferCapacityCompare);
	if(it != m_buffer_table.end())
	{
		Buffer* buf = *it;
		m_buffer_table.erase(it);
		return buf;
	}
	else
	{
		Buffer* buf = NULL;
		NEW(buf, Buffer(size));
		return buf;
	}

}

void BufferFactory::Release(Buffer* buf)
{
	RETURN_IF_NULL(buf);
	uint32_t capacity = buf->Capacity();
	if(capacity == 0)
	{
	    DELETE(buf);
	    return;
	}
	deque<Buffer*>::iterator it = lower_bound(m_buffer_table.begin(),
	        m_buffer_table.end(), capacity, LessBufferCapacityCompare);
	buf->Clear();
	m_buffer_table.insert(it, buf);
}

BufferFactory::~BufferFactory()
{
	deque<Buffer*>::iterator it = m_buffer_table.begin();
	while (it != m_buffer_table.end())
	{
		Buffer* buf = *it;
		DELETE(buf);
		it++;
	}
}
