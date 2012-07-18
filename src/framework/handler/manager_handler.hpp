/*
 * ServiceMessageHandler.hpp
 *
 *  Created on: 2011-2-12
 *      Author: qiyingwang
 */

#ifndef NOVA_MANAGERHANDLER_HPP_
#define NOVA_MANAGERHANDLER_HPP_
#include "channel/all_includes.hpp"
#include "buffer/buffer.hpp"
#include "framework/handler/io_process_handler.hpp"

using arch::buffer::Buffer;
using arch::channel::Channel;

namespace arch
{
    namespace framework
    {
        class ManagerProcess;
        class ServiceProcess;
        /**
         * manager进程和业务子进程之间的IPC event逻辑处理
         * 用户需要实现此虚基类，并注册到manager process中
         */
        class ManagerHandler:public IOProcessHandler
        {
            private:
                ManagerProcess* m_proc;
                friend class ManagerProcess;
            public:
                ManagerHandler() :
                    m_proc(NULL)
                {
                }
                ManagerProcess& GetManagerProcess();

                virtual ~ManagerHandler()
                {
                }
        };
    }
}

#endif /* SERVICEMESSAGEHANDLER_HPP_ */
