/*
 * dispatcher_handler.hpp
 *
 *  Created on: 2011-7-23
 *      Author: wqy
 */

#ifndef IO_HANDLER_HPP_
#define IO_HANDLER_HPP_

#include "buffer/buffer.hpp"
#include "channel/all_includes.hpp"
#include "framework/framework.hpp"

using arch::buffer::Buffer;
using arch::channel::Channel;

namespace arch
{
    namespace framework
    {
        /**
         * IO进程和业务子进程之间的IO IPC event逻辑处理
         * 用户需要实现此虚基类，并注册到manager/dispatcher IO process中
         */
        struct IOProcessHandler
        {
                virtual bool OnInit()
                {
                    return true;
                }
                virtual bool OnDestroy()
                {
                    return true;
                }
                /**
                 * 例行检查回调
                 */
                virtual void OnRoutine()
                {
                }

                virtual void OnIPCChannelFlushComplete(Channel* ch)
                {

                }

                virtual void OnChannelWriteFailed(Channel* ch)
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
                 * 收到业务子进程发给当前进程的消息回调（不是发给当前进程的需要中转的消息）
                 */
                virtual void OnIPCMessage(ServiceType type, ServiceIndex idx,
                        Buffer& content)
                {
                }
                /**
                 * 收到业务子进程创建连接外部TCP/UDP/UNIX socket server的请求回调
                 */
                virtual Channel* TCPSocketConnectRequest(
                        const std::string& host, uint16 port)
                {
                    return NULL;
                }

                virtual Channel* UDPSocketConnectRequest(
                        const std::string& host, uint16 port)
                {
                    return NULL;
                }
                virtual Channel* UnixSocketConnectRequest(
                        const std::string& remote)
                {
                    return NULL;
                }

                /**
                 * 收到业务子进程创建TCP/UDP/UNIX socket请求回调
                 */
                virtual Channel* TCPSocketOpenRequest()
                {
                    return NULL;
                }
                virtual Channel* UDPSocketOpenRequest()
                {
                    return NULL;
                }
                virtual Channel * UnixSocketOpenRequest(
                        const std::string& local)
                {
                    return NULL;
                }
                virtual ~IOProcessHandler()
                {

                }
        };
    }
}
#endif /* DISPATCHER_HANDLER_HPP_ */
