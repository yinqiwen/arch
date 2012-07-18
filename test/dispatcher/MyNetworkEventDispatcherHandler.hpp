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
class MyDispatcherProcess;
class MyManagerIPCHandler;
class MyNetworkEventDispatcherHandler: public ChannelUpstreamHandler<Buffer> ,
        public ChannelUpstreamHandler<DatagramPacket>
{
	private:
		MyManagerProcess* m_manager;
		MyDispatcherProcess* m_dispatcher;
		friend class MyManagerProcess;
		friend class MyManagerIPCHandler;
		friend class MyDispatcherProcess;
		void ChannelBound(ChannelHandlerContext& ctx, ChannelStateEvent& e)
		{

		}
		void ChannelConnected(ChannelHandlerContext& ctx, ChannelStateEvent& e);
		void ChannelOpen(ChannelHandlerContext& ctx, ChannelStateEvent& e)
		{

		}
		void ChannelClosed(ChannelHandlerContext& ctx, ChannelStateEvent& e);
		void MessageReceived(ChannelHandlerContext& ctx,
		        MessageEvent<Buffer>& e);
		void MessageReceived(ChannelHandlerContext& ctx, MessageEvent<
		        DatagramPacket>& e);
		//		void WriteComplete(ChannelHandlerContext& ctx, WriteCompletionEvent& e)
		//		{
		//
		//		}
		void ExceptionCaught(ChannelHandlerContext& ctx, ExceptionEvent& e);
	public:
		MyNetworkEventDispatcherHandler(MyDispatcherProcess* dis) :
			m_manager(NULL), m_dispatcher(dis)
		{
		}
		MyNetworkEventDispatcherHandler(MyManagerProcess* manager) :
			m_manager(manager), m_dispatcher(NULL)
		{
		}
};

#endif /* MYMANAGERHANDLER_HPP_ */
