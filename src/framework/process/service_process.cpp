/*
 * ServiceProcess.cpp
 *
 *  Created on: 2011-1-29
 *      Author: wqy
 */
#include "framework/process/service_process.hpp"
#include "framework/process/manager_process.hpp"
#include "framework/process/dispatcher_process.hpp"
#include "framework/process/service_process_factory.hpp"
#include "net/socket_unix_address.hpp"
#include "exception/exception.hpp"
#include "util/time_helper.hpp"
#include "util/perftools_helper.h"
#include "logging/logger_macros.hpp"
#include <signal.h>
#include <sstream>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

using namespace arch::framework;
using namespace arch::util;
using namespace arch::channel::fifo;
using arch::net::SocketUnixAddress;

arch::framework::ServiceProcess* g_service_proc = NULL;

ServiceProcess::ServiceProcess(uint32 maxChannels) :
		m_in_child_service_process(false), m_status(NULL), m_running(true), m_channel_service(
				NULL), m_factory(NULL), m_fifo(NULL), m_shm_fifo(NULL), m_unix_sock(
				NULL), m_ctrl_ch(NULL), m_index(0), m_ipc_channel_ID(0), m_dispatcher_ipc_type(
				IPC_UnixDomainSocket)
{
	NEW(m_channel_service, ChannelService(maxChannels));
}

void ServiceProcess::SetDelegateChannelTable(const DelegateChannelTable& table)
{
	m_delegate_channel_table = table;
}

Channel* ServiceProcess::GetDelegateChannel(const std::string& name)
{
	DelegateChannelTable::iterator found = m_delegate_channel_table.find(name);
	if (found != m_delegate_channel_table.end())
	{
		return found->second;
	}
	return NULL;
}

Channel* ServiceProcess::GetDelegateChannel(int fd)
{
	DelegateChannelTable::iterator it = m_delegate_channel_table.begin();
	while (it != m_delegate_channel_table.end())
	{
		if (it->second->GetReadFD() == fd)
		{
			return it->second;
		}
		it++;
	}
	return NULL;
}

ManagerProcess* ServiceProcess::GetManagerProcess()
{
	return g_manager_proc;
}

std::string ServiceProcess::GetServiceName()
{
	return m_factory->GetName();
}

void ServiceProcess::OnSignal(int signo, siginfo_t& info)
{
	char* sigstr = strsignal(signo);
	WARN_LOG(
			"Received signal(%s:%d) from process:%u", NULL != sigstr?sigstr:"UnknownSignal", signo, info.si_pid);
	if (signo == SIGQUIT)
	{
		m_running = false;
		GetChannelService().Stop();
	}
	else if (signo == SIGUSR1)
	{

	}
	else if (signo == SIGUSR2)
	{
		//ProfilerStop();
	}
}

void ServiceProcess::BaseInit(FrameworkOptions options)
{
	//m_options = options;
	void* mem = g_manager_proc->GetIPCSharedMemoryPool()->Malloc(
			sizeof(ServiceProcessStatus));
	if (NULL == mem)
	{
		ERROR_LOG("Failed to malloc mem from shm.");
		return;
	}
	m_status = new (mem) ServiceProcessStatus(m_options.timeout_count_limit,
			m_options.timeout);
}

void ServiceProcess::SetProcessIndex(ServiceIndex index)
{
	m_index = index;
}

void ServiceProcess::SetServiceProcessFactory(ServiceProcessFactory* factory)
{
	m_factory = factory;
}

ServiceType ServiceProcess::GetServiceType()
{
	return m_factory->GetType();
}

void ServiceProcess::SetFIFO(FIFOChannel* ch)
{
	m_fifo = ch;
	m_ipc_channel_ID = ch->GetID();
}

void ServiceProcess::SetShmFIFO(SharedMemoryFIFOChannel* ch)
{
	m_shm_fifo = ch;
	m_ipc_channel_ID = ch->GetID();
}

void ServiceProcess::SetIPCUnixSocket(ClientSocketChannel* ch)
{
	m_unix_sock = ch;
	if (NULL != ch)
	{
		m_ipc_channel_ID = ch->GetID();
	}
	else
	{
		m_ipc_channel_ID = 0;
	}
}

void ServiceProcess::SetCtrlChannel(Channel* ch)
{
	m_ctrl_ch = ch;
}

Channel* ServiceProcess::GetCtrlChannel()
{
	return m_ctrl_ch;
}

Channel* ServiceProcess::GetIPCChannel()
{
	switch (g_manager_proc->GetFrameworkOptions().ipc_type)
	{
		case IPC_SharedMemory:
		{
			return m_shm_fifo;
		}
		case IPC_FIFO:
		{
			return m_fifo;
		}
		case IPC_UnixDomainSocket:
		{
			return m_unix_sock;
		}
		default:
		{
			return NULL;
		}
	}
}

void ServiceProcess::HandleReadEvent()
{
	Channel* ch = GetIPCChannel();
	ch->HandleReadEvent();
}

void ServiceProcess::CloseIPCChannel()
{
	Channel* ch = GetIPCChannel();
	if (NULL != ch)
	{
		ch->Close();
	}
	m_shm_fifo = NULL;
	m_fifo = NULL;
	m_unix_sock = NULL;
}

void ServiceProcess::BaseRoutine()
{
	uint64 now = get_current_monotonic_millis();
	m_status->Ping(now);
	//exit event loop while orphan process detected
	if (getppid() == 1)
	{
		WARN_LOG(
				"Exit service process since current process is an orphan now.");
		GetChannelService().Stop();
	}
	CheckDispatcherIPCChannelTable();
	if (g_manager_proc->GetFrameworkOptions().ipc_type == IPC_UnixDomainSocket)
	{
		if (NULL == m_unix_sock)
		{
			ConnectManagerIPCUnixSocketServer();
		}
	}
	if (g_manager_proc->GetFrameworkOptions().enable_ctrl_channel
			&& NULL == m_ctrl_ch)
	{
		ConnectManagerCtrlUnixSocketServer();
	}
}

bool ServiceProcess::OnInit()
{
	return true;
}

bool ServiceProcess::OnDestroy()
{
	return true;
}

void ServiceProcess::OnRoutine()
{
}
//routine method
void ServiceProcess::Run()
{
	if (m_running)
	{
		BaseRoutine();
		GetChannelService().Routine();
		if (NULL != m_ipc_event_handler.m_handler)
		{
			m_ipc_event_handler.m_handler->OnRoutine();
		}
		OnRoutine();
	}
}

static void IPCChannelInit(ChannelPipeline* pipeline, void* data)
{
	ServiceIPCEventHandler* ipchandler = (ServiceIPCEventHandler*) data;
	IPCEventEncoder* encoder = NULL;
	NEW(encoder, IPCEventEncoder);
	IPCEventDecoder* decoder = NULL;
	NEW(decoder, IPCEventDecoder);
	pipeline->AddLast("decoder", decoder);
	pipeline->AddLast("encoder", encoder);
	pipeline->AddLast("handler", ipchandler);
}

static void IPCChannelFinalize(ChannelPipeline* pipeline, void* data)
{
	ChannelHandler* decoder = pipeline->Get("decoder");
	DELETE(decoder);
	ChannelHandler* encoder = pipeline->Get("encoder");
	DELETE(encoder);
}

void ServiceProcess::CheckDispatcherIPCChannelTable()
{
	IPCChannelTable::iterator it = m_dispacher_ipc_channels.begin();
	while (it != m_dispacher_ipc_channels.end())
	{
		Channel* ch = it->second;
		if (!ch->IsWriteReady())
		{
			ch->Open();
		}
		it++;
	}
}

int ServiceProcess::ConnectDispatcher(const ProcessAddress& addr)
{
	//Dispatcher使用FIFO和其它进程通信
	if (m_dispatcher_ipc_type == IPC_FIFO)
	{
		string fifodir = g_manager_proc->GetFrameworkOptions().home_dir
				+ "/ipc";
		ServiceProcessFactory* pfactory =
				g_manager_proc->GetServiceProcessFactory(addr.proc_type);
		string name = pfactory->GetName();
		char temp[fifodir.size() + name.size() + 100];
		sprintf(temp, "%s/%s%u_%s%u", fifodir.c_str(), GetServiceName().c_str(),
				GetProcessIndex(), name.c_str(), addr.proc_idx);
		FIFOChannel* fifo = GetChannelService().NewFIFOChannel();
		fifo->SetReadPath(temp);
		sprintf(temp, "%s/%s%u_%s%u", fifodir.c_str(), name.c_str(),
				addr.proc_idx, GetServiceName().c_str(), GetProcessIndex());
		fifo->SetWritePath(temp);
		fifo->SetOpenFlag(O_NONBLOCK);
		fifo->Open();
		ChannelOptions ops;
		ops.user_write_buffer_water_mark = 8192;
		ops.user_write_buffer_flush_timeout_mills = 10;
		ops.max_write_buffer_size =
				g_manager_proc->GetFrameworkOptions().max_ipc_buffer_size;
		fifo->Configure(ops);
		fifo->SetChannelPipelineInitializor(IPCChannelInit,
				&m_ipc_event_handler);
		fifo->SetChannelPipelineFinalizer(IPCChannelFinalize, NULL);
		m_dispacher_ipc_channels[addr] = fifo;
		return 1;
	}
	else if (m_dispatcher_ipc_type == IPC_UnixDomainSocket)
	{
		//Dispatcher使用Unix Socket和其它进程通信
		string ipcdir = g_manager_proc->GetFrameworkOptions().home_dir + "/ipc";
		int retcode = chdir(ipcdir.c_str());
		if (retcode != 0)
		{
			ERROR_LOG("Failed to chdir to %s", ipcdir.c_str());
			return -1;
		}
		std::string remote_path;
		std::string local_path;
		ProcessAddress proc_addr;
		proc_addr.proc_type = GetServiceType();
		proc_addr.proc_idx = GetProcessIndex();
		GetManagerProcess()->GetDispatcherClientUnixSocketFileName(proc_addr,
				addr.proc_idx, local_path);
		remote_path = DispatcherProcessFactory::GetServerUnixSocketFileName(
				addr.proc_idx);
		SocketUnixAddress local(local_path);
		SocketUnixAddress remote(remote_path);
		ClientSocketChannel* client =
				GetChannelService().NewClientSocketChannel();
		client->SetChannelPipelineInitializor(IPCChannelInit,
				&m_ipc_event_handler);
		client->SetChannelPipelineFinalizer(IPCChannelFinalize, NULL);
		if (!client->Bind(&local))
		{
			ERROR_LOG(
					"Failed to bind local unix socket:%s", local.GetPath().c_str());
			client->Close();
			return -1;
		}
		ChannelOptions ops;
		ops.user_write_buffer_water_mark = 8182;
		ops.user_write_buffer_flush_timeout_mills = 1;
		ops.max_write_buffer_size =
				g_manager_proc->GetFrameworkOptions().max_ipc_buffer_size;
		client->Configure(ops);
		if (!client->Connect(&remote))
		{
			ERROR_LOG(
					"Failed to connect remote unix socket:%s", remote.GetPath().c_str());
			client->Close();
			return -1;
		}
		m_dispacher_ipc_channels[addr] = client;
		chdir(g_manager_proc->GetFrameworkOptions().home_dir.c_str());
		return 1;
	}
	else
	{
		ERROR_LOG(
				"Not supported dispatcher IPC type:%u", m_dispatcher_ipc_type);
		return -1;
	}

}

bool ServiceProcess::ConnectDispatchers()
{
	if (GetServiceType() == DISPATCHER_SERVICE_PROCESS_TYPE)
	{
		return true;
	}

	uint32 total_dispatcher_count = g_manager_proc->GetDispatcherCount();
	if (total_dispatcher_count <= 1)
	{
		return true;
	}

	ServiceProcessFactory* dispatcherFactory =
			g_manager_proc->GetServiceProcessFactory(
					DISPATCHER_SERVICE_PROCESS_TYPE);
	DispatcherProcessFactory* temp =
			static_cast<DispatcherProcessFactory*>(dispatcherFactory);
	m_dispatcher_ipc_type = temp->GetIPCType();

	bool ret = true;
	if (total_dispatcher_count > 0)
	{
		DEBUG_LOG(
				"Connect total %u dispatcher processes.", total_dispatcher_count);

		for (uint32 i = 0; i < total_dispatcher_count; i++)
		{
			ProcessAddress proc_addr(DISPATCHER_SERVICE_PROCESS_TYPE, i);
			int retcode = ConnectDispatcher(proc_addr);
			if (retcode < 0)
			{
				ret = false;
				break;
			}
		}
	}
	return ret;
}

void ServiceProcess::AddClosedDispatcherIPCChannel(Channel* ch)
{
	bool found = false;
	ProcessAddress addr;
	IPCChannelTable::iterator it = m_dispacher_ipc_channels.begin();
	while (it != m_dispacher_ipc_channels.end())
	{
		if (it->second == ch)
		{
			addr = it->first;
			m_dispacher_ipc_channels.erase(it);
			found = true;
			break;
		}
		it++;
	}
	if (found)
	{
		ConnectDispatcher(addr);
	}
}

ClientSocketChannel* ServiceProcess::ConnectManagerUnixSocketServer(
		SocketUnixAddress& remote, SocketUnixAddress& local)
{
	ClientSocketChannel* client = GetChannelService().NewClientSocketChannel();
	client->SetChannelPipelineInitializor(IPCChannelInit, &m_ipc_event_handler);
	client->SetChannelPipelineFinalizer(IPCChannelFinalize, NULL);
	if (!client->Bind(&local))
	{
		ERROR_LOG(
				"Failed to bind local unix socket:%s", local.GetPath().c_str());
		client->Close();
		return NULL;
	}
	ChannelOptions ops;
	ops.user_write_buffer_water_mark = 8182;
	ops.user_write_buffer_flush_timeout_mills = 1;
	ops.max_write_buffer_size =
			g_manager_proc->GetFrameworkOptions().max_ipc_buffer_size;
	client->Configure(ops);
	if (!client->Connect(&remote))
	{
		ERROR_LOG(
				"Failed to connect remote unix socket:%s", remote.GetPath().c_str());
		client->Close();
		return NULL;
	}

	return client;
}

int ServiceProcess::ConnectManagerCtrlUnixSocketServer()
{
	string fifodir = g_manager_proc->GetFrameworkOptions().home_dir + "/ipc";
	string name = m_factory->GetName();
	char temp[fifodir.size() + name.size() + 100];
	sprintf(temp, "%s/MAN_CTRL.SOCK", fifodir.c_str());
	SocketUnixAddress remote(temp);
	string local_path;
	ProcessAddress proc_addr;
	proc_addr.proc_type = GetServiceType();
	proc_addr.proc_idx = GetProcessIndex();
	GetManagerProcess()->GetIPCClientUnixSocketFileName(proc_addr, local_path);
	SocketUnixAddress local(fifodir + "/CTRL_" + local_path);
	ClientSocketChannel* ch = ConnectManagerUnixSocketServer(remote, local);
	if (NULL == ch)
	{
		return -1;
	}
//    int on = 1;
//    if (setsockopt(ch->GetReadFD(), SOL_SOCKET, SO_PEERCRED, &on, sizeof(int))
//            < 0)
//    {
//        int err = errno;
//        ch->Close();
//        ERROR_LOG(
//                "Failed to set sock opt for ctrl socket for cause:%s", strerror(err));
//        return -1;
//    }
	m_ctrl_ch = ch;
	return 0;
}

int ServiceProcess::ConnectManagerIPCUnixSocketServer()
{
	string fifodir = g_manager_proc->GetFrameworkOptions().home_dir + "/ipc";
	string name = m_factory->GetName();
	char temp[fifodir.size() + name.size() + 100];
	sprintf(temp, "%s/MAN.SOCK", fifodir.c_str());
	SocketUnixAddress remote(temp);
	string local_path;
	ProcessAddress proc_addr;
	proc_addr.proc_type = GetServiceType();
	proc_addr.proc_idx = GetProcessIndex();
	GetManagerProcess()->GetIPCClientUnixSocketFileName(proc_addr, local_path);
	SocketUnixAddress local(fifodir + "/" + local_path);
	ClientSocketChannel* ch = ConnectManagerUnixSocketServer(remote, local);
	if (NULL == ch)
	{
		return -1;
	}
	m_unix_sock = ch;
	return 0;
}

Channel* ServiceProcess::GetDispatcherIPCChannel(uint32 index)
{
	ProcessAddress proc_addr(DISPATCHER_SERVICE_PROCESS_TYPE, index);
	IPCChannelTable::iterator it = m_dispacher_ipc_channels.find(proc_addr);
	if (it != m_dispacher_ipc_channels.end())
	{
		Channel* ch = it->second;
		if (!ch->IsWriteReady())
		{
			ch->Open();
			if (!ch->IsWriteReady())
			{
				return NULL;
			}
		}
		return ch;
	}
	return NULL;
}

int ServiceProcess::Execute()
{
	adjust_google_profiler();
	m_in_child_service_process = true;
	m_pid = getpid();
	std::stringstream proc_name(std::stringstream::in | std::stringstream::out);
	uint32 proc_idx = GetProcessIndex();
	//uint32 srv_type = GetServiceType();
	if (GetServiceProcessFactory()->GetProcessCount() > 1)
	{
		proc_name << m_factory->GetName() << "_" << proc_idx;
	}
	else
	{
		proc_name << m_factory->GetName();
	}
	setenv(PROC_NAME_ENV_VAR_NAME, proc_name.str().c_str(), 1);

	logging::LoggerFactory::Destroy();
	logging::LoggerFactory::SetLoggerTimer(&(GetChannelService().GetTimer()));
	logging::LoggerFactory::Configure(
			g_manager_proc->GetFrameworkOptions().home_dir
					+ "/conf/logging.conf");

	g_service_proc = this;
	//ChannelService channel_service(m_max_channels);
	//g_manager_proc->GetChannelService().GetSignalChannel().Clear();
	GetChannelService().GetSignalChannel().Register(SIGPIPE, g_service_proc);
	GetChannelService().GetSignalChannel().Register(SIGQUIT, g_service_proc);
	GetChannelService().GetSignalChannel().Register(SIGUSR1, g_service_proc);
	GetChannelService().GetSignalChannel().Register(SIGUSR2, g_service_proc);

	//m_channel_service = &channel_service;
	Channel* fifo_channel = NULL;
	string fifodir = g_manager_proc->GetFrameworkOptions().home_dir + "/ipc";
	string name = m_factory->GetName();
	std::string manager_proc_name =
			g_manager_proc->GetFrameworkOptions().manager_process_name;
	switch (g_manager_proc->GetFrameworkOptions().ipc_type)
	{
		case IPC_FIFO:
		{
			char temp[fifodir.size() + name.size() + 100];
			sprintf(temp, "%s/%s_%s%u", fifodir.c_str(), name.c_str(),
					manager_proc_name.c_str(), GetProcessIndex());
			FIFOChannel* fifo = GetChannelService().NewFIFOChannel();
			fifo->SetReadPath(temp);
			fifo->Open();
			ChannelOptions ops;
			ops.user_write_buffer_water_mark = 8192;
			ops.user_write_buffer_flush_timeout_mills = 10;
			ops.max_write_buffer_size =
					g_manager_proc->GetFrameworkOptions().max_ipc_buffer_size;
			fifo->Configure(ops);
			if (NULL != m_fifo)
			{
				//m_fifo->
				//channel_service.RemoveChannel(m_fifo);
			}
			SetFIFO(fifo);
			sprintf(temp, "%s/%s_%s%u", fifodir.c_str(),
					manager_proc_name.c_str(), name.c_str(), GetProcessIndex());

			fifo->SetWritePath(temp);
			fifo->Open();
			fifo_channel = fifo;
			break;
		}
		case IPC_SharedMemory:
		{
			if (NULL == m_shm_fifo)
			{
				ERROR_LOG("No shm fifo inherited from parent process!");
				return -1;
			}
			SharedMemoryFIFOChannel* fifo =
					GetChannelService().NewSharedMemoryFIFOChannel();
			fifo->Clone(m_shm_fifo);
			fifo->SetProcessRole(false);
			SetShmFIFO(fifo);
			fifo_channel = fifo;
			break;
		}
		case IPC_UnixDomainSocket:
		{
			ConnectManagerIPCUnixSocketServer();
			break;
		}
	}
	if (g_manager_proc->GetFrameworkOptions().enable_ctrl_channel)
	{
		if (-1 == ConnectManagerCtrlUnixSocketServer())
		{
			return -1;
		}
	}

	ServiceHandler* srHandler = GetServiceHandler();
	m_ipc_event_handler.m_handler = srHandler;
	m_ipc_event_handler.SetServiceProcess(this);
	if (NULL != srHandler)
	{
		srHandler->m_proc = this;
	}
	if (NULL != fifo_channel)
	{
		fifo_channel->GetPipeline().AddLast("IPCEventDecoder", &m_decoder);
		fifo_channel->GetPipeline().AddLast("IPCEventEncoder", &m_encoder);
		fifo_channel->GetPipeline().AddLast("IPCEventHandler",
				&m_ipc_event_handler);
	}

	/*
	 * 注册从父进程delegate的channels到channel service
	 */
	DelegateChannelTable::iterator delegate_it =
			m_delegate_channel_table.begin();
	DelegateChannelTable newTable;
	std::set<Channel*> exceptions;
	while (delegate_it != m_delegate_channel_table.end())
	{
		Channel* ch = delegate_it->second;
		exceptions.insert(ch);
		Channel* newch = GetChannelService().AttachChannel(ch);
		if (NULL != newch)
		{
			newTable[delegate_it->first] = newch;
		}
		ServiceIndex idx = 0;
		g_manager_proc->CurrentControlProcess(ch, idx);
		if (g_service_proc->GetProcessIndex() != idx)
		{
			newch->DetachFD();
		}
		delegate_it++;
	}
	SetDelegateChannelTable(newTable);

	//Release resource copied from parent manager before entering event loop
	g_manager_proc->GetChannelService().CloseAllChannelFD(exceptions);

	if (!ConnectDispatchers())
	{
		return -1;
	}

	VirtualChannelHelper::Init();

	if (!OnInit())
	{
		ERROR_LOG(
				"Failed to init service process:%s_%u", m_factory->GetName().c_str(), GetProcessIndex());
		return -1;
	}

	if (NULL != srHandler && !srHandler->OnInit())
	{
		ERROR_LOG(
				"Failed to init service handler for service process :%s_%u", m_factory->GetName().c_str(), GetProcessIndex());
		return -1;
	}
	GetChannelService().GetTimer().Schedule(this, 0, m_options.routine_period);

	INFO_LOG(
			"Service %s_%u process(%u) started successfully.", m_factory->GetName().c_str(), GetProcessIndex(), GetPID());
	GetChannelService().Start(false);
	if (NULL != srHandler && !srHandler->OnDestroy())
	{
		ERROR_LOG(
				"Failed to destroy service handler for service process:%s_%u", m_factory->GetName().c_str(), GetProcessIndex());
	}
	if (!OnDestroy())
	{
		ERROR_LOG(
				"Failed to destroy service process :%s_%u", m_factory->GetName().c_str(), GetProcessIndex());
	}
	if (NULL != GetIPCChannel())
	{
		GetIPCChannel()->Close();
	}
	//channel_service.RemoveChannel(fifo_channel);
	m_fifo = NULL;
	m_shm_fifo = NULL;
	logging::LoggerFactory::Destroy();
	return 0;
}

ServiceProcess::~ServiceProcess()
{
	if (!m_in_child_service_process)
	{
		if (NULL != g_manager_proc)
		{
			if (NULL
					!= g_manager_proc->GetChannelService().GetChannel(
							m_ipc_channel_ID))
			{
				g_manager_proc->GetChannelService().GetChannel(m_ipc_channel_ID)->Close();
			}
			//g_manager_proc->GetChannelService().RemoveChannel(m_fifo);
			//g_manager_proc->GetChannelService().RemoveChannel(m_shm_fifo);
			m_status->~ServiceProcessStatus();
			g_manager_proc->GetIPCSharedMemoryPool()->Free(m_status);
		}
	}
	DELETE(m_channel_service);
}

//#endif /* SERVICEPROCESS_CPP_ */
