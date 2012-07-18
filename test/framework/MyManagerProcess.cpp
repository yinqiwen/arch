/*
 * MyManagerProcess.cpp
 *
 *  Created on: 2011-2-7
 *      Author: wqy
 */
#include "MyManagerProcess.hpp"
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
    m_handler.m_manager = &(GetManagerProcess());
    return true;
}

bool MyManagerIPCHandler::OnDestroy()
{
    return true;
}

//void MyManagerIPCHandler::OnIPCMessage(ServiceProcess& service, Buffer& content)
//{
//}

//Channel* MyManagerIPCHandler::SocketConnectRequest(const string& host,
//        uint16 port, bool isTcp)
//{
//    DEBUG_LOG("####MyManagerIPCHandler::SocketConnectRequest with %s:%u", host.c_str(), port);
//    Channel* ch =
//            GetManagerProcess().GetChannelService().NewClientSocketChannel();
//    ChannelUpstreamHandler<Buffer>* handler = &m_handler;
//    ch->GetPipeline().AddLast("handler", handler);
//    SocketHostAddress addr(host, port);
//    ch->Connect(&addr);
//    return ch;
//}
//Channel* MyManagerIPCHandler::SocketOpenRequest(bool isTcp)
//{
//    return NULL;
//}

void MyManagerProcess::OnRoutine()
{
    INFO_LOG("Recv %u packets", g_manager_recv_counts);
}

static void pipelineInit(ChannelPipeline* pipeline, void* data)
{
    ChannelUpstreamHandler<Buffer>* handler = (MyManagerHandler*) data;
    pipeline->AddLast("decoder", new DelimiterBasedFrameDecoder(8192,
            CommonDelimiters::CRLFDelimiter()));
    pipeline->AddLast("handler", handler);
}

static void pipelineFinalize(ChannelPipeline* pipeline, void* data)
{
    DelimiterBasedFrameDecoder* decoder =
            (DelimiterBasedFrameDecoder*) (pipeline->Get("decoder"));
    delete decoder;
}

bool MyManagerProcess::OnInit()
{
    ngx_setproctitle("MyManager");
    m_server = GetChannelService().NewServerSocketChannel();
    ChannelOptions ops;
    ops.user_write_buffer_water_mark = 8192;
    ops.user_write_buffer_flush_timeout_mills = 1;
    SocketHostAddress host("0.0.0.0", 48100);
    DatagramChannel* udpserv = GetChannelService().NewDatagramSocketChannel();
    DEBUG_LOG("Enter here!");
    if (m_server->Bind(&host))
    {
        m_handler.m_manager = this;
        m_server->Configure(ops);
        m_server->SetChannelPipelineInitializor(pipelineInit, &m_handler);
        m_server->SetChannelPipelineFinalizer(pipelineFinalize, NULL);
        //m_server->getPipeline().addLast("decoder", &m_decoder);
        //m_server->GetPipeline().AddLast("decoder", );
        //ChannelUpstreamHandler<Buffer>* handler = &m_handler;
        //m_server->GetPipeline().AddLast("handler", handler);
        //ChannelUpstreamHandler<DatagramPacket>* udphandler = &m_handler;
        //udpserv->GetPipeline().AddLast("handler", udphandler);
        //sleep(100);
        DEBUG_LOG("Bind success!");
        return true;
    }
    else
    {
        ERROR_LOG("Failed to bind server.");
        return false;
    }
}
