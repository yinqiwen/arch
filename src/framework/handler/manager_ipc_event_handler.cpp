/*
 * InternalMessageDispatchHandler.cpp
 *
 *  Created on: 2011-2-6
 *      Author: wqy
 */
#include "framework/process/manager_process.hpp"
#include "framework/handler/manager_ipc_event_handler.hpp"
#include "channel/channel.hpp"
#include "net/datagram_packet.hpp"
#include "logging/logger_macros.hpp"

using namespace arch::framework;
using namespace arch::channel;
using namespace arch::channel::fifo;
using namespace arch::net;
using arch::exception::Exception;

ManagerIPCEventHandler::ManagerIPCEventHandler() :
		m_manager(NULL), m_handler(NULL)
{
}

void ManagerIPCEventHandler::ChannelConnected(ChannelHandlerContext& ctx,
        ChannelStateEvent& e)
{
	Channel* ch = ctx.GetChannel();
	Address* remote = const_cast<Address*>(ch->GetRemoteAddress());
	Address* local = const_cast<Address*>(ch->GetLocalAddress());
	if (NULL != remote)
	{
		if (InstanceOf<SocketUnixAddress>(remote).OK)
		{
			SocketUnixAddress* un = (SocketUnixAddress*) remote;
			SocketUnixAddress* lun = (SocketUnixAddress*) local;
			const std::string& path = un->GetPath();
			ProcessAddress proc_addr;
			m_manager->ParseIPCClientUnixSocketAddress(path, proc_addr);
			DEBUG_LOG(
			        "Manager accept a client from process %u_%u", proc_addr.proc_type, proc_addr.proc_idx);
            ClientSocketChannel* sock_ch = (ClientSocketChannel*)ch;
            ServiceProcess* proc = m_manager->GetServiceProcess(proc_addr.proc_type, proc_addr.proc_idx);
            if(NULL == proc)
            {
            	ERROR_LOG("NULL process found for %u:%u", proc_addr.proc_type, proc_addr.proc_idx);
            	return;
            }
            if(m_manager->IsManagerIPCUnixSocketServer(lun->GetPath()))
            {
                proc->SetIPCUnixSocket(sock_ch);
            }
            else if(m_manager->IsManagerCtrlUnixSocketServer(lun->GetPath()))
            {
                proc->SetCtrlChannel(sock_ch);
                sock_ch->DetachFD();
                ch->SetIOEventCallback(VirtualChannelHelper::CtrlChannelIOEventCallback, AE_READABLE, ch);
            }
            else
            {
                ERROR_LOG("Unknown path %s", un->GetPath().c_str());
            }
		}
	}
}

ChannelService* ManagerIPCEventHandler::GetChannelService()
{
	return &(m_manager->GetChannelService());
}

ServiceType ManagerIPCEventHandler::GetServiceProcessType()
{
	return 0;
}

ServiceIndex ManagerIPCEventHandler::GetServiceProcessIndex()
{
	return 0;
}

Channel* ManagerIPCEventHandler::GetServiceProcessIPCChannel(ServiceType type,
        ServiceIndex idx)
{
	ServiceProcess* proc = m_manager->GetServiceProcess(type, idx);
	if (NULL != proc)
	{
		return proc->GetIPCChannel();
	}
	return NULL;
}

void ManagerIPCEventHandler::GetAllGetServiceProcessIPCChannels(
        std::list<Channel*>& channelList)
{
	channelList.clear();
	std::list<ServiceProcess*> proc_list;
	m_manager->GetServiceProcessList(proc_list, 0);
	std::list<ServiceProcess*>::iterator it = proc_list.begin();
	while (it != proc_list.end())
	{
		ServiceProcess* proc = *it;
		Channel* ch = proc->GetIPCChannel();
		if (NULL != ch)
		{
			channelList.push_back(ch);
		}

		it++;
	}
}

