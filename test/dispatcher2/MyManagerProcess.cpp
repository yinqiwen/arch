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

class TempHandler: public ChannelUpstreamHandler<Buffer>
{
    private:
//        MyManagerProcess* m_manager;
//        MyDispatcherProcess* m_dispatcher;
        friend class MyManagerProcess;
        friend class MyManagerIPCHandler;
        friend class MyDispatcherProcess;
        void ChannelConnected(ChannelHandlerContext& ctx, ChannelStateEvent& e)
        {
            static uint32 i = 0;
            i++;
            DEBUG_LOG("##############ChannelConnected");
            ClientSocketChannel* ch = (ClientSocketChannel*) ctx.GetChannel();
            VirtualChannelHelper::SendClientSocketChannelFD(
                    DISPATCHER_SERVICE_PROCESS_TYPE, i % 2, ch);
        }
        void ChannelClosed(ChannelHandlerContext& ctx, ChannelStateEvent& e)
        {

        }
        void MessageReceived(ChannelHandlerContext& ctx,
                MessageEvent<Buffer>& e)
        {
        }
    public:
        TempHandler()
        {
        }

};

static void pipelineInit(ChannelPipeline* pipeline, void* data)
{
    pipeline->AddLast("handler", new TempHandler);
}

static void pipelineFinalize(ChannelPipeline* pipeline, void* data)
{
    ChannelHandler* handler = (pipeline->Get("handler"));
    delete handler;
}

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
        m_server->SetChannelPipelineInitializor(pipelineInit, NULL);
        m_server->SetChannelPipelineFinalizer(pipelineFinalize, NULL);
        //ServiceProcessFactory* dispatcherFac = GetServiceProcessFactory(
        //        DISPATCHER_SERVICE_PROCESS_TYPE);
        //dispatcherFac->DelegateChannel("ListenerPoint", m_server);
        DEBUG_LOG("Bind success!");
        return true;
    }
    else
    {
        ERROR_LOG("Failed to bind server.");
        return false;
    }
}
