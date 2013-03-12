/*
 * dispatcher_ipc_event_handler.cpp
 *
 *  Created on: 2011-7-23
 *      Author: wqy
 */
#include "framework/handler/dispatcher_ipc_event_handler.hpp"
#include "framework/process/service_process.hpp"
#include "framework/process/manager_process.hpp"
#include "framework/process/dispatcher_process.hpp"
#include "net/socket_unix_address.hpp"

using namespace arch::framework;
using arch::net::SocketUnixAddress;

void DispatcerIPCEventHandler::AddServiceIPCChannel(const ProcessAddress& addr,
        Channel* ch)
{
    m_ipc_channel_table[addr] = ch;
    m_client_channel_list.push_back(ch);
}

void DispatcerIPCEventHandler::ChannelConnected(ChannelHandlerContext& ctx,
        ChannelStateEvent& e)
{
    DEBUG_LOG("Dispatcher(%s_%u) accept a client.", g_service_proc->GetServiceName().c_str(), g_service_proc->GetProcessIndex());
    Channel* ch = ctx.GetChannel();
    Address* remote = const_cast<Address*> (ch->GetRemoteAddress());
    if (NULL != remote)
    {
        if (InstanceOf<SocketUnixAddress> (remote).OK)
        {
            SocketUnixAddress* un = (SocketUnixAddress*) remote;
            const std::string& path = un->GetPath();
            ProcessAddress proc_addr;
            g_service_proc->GetManagerProcess()->ParseDispatcherClientUnixSocketAddress(path,
                    proc_addr);
            DEBUG_LOG("Parsed process address %u_%u", proc_addr.proc_type, proc_addr.proc_idx);
            IPCChannelTable::iterator tit = m_ipc_channel_table.find(proc_addr);
            if (tit == m_ipc_channel_table.end())
            {
            	AddServiceIPCChannel(proc_addr, ch);
            }
        }
    }

}

void DispatcerIPCEventHandler::ChannelClosed(ChannelHandlerContext& ctx,
        ChannelStateEvent& e)
{
    DEBUG_LOG("Dispatcher(%s_%u) close a client.",g_service_proc->GetServiceName().c_str(), g_service_proc->GetProcessIndex());
    ChannelList::iterator it = m_client_channel_list.begin();
    while (it != m_client_channel_list.end())
    {
        if (ctx.GetChannel() == *it)
        {
            m_client_channel_list.erase(it);
            break;
        }
        it++;
    }

    IPCChannelTable::iterator tit = m_ipc_channel_table.begin();
    while (tit != m_ipc_channel_table.end())
    {
        if (ctx.GetChannel() == tit->second)
        {
            ProcessAddress addr = tit->first;
            m_ipc_channel_table.erase(tit);
            DispatcherProcess* proc = (DispatcherProcess*) g_service_proc;
            if (proc->GetIPCType() == IPC_FIFO)
            {
                proc->CreateServiceProcessIPCFIFOChannel(addr);
            }
            break;
        }
        tit++;
    }
}

ChannelService* DispatcerIPCEventHandler::GetChannelService()
{
    return &(g_service_proc->GetChannelService());
}

ServiceType DispatcerIPCEventHandler::GetServiceProcessType()
{
    return DISPATCHER_SERVICE_PROCESS_TYPE;
}

ServiceIndex DispatcerIPCEventHandler::GetServiceProcessIndex()
{
    return g_service_proc->GetProcessIndex();
}

Channel* DispatcerIPCEventHandler::GetServiceProcessIPCChannel(
        ServiceType type, ServiceIndex idx)
{
    ProcessAddress addr;
    addr.proc_type = type;
    addr.proc_idx = idx;
    IPCChannelTable::iterator tit = m_ipc_channel_table.find(addr);
    if (tit != m_ipc_channel_table.end())
    {
        Channel* ch = tit->second;
        DispatcherProcess* dispatchProc = static_cast<DispatcherProcess*>(g_service_proc);
        if (dispatchProc->GetIPCType() == IPC_FIFO)
        {
            if (!ch->IsWriteReady())
            {
                ch->Open();
                if (!ch->IsWriteReady())
                {
                    return NULL;
                }
            }
        }
        return ch;
    }
    return NULL;
}

void DispatcerIPCEventHandler::GetAllGetServiceProcessIPCChannels(std::list<
        Channel*>& channelList)
{
    channelList.clear();
    channelList = m_client_channel_list;
}
