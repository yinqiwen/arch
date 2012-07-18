/*
 * ServiceMessageHandler.hpp
 *
 *  Created on: 2011-2-12
 *      Author: qiyingwang
 */

#ifndef NOVA_SERVICEINTERNALMESSAGEHANDLER_HPP_
#define NOVA_SERVICEINTERNALMESSAGEHANDLER_HPP_
#include "common/base.hpp"
#include "channel/all_includes.hpp"
#include "framework/event/ipc_event.hpp"
#include "framework/handler/service_handler.hpp"
#include "framework/admin/admin_command_handler.hpp"

using arch::channel::ChannelUpstreamHandler;
using arch::channel::ChannelHandlerContext;
using arch::channel::ExceptionEvent;
using arch::channel::MessageEvent;
using arch::channel::codec::FrameDecoder;
namespace arch
{
	namespace framework
	{
		class ServiceProcess;
        /**
         * manager进程与业务子进程之间的业务子进程侧IPC消息处理handler
         * 此handler尽量处理可以理解的event， 无法处理的event则交由用户注册的ServiceHandler处理
         */
		class ServiceIPCEventHandler: public ChannelUpstreamHandler<IPCEvent>
		{
			private:
				ServiceHandler* m_handler;
				ServiceProcess* m_service;
				bool m_has_dispatcher;
				friend class ServiceProcess;
				void ChannelConnected(ChannelHandlerContext& ctx,
								        ChannelStateEvent& e);
				void ChannelClosed(ChannelHandlerContext& ctx,
								        ChannelStateEvent& e);
				void MessageReceived(ChannelHandlerContext& ctx,
				        MessageEvent<IPCEvent>& e);
				void SetServiceProcess(ServiceProcess* proc);
			public:
				ServiceIPCEventHandler() :
					m_handler(NULL), m_service(NULL),m_has_dispatcher(false)
				{
				}
				ServiceProcess* GetServiceProcess()
				{
					return m_service;
				}
		};
	}
}
#endif /* SERVICEMESSAGEHANDLER_HPP_ */
