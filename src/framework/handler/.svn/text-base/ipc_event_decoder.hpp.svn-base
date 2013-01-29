/*
 * InternalMessageDecoder.hpp
 *
 *  Created on: 2011-1-29
 *      Author: wqy
 */

#ifndef NOVA_INTERNALMESSAGEDECODER_HPP_
#define NOVA_INTERNALMESSAGEDECODER_HPP_
#include "common/base.hpp"
#include "channel/all_includes.hpp"
#include "framework/event/ipc_event.hpp"

using arch::channel::ChannelHandlerContext;
using arch::channel::ExceptionEvent;
using arch::channel::MessageEvent;
using arch::channel::codec::FrameDecoder;
using arch::channel::codec::FrameDecodeResult;

namespace arch
{
	namespace framework
	{
        /**
         * manager进程和业务子进程之间的IPC event解码
         */
		class IPCEventDecoder: public FrameDecoder<IPCEvent>
		{
			private:
				FrameDecodeResult<IPCEvent> Decode(ChannelHandlerContext& ctx,
				        Channel* channel, Buffer& buffer);
		};
	}
}

#endif /* INTERNALMESSAGEDECODER_HPP_ */
