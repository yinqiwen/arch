/*
 * InternalMessageEncoder.cpp
 *
 *  Created on: 2011-2-12
 *      Author: qiyingwang
 */
#include "framework/handler/ipc_event_encoder.hpp"
#include "framework/event/ipc_event_factory.hpp"
#include "channel/all_includes.hpp"
#include "logging/logger_macros.hpp"

using namespace arch::framework;
using namespace arch::channel;

bool IPCEventEncoder::WriteRequested(ChannelHandlerContext& ctx, MessageEvent<
        IPCEvent>& e)
{
    TRACE_LOG("Enter IPCEventEncoder::writeRequested with IPCEvent");
	IPCEvent* msg = e.GetMessage();
	//consider using static object?
	//static Buffer buffer(1024);
	static Buffer buffer(1024);
	buffer.Clear();
	if (msg->Encode(&buffer))
	{
		return ctx.GetChannel()->Write(buffer);
	}
	return false;

}
