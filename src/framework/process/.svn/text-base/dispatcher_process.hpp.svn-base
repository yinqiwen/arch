/*
 * dispatcher_process.hpp
 *
 *  Created on: 2011-7-22
 *      Author: wqy
 */

#ifndef DISPATCHER_PROCESS_HPP_
#define DISPATCHER_PROCESS_HPP_

#include "ipc/types.hpp"
#include "framework/process/service_process_factory.hpp"
#include "framework/handler/dispatcher_ipc_event_handler.hpp"
#include "framework/handler/dispatcher_handler.hpp"
#include <string>
#include <map>
#include <list>

using std::string;
using namespace arch::ipc;
namespace arch
{
    namespace framework
    {
        /**
         * IO分发进程类，功能等同于与Manager进程的IPC协作部分，在分发成为瓶颈时可应用
         * 用户需要定义自己的类继承此类， 并实现虚方法GetServiceHandler()， GetDispatcherHandler()
         */
        class DispatcherProcess: public ServiceProcess
        {
            private:
                DispatcerIPCEventHandler m_ipc_handler;
                IOProcessHandler* m_process_handler;
                IPCType m_ipc_type;
                bool OnInit();
                void OnRoutine();
                bool OnDestroy();
                Channel* CreateServiceProcessIPCFIFOChannel(const ProcessAddress& addr);
                friend class DispatcerIPCEventHandler;
            protected:
                virtual DispatcherHandler* GetDispatcherHandler() = 0;
            public:
                DispatcherProcess(uint32 maxChannels = 10240);
                DispatcerIPCEventHandler& GetIPCEventHandler()
                {
                    return m_ipc_handler;
                }
                IPCType GetIPCType()
                {
                	return m_ipc_type;
                }
                virtual ~DispatcherProcess()
                {
                }
        };

        class DispatcherProcessFactory: public ServiceProcessFactory
        {
            protected:
                static const char* NAME;
                /**
                 *  Dispatch进程Name固定为"Dispatcher"
                 */
                string GetName()
                {
                    return NAME;
                }
                /**
                 *  Dispatch进程Type固定为DISPATCHER_SERVICE_PROCESS_TYPE(65535)
                 */
                ServiceType GetType()
                {
                    return DISPATCHER_SERVICE_PROCESS_TYPE;
                }

            public:
                virtual IPCType GetIPCType()
                {
                	return IPC_UnixDomainSocket;
                }
                static string GetServerUnixSocketFileName(ServiceIndex idx);
                virtual ~DispatcherProcessFactory()
                {
                }

        };
    }
}
#endif /* DISPATCHER_PROCESS_HPP_ */
