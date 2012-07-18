/*
 * datagram_packet.cpp
 *
 *  Created on: 2011-3-4
 *      Author: qiyingwang
 */
#include "net/datagram_packet.hpp"

using namespace arch::net;

DatagramPacket::DatagramPacket(uint32 size):m_buffer(NULL), m_is_buffer_self(true)
{
    NEW(m_buffer, arch::buffer::Buffer(size));
}
DatagramPacket::DatagramPacket(arch::buffer::Buffer* buffer, const SocketInetAddress& addr):m_addr(addr), m_buffer(buffer), m_is_buffer_self(false)
{

}

DatagramPacket::~DatagramPacket()
{
    if(m_is_buffer_self)
    {
        DELETE(m_buffer);
    }
}
