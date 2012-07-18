/*
 * MyManagerProcess.cpp
 *
 *  Created on: 2011-2-7
 *      Author: wqy
 */
#include "MyManagerProcess.hpp"
#include "MyDispatcherHandler.hpp"
#include "ipc/shm_allocator.hpp"
#include "net/datagram_packet.hpp"
#include "logging/logger_macros.hpp"
#include "util/proctitle_helper.h"
#include <vector>
using namespace arch::ipc;
using namespace std;
using namespace arch::net;
using namespace arch::channel;
using namespace arch::channel::codec;

bool MyManagerIPCHandler::OnInit()
{
	//m_handler.m_manager = &(GetManagerProcess());
	return true;
}

bool MyManagerIPCHandler::OnDestroy()
{
	return true;
}

void MyManagerProcess::OnRoutine()
{
	//INFO_LOG("Recv %u packets", g_manager_recv_counts);
}

bool MyManagerProcess::OnInit()
{
	//ngx_setproctitle("MyManager");
	m_server = GetChannelService().NewServerSocketChannel();
	ChannelOptions ops;
	ops.user_write_buffer_water_mark = 8192;
	ops.user_write_buffer_flush_timeout_mills = 1;
	SocketHostAddress host("0.0.0.0", 48100);
	DatagramChannel* udpserv = GetChannelService().NewDatagramSocketChannel();
	DEBUG_LOG("Enter here!");
	if (m_server->Bind(&host))
	{
		m_server->Configure(ops);
		ServiceProcessFactory* dispatcherFac = GetServiceProcessFactory(
		        DISPATCHER_SERVICE_PROCESS_TYPE);
		dispatcherFac->DelegateChannel("ListenerPoint", m_server);
		DEBUG_LOG("Bind success!");
		return true;
	}
	else
	{
		ERROR_LOG("Failed to bind server.");
		return false;
	}
}
