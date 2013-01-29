/*
 * Main.cpp
 *
 *  Created on: 2011-1-10
 *      Author: qiyingwang
 */
#include "channel/all_includes.hpp"
#include "channel/http/http_decoder.hpp"
#include "channel/http/http_encoder.hpp"
#include "protocol/redis/redis_frame_decoder.hpp"
#include "logging/logger_macros.hpp"
#include <stdio.h>

using namespace arch::channel;
using namespace arch::channel::http;
using namespace arch::channel::signal;
using namespace arch::buffer;
using namespace arch::redis;

class TempHandler: public ChannelUpstreamHandler<RedisCommandFrame>,
        public SoftSignalHandler
{
    protected:
        void OnSoftSignal(uint32 soft_signo, uint32 appendinfo)
        {
            DEBUG_LOG(
                    "#####soft_signo:%u, appendinfo:%u", soft_signo, appendinfo);
        }
        void ChannelOpen(ChannelHandlerContext& ctx, ChannelStateEvent& e)
        {
            DEBUG_LOG("channelOpen!");
        }
        void ChannelBound(ChannelHandlerContext& ctx, ChannelStateEvent& e)
        {
            DEBUG_LOG("channelBound!");
        }
        void ChannelConnected(ChannelHandlerContext& ctx, ChannelStateEvent& e)
        {
            DEBUG_LOG("channelConnected!");
        }
        void ExceptionCaught(ChannelHandlerContext& ctx, ExceptionEvent& e)
        {
            DEBUG_LOG(
                    "ExceptionCaught:%s", e.GetException().GetCause().c_str());
            ctx.GetChannel()->Close();
        }
        void ChannelClosed(ChannelHandlerContext& ctx, ChannelStateEvent& e)
        {
            DEBUG_LOG("channelClosed!");
        }
        void MessageReceived(ChannelHandlerContext& ctx,
                MessageEvent<RedisCommandFrame>& e)
        {
            DEBUG_LOG("messageReceived!");
            //e.GetChannel()->Close();
            RedisCommandFrame* cmd = e.GetMessage();
            RedisCommandFrame::ArgumentArray& args = cmd->GetArguments();
            DEBUG_LOG("Arg size:%d", args.size());
            RedisCommandFrame::ArgumentArray::iterator it = args.begin();
            while (it != args.end())
            {
                Buffer* buf = *it;
                DEBUG_LOG("Arg:%s", buf->AsString().c_str());
                it++;
            }
        }
};

class HTTPTempHandler: public ChannelUpstreamHandler<HTTPMessage>
{
    protected:
        void ChannelOpen(ChannelHandlerContext& ctx, ChannelStateEvent& e)
        {
            DEBUG_LOG("http channelOpen!");
        }
        void ChannelBound(ChannelHandlerContext& ctx, ChannelStateEvent& e)
        {
            DEBUG_LOG("http channelBound!");
        }
        void ChannelConnected(ChannelHandlerContext& ctx, ChannelStateEvent& e)
        {
            DEBUG_LOG("http channelConnected!");
        }
        void ExceptionCaught(ChannelHandlerContext& ctx, ExceptionEvent& e)
        {
            DEBUG_LOG(
                    "http exceptionCaught:%s", e.GetException().GetCause().c_str());
            ctx.GetChannel()->Close();
        }
        void ChannelClosed(ChannelHandlerContext& ctx, ChannelStateEvent& e)
        {
            DEBUG_LOG("http channelClosed!");
        }
        void MessageReceived(ChannelHandlerContext& ctx,
                MessageEvent<HTTPMessage>& e)
        {
            DEBUG_LOG("http messageReceived!");
            //e.GetChannel()->Close();
            HTTPMessage* msg = e.GetMessage();
            std::string str = msg->ToString();
            DEBUG_LOG("##%s", str.c_str());
            HTTPMessage response;
            response.SetStatusCode(200);
            response.AddHeader("Connection", "Close");
            ctx.GetChannel()->Write(response);
        }
};

static void channelPipelineInit(ChannelPipeline* pipeline, void* data)
{
    //TempHandler handler;
    RedisFrameDecoder* decoder = new RedisFrameDecoder;
    pipeline->AddLast("decode", decoder);
    pipeline->AddLast("handler", (TempHandler*) data);
    DEBUG_LOG("############channelPipelineInit");

}

static void channelPipelineFinallize(ChannelPipeline* pipeline, void* data)
{
    DEBUG_LOG("############channelPipelineFinallize");
    ChannelHandler* handler = pipeline->Get("decoder");
    DELETE(handler);
}

static void httpChannelPipelineInit(ChannelPipeline* pipeline, void* data)
{
    //TempHandler handler;
    HTTPMessageDecoder* decoder = new HTTPMessageDecoder(true);
    HTTPMessageEncoder* encoder = new HTTPMessageEncoder;
    //HTTPMessageDecoder* decoder = new HTTPMessageDecoder(false);
    pipeline->AddLast("decode", decoder);
    pipeline->AddLast("encoder", encoder);
    pipeline->AddLast("handler", (HTTPTempHandler*) data);
    DEBUG_LOG("############httpChannelPipelineInit");

}

static void httpChannelPipelineFinallize(ChannelPipeline* pipeline, void* data)
{
    DEBUG_LOG("############httpChannelPipelineFinallize");
    ChannelHandler* handler = pipeline->Get("decoder");
    DELETE(handler);
    handler = pipeline->Get("encoder");
    DELETE(handler);
}

static void TempInotifyCallBack(struct inotify_event * event)
{

}


int main(int argc, char** argv)
{
    ChannelService service;
    Channel* ch = service.NewServerSocketChannel();
    TempHandler handler;
    ch->SetChannelPipelineInitializor(channelPipelineInit, &handler);
    ch->SetChannelPipelineFinalizer(channelPipelineFinallize, &handler);
    SocketHostAddress host("0.0.0.0", 48100);
    bool ret = ch->Bind(&host);
    if (!ret)
    {
        ERROR_LOG("Failed to bind!\n");
    }

    HTTPTempHandler httpHandler;
    Channel* http_ch = service.NewServerSocketChannel();
    http_ch->SetChannelPipelineInitializor(httpChannelPipelineInit,
            &httpHandler);
    http_ch->SetChannelPipelineFinalizer(httpChannelPipelineFinallize,
            &httpHandler);
    SocketHostAddress httpHost("0.0.0.0", 48101);
    ret = http_ch->Bind(&httpHost);
    if (!ret)
    {
        ERROR_LOG("Failed to bind!\n");
    }
    SoftSignalChannel* soft_ch = service.NewSoftSignalChannel();
    soft_ch->Register(1, &handler);
    soft_ch->FireSoftSignal(1, 123456);

    InotifyChannel* inotify = service.NewInotifyChannel();
    inotify->RegisterCallBack(TempInotifyCallBack);
    inotify->Watch("");
    service.Start(false);
    INFO_LOG("Exit\n");
    return 1;
}
