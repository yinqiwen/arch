/*
 * SocketAddress.cpp
 *
 *  Created on: 2011-1-9
 *      Author: wqy
 */
#include "net/network_helper.hpp"
#include "net/socket_address.hpp"

using namespace arch::net;

SocketAddress::SocketAddress(const string& host, uint32 port):SocketHostAddress(host,port),m_inet_addr(getInetAddress(host,port))
{

}
