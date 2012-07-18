/*
 * virtual_client_connection_manager.cpp
 *
 *  Created on: 2011-8-7
 *      Author: wqy
 */
#include "framework/vcs/virtual_client_connection_manager.hpp"
#include "framework/vcs/virtual_channel_helper.hpp"

using namespace arch::framework;

void VirtualClientConnectionManager::CloseRemote(
        const SocketHostAddress& address)
{
	ClientSocketIDTable::iterator found = m_client_socket_table.find(address);
	if (found != m_client_socket_table.end())
	{
		VirtualChannelHelper::CloseSocket(found->second);
		m_client_socket_table.erase(found);
	}
}

void VirtualClientConnectionManager::ConnectRemote(
        const SocketHostAddress& address)
{
	ConnectingAddressSet::iterator found = m_connecting_set.find(address);
	if (found != m_connecting_set.end())
	{
	    DEBUG_LOG("Duplicate connecting address while connecting set size:%u", m_connecting_set.size());
		return;
	}
	m_connecting_set.insert(address);
	VirtualChannelHelper::SocketConnectRequest(address, true);
}

void VirtualClientConnectionManager::SocketConnectFailed(uint32 sock_id,
        const SocketHostAddress& address)
{
	ConnectingAddressSet::iterator found = m_connecting_set.find(address);
	if (found != m_connecting_set.end())
	{
		m_connecting_set.erase(found);
	}
	DEBUG_LOG("VirtualClientConnectionManager::SocketConnectFailed connection set size:%u", m_connecting_set.size());
}

void VirtualClientConnectionManager::SocketClosed(uint32 sock_id)
{
	ClientSocketIDTable::iterator it = m_client_socket_table.begin();
	while (it != m_client_socket_table.end())
	{
		if (it->second == sock_id)
		{
			m_client_socket_table.erase(it);
			break;
		}
		it++;
	}
}
void VirtualClientConnectionManager::SocketConnected(uint32 sock_id,
        const SocketHostAddress& address)
{
    DEBUG_LOG("VirtualClientConnectionManager::SocketConnected(%u)  with address %s:%u", sock_id, address.GetHost().c_str(), address.GetPort());
	ClientSocketIDTable::iterator found = m_client_socket_table.find(address);
	if (found != m_client_socket_table.end() && found->second != sock_id)
	{
        //Close duplicate connection, use new socket instead
	    VirtualChannelHelper::CloseSocket(found->second);
	    m_client_socket_table.erase(found);
	}
	m_client_socket_table[address] = sock_id;

	ConnectingAddressSet::iterator found_it = m_connecting_set.find(address);
	if (found_it != m_connecting_set.end())
	{
		m_connecting_set.erase(found_it);
	}
	//DEBUG_LOG("VirtualClientConnectionManager::SocketConnected connection set size:%u", m_connecting_set.size());
}

bool VirtualClientConnectionManager::GetConnectedSocketID(
        const SocketHostAddress& address, uint32& id)
{
	ClientSocketIDTable::iterator found = m_client_socket_table.find(address);
	if (found == m_client_socket_table.end())
	{
		return false;
	}
	else
	{
		id = found->second;
		return true;;
	}
}
