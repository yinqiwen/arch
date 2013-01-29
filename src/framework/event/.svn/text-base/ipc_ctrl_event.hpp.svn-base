/*
 * ipc_ctrl_event.hpp
 *
 *  Created on: 2011-9-6
 *      Author: qiyingwang
 */

#ifndef IPC_CTRL_EVENT_HPP_
#define IPC_CTRL_EVENT_HPP_
#include "common/base.hpp"
#include "buffer/buffer.hpp"
#include "framework/event/ipc_event.hpp"
#include <string>

using arch::buffer::Buffer;

#define DELEGATE_CHANNEL_CONTROL_REQUEST 0
#define DELEGATE_CHANNEL_CONTROL_RESPONSE 1
#define DELEGATE_CHANNEL_CONTROL_ERROR 2

#define GENERATE_DELEGATE_CHANNEL_CONTROL_EVENT(Type, FD) ((((uint64)Type)<<32) + FD)
#define RETRIVE_DELEGATE_FD(ev)  (ev&0xffffffff)
#define RETRIVE_DELEGATE_TYPE(ev) ((ev >> 32) &0xffffffff)

namespace arch
{
	namespace framework
	{
		enum IPCCtrlType
		{
			IPC_CTRL_INVALID = 0,
			SERV_PROC_STARTED = 1,
			SERV_PROC_STOPED = 2,
			TRANSMIT_CLIENT_SOCKET_FD = 3,
			TRANSMIT_CHANNEL_CONTROL = 4,
		};

		class IPCCtrlEvent: public IPCEvent
		{
			private:
				IPCCtrlType m_ctrl_type;
				ServiceType m_dst_type;
				ServiceIndex m_dst_idx;
				uint64 m_event_info;
				bool OnDecode(Buffer* buffer);
				bool OnEncode(Buffer* buffer);
				friend class IPCEventFactory;
			public:
				IPCCtrlEvent() :
						m_ctrl_type(IPC_CTRL_INVALID), m_dst_type(0), m_dst_idx(
						        0), m_event_info(0)
				{
				}
				inline void SetIPCCtrlType(IPCCtrlType type)
				{
					m_ctrl_type = type;
				}
				inline IPCCtrlType GetIPCCtrlType()
				{
					return m_ctrl_type;
				}
				inline void SetDstType(ServiceType type)
				{
					m_dst_type = type;
				}
				inline void SetDstIndex(ServiceIndex idx)
				{
					m_dst_idx = idx;
				}
				inline void SetEventInfo(uint64 info)
				{
					m_event_info = info;
				}
				inline ServiceType GetDstType()
				{
					return m_dst_type;
				}
				inline ServiceIndex GetDstIndex()
				{
					return m_dst_idx;
				}
				inline IPCEventType GetType()
				{
					return IPC_CTRL;
				}
				inline uint64 GetEventInfo()
				{
					return m_event_info;
				}
		};
	}
}

#endif /* IPC_CTRL_EVENT_HPP_ */
