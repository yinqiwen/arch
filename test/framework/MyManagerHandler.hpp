/*
 * MyManagerHandler.hpp
 *
 *  Created on: 2011-2-7
 *      Author: wqy
 */

#ifndef MYMANAGERHANDLER_HPP_
#define MYMANAGERHANDLER_HPP_
#include "framework/process/manager_process.hpp"
#include "channel/all_includes.hpp"
#include "net/datagram_packet.hpp"

using arch::framework::ManagerProcess;
using namespace arch::channel;
using namespace arch::buffer;
using namespace arch::net;
class MyManagerProcess;
class MyManagerIPCHandler;
class MyManagerHandler: public ChannelUpstreamHandler<Buffer>
, public ChannelUpstreamHandler<DatagramPacket>
{
	private:
        ManagerProcess* m_manager;
		friend class MyManagerProcess;
		friend class MyManagerIPCHandler;
		void ChannelBound(ChannelHandlerContext& ctx, ChannelStateEvent& e)
		{

		}
		void ChannelConnected(ChannelHandlerContext& ctx, ChannelStateEvent& e);
		void ChannelOpen(ChannelHandlerContext& ctx, ChannelStateEvent& e)
		{

		}
		void ChannelClosed(ChannelHandlerContext& ctx, ChannelStateEvent& e);
		void MessageReceived(ChannelHandlerContext& ctx, MessageEvent<Buffer>& e);
		void MessageReceived(ChannelHandlerContext& ctx, MessageEvent<DatagramPacket>& e);
//		void WriteComplete(ChannelHandlerContext& ctx, WriteCompletionEvent& e)
//		{
//
//		}
		void ExceptionCaught(ChannelHandlerContext& ctx, ExceptionEvent& e);
};

#endif /* MYMANAGERHANDLER_HPP_ */
