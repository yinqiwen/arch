/*
 * socket_message_event.hpp
 *
 *  Created on: 2011-2-25
 *      Author: qiyingwang
 */

#ifndef SOCKET_MESSAGE_EVENT_HPP_
#define SOCKET_MESSAGE_EVENT_HPP_

#include "common/base.hpp"
#include "buffer/buffer.hpp"
#include "framework/event/ipc_event.hpp"
#include "net/socket_inet_address.hpp"

using arch::buffer::Buffer;
using arch::net::SocketInetAddress;

namespace arch
{
	namespace framework
	{
		class SocketMessageEvent: public IPCEvent
		{
			private:
				uint32 m_channel_id;
				SocketInetAddress* m_address;
				Buffer* m_content;
				bool m_is_self_decode_buf;
				bool m_is_self_decode_address;
				bool OnDecode(Buffer* buffer);
				bool OnEncode(Buffer* buffer);

				friend class IPCEventFactory;
			public:
				SocketMessageEvent() :
					m_channel_id(0), m_address(NULL), m_content(NULL),
					        m_is_self_decode_buf(false),
					        m_is_self_decode_address(false)
				{
				}
				void SetChannelID(uint32 id)
				{
					m_channel_id = id;
				}
				uint32 GetChannelID() const
				{
					return m_channel_id;
				}
				Buffer* GetContent()
				{
					return m_content;
				}
				void SetContent(Buffer* content)
				{
					m_content = content;
					m_is_self_decode_buf = false;
				}
				void SetSocketInetAddress(SocketInetAddress* addr)
				{
					m_address = addr;
					m_is_self_decode_address = false;
				}
				SocketInetAddress* GetSocketInetAddress()
				{
					return m_address;
				}
				virtual IPCEventType GetType()
				{
					return SOCKET_MSG;
				}
				~SocketMessageEvent();
		};
	}
}
#endif /* SOCKET_MESSAGE_EVENT_HPP_ */
