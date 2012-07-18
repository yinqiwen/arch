/*
 * MyDispatcherProcess.hpp
 *
 *  Created on: 2011-7-23
 *      Author: wqy
 */

#ifndef MYDISPATCHERPROCESS_HPP_
#define MYDISPATCHERPROCESS_HPP_
#include "framework/process/dispatcher_process.hpp"
#include "channel/all_includes.hpp"
#include "MyServiceHandler.hpp"
#include "MyDispatcherHandler.hpp"
using namespace arch::framework;

class MyDispatcherProcess: public DispatcherProcess
{
    private:
		MyDispatcherHandler handler;
        DispatcherHandler* GetDispatcherHandler()
		{
        	return &handler;
		}
        ServiceHandler* GetServiceHandler()
        {
            return NULL;
        }
};

class MyDispatcherProcessFactory: public DispatcherProcessFactory
{
    protected:
        ServiceProcess* CreateServiceProcess()
        {
            return new MyDispatcherProcess;
        }
        void DestroyServiceProcess(ServiceProcess* proc)
        {
            delete proc;
        }
    public:
        ServiceOptions GetOptions()
        {
            ServiceOptions opt;
            opt.routine_period = 5000;
            opt.timeout = 8000;
            opt.timeout_count_limit = 3;
            return opt;
        }
        ServiceIndex GetProcessCount()
        {
            return 2;
        }
		IPCType GetIPCType()
		{
			return IPC_UnixDomainSocket;
		}
};

#endif /* MYDISPATCHERPROCESS_HPP_ */
