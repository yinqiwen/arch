/*
 * BusinessProcess.hpp
 *
 *  Created on: 2011-1-28
 *      Author: wqy
 */

#ifndef NOVA_BUSINESSPROCESS_HPP_
#define NOVA_BUSINESSPROCESS_HPP_
#include "common/base.hpp"
#include "channel/all_includes.hpp"
#include "ipc/types.hpp"
#include "process/process.hpp"
#include "framework/framework.hpp"
#include "framework/process/service_process_status.hpp"
#include "framework/handler/ipc_event_decoder.hpp"
#include "framework/handler/ipc_event_encoder.hpp"
#include "framework/handler/service_ipc_event_handler.hpp"
#include "framework/handler/service_handler.hpp"
#include "ipc/shm_pool.hpp"
#include "buffer/shm_byte_array.hpp"
#include "framework/vcs/virtual_channel_helper.hpp"
#include <string>
#include <list>
#include <utility>

using std::string;
using namespace arch::ipc;
using arch::channel::Channel;
using arch::channel::ChannelHandler;
using arch::process::Process;
using arch::process::ProcessRunner;
using arch::framework::FrameworkOptions;
using arch::ipc::SharedMemoryPool;
using arch::buffer::SHMByteArray;
using std::list;
using std::pair;
namespace arch
{
    namespace framework
    {
        struct ServiceOptions
        {
                uint32 routine_period;
                uint32 timeout;
                uint32 timeout_count_limit;
                bool restart_when_timeout;

                ServiceOptions() :
                        routine_period(1000), timeout(routine_period * 2), timeout_count_limit(
                                5), restart_when_timeout(true)
                {
                }
        };

        class ManagerProcess;
        class ManagerIPCEventHandler;
        class ServiceProcessFactory;
        /**
         * 业务进程类
         * 用户需要定义自己的类继承此类， 并实现虚方法GetServiceHandler()
         */
        class ServiceProcess: public ProcessRunner,
                public Runnable,
                public SignalHandler
        {
            protected:
                //uint32 m_max_channels;
                bool m_in_child_service_process;

                /**
                 * Service process status monitored by manager process
                 */
                ServiceProcessStatus* m_status;

                IPCEventDecoder m_decoder;
                IPCEventEncoder m_encoder;
                ServiceIPCEventHandler m_ipc_event_handler;
                volatile bool m_running;
                int Execute();
                void Run();
                void BaseInit(FrameworkOptions options);
                void OnSignal(int signo, siginfo_t& info);

                //typedef SharedMemoryPool<IPC_MEM_POOL> IpcShmPool;
                typedef SharedMemoryAllocator<SHMByteArray*, IPC_FIFO_SHM_POOL> IpcShmFIFOAllocator;
                typedef std::list<SHMByteArray*, IpcShmFIFOAllocator> SHMFIFO;

                typedef std::map<std::string, Channel*> DelegateChannelTable;
                DelegateChannelTable m_delegate_channel_table;
                void SetDelegateChannelTable(const DelegateChannelTable& table);

                typedef std::map<ProcessAddress, Channel*> IPCChannelTable;
                IPCChannelTable m_dispacher_ipc_channels;

                void CheckDispatcherIPCChannelTable();
                int ConnectDispatcher(const ProcessAddress& addr);
                ClientSocketChannel* ConnectManagerUnixSocketServer(
                        SocketUnixAddress& server_path,
                        SocketUnixAddress& local_path);
                int ConnectManagerIPCUnixSocketServer();
                int ConnectManagerCtrlUnixSocketServer();
            protected:
                ChannelService* m_channel_service;
                ServiceProcessFactory* m_factory;
                FIFOChannel* m_fifo;
                SharedMemoryFIFOChannel* m_shm_fifo;
                ClientSocketChannel* m_unix_sock;
                Channel* m_ctrl_ch;
                ServiceOptions m_options;

                ServiceIndex m_index;
                pid_t m_pid;
                uint32 m_ipc_channel_ID;
                IPCType m_dispatcher_ipc_type;
                ServiceProcessStatus* GetStatus()
                {
                    return m_status;
                }

                void SetFIFO(FIFOChannel* ch);
                void SetShmFIFO(SharedMemoryFIFOChannel* ch);
                void SetIPCUnixSocket(ClientSocketChannel* ch);
                void SetCtrlChannel(Channel* ch);
                void SetProcessIndex(ServiceIndex index);
                void SetServiceProcessFactory(ServiceProcessFactory* factory);
                IPCEventDecoder& GetIPCEventDecoder()
                {
                    return m_decoder;
                }
                IPCEventEncoder& GetIPCEventEncoder()
                {
                    return m_encoder;
                }
                void BaseRoutine();
                bool ConnectDispatchers();
                void AddClosedDispatcherIPCChannel(Channel* ch);

                friend class ManagerProcess;
                friend class ManagerIPCEventHandler;
                friend class ServiceIPCEventHandler;
                friend class ServiceProcessFactory;

                virtual bool OnInit();
                virtual void OnRoutine();
                virtual bool OnDestroy();
            public:
                ServiceProcess(uint32 maxChannels = 10240);
                ServiceIndex GetProcessIndex()
                {
                    return m_index;
                }
                ServiceType GetServiceType();
                std::string GetServiceName();
                /**
                 *  获取用于与manager进程IPC通信逻辑的handler
                 */
                virtual ServiceHandler* GetServiceHandler() = 0;
                inline ServiceProcessFactory* GetServiceProcessFactory()
                {
                    return m_factory;
                }
                pid_t GetPID()
                {
                    return m_pid;
                }
                /**
                 * 获取用于进程间通信的FIFOChannel
                 */
                Channel* GetIPCChannel();
                Channel* GetCtrlChannel();
                void CloseIPCChannel();
                Channel* GetDispatcherIPCChannel(uint32 index);
                Channel* GetDelegateChannel(const std::string& name);
                Channel* GetDelegateChannel(int fd);
                ManagerProcess* GetManagerProcess();
                inline ChannelService& GetChannelService()
                {
                    return *m_channel_service;
                }
                void HandleReadEvent();
                virtual ~ServiceProcess();

        };
    }
}

extern arch::framework::ServiceProcess* g_service_proc;

#endif /* BUSINESSPROCESS_HPP_ */
