/*
 * channel_client.cpp
 *
 *  Created on: 2011-4-25
 *      Author: qiyingwang
 */
#include "channel/all_includes.hpp"
#include "util/time_helper.hpp"
#include "net/socket_unix_address.hpp"

using namespace arch::util;
using namespace arch::net;
using namespace arch::channel;
using namespace arch::channel::codec;

struct ServerHandler: public ChannelUpstreamHandler<Buffer>
{
		uint32 total_recv;
		ServerHandler() :
			total_recv(0)
		{
		}

		void ChannelBound(ChannelHandlerContext& ctx, ChannelStateEvent& e)
		{

		}
		void ChannelConnected(ChannelHandlerContext& ctx, ChannelStateEvent& e)
		{
			INFO_LOG("Success to connect");
			//            Buffer buf(16);
			//            buf.Write("Hello,world\r\n", 13);
			//            for (uint32 i = 0; i < 2000; i++)
			//            {
			//                if (ctx.GetChannel()->Write(buf))
			//                {
			//                    total_send++;
			//                }
			//                buf.SetReadIndex(0);
			//
			//            }
			//INFO_LOG("Write %d request", total_send);
		}
		void ChannelOpen(ChannelHandlerContext& ctx, ChannelStateEvent& e)
		{

		}
		void ChannelClosed(ChannelHandlerContext& ctx, ChannelStateEvent& e)
		{
			INFO_LOG("Channel Closed!");
		}
		void MessageReceived(ChannelHandlerContext& ctx,
		        MessageEvent<Buffer>& e)
		{
			total_recv += (e.GetMessage()->ReadableBytes());
			Buffer send(7);
			send.Write("Hello\r\n", 7);
			ctx.GetChannel()->Write(send);
			//total_received++;
			//DEBUG_LOG("Received response with total:%d", total_received);
			//            if (total_received == total_would_send)
			//            {
			//                ctx.GetChannel()->Close();
			//                ctx.GetChannel()->GetService().Stop();
			//            }
			//            else if (total_received % 2000 == 0)
			//            {
			//                //INFO_LOG("Received %d response\n", total_received);
			//                Buffer buf(16);
			//                buf.Write("Hello,world\r\n", 13);
			//                for (uint32 i = 0; i < 2000; i++)
			//                {
			//                    if (!ctx.GetChannel()->Write(buf))
			//                    {
			//                        ERROR_LOG("Failed to write buf to channel with total send:%d", total_send);
			//                        continue;
			//                    }
			//                    total_send++;
			//                    buf.SetReadIndex(0);
			//                }
			//                //INFO_LOG("Write %d request\n", total_send);
			//            }
		}
		void MessageReceived(ChannelHandlerContext& ctx, MessageEvent<
		        DatagramPacket>& e)
		{

		}
		void ExceptionCaught(ChannelHandlerContext& ctx, ExceptionEvent& e)
		{

		}
};

static void pipelineInit(ChannelPipeline* pipeline, void* data)
{
	ChannelUpstreamHandler<Buffer>* handler = (ServerHandler*) data;
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

int main(int argc, char** argv)
{
	//    if (argc != 2)
	//    {
	//        printf("Nedd one arg for this programme.");
	//        return -1;
	//    }
	//    int total = atoi(argv[1]);
	//    printf("Total:%d\n", total);
	ChannelService service;
	ServerSocketChannel* server = service.NewServerSocketChannel();
	ChannelOptions ops;
	ops.user_write_buffer_water_mark = 8192;
	ops.user_write_buffer_flush_timeout_mills = 1;


	SocketHostAddress address("192.168.56.101", 48100);
	//SocketHostAddress local("0.0.0.0", 20001);
	SocketUnixAddress unixLocal("./server.unix");
	server->Bind(&unixLocal);
	server->Configure(ops);
	//client->Connect(&address);
	//client->Configure(ops);
	//client->GetPipeline().AddLast("decoder", new DelimiterBasedFrameDecoder(8192, CommonDelimiters::CRLFDelimiter()));
	ServerHandler handler;
	//server->GetPipeline().AddLast("default", new ServerHandler());
	server->SetChannelPipelineInitializor(pipelineInit, &handler);
	server->SetChannelPipelineFinalizer(pipelineFinalize, NULL);

	uint64 start = get_current_epoch_millis();
	arch::logging::Logger* logger =
	        arch::logging::LoggerFactory::GetDevelopLogger();
	logger->SetLevel(arch::logging::INFO_LOG_LEVEL);
	service.Start(true);
	uint64 end = get_current_epoch_millis();
	//printf("Cost %ums for total %d send/recv transaction.\n", (end - start),
	//        total);
	return 1;
}
