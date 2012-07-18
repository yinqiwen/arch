/*
 * NetworkMessage.cpp
 *
 *  Created on: 2011-2-12
 *      Author: qiyingwang
 */

#include "framework/event/socket_ctrl_event.hpp"
#include "buffer/buffer_helper.hpp"
using namespace arch::framework;
using namespace arch::buffer;

bool SocketCtrlEvent::OnDecode(Buffer* buffer)
{
    char ctrl_type_char;
    if (!buffer->ReadByte(ctrl_type_char))
    {
        return false;
    }
    m_ctrl_type = (SocketCtrlType) ctrl_type_char;
    if (!BufferHelper::ReadFixUInt32(*buffer, m_socket_channel_id))
    {
        return false;
    }
    uint8 address_type;
    if (!buffer->Read(&address_type,1))
    {
        return false;
    }
    switch (address_type)
    {
        case 1:
        {
            string host;
            uint16 port;
            if (BufferHelper::ReadVarString(*buffer, host)
                    && BufferHelper::ReadFixUInt16(*buffer, port))
            {
                NEW(m_address, SocketHostAddress(host, port));
                m_is_self_decode_address = true;
            }
            else
            {
                return false;
            }
            break;
        }
        case 2:
        {
            string path;
            if (BufferHelper::ReadVarString(*buffer, path))
            {
                NEW(m_unix_address, SocketUnixAddress(path));
                m_is_self_decode_address = true;
            }
            else
            {
                return false;
            }
            break;
        }
        default:
        {
            break;
        }
    }
    return true;
}

bool SocketCtrlEvent::OnEncode(Buffer* buffer)
{
    RETURN_FALSE_IF_NULL(buffer);
    buffer->WriteByte((char) m_ctrl_type);
    BufferHelper::WriteFixUInt32(*buffer, m_socket_channel_id);
    uint8 address_type = 0;
    if (NULL != m_address)
    {
        address_type = 1;
    }
    else if (NULL != m_unix_address)
    {
        address_type = 2;
    }
    buffer->Write(&address_type, 1);
    if (NULL != m_address)
    {
        BufferHelper::WriteVarString(*buffer, m_address->GetHost());
        BufferHelper::WriteFixUInt16(*buffer, m_address->GetPort());
    }
    else if(NULL != m_unix_address)
    {
        BufferHelper::WriteVarString(*buffer, m_unix_address->GetPath());
    }
    return true;
}

SocketCtrlEvent::~SocketCtrlEvent()
{
    if (m_is_self_decode_address)
    {
        DELETE(m_address);
        DELETE(m_unix_address);
    }
}
