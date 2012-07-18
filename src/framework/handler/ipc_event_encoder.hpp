/*
 * InternalMessageDecoder.hpp
 *
 *  Created on: 2011-1-29
 *      Author: wqy
 */

#ifndef NOVA_INTERNALMESSAGEENCODER_HPP_
#define NOVA_INTERNALMESSAGEENCODER_HPP_
#include "common/base.hpp"
#include "channel/all_includes.hpp"
#include "framework/event/ipc_message_event.hpp"
#include "framework/event/socket_ctrl_event.hpp"
#include "framework/event/socket_message_event.hpp"

using arch::channel::ChannelDownstreamHandler;
namespace arch
{
    namespace framework
    {
        /**
         * manager进程和业务子进程之间的IPC event编码
         */
        class IPCEventEncoder: public ChannelDownstreamHandler<IPCEvent>
        {
            private:
                bool WriteRequested(ChannelHandlerContext& ctx, MessageEvent<
                        IPCEvent>& e);
        };
    }
}

#endif /* INTERNALMESSAGEDECODER_HPP_ */
