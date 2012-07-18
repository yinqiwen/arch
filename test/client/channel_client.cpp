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

struct ClientHandler: public ChannelUpstreamHandler<Buffer>
{
        int total_would_send;
        int total_send;
        int total_received;
        ClientHandler(int total) :
            total_would_send(total), total_received(0), total_send(0)
        {
        }

        void ChannelBound(ChannelHandlerContext& ctx, ChannelStateEvent& e)
        {

        }
        void ChannelConnected(ChannelHandlerContext& ctx, ChannelStateEvent& e)
        {
            INFO_LOG("Success to connect remote");
            Buffer buf(16);
            buf.Write("Hello,world\r\n", 13);
            for (uint32 i = 0; i < 2000; i++)
            {
                if(ctx.GetChannel()->Write(buf))
                {
                    total_send++;
                }
                buf.SetReadIndex(0);

            }
            //INFO_LOG("Write %d request", total_send);
        }
        void ChannelOpen(ChannelHandlerContext& ctx, ChannelStateEvent& e)
        {

        }
        void ChannelClosed(ChannelHandlerContext& ctx, ChannelStateEvent& e)
        {
            INFO_LOG("Channel Closed!");
        }
        void MessageReceived(ChannelHandlerContext& ctx, MessageEvent<
                Buffer>& e)
        {
            total_received++;
            DEBUG_LOG("Received response with total:%d", total_received);
            if (total_received == total_would_send)
            {
                ctx.GetChannel()->Close();
                ctx.GetChannel()->GetService().Stop();
            }
            else if (total_received % 2000 == 0)
            {
                //INFO_LOG("Received %d response\n", total_received);
                Buffer buf(16);
                buf.Write("Hello,world\r\n", 13);
                for (uint32 i = 0; i < 2000; i++)
                {
                    if(!ctx.GetChannel()->Write(buf))
                    {
                        ERROR_LOG("Failed to write buf to channel with total send:%d", total_send);
                        continue;
                    }
                    total_send++;
                    buf.SetReadIndex(0);
                }
                //INFO_LOG("Write %d request\n", total_send);
            }
        }
        void MessageReceived(ChannelHandlerContext& ctx, MessageEvent<
                DatagramPacket>& e)
        {

        }
        void ExceptionCaught(ChannelHandlerContext& ctx, ExceptionEvent& e)
        {

        }
};

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Need one arg for this programme.");
        return -1;
    }
    int total = atoi(argv[1]);
    printf("Total:%d\n", total);
    ChannelService service;
    ClientSocketChannel* client = service.NewClientSocketChannel();
    ChannelOptions ops;
    ops.user_write_buffer_water_mark = 8192;
    ops.user_write_buffer_flush_timeout_mills = 1;

    SocketHostAddress address("127.0.0.1", 48100);
    SocketUnixAddress unix_address("./server.unix");
    SocketHostAddress local("127.0.0.1", 5678);
    SocketUnixAddress local_unix_address("./client.unix");
    client->Bind(&local_unix_address);
    client->Connect(&unix_address);
    client->Configure(ops);
    client->GetPipeline().AddLast("decoder", new DelimiterBasedFrameDecoder(8192, CommonDelimiters::CRLFDelimiter()));
    client->GetPipeline().AddLast("default", new ClientHandler(
            total));

    uint64 start = get_current_epoch_millis();
    arch::logging::Logger* logger = arch::logging::LoggerFactory::GetDevelopLogger();
    logger->SetLevel(arch::logging::INFO_LOG_LEVEL);
    service.Start(true);
    uint64 end = get_current_epoch_millis();
    printf("Cost %llums for total %d send/recv transaction.\n", (end - start), total);
    return 1;
}
