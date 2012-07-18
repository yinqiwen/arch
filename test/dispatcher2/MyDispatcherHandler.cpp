/*
 * MyDispatcherHandler.cpp
 *
 *  Created on: 2011-7-23
 *      Author: wqy
 */
#include "MyDispatcherHandler.hpp"
#include "MyNetworkEventDispatcherHandler.hpp"
#include "channel/all_includes.hpp"
#include "MyDispatcherProcess.hpp"

using namespace arch::channel;
using namespace arch::channel::codec;

static void pipelineInit(ChannelPipeline* pipeline, void* data)
{
	MyDispatcherProcess* proc = (MyDispatcherProcess*) data;
	pipeline->AddLast("decoder", new DelimiterBasedFrameDecoder(8192,
	        CommonDelimiters::CRLFDelimiter()));
	ChannelUpstreamHandler<Buffer>* handler =
	        new MyNetworkEventDispatcherHandler(proc);
	pipeline->AddLast("handler", handler);
}

static void pipelineFinalize(ChannelPipeline* pipeline, void* data)
{
	ChannelHandler* decoder = (pipeline->Get("decoder"));
	delete decoder;
	ChannelHandler* handler = (pipeline->Get("handler"));
	delete handler;
}

void MyDispatcherHandler::OnTransferFD(ServiceType src_type,
                                ServiceIndex src_idx, Channel* ch)
{
    DEBUG_LOG("######Receive FD!");
    MyDispatcherProcess* proc =
            (MyDispatcherProcess*) (&GetDispatcherProcess());
    ch->SetChannelPipelineInitializor(pipelineInit, proc);
    ch->SetChannelPipelineFinalizer(pipelineFinalize, NULL);
}

bool MyDispatcherHandler::OnInit()
{
//	Channel* ch = GetDispatcherProcess().GetDelegateChannel("ListenerPoint");
//	MyDispatcherProcess* proc =
//	        (MyDispatcherProcess*) (&GetDispatcherProcess());
//	ch->SetChannelPipelineInitializor(pipelineInit, proc);
//	ch->SetChannelPipelineFinalizer(pipelineFinalize, NULL);
	return true;
}

bool MyDispatcherHandler::OnDestroy()
{
	return true;
}
/**
 * 例行检查回调
 */
void MyDispatcherHandler::OnRoutine()
{

}

