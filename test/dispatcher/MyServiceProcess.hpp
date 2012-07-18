/*
 * MyServiceProcess.hpp
 *
 *  Created on: 2011-2-7
 *      Author: wqy
 */

#ifndef MYSERVICEPROCESS_HPP_
#define MYSERVICEPROCESS_HPP_
#include "framework/process/service_process.hpp"
#include "channel/all_includes.hpp"
#include "MyServiceHandler.hpp"
using namespace arch::framework;

class MyServiceProcess: public ServiceProcess
{
    private:
        MyServiceHandler handler;
        ServiceHandler* GetServiceHandler()
        {
            return &handler;
        }
};

class MyServiceProcessFactory: public ServiceProcessFactory
{
    protected:
        ServiceProcess* CreateServiceProcess()
        {
            return new MyServiceProcess;
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
        string GetName()
        {
            return "HYK";
        }
        ServiceType GetType()
        {
            return 1;
        }
        ServiceIndex GetProcessCount()
        {
            return 1;
        }
};

#endif /* MYSERVICEPROCESS_HPP_ */
