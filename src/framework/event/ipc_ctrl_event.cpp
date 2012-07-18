/*
 * ipc_ctrl_event.cpp
 *
 *  Created on: 2011-9-6
 *      Author: qiyingwang
 */
#include "framework/event/ipc_ctrl_event.hpp"
#include "buffer/buffer_helper.hpp"
#include "channel/channel_buffer_pool.hpp"
using namespace arch::framework;
using namespace arch::buffer;
using namespace arch::channel;

bool IPCCtrlEvent::OnDecode(Buffer* buffer)
{
	uint32 type;
	if (!BufferHelper::ReadFixUInt32(*buffer, type))
	{
		return false;
	}
	m_ctrl_type = (IPCCtrlType) type;
	if (buffer->Read(&m_dst_type, sizeof(m_dst_type))
	        && buffer->Read(&m_dst_idx, sizeof(m_dst_idx)))
	{
	}
	else
	{
		return false;
	}
	if (!BufferHelper::ReadFixUInt64(*buffer, m_event_info))
	{
		return false;
	}
	return true;
}

bool IPCCtrlEvent::OnEncode(Buffer* buffer)
{
	uint32 type = (uint32) m_ctrl_type;
	BufferHelper::WriteFixUInt32(*buffer, type);
	buffer->Write(&m_dst_type, sizeof(m_dst_type));
	buffer->Write(&m_dst_idx, sizeof(m_dst_idx));
	BufferHelper::WriteFixUInt64(*buffer, m_event_info);
	return true;
}
