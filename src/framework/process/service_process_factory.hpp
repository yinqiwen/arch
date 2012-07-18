/*
 * ServiceProcessFactory.hpp
 *
 *  Created on: 2011-1-31
 *      Author: wqy
 */

#ifndef NOVA_SERVICEPROCESSFACTORY_HPP_
#define NOVA_SERVICEPROCESSFACTORY_HPP_

#include "framework/process/service_process.hpp"
#include <string>
#include <map>
#include <list>
using std::string;
namespace arch
{
	namespace framework
	{
		/**
		 * 业务进程的工厂类, 用户需要继承此类提供详细的业务进程创建参数/逻辑
		 * 需要注册到ManagerProcess中使用
		 */
	    class ManagerProcess;
		class ServiceProcessFactory
		{
			protected:
				typedef std::map<ServiceIndex, ServiceProcess*>
				        ServiceProcessTable;
				ServiceProcessTable m_serv_proc_table;
				ServiceProcess::DelegateChannelTable m_delegate_channel_table;
				virtual ServiceProcess* CreateServiceProcess() = 0;
				virtual void DestroyServiceProcess(ServiceProcess* proc) = 0;
				friend class ManagerProcess;
			public:
				/*
				 * 委派channel管理到子进程组中，并注册到子进程的event loop中；一般由父进程Manager调用
				 */
				int DelegateChannel(const std::string& name, Channel* ch);
				ServiceProcess* NewServiceProcess(ServiceIndex index);
				void DeleteServiceProcess(ServiceProcess* proc);
				void DeleteAllServiceProcess();
				size_t GetActiveServiceProcessSize();
				void GetActiveServiceProcessList(
				        list<ServiceProcess*>& serv_list);
				ServiceProcess* GetServiceProcessByPID(pid_t pid);
				ServiceProcess* GetServiceProcessByIndex(ServiceIndex idx);
				virtual string GetName() = 0;
				virtual ServiceType GetType() = 0;
				virtual ServiceOptions GetOptions()
				{
					ServiceOptions default_opt;
					return default_opt;
				}
				virtual ServiceIndex GetProcessCount()
				{
					return 1;
				}
				virtual ~ServiceProcessFactory();
		};
	}
}

#endif /* SERVICEPROCESSFACTORY_HPP_ */
