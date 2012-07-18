/*
 * ipc_ctrl_event.hpp
 *
 *  Created on: 2011-2-24
 *      Author: qiyingwang
 */

#ifndef SOCKET_COMMAND_EVENT_HPP_
#define SOCKET_COMMAND_EVENT_HPP_
#include "common/base.hpp"
#include "buffer/buffer.hpp"
#include "framework/event/ipc_event.hpp"
#include "net/socket_host_address.hpp"
#include "net/socket_unix_address.hpp"

using arch::buffer::Buffer;
using arch::net::SocketHostAddress;
using arch::net::SocketUnixAddress;

namespace arch
{
	namespace framework
	{
		enum SocketCtrlType
		{
			SOCKET_OPEN,
			SOCKET_OPENED,
			SOCKET_CONNECT,
			SOCKET_CLOSE,
			SOCKET_CONNECTED,
			SOCKET_CLOSED,
			SOCKET_CONNECT_FAILED
		};

		class SocketCtrlEvent: public IPCEvent
		{
			private:
				SocketCtrlType m_ctrl_type;
				uint32_t m_socket_channel_id;
				SocketHostAddress* m_address;
				SocketUnixAddress* m_unix_address;
				bool m_is_self_decode_address;
				bool OnDecode(Buffer* buffer);
				bool OnEncode(Buffer* buffer);
				friend class IPCEventFactory;
			public:
				SocketCtrlEvent() :
					m_ctrl_type(SOCKET_CONNECT), m_socket_channel_id(0),
					        m_address(NULL), m_unix_address(NULL),m_is_self_decode_address(false)
				{
				}
				inline void SetSocketCtrlType(SocketCtrlType type)
				{
					m_ctrl_type = type;
				}
				inline SocketCtrlType GetSocketCtrlType()
				{
					return m_ctrl_type;
				}
				inline uint32 GetSocketChannelID()
				{
					return m_socket_channel_id;
				}
				inline void SetSocketChannelID(uint32_t id)
				{
					m_socket_channel_id = id;
				}
				inline IPCEventType GetType()
				{
					return SOCKET_CTRL;
				}
				inline const SocketHostAddress* GetAddress()
				{
					return m_address;
				}
				inline const SocketUnixAddress* GetUnixAddress()
				{
					return m_unix_address;
				}
				inline void SetAddress(SocketHostAddress* addr)
				{
					m_address = addr;
					m_is_self_decode_address = false;
				}

				inline void SetUnixAddress(SocketUnixAddress* addr)
				{
					m_unix_address = addr;
					m_is_self_decode_address = false;
				}

				~SocketCtrlEvent();
		};
	}
}

#endif /* IPC_CTRL_EVENT_HPP_ */
