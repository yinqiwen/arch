/*
 * SocketChannel.hpp
 *
 *  Created on: 2011-1-8
 *      Author: wqy
 */

#ifndef NOVA_SOCKETCHANNEL_HPP_
#define NOVA_SOCKETCHANNEL_HPP_
#include "channel/channel.hpp"
#include "net/socket_address.hpp"

using arch::net::SocketHostAddress;
using arch::net::SocketInetAddress;
using arch::channel::ChannelOptions;
namespace arch
{
	namespace channel
	{
		namespace socket
		{
			enum SocketChannelState
			{
				INVALID, INIT, CONNECTING, CONNECTED, CLOSED
			};

			enum SocketProtocolType
			{
				IPV4, IPV6, UNIX
			};

			class SocketChannel: public arch::channel::Channel
			{
				protected:
					Address* m_localAddr;
					Address* m_remoteAddr;
					SocketChannelState m_state;
					const Address* GetLocalAddress();
					const Address* GetRemoteAddress();
					void setRemoteAddress(Address* addr);
					virtual int GetProtocol()
					{
						//default TCP
						return SOCK_STREAM;
					}
					int GetSocketFD(int domain);
					virtual bool DoOpen();
					virtual bool DoBind(Address* local);
					virtual bool DoConnect(Address* remote);
					virtual bool DoClose();
					virtual bool DoConfigure(const ChannelOptions& options);
					virtual int32 HandleExceptionEvent(int32 event);
					void OnWrite();
				public:
					SocketChannel(ChannelService& factory) :
						Channel(NULL, factory), m_localAddr(NULL),
						        m_remoteAddr(NULL), m_state(INIT)
					{
					}
					virtual ~SocketChannel();
			};
		}

	}
}

#endif /* SOCKETCHANNEL_HPP_ */
