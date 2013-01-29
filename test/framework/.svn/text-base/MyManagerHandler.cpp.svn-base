/*
 * MyManagerHandler.cpp
 *
 *  Created on: 2011-2-7
 *      Author: wqy
 */
#include "MyManagerHandler.hpp"
#include "MyManagerProcess.hpp"
#include "logging/logger_macros.hpp"
#include "net/network_helper.hpp"

uint32 g_manager_recv_counts = 0;

void MyManagerHandler::ChannelClosed(ChannelHandlerContext& ctx,
        ChannelStateEvent& e)
{
	DEBUG_LOG("#####Notify ChannelClosed!\n");
	m_manager->GetIPCEventHandler().NotifyServiceSocketClosed(e.GetChannel());
}

void MyManagerHandler::ChannelConnected(ChannelHandlerContext& ctx,
        ChannelStateEvent& e)
{
	DEBUG_LOG("#####Notify ChannelConnected!\n");
	m_manager->GetIPCEventHandler().NotifyServiceSocketConnected(e.GetChannel());
}

void MyManagerHandler::ExceptionCaught(ChannelHandlerContext& ctx,
        ExceptionEvent& e)
{
	DEBUG_LOG("#####exceptionCaught!\n");
}

void MyManagerHandler::MessageReceived(ChannelHandlerContext& ctx,
        MessageEvent<DatagramPacket>& e)
{
	g_manager_recv_counts++;
	DatagramPacket* packet = e.GetMessage();
	SocketInetAddress& inet_addr = packet->GetInetAddress();
	SocketHostAddress host_addr = get_host_address(inet_addr);
	//    ServiceProcess* proc = m_manager->GetServiceProcess(1, 0);
	bool ret = m_manager->GetIPCEventHandler().DispatchUdpMsg(1, 0,
	        e.GetChannel(), packet->GetInetAddress(), packet->GetBuffer());
	DEBUG_LOG("UDP message received with %u bytes from %s:%u.", packet->GetBuffer().ReadableBytes(), host_addr.GetHost().c_str(), host_addr.GetPort());
}

void MyManagerHandler::MessageReceived(ChannelHandlerContext& ctx,
        MessageEvent<Buffer>& e)
{
	g_manager_recv_counts++;
	Buffer* content = e.GetMessage();
	ServiceProcessFactory* factory = m_manager->GetServiceProcessFactory(1);
	//    ServiceProcess* proc = factory->GetServiceProcessByIndex(
	//            g_manager_recv_counts % (factory->GetProcessCount()));
	DEBUG_LOG("#####Dispatch %d bytes to process 0 for channel(%u)", content->ReadableBytes(), ctx.GetChannel()->GetID());
	//FORCE_CORE_DUMP();
	bool ret = m_manager->GetIPCEventHandler().DispatchTcpMsg(1,
	        g_manager_recv_counts % (factory->GetProcessCount()),
	        //e.GetChannel(), *content);
	        ctx.GetChannel(), *content);
	if (!ret)
	{
		ERROR_LOG("Failed to dispatch to sub process with count:%d", g_manager_recv_counts);
	}


	//	if (InstanceOf<Buffer> ()(msg))
	//	{
	//		content = (Buffer*) msg;
	//
	//	}
	//	else if (InstanceOf<DatagramPacket> ()(msg))
	//	{
	//		DatagramPacket* packet = (DatagramPacket*) msg;
	//		SocketInetAddress& inet_addr = packet->GetInetAddress();
	//		SocketHostAddress host_addr = getHostAddress(inet_addr);
	//		ServiceProcess* proc = m_manager->GetServiceProcess(1, 0);
	//		bool ret = m_manager->GetIPCEventHandler().DispatchUdpMsg(proc,
	//		        e.GetChannel(), packet->GetInetAddress(), packet->GetBuffer());
	//		DEBUG_LOG("UDP message received with %u bytes from %s:%u.", packet->GetBuffer().ReadableBytes(), host_addr.GetHost().c_str(), host_addr.GetPort());
	//	}
	//	else
	//	{
	//		ctx.SendUpstream(e);
	//	}

}
