/*
 * virtual_channel_manager.hpp
 *
 *  Created on: 2011-2-26
 *      Author: wqy
 */

#ifndef VIRTUAL_CHANNEL_HELPER_HPP_
#define VIRTUAL_CHANNEL_HELPER_HPP_
#include "buffer/buffer.hpp"
#include "channel/all_includes.hpp"
#include "net/socket_host_address.hpp"
#include "net/socket_inet_address.hpp"
#include "net/socket_unix_address.hpp"
#include "framework/framework.hpp"
#include <map>
#include <tr1/unordered_map>
using arch::net::SocketHostAddress;
using arch::net::SocketInetAddress;
using arch::net::SocketUnixAddress;
using arch::buffer::Buffer;
using arch::channel::Channel;
namespace arch
{
	namespace framework
	{
		struct ChannelAndSocketID
		{
				uint32 transport_channel_ID;
				uint32 real_socket_ID;
		};
		typedef std::tr1::unordered_map<uint32, std::map<uint32, uint32> > TransportChannelIDTable;
		typedef std::tr1::unordered_map<uint32, ChannelAndSocketID> VirtualSocketIDTable;

		class ServiceIPCEventHandler;
		class ManagerIPCEventHandler;
		class VirtualChannelHelper
		{
			private:
				static TransportChannelIDTable m_transport_table;
				static VirtualSocketIDTable m_virtual_id_table;
				static int GetVirtualSocketID(Channel* src_ch, uint32 realID,
				        uint32& virtual_id);
				static int GetChannelAndScoketID(uint32 virtual_sock_id,
				        Channel*& dst_ch, uint32& realID);
				static void ClearTable(Channel* src_ch);
				static void ClearTable(Channel* src_ch, uint32 realID);
				static Channel* GetIPCChannel();
				static void CtrlChannelIOEventCallback(
				        struct aeEventLoop *eventLoop, int fd, void *clientData,
				        int mask);
				friend class ServiceIPCEventHandler;
				friend class ManagerIPCEventHandler;
			public:

				static int Init();
				/**
				 *  发送open socket请求到manager进程
				 */
				static int SocketOpenRequest(bool is_tcp = true, uint64 addon = 0);
				static int SocketConnectRequest(const SocketHostAddress& remote,
				        bool is_tcp = true,  uint64 addon = 0);
				static int SocketConnectRequest(uint32 socket_channel_id,
				        const SocketHostAddress& remote, uint64 addon = 0);
				static int SocketConnectRequest(
				        const SocketUnixAddress& remote, uint64 addon = 0);

				/**
				 *  通知manager进程发送网络消息
				 */
				static int WriteSocket(uint32 socket_channel_id,
				        Buffer& content);
				static int WriteSocket(uint32 socket_channel_id,
				        Buffer& content, const SocketInetAddress& remote);
				static int CloseSocket(uint32 socket_channel_id, uint64 addon = 0);

				/**
				 *  发送IPC消息event到指定进程
				 */
				static int WriteIPCMessage(ServiceType service_type,
				        ServiceIndex index, Buffer& content);
				static int SendClientSocketChannelFD(ServiceType service_type,
				        ServiceIndex index, ClientSocketChannel* ch);

		};
	}
}

#endif /* VIRTUAL_CHANNEL_MANAGER_HPP_ */
