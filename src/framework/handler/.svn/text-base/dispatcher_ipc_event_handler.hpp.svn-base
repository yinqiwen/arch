/*
 * dispatcher_ipc_event_handler.hpp
 *
 *  Created on: 2011-7-23
 *      Author: wqy
 */

#ifndef DISPATCHER_IPC_EVENT_HANDLER_HPP_
#define DISPATCHER_IPC_EVENT_HANDLER_HPP_
#include "framework/handler/io_ipc_event_handler.hpp"
namespace arch
{
	namespace framework
	{
		class DispatcherProcess;
		class DispatcerIPCEventHandler: public IOIPCEventHandler
		{
			private:
				typedef std::list<Channel*> ChannelList;
				ChannelList m_client_channel_list;
				typedef std::map<ProcessAddress, Channel*> IPCChannelTable;
				IPCChannelTable m_ipc_channel_table;
				void ChannelConnected(ChannelHandlerContext& ctx,
				        ChannelStateEvent& e);
				void ChannelClosed(ChannelHandlerContext& ctx,
				        ChannelStateEvent& e);
				ChannelService* GetChannelService();
				ServiceType GetServiceProcessType();
				ServiceIndex GetServiceProcessIndex();
				Channel* GetServiceProcessIPCChannel(ServiceType type,
				        ServiceIndex idx);
				void GetAllGetServiceProcessIPCChannels(
				        std::list<Channel*>& channelList);
                void AddServiceIPCChannel(const ProcessAddress& addr, Channel*);
				friend class DispatcherProcess;
		};
	}
}

#endif /* DISPATCHER_IPC_EVENT_HANDLER_HPP_ */
