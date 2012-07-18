/*
 * NetworkMessage.cpp
 *
 *  Created on: 2011-2-12
 *      Author: qiyingwang
 */

#include "framework/event/socket_message_event.hpp"
#include "channel/channel_buffer_pool.hpp"
#include "buffer/buffer_helper.hpp"
#include "logging/logger_macros.hpp"
using namespace arch::framework;
using namespace arch::buffer;
using namespace arch::channel;

bool SocketMessageEvent::OnDecode(Buffer* buffer)
{
	//DEBUG_LOG("before writelen1 buffer left %u\n", buffer->ReadableBytes());
	if (!BufferHelper::ReadFixUInt32(*buffer, m_channel_id))
	{
		return false;
	}
	char hasAddress;
	if (!buffer->ReadByte(hasAddress))
	{
		return false;
	}
	if (hasAddress)
	{
		char addrsize;
		if (!buffer->ReadByte(addrsize))
		{
			return false;
		}
		char temp[addrsize];
		if (buffer->Read(temp, addrsize))
		{
			struct sockaddr* addr = (struct sockaddr*)temp;
			NEW(m_address, SocketInetAddress(addr, addrsize));
			m_is_self_decode_address = true;
		}
		else
		{
			return false;
		}
	}
	uint32 content_len;
	if (!BufferHelper::ReadVarUInt32(*buffer, content_len))
	{
		return false;
	}
	if (content_len > 0)
	{
		NEW(m_content, Buffer(content_len));
		m_is_self_decode_buf = true;
	}
	if(0 == content_len)
	{
		return 0;
	}
	//DEBUG_LOG("before writelen2 buffer left %u\n",  buffer->ReadableBytes());
	uint32_t len = m_content->Write(buffer, content_len);
	//DEBUG_LOG("writelen is %d m_content_len is %d, buffer left %u", len, content_len, buffer->ReadableBytes());
	return len == content_len;
}

bool SocketMessageEvent::OnEncode(Buffer* buffer)
{
	RETURN_FALSE_IF_NULL(m_content);
	BufferHelper::WriteFixUInt32(*buffer, m_channel_id);
	buffer->WriteByte(NULL != m_address ? 1 : 0);
	if (NULL != m_address)
	{
		char size = (char)(m_address->GetRawSockAddrSize());
		buffer->WriteByte(size);
		buffer->Write(&(m_address->GetRawSockAddr()), size);
	}
	if (NULL != m_content)
	{
		BufferHelper::WriteVarUInt32(*buffer, m_content->ReadableBytes());
		buffer->Write(m_content, m_content->ReadableBytes());
	}
	else
	{
		BufferHelper::WriteVarUInt32(*buffer, 0);
	}
	return true;
}

SocketMessageEvent::~SocketMessageEvent()
{
	if (m_is_self_decode_buf)
	{
		DELETE(m_content);
		//ChannelBufferPool::Release(m_content);
	}
}
