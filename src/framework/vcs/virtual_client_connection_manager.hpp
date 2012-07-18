/*
 * virtual_client_connection_manager.hpp
 *
 *  Created on: 2011-8-7
 *      Author: wqy
 */

#ifndef VIRTUAL_CLIENT_CONNECTION_MANAGER_HPP_
#define VIRTUAL_CLIENT_CONNECTION_MANAGER_HPP_
#include "net/socket_host_address.hpp"
#include <map>
#include <set>
using arch::net::SocketHostAddress;
namespace arch
{
	namespace framework
	{
		class VirtualClientConnectionManager
		{
			protected:
				typedef std::map<SocketHostAddress, uint32> ClientSocketIDTable;
				typedef std::set<SocketHostAddress> ConnectingAddressSet;
				ClientSocketIDTable m_client_socket_table;
				ConnectingAddressSet m_connecting_set;

			public:
				bool GetConnectedSocketID(const SocketHostAddress& address,
				        uint32& id);
				void ConnectRemote(const SocketHostAddress& address);
				void CloseRemote(const SocketHostAddress& address);
				void SocketClosed(uint32 sock_id);
				void SocketConnected(uint32 sock_id,
				        const SocketHostAddress& address);
				void SocketConnectFailed(uint32 sock_id,
				        const SocketHostAddress& address);
				virtual ~VirtualClientConnectionManager()
				{
				}
		};
	}
}

#endif /* VIRTUAL_CLIENT_CONNECTION_MANAGER_HPP_ */
