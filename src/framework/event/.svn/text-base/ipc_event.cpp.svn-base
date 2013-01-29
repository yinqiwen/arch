/*
 * NetworkMessage.cpp
 *
 *  Created on: 2011-2-12
 *      Author: qiyingwang
 */

#include "framework/event/ipc_event.hpp"
#include "buffer/buffer_helper.hpp"
#include "channel/channel_buffer_pool.hpp"
using namespace arch::framework;
using namespace arch::buffer;
using namespace arch::channel;

bool IPCEvent::Encode(Buffer* buffer)
{
    RETURN_FALSE_IF_NULL(buffer);
    uint32 writeIndex = buffer->GetWriteIndex();
    BufferHelper::WriteFixUInt32(*buffer, 1);
    buffer->WriteByte((char) (GetType()));
    buffer->Write(&m_src_type, sizeof(m_src_type));
    buffer->Write(&m_src_idx, sizeof(m_src_idx));
    if (!OnEncode(buffer))
    {
        buffer->SetWriteIndex(writeIndex);
        return false;
    }
    else
    {
        uint32 curWriteIndex = buffer->GetWriteIndex();
        uint32 len = curWriteIndex - writeIndex;
        buffer->SetWriteIndex(writeIndex);
        BufferHelper::WriteFixUInt32(*buffer, len);
        buffer->SetWriteIndex(curWriteIndex);
        return true;
    }
}

bool IPCEvent::Decode(Buffer* buffer)
{
    RETURN_FALSE_IF_NULL(buffer);
    if (buffer->Read(&m_src_type, sizeof(m_src_type)) && buffer->Read(
            &m_src_idx, sizeof(m_src_idx)))
    {
        return OnDecode(buffer);
    }
    return false;

}
