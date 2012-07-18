/*
 * InternalMessageDispatchHandler.hpp
 *
 *  Created on: 2011-2-6
 *      Author: wqy
 */

#ifndef NOVA_INTERNALMESSAGEDISPATCHHANDLER_HPP_
#define NOVA_INTERNALMESSAGEDISPATCHHANDLER_HPP_
#include "common/base.hpp"
#include "channel/all_includes.hpp"
#include "framework/event/ipc_event_factory.hpp"
#include "framework/handler/manager_handler.hpp"
#include "framework/handler/io_ipc_event_handler.hpp"
#include "net/socket_inet_address.hpp"
#include <map>
#include <list>
using arch::channel::ChannelUpstreamHandler;
using arch::net::SocketInetAddress;
using std::map;
using std::list;
namespace arch
{
	namespace framework
	{
		class ManagerProcess;
//		struct ServiceChannelBindingItem
//		{
//				Channel* ch;
//				ServiceProcess* serv_roc;
//				SocketHostAddress connect_addr;
//				bool is_channel_connected;
//		};


		/**
		 * manager进程与业务子进程之间的manager侧IPC消息处理handler
		 * 此handler尽量处理可以理解的event， 无法处理的event则交由用户注册的ManagerHandler处理
		 * 此外，此handler也提供一些外部方法用于主动生成IPC event并分发给业务子进程
		 */
		class ManagerIPCEventHandler: public IOIPCEventHandler
		{
			private:
				ManagerProcess* m_manager;
				ManagerHandler* m_handler;
				friend class ManagerProcess;
			protected:
				void ChannelConnected(ChannelHandlerContext& ctx,
								        ChannelStateEvent& e);
				ManagerProcess* GetManagerProcess()
				{
					return m_manager;
				}
				ChannelService* GetChannelService();
				ServiceType GetServiceProcessType();
				ServiceIndex GetServiceProcessIndex();
				Channel* GetServiceProcessIPCChannel(ServiceType type,
				        ServiceIndex idx);
				void GetAllGetServiceProcessIPCChannels(
				        std::list<Channel*>& channelList);
			public:
				ManagerIPCEventHandler();
				virtual ~ManagerIPCEventHandler()
				{
				}
		};
	}
}

#endif /* INTERNALMESSAGEDISPATCHHANDLER_HPP_ */
