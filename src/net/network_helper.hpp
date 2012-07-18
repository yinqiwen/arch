/*
 * NetworkHelper.hpp
 *
 *  Created on: 2011-1-8
 *      Author: wqy
 */

#ifndef NOVA_NETWORKHELPER_HPP_
#define NOVA_NETWORKHELPER_HPP_
#include "net/socket_host_address.hpp"
#include "net/socket_inet_address.hpp"
#include "net/socket_unix_address.hpp"
#include "net/mac_address.hpp"
#include <list>
#include <vector>

using std::list;

namespace arch
{
	namespace net
	{
		SocketInetAddress getInetAddress(const string& host, uint16 port);
		SocketInetAddress getInetAddress(const SocketHostAddress& addr);
		SocketInetAddress getInetAddress(const SocketUnixAddress& addr);
		SocketHostAddress getHostAddress(const sockaddr& addr);
		SocketHostAddress getHostAddress(const SocketInetAddress& addr);
		SocketUnixAddress getUnixAddress(const sockaddr& addr);
		SocketUnixAddress getUnixAddress(const SocketInetAddress& addr);

		SocketInetAddress
		getSocketInetAddress(int32 fd);
		SocketHostAddress
		getHostAddress(int32 fd);
		SocketInetAddress
		getRemoteInetAddress(int32 fd);
		SocketHostAddress
		getRemoteHostAddress(int32 fd);

		uint64 ntohll(uint64 v);
		uint64 htonll(uint64 v);

		MACAddress getMACAddress(const std::string& nicName);
		list<MACAddress> getAllMACAddresses();

		int getIPByNICName(const std::string& ifName, std::string& ip);
		int getLocalHostIPList(std::vector<std::string>& iplist);
	}
}

#endif /* NETWORKHELPER_HPP_ */
