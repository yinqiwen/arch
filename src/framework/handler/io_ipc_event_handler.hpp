/*
 * dispatcher_ipc_event_handler.hpp
 *
 *  Created on: 2011-7-23
 *      Author: wqy
 */

#ifndef IO_IPC_EVENT_HANDLER_HPP_
#define IO_IPC_EVENT_HANDLER_HPP_

#include "common/base.hpp"
#include "channel/all_includes.hpp"
#include "framework/event/ipc_event_factory.hpp"
#include "framework/handler/io_process_handler.hpp"
#include "net/socket_inet_address.hpp"
#include "net/socket_host_address.hpp"
#include "net/socket_unix_address.hpp"
#include <map>
#include <list>
#include <string>
using arch::channel::ChannelUpstreamHandler;
using arch::net::SocketInetAddress;
using arch::net::SocketHostAddress;
using arch::net::SocketUnixAddress;
using std::map;
using std::list;
using std::string;

namespace arch
{
    namespace framework
    {
        struct ServiceProcessChannelBindingItem
        {
                Channel* ch;
                ServiceType serv_type;
                ServiceIndex serv_idx;
                SocketUnixAddress unix_addr;
                SocketHostAddress host_addr;
                bool is_channel_connected;
                bool is_host_addr;
                ServiceProcessChannelBindingItem() :
                    is_channel_connected(false), is_host_addr(false)
                {
                }
                void SetAddress(SocketUnixAddress* addr)
                {
                    unix_addr = *addr;
                    is_host_addr = false;
                }
                void SetAddress(SocketHostAddress* addr)
                {
                    host_addr = *addr;
                    is_host_addr = true;
                }
                SocketUnixAddress* GetUnixAddress()
                {
                    return is_host_addr ? NULL : &unix_addr;
                }
                SocketHostAddress* GetHostAddress()
                {
                    return is_host_addr ? &host_addr : NULL;
                }

        };
        /**
         * manager进程与业务子进程之间的manager侧IPC消息处理handler
         * 此handler尽量处理可以理解的event， 无法处理的event则交由用户注册的ManagerHandler处理
         * 此外，此handler也提供一些外部方法用于主动生成IPC event并分发给业务子进程
         */
        class IOIPCEventHandler: public ChannelUpstreamHandler<IPCEvent>
        {
            private:
                IOProcessHandler* m_handler;

                void MessageReceived(ChannelHandlerContext& ctx, MessageEvent<
                        IPCEvent>& e);
//                void WriteComplete(ChannelHandlerContext& ctx,
//                        ChannelStateEvent& e);
                void WriteFailed(Channel* ch);

                void BroadcastServiceProcessStarted(ServiceType type,
                        ServiceIndex idx);
                void NotifyerviceProcessStarted(ServiceType src_type,
                        ServiceIndex src_idx, ServiceType dst_type,
                        ServiceIndex dst_idx);
                void BroadcastServiceProcessStoped(ServiceType type,
                        ServiceIndex idx);
                void WriteIPCEvent(IPCEvent& ev, ServiceType dstType, ServiceIndex dstIndex);
                friend class ManagerProcess;
            protected:
                map<Channel*, ServiceProcessChannelBindingItem>
                        m_channel_service_binding_table;
                list<SocketCtrlEvent*> m_pending_ctrl_events;
                void SetIOProcessHandler(IOProcessHandler* handler);
                //bool SendSocketMessage(uint32 dstChannelID, Buffer& content);
                void HandleSocketCtrlEvent(Channel* src_channel,
                        ServiceType type, ServiceIndex idx,
                        SocketCtrlEvent* event);
                void HandleIPCMessageEvent(Channel* src_channel,
                        ServiceType type, ServiceIndex idx,
                        IPCMessageEvent* event);

                bool BroadcastServiceSocketClosed(uint32 channelID);

                virtual ServiceType GetServiceProcessType() = 0;
                virtual ServiceIndex GetServiceProcessIndex() = 0;

                virtual void GetAllGetServiceProcessIPCChannels(std::list<
                        Channel*>& channelList) = 0;
                ServiceType m_type;
                ServiceIndex m_idx;
                ChannelService* m_channel_service;
            public:
                IOIPCEventHandler();
                virtual ChannelService* GetChannelService() = 0;
                int Init();
                virtual Channel* GetServiceProcessIPCChannel(ServiceType type,
                        ServiceIndex idx) = 0;
                bool DispatchTcpMsg(ServiceType type, ServiceIndex idx,
                        Channel* src_channel, Buffer& content);
                bool DispatchUdpMsg(ServiceType type, ServiceIndex idx,
                        Channel* src_channel, SocketInetAddress& address,
                        Buffer& content);
                bool DispatchIPCMsg(ServiceType type, ServiceIndex idx,
                        Buffer& content, IPCEventType event_type = IPC_MSG);
                bool NotifyServiceSocketConnected(Channel* channel,
                        bool broadcast = false);
                bool NotifyServiceSocketClosed(Channel* channel,
                        bool broadcast = false);
                bool NotifyServiceSocketOpened(Channel* channel,
                        bool broadcast = false);
                bool IsServiceIPCOverflow(ServiceType type, ServiceIndex idx,
                        uint32 expectedMaxSize);
                virtual ~IOIPCEventHandler()
                {
                }
        };
    }
}

#endif /* DISPATCHER_IPC_EVENT_HANDLER_HPP_ */
