/*
 * ServiceMessageHandler.hpp
 *
 *  Created on: 2011-2-12
 *      Author: qiyingwang
 */

#ifndef NOVA_SERVICEMESSAGEHANDLER_HPP_
#define NOVA_SERVICEMESSAGEHANDLER_HPP_

#include "framework/vcs/virtual_channel_helper.hpp"
#include "buffer/buffer.hpp"
#include "net/socket_inet_address.hpp"
#include "net/socket_host_address.hpp"
#include "net/socket_unix_address.hpp"

using arch::buffer::Buffer;
using arch::net::SocketInetAddress;
using arch::net::SocketHostAddress;
using arch::net::SocketUnixAddress;

namespace arch
{
    namespace framework
    {
        class ServiceProcess;
        /**
         * manager进程和业务子进程之间的IPC event逻辑处理
         * 用户需要实现此虚基类，并注册到service process中
         */
        class ServiceHandler
        {
            private:
                ServiceProcess* m_proc;
                friend class ServiceProcess;
            public:
                ServiceHandler() :
                        m_proc(NULL)
                {
                }
                ServiceProcess& GetServiceProcess();
                virtual bool OnInit()
                {
                    return true;
                }
                virtual bool OnDestroy()
                {
                    return true;
                }
                virtual void OnRoutine()
                {
                }

                /**
                 * 收到其它进程的启动/终止回调
                 */
                virtual void OnServiceProcessStarted(ServiceType src_type,
                        ServiceIndex src_idx)
                {

                }
                /**
                 * 收到其它进程的启动/终止回调
                 */
                virtual void OnServiceProcessStoped(ServiceType src_type,
                        ServiceIndex src_idx)
                {

                }

                /**
                 * 收到其它进程的文件描述符
                 */
                virtual void OnTransferFD(ServiceType src_type,
                        ServiceIndex src_idx, Channel* ch)
                {

                }

                /**
                 * 收到其它进程的IPC消息的回调
                 */
                virtual void OnIPCMessage(ServiceType src_type,
                        ServiceIndex src_idx, Buffer& content)
                {
                }

                /**
                 * 收到TCP消息的回调(由manager进程分发)
                 */
                virtual void OnSocketMessage(uint32 socket_channel_id,
                        Buffer& content)
                {
                }

                /**
                 * 收到UDP消息的回调(由manager进程分发)
                 */
                virtual void OnSocketMessage(uint32 socket_channel_id,
                        Buffer& content,
                        const SocketInetAddress& remote_address)
                {
                }

                /**
                 * 收到socket状态变更的回调(由manager进程分发)
                 */
                virtual void OnSocketOpened(uint32 socket_channel_id)
                {
                }

                /**
                 * 收到socket状态变更的回调(由manager进程分发)
                 */
                virtual void OnSocketConnected(uint32 socket_channel_id,
                        const SocketHostAddress& remote)
                {
                }

                virtual void OnSocketConnected(uint32 socket_channel_id,
                        const SocketUnixAddress& remote)
                {
                }

                /**
                 * 收到socket状态变更的回调(由manager进程分发)
                 */
                virtual void OnSocketConnectFailed(uint32 socket_channel_id,
                        const SocketHostAddress& remote)
                {
                }
                virtual void OnSocketConnectFailed(uint32 socket_channel_id,
                        const SocketUnixAddress& remote)
                {
                }

                /**
                 * 收到socket状态变更的回调(由manager进程分发)
                 */
                virtual void OnSocketClosed(uint32 socket_channel_id)
                {
                }
                virtual ~ServiceHandler()
                {
                }
        };
    }
}

#endif /* SERVICEMESSAGEHANDLER_HPP_ */
