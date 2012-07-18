/*
 * ManagerProcess.hpp
 *
 *  Created on: 2011-1-28
 *      Author: wqy
 */

#ifndef NOVA_MANAGERPROCESS_HPP_
#define NOVA_MANAGERPROCESS_HPP_
#include "common/base.hpp"
#include "channel/all_includes.hpp"
#include "process/process.hpp"
#include "framework/process/service_process_factory.hpp"
#include "framework/handler/manager_ipc_event_handler.hpp"
#include "framework/admin/admin_message_handler.hpp"
#include "framework/admin/admin_command_handler.hpp"
#include "framework/framework.hpp"
#include "framework/handler/manager_handler.hpp"

#include <utility>
#include <vector>
#include <list>

using arch::process::ProcessRunner;
using arch::channel::ChannelService;
using arch::channel::codec::DelimiterBasedFrameDecoder;
using arch::concurrent::SignalHandler;
using arch::ipc::SharedMemoryPool;
using arch::buffer::Buffer;
using std::vector;
namespace arch
{
	namespace framework
	{
		/**
		 * 主控manager进程类， 管理业务进程
		 * 用户需要定义自己的类继承此类， 并实现虚方法GetManagerHandler()
		 * 进程间通信handler关系如下图
		 *
		 *    ServiceHandler0       ServiceHandler1   ...  ServiceHandlerN
		 *           |                     |                      |
		 *    ServiceProcess0       ServiceProcess1   ...  ServiceProcessN
		 *          /|\                   /|\                    /|\
         *           |                     |                      |
		 *           |--------------->ManagerHandler<-------------|
		 *                               |     |
		 *                     ManagerProcess  |----(NetworkMessageHandlers: Optional)
		 *                                                 /|\
         *                                                  |
		 *                                                 \|/
		 *                                             Network I/O
		 *
		 */
		class ManagerProcess: public ProcessRunner,
		        public Runnable,
		        public SignalHandler
		{
			private:
				int Execute();
				void Run();
				void ClearAllServiceProcesses();

				void LogPid4ProcessesInFile();
				void OnSignal(int signo, siginfo_t& info);
				void TryStop();
				//volatile bool m_running;
			protected:
				Channel* m_admin_channel;
				//DelimiterBasedFrameDecoder m_admin_msg_decoder;
				AdminMessageHandler m_admin_msg_handler;
				ChannelService m_channel_service;
				ManagerIPCEventHandler m_ipc_event_handler;
				FrameworkOptions m_options;
				typedef map<ServiceType, ServiceProcessFactory*> ServiceProcessFactoryTable;
				ServiceProcessFactoryTable m_service_factory_table;
				typedef map<int, ServiceIndex> DelegateChannelControlProcessTable;
				DelegateChannelControlProcessTable m_delegate_channel_control_table;

				SharedMemoryPool* m_shm;
				void BaseRoutine();
				/**
				 * 初始化回调
				 */
				virtual bool OnInit()
				{
					return true;
				}
				/**
				 * 销毁回调
				 */
				virtual bool OnDestroy()
				{
					return true;
				}
				virtual void OnServiceRoutineTimeout(ServiceProcess* serv);
				int Wait4AllServiceProcStarted();
				//void HandleDeadServiceProcesses();
				void CreateServiceProcess(ServiceProcessFactory* pfactory,
				        ServiceIndex index);

				void SetControlProcess4FD(int fd, ServiceIndex idx)
				{
					m_delegate_channel_control_table[fd] = idx;
				}
				/**
				 * 例行检查回调
				 */
				virtual void OnRoutine();

				friend class ManagerIPCEventHandler;
				friend class ServiceProcessFactory;
				friend class IOIPCEventHandler;
			public:
				ManagerProcess(FrameworkOptions options, uint32 maxChannels =
				        10240);
				/**
				 *  获取用于与业务子进程IPC通信逻辑的handler
				 */
				virtual ManagerHandler* GetManagerHandler() = 0;
				ChannelService& GetChannelService()
				{
					return m_channel_service;
				}
				SharedMemoryPool* GetIPCSharedMemoryPool()
				{
					return m_shm;
				}
				FrameworkOptions& GetFrameworkOptions()
				{
					return m_options;
				}
				void RegisterServiceProcessFactory(
				        ServiceProcessFactory* pfactory) throw ();
				void Start();

				int CurrentControlProcess(Channel* ch, ServiceIndex& idx);
				ServiceProcess* GetServiceProcess(ServiceType type,
				        ServiceIndex index);
				ServiceProcess* GetServiceProcess(pid_t pid);
				ServiceProcessFactory* GetServiceProcessFactory(
				        ServiceType type);
				void
				GetServiceProcessList(std::list<ServiceProcess*>& proc_list,
				        ServiceType type);
				void GetAllServiceProcessFactory(
				        std::list<ServiceProcessFactory*>& proc_factory_list);
				void KillServiceProcess(ServiceProcess* proc);

				ManagerIPCEventHandler& GetIPCEventHandler()
				{
					return m_ipc_event_handler;
				}

				uint32 GetDispatcherCount();
				void PrintPid4Processes(Buffer& buf);
				int GetDispatcherClientUnixSocketFileName(
				        const ProcessAddress& addr, uint32 index,
				        std::string& result);
				int ParseDispatcherClientUnixSocketAddress(
				        const std::string& unixFileName, ProcessAddress& addr);
				int GetIPCClientUnixSocketFileName(const ProcessAddress& addr,
				        std::string& result);
				int ParseIPCClientUnixSocketAddress(
				        const std::string& unixFileName, ProcessAddress& addr);
				bool IsManagerIPCUnixSocketServer(const std::string& path);
				bool IsManagerCtrlUnixSocketServer(const std::string& path);
				virtual ~ManagerProcess();
		};
	}
}

extern arch::framework::ManagerProcess* g_manager_proc;

#endif /* MANAGERPROCESS_HPP_ */
