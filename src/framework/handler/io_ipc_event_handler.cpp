/*
 * InternalMessageDispatchHandler.cpp
 *
 *  Created on: 2011-2-6
 *      Author: wqy
 */
#include "framework/process/manager_process.hpp"
#include "framework/handler/io_ipc_event_handler.hpp"
#include "framework/vcs/virtual_channel_helper.hpp"
#include "channel/channel.hpp"

#include "net/datagram_packet.hpp"
#include "logging/logger_macros.hpp"

using namespace arch::framework;
using namespace arch::channel;
using namespace arch::channel::fifo;
using namespace arch::net;
using arch::exception::Exception;

IOIPCEventHandler::IOIPCEventHandler() :
		m_handler(NULL), m_type(0), m_idx(0), m_channel_service(NULL)
{
}

int IOIPCEventHandler::Init()
{
	m_type = GetServiceProcessType();
	m_idx = GetServiceProcessIndex();
	m_channel_service = GetChannelService();
	return 0;
}

void IOIPCEventHandler::SetIOProcessHandler(IOProcessHandler* handler)
{
	m_handler = handler;
}

void IOIPCEventHandler::HandleSocketCtrlEvent(Channel* src_channel,
        ServiceType type, ServiceIndex idx, SocketCtrlEvent* event)
{
	TRACE_LOG(
	        "Enter HandleSocketCtrlEvent with ctrl type:%u and socket channel id %u", event->GetSocketCtrlType(), event->GetSocketChannelID());
	switch (event->GetSocketCtrlType())
	{
		case SOCKET_OPEN:
		{
			Channel* ch = NULL;
			if (IS_TCP_CHANNEL(event->GetSocketChannelID()))
			{
				const SocketHostAddress* addr = event->GetAddress();
				const SocketUnixAddress* unix_addr = event->GetUnixAddress();
				if (NULL == addr && NULL == unix_addr)
				{
					ch = m_handler->TCPSocketOpenRequest();
				}
				else if (NULL != unix_addr)
				{
					ch = m_handler->UnixSocketOpenRequest(unix_addr->GetPath());
				}
			}
			else
			{
				ch = m_handler->UDPSocketOpenRequest();
			}
			break;
		}
		case SOCKET_CONNECT:
		{
			const SocketHostAddress* addr = event->GetAddress();
			const SocketUnixAddress* unix_addr = event->GetUnixAddress();
			Channel* ch = GetChannelService()->GetChannel(
			        event->GetSocketChannelID());
			if (NULL == ch)
			{
				if (IS_TCP_CHANNEL(event->GetSocketChannelID()))
				{
					if (NULL != addr)
					{
						ch = m_handler->TCPSocketConnectRequest(addr->GetHost(),
						        addr->GetPort());
					}
					else if (NULL != unix_addr)
					{
						ch = m_handler->UnixSocketConnectRequest(
						        unix_addr->GetPath());
					}
				}
				else
				{
					ch = m_handler->UDPSocketConnectRequest(addr->GetHost(),
					        addr->GetPort());
				}
			}
			else
			{
				bool ret = false;
				if (NULL != addr)
				{
					ret = ch->Connect(const_cast<SocketHostAddress*>(addr));
				}
				else if (NULL != unix_addr)
				{
					ret = ch->Connect(
					        const_cast<SocketUnixAddress*>(unix_addr));
				}
				if (!ret)
				{
					ch = NULL;
				}
			}

			if (NULL != ch)
			{
				ServiceProcessChannelBindingItem item;
				item.ch = ch;
				if (NULL != addr)
				{
					item.SetAddress(const_cast<SocketHostAddress*>(addr));
				}
				else if (NULL != unix_addr)
				{
					item.SetAddress(const_cast<SocketUnixAddress*>(unix_addr));
				}
				item.is_channel_connected = false;
				item.serv_type = type;
				item.serv_idx = idx;
				m_channel_service_binding_table[ch] = item;
			}
			else
			{
				SocketCtrlEvent faile_event;
				IPCEvent& ev = faile_event;
				faile_event.SetSocketCtrlType(SOCKET_CONNECT_FAILED);
				faile_event.SetSocketChannelID(event->GetSocketChannelID());
				if (NULL != addr)
				{
					faile_event.SetAddress(
					        const_cast<SocketHostAddress*>(event->GetAddress()));
				}
				else if (NULL != unix_addr)
				{
					faile_event.SetUnixAddress(
					        const_cast<SocketUnixAddress*>(event->GetUnixAddress()));
				}
				src_channel->Write(ev);
			}
			break;
		}
		case SOCKET_CLOSE:
		{
			Channel* ch = GetChannelService()->GetChannel(
			        event->GetSocketChannelID());
			if (NULL != ch)
			{
				ch->Close();
			}
			break;
		}
		default:
		{
			ERROR_LOG(
			        "Unknown socket ctrl type:%u", event->GetSocketCtrlType());
			break;
		}
	}
}

void IOIPCEventHandler::WriteIPCEvent(IPCEvent& ev, ServiceType dstType,
        ServiceIndex dstIndex)
{
    TRACE_LOG( "Write IPC message to %u:%u", dstType, dstIndex);
	Channel* dst_ch = GetServiceProcessIPCChannel(dstType, dstIndex);
	//IPCEvent & ev = *event;
	if (NULL != dst_ch)
	{
		if (!dst_ch->Write(ev))
		{
			WriteFailed(dst_ch);
		}
	}
	else
	{
		ERROR_LOG(
		        "Failed to get dest channel to forward msg for %u:%u from %u:%u.", dstType, dstIndex, ev.GetSrcType(), ev.GetSrcIndex());
	}
}

void IOIPCEventHandler::HandleIPCMessageEvent(Channel* src_channel,
        ServiceType type, ServiceIndex idx, IPCMessageEvent* event)
{
	if (NULL != src_channel && NULL != event->GetContent())
	{
		if (event->GetDstType() == m_type && event->GetDstIndex() == m_idx
		        && NULL != m_handler)
		{
			m_handler->OnIPCMessage(type, idx, *(event->GetContent()));
		}
		else
		{
			WriteIPCEvent(*event, event->GetDstType(), event->GetDstIndex());
		}
	}
}

void IOIPCEventHandler::WriteFailed(Channel* ch)
{
	if (NULL != m_handler)
	{
		m_handler->OnChannelWriteFailed(ch);
	}
}

void IOIPCEventHandler::MessageReceived(ChannelHandlerContext& ctx,
        MessageEvent<IPCEvent>& e)
{

	IPCEvent* ipc_event = e.GetMessage();

	switch (ipc_event->GetType())
	{
		case SOCKET_CTRL:
		{
			HandleSocketCtrlEvent(ctx.GetChannel(), ipc_event->GetSrcType(),
			        ipc_event->GetSrcIndex(), (SocketCtrlEvent*) ipc_event);
			break;
		}
		case SOCKET_MSG:
		{
			SocketMessageEvent* event = (SocketMessageEvent*) ipc_event;
			Channel* ch = GetChannelService()->GetChannel(
			        event->GetChannelID());
			if (NULL != ch)
			{
				if (IS_TCP_CHANNEL(event->GetChannelID()))
				{
					Buffer* content = event->GetContent();
					//DEBUG_LOG("Write %u bytes to socket:%u.", content->ReadableBytes(), ch->GetID());
					if (!ch->Write(*content))
					{
						WriteFailed(ch);
					}
				}
				else if (IS_UDP_CHANNEL(event->GetChannelID()))
				{
					Buffer* content = event->GetContent();
					SocketInetAddress* address = event->GetSocketInetAddress();
					if (NULL != address)
					{
						DatagramPacket packet(content, *address);
						if (!ch->Write(packet))
						{
							WriteFailed(ch);
						}
					}
				}
				else
				{
					ERROR_LOG(
					        "Invalid channel ID:%u for socket message.", event->GetChannelID());
				}
			}
			else
			{
				if (g_proc_running)
				{
					ERROR_LOG(
					        "Failed to get channel by channel ID:%u", event->GetChannelID());
				}
				WriteFailed(ch);
				if (IS_TCP_CHANNEL(event->GetChannelID())
				        || IS_UDP_CHANNEL(event->GetChannelID()))
				{
					BroadcastServiceSocketClosed(event->GetChannelID());
				}
			}
			break;
		}
		case IPC_MSG:
		{
			IPCMessageEvent* event = (IPCMessageEvent*) ipc_event;
			HandleIPCMessageEvent(ctx.GetChannel(), ipc_event->GetSrcType(),
			        ipc_event->GetSrcIndex(), event);
			break;
		}
		case IPC_CTRL:
		{
			IPCCtrlEvent* event = (IPCCtrlEvent*) ipc_event;
			DEBUG_LOG(
			        "IPCCTRL event(%u) received from %u:%u.", event->GetIPCCtrlType(), event->GetSrcType(), event->GetSrcIndex());
			switch (event->GetIPCCtrlType())
			{
//				case TRANSMIT_CLIENT_SOCKET_FD:
//				{
//					ClientSocketChannel*ch =
//					        VirtualChannelHelper::ReadClientSocketChannelFD();
//					if (NULL != ch)
//					{
//						m_handler->OnTransferFD(event->GetSrcType(),
//						        event->GetSrcIndex(), ch);
//						fireChannelConnected(ch);
//					}
//					break;
//				}
				case TRANSMIT_CHANNEL_CONTROL:
				{
					WriteIPCEvent(*event, event->GetDstType(), event->GetDstIndex());
					uint32 type = RETRIVE_DELEGATE_TYPE(event->GetEventInfo());
					uint32 fd = RETRIVE_DELEGATE_FD(event->GetEventInfo());
					if (type == DELEGATE_CHANNEL_CONTROL_RESPONSE)
					{
						g_manager_proc->SetControlProcess4FD(fd,
						        event->GetSrcIndex());
					}
					break;
				}
				default:
				{
					ERROR_LOG(
					        "Not supported IPC ctrl event type:%u", event->GetIPCCtrlType());
					break;
				}
			}
			break;
		}
		default:
		{
			ERROR_LOG("Not supported IPC event type:%u", ipc_event->GetType());
			break;
		}
	}
}

void IOIPCEventHandler::BroadcastServiceProcessStarted(ServiceType type,
        ServiceIndex idx)
{
	IPCCtrlEvent event;
	IPCEvent& ev = event;
	event.SetSrcIndex(idx);
	event.SetSrcType(type);
	event.SetIPCCtrlType(SERV_PROC_STARTED);
	std::list<Channel*> channel_list;
	GetAllGetServiceProcessIPCChannels(channel_list);
	Channel* ipc_ch = GetServiceProcessIPCChannel(type, idx);
	DEBUG_LOG(
	        "Broadcast service(%u:%u) started to all %u services!", type, idx, channel_list.size());
	std::list<Channel*>::iterator it = channel_list.begin();
	while (it != channel_list.end())
	{
		Channel* ch = *it;
		if (ch != ipc_ch)
		{
			ch->Write(ev);
		}
		it++;
	}
}

void IOIPCEventHandler::NotifyerviceProcessStarted(ServiceType src_type,
        ServiceIndex src_idx, ServiceType dst_type, ServiceIndex dst_idx)
{
	IPCCtrlEvent event;
	IPCEvent& ev = event;
	event.SetSrcType(src_type);
	event.SetSrcIndex(src_idx);
	event.SetIPCCtrlType(SERV_PROC_STARTED);
	Channel* ipc_ch = GetServiceProcessIPCChannel(dst_type, dst_idx);
	RETURN_IF_NULL(ipc_ch);
	ipc_ch->Write(ev);
}

void IOIPCEventHandler::BroadcastServiceProcessStoped(ServiceType type,
        ServiceIndex idx)
{
	IPCCtrlEvent event;
	IPCEvent& ev = event;
	event.SetSrcIndex(type);
	event.SetSrcType(idx);
	event.SetIPCCtrlType(SERV_PROC_STOPED);
	std::list<Channel*> channel_list;
	GetAllGetServiceProcessIPCChannels(channel_list);
	DEBUG_LOG(
	        "Broadcast service(%u:%u) stoped to all %u services!", type, idx, channel_list.size());
	std::list<Channel*>::iterator it = channel_list.begin();
	while (it != channel_list.end())
	{
		Channel* ch = *it;
		ch->Write(ev);
		it++;
	}
}

bool IOIPCEventHandler::BroadcastServiceSocketClosed(uint32 channelID)
{
	SocketCtrlEvent event;
	IPCEvent& ev = event;
	event.SetSrcIndex(m_type);
	event.SetSrcType(m_idx);
	event.SetSocketCtrlType(SOCKET_CLOSED);
	event.SetSocketChannelID(channelID);
	std::list<Channel*> channel_list;
	GetAllGetServiceProcessIPCChannels(channel_list);
	DEBUG_LOG(
	        "Broadcast socket:%u closed to all %u services!", channelID, channel_list.size());
	std::list<Channel*>::iterator it = channel_list.begin();
	while (it != channel_list.end())
	{
		Channel* ch = *it;
		ch->Write(ev);
		it++;
	}
	return true;
}

bool IOIPCEventHandler::NotifyServiceSocketConnected(Channel* channel,
        bool broadcast)
{
	SocketCtrlEvent event;
	IPCEvent& ev = event;
	event.SetSrcIndex(m_type);
	event.SetSrcType(m_idx);
	event.SetSocketCtrlType(SOCKET_CONNECTED);
	event.SetSocketChannelID(channel->GetID());

	const Address* remote_address = channel->GetRemoteAddress();
	if (NULL == remote_address)
	{
		channel->Close();
		return false;
	}
	Address* conn_addr = const_cast<Address*>(remote_address);
	if (InstanceOf<SocketHostAddress>(conn_addr).Value)
	{
		SocketHostAddress* host_addr = (SocketHostAddress*) conn_addr;
		event.SetAddress(host_addr);
	}
	else if (InstanceOf<SocketUnixAddress>(conn_addr).Value)
	{
		SocketUnixAddress* unix_addr = (SocketUnixAddress*) conn_addr;
		event.SetUnixAddress(unix_addr);
	}
	map<Channel*, ServiceProcessChannelBindingItem>::iterator found =
	        m_channel_service_binding_table.find(channel);
	if (found != m_channel_service_binding_table.end())
	{
		ServiceProcessChannelBindingItem& item = found->second;
		if (!broadcast)
		{
			Channel* dst_ch = GetServiceProcessIPCChannel(item.serv_type,
			        item.serv_idx);
			item.is_channel_connected = true;
			bool ret = false;
			//event.SetAddress(&(item.connect_addr));
			if (NULL != dst_ch)
			{
				ret = dst_ch->Write(ev);
			}
			else
			{
				ret = false;
			}
			m_channel_service_binding_table.erase(found);
			return ret;
		}
		m_channel_service_binding_table.erase(found);
	}
	//    if (!broadcast)
	//    {
	//        map<Channel*, ServiceProcessChannelBindingItem>::iterator found =
	//                m_channel_service_binding_table.find(channel);
	//        if (found != m_channel_service_binding_table.end())
	//        {
	//            ServiceProcessChannelBindingItem& item = found->second;
	//            Channel* dst_ch = GetServiceProcessIPCChannel(item.serv_type,
	//                    item.serv_idx);
	//            item.is_channel_connected = true;
	//            //event.SetAddress(&(item.connect_addr));
	//            if (NULL != dst_ch)
	//            {
	//                return dst_ch->Write(ev);
	//            }
	//            else
	//            {
	//                return false;
	//            }
	//        }
	//    }
	//broadcast = true or no binding service process found
	std::list<Channel*> channel_list;
	GetAllGetServiceProcessIPCChannels(channel_list);
	DEBUG_LOG(
	        "Broadcast socket:%u connected to all %u services!", channel->GetID(), channel_list.size());
	std::list<Channel*>::iterator it = channel_list.begin();
	while (it != channel_list.end())
	{
		Channel* ch = *it;
		ch->Write(ev);
		it++;
	}
	return true;
}

bool IOIPCEventHandler::NotifyServiceSocketOpened(Channel* channel,
        bool broadcast)
{
	SocketCtrlEvent event;
	IPCEvent& ev = event;
	event.SetSrcIndex(m_type);
	event.SetSrcType(m_idx);
	event.SetSocketCtrlType(SOCKET_OPENED);
	event.SetSocketChannelID(channel->GetID());
	if (!broadcast)
	{
		map<Channel*, ServiceProcessChannelBindingItem>::iterator found =
		        m_channel_service_binding_table.find(channel);
		if (found != m_channel_service_binding_table.end())
		{
			ServiceProcessChannelBindingItem& item = found->second;
			Channel* dst_ch = GetServiceProcessIPCChannel(item.serv_type,
			        item.serv_idx);
			if (NULL != dst_ch)
			{
				return dst_ch->Write(ev);
			}
			else
			{
				return false;
			}
		}
	}
	//broadcast = true or no binding service process found
	std::list<Channel*> channel_list;
	GetAllGetServiceProcessIPCChannels(channel_list);
	DEBUG_LOG(
	        "Broadcast socket:%u open to all %u services!", channel->GetID(), channel_list.size());
	std::list<Channel*>::iterator it = channel_list.begin();
	while (it != channel_list.end())
	{
		Channel* ch = *it;
		ch->Write(ev);
		it++;
	}
	return true;
}

bool IOIPCEventHandler::NotifyServiceSocketClosed(Channel* channel,
        bool broadcast)
{
	SocketCtrlEvent event;
	IPCEvent& ev = event;
	event.SetSrcIndex(m_type);
	event.SetSrcType(m_idx);
	event.SetSocketCtrlType(SOCKET_CLOSED);
	event.SetSocketChannelID(channel->GetID());
	map<Channel*, ServiceProcessChannelBindingItem>::iterator found =
	        m_channel_service_binding_table.find(channel);
	if (found != m_channel_service_binding_table.end())
	{
		event.SetSocketCtrlType(SOCKET_CONNECT_FAILED);
		ServiceProcessChannelBindingItem& item = found->second;
		SocketHostAddress* host_addr = item.GetHostAddress();
		SocketUnixAddress* unix_addr = item.GetUnixAddress();
		if (NULL != host_addr)
		{
			event.SetAddress(host_addr);
		}
		else if (NULL != unix_addr)
		{
			event.SetUnixAddress(unix_addr);
		}
	}
	if (!broadcast)
	{
		if (found != m_channel_service_binding_table.end())
		{
			ServiceProcessChannelBindingItem& item = found->second;
			Channel* dst_ch = GetServiceProcessIPCChannel(item.serv_type,
			        item.serv_idx);
			if (NULL != dst_ch)
			{
				//                if (!item.is_channel_connected)
				//                {
				//                    event.SetSocketCtrlType(SOCKET_CONNECT_FAILED);
				//                    SocketHostAddress* host_addr = item.GetHostAddress();
				//                    SocketUnixAddress* unix_addr = item.GetUnixAddress();
				//                    if (NULL != host_addr)
				//                    {
				//                        event.SetAddress(host_addr);
				//                    }
				//                    else if (NULL != unix_addr)
				//                    {
				//                        event.SetUnixAddress(unix_addr);
				//                    }
				//                    //event.SetAddress(&(item.connect_addr));
				//                }
				dst_ch->Write(ev);
			}
			m_channel_service_binding_table.erase(found);
			return true;
		}
	}
	else
	{

		//broadcast = true or no binding service process found
		std::list<Channel*> channel_list;
		GetAllGetServiceProcessIPCChannels(channel_list);
		DEBUG_LOG(
		        "Broadcast socket:%u closed to all %u services!", channel->GetID(), channel_list.size());
		std::list<Channel*>::iterator it = channel_list.begin();
		while (it != channel_list.end())
		{
			Channel* ch = *it;
			ch->Write(ev);
			it++;
		}
		if (found != m_channel_service_binding_table.end())
		{
			m_channel_service_binding_table.erase(found);
		}

	}
	return true;
}

bool IOIPCEventHandler::DispatchTcpMsg(ServiceType type, ServiceIndex idx,
        Channel* src_channel, Buffer& content)
{
	if (!g_proc_running)
	{
		return true;
	}
	RETURN_FALSE_IF_NULL(src_channel);

	Channel* dst_ch = GetServiceProcessIPCChannel(type, idx);
	RETURN_FALSE_IF_NULL(dst_ch);

	SocketMessageEvent event;
	IPCEvent& ev = event;
	event.SetSrcIndex(m_type);
	event.SetSrcType(m_idx);
	event.SetChannelID(src_channel->GetID());
	event.SetContent(&content);
	TRACE_LOG(
	        "Dispatch tcp msg from channel(%u) to process(%u_%u)", src_channel->GetID(), type, idx);
	return dst_ch->Write(ev);
}

bool IOIPCEventHandler::DispatchUdpMsg(ServiceType type, ServiceIndex idx,
        Channel* src_channel, SocketInetAddress& address, Buffer& content)
{
	RETURN_FALSE_IF_NULL(src_channel);
	Channel* dst_ch = GetServiceProcessIPCChannel(type, idx);
	RETURN_FALSE_IF_NULL(dst_ch);
	SocketMessageEvent event;
	IPCEvent& ev = event;
	event.SetSrcIndex(m_type);
	event.SetSrcType(m_idx);
	event.SetChannelID(src_channel->GetID());
	event.SetContent(&content);
	event.SetSocketInetAddress(&address);
	return dst_ch->Write(ev);
}

bool IOIPCEventHandler::DispatchIPCMsg(ServiceType type, ServiceIndex idx,
        Buffer& content, IPCEventType event_type)
{
	Channel* dst_ch = GetServiceProcessIPCChannel(type, idx);
	RETURN_FALSE_IF_NULL(dst_ch);
	IPCMessageEvent event;
	IPCEvent& ev = event;
	event.SetType(event_type);
	event.SetContent(&content);
	event.SetSrcIndex(m_type);
	event.SetSrcType(m_idx);
	event.SetDstType(type);
	event.SetDstIndex(idx);
	bool ret = dst_ch->Write(ev);
	if (event_type == ADMIN && ret)
	{
		dst_ch->Flush();
	}
	return ret;
}

bool IOIPCEventHandler::IsServiceIPCOverflow(ServiceType type, ServiceIndex idx,
        uint32 expectedMaxSize)
{
	Channel* dst_ch = GetServiceProcessIPCChannel(type, idx);
	RETURN_FALSE_IF_NULL(dst_ch);
	uint32 writeBufSize = dst_ch->WritableBytes();
	return writeBufSize > expectedMaxSize;
}

