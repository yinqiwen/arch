/*
 * NetworkMessage.cpp
 *
 *  Created on: 2011-2-12
 *      Author: qiyingwang
 */

#include "channel/channel_buffer_pool.hpp"
#include "framework/event/ipc_message_event.hpp"
#include "buffer/buffer_helper.hpp"
#include "logging/logger_macros.hpp"
using namespace arch::framework;
using namespace arch::buffer;
using namespace arch::channel;

bool IPCMessageEvent::OnDecode(Buffer* buffer)
{
    if (buffer->Read(&m_dst_type, sizeof(m_dst_type)) && buffer->Read(
            &m_dst_idx, sizeof(m_dst_idx)))
    {
    }
    else
    {
        return false;
    }
    uint32 content_len;
    if (!BufferHelper::ReadVarUInt32(*buffer, content_len))
    {
        return false;
    }
    if (NULL == m_content)
    {
        NEW(m_content, Buffer(content_len));
        m_is_self_decode_buf = true;
    }
    uint32 len = m_content->Write(buffer, content_len);
    //DEBUG_LOG("writelen is %d m_content_len is %d\n", len, content_len);
    return len == content_len;
}

bool IPCMessageEvent::OnEncode(Buffer* buffer)
{
    RETURN_FALSE_IF_NULL(m_content);
    buffer->Write(&m_dst_type, sizeof(m_dst_type));
    buffer->Write(&m_dst_idx, sizeof(m_dst_idx));
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

IPCMessageEvent::~IPCMessageEvent()
{
    if (m_is_self_decode_buf)
    {
        DELETE(m_content);
    }
}
