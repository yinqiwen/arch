/*
 * admin_handler.hpp
 *
 *  Created on: 2011-5-16
 *      Author: qiyingwang
 */

#ifndef ADMIN_HANDLER_HPP_
#define ADMIN_HANDLER_HPP_

#include "channel/channel_upstream_handler.hpp"
#include "framework/admin/admin_command_handler.hpp"
#include "buffer/buffer.hpp"

#include <string>
#include <vector>

using namespace arch::channel;
using arch::buffer::Buffer;

namespace arch
{
    namespace framework
    {


        class AdminMessageHandler: public ChannelUpstreamHandler<Buffer>
        {
            private:
                //AdminCommandHandler m_handler;
                void GetRemoteHostPort(ChannelHandlerContext& ctx, std::string& result);
                void WriteResult(ChannelHandlerContext& ctx, const char* format, ...);
                void ChannelConnected(ChannelHandlerContext& ctx,
                        ChannelStateEvent& e);
                void ChannelClosed(ChannelHandlerContext& ctx,
                        ChannelStateEvent& e);
                void ExceptionCaught(ChannelHandlerContext& ctx,
                        ExceptionEvent& e);
                void MessageReceived(ChannelHandlerContext& ctx, MessageEvent<
                        Buffer>& e);
        };
    }
}

#endif /* ADMIN_HANDLER_HPP_ */
