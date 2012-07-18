/*
 * dispatcher_process.cpp
 *
 *  Created on: 2011-7-23
 *      Author: wqy
 */

#include "framework/process/dispatcher_process.hpp"
#include "framework/process/manager_process.hpp"
#include "framework/handler/ipc_event_decoder.hpp"
#include "framework/handler/ipc_event_encoder.hpp"
#include "net/socket_unix_address.hpp"
#include "util/proctitle_helper.h"
#include "logging/logger_macros.hpp"

using namespace arch::framework;
using arch::net::SocketUnixAddress;

const char* DispatcherProcessFactory::NAME = "Dispatcher";

static void pipelineInit(ChannelPipeline* pipeline, void* data)
{
    DispatcerIPCEventHandler* ipchandler = (DispatcerIPCEventHandler*) data;
    IPCEventEncoder* encoder = NULL;
    NEW(encoder, IPCEventEncoder);
    IPCEventDecoder* decoder = NULL;
    NEW(decoder, IPCEventDecoder);
    pipeline->AddLast("decoder", decoder);
    pipeline->AddLast("encoder", encoder);
    pipeline->AddLast("handler", ipchandler);
}

static void pipelineFinalize(ChannelPipeline* pipeline, void* data)
{
    ChannelHandler* decoder = pipeline->Get("decoder");
    DELETE(decoder);
    ChannelHandler* encoder = pipeline->Get("encoder");
    DELETE(encoder);
}

DispatcherProcess::DispatcherProcess(uint32 maxChannels) :
        ServiceProcess(maxChannels), m_process_handler(NULL), m_ipc_type(
                IPC_UnixDomainSocket)
{

}

Channel* DispatcherProcess::CreateServiceProcessIPCFIFOChannel(
        const ProcessAddress& addr)
{
    ServiceProcessFactory* pfactory =
            GetManagerProcess()->GetServiceProcessFactory(addr.proc_type);
    RETURN_NULL_IF_NULL(pfactory);
    string fifodir = g_manager_proc->GetFrameworkOptions().home_dir + "/ipc";
    //make_dir(fifodir);
    string name = pfactory->GetName();
    char temp[fifodir.size() + name.size() + 100];
    sprintf(temp, "%s/%s%u_%s%u", fifodir.c_str(), GetServiceName().c_str(),
            GetProcessIndex(), name.c_str(), addr.proc_idx);
    DEBUG_LOG("Create read FIFO %s for dispatcher process.", temp);
    FIFOChannel* fifo = GetChannelService().NewFIFOChannel();
    fifo->SetReadPath(temp);
    sprintf(temp, "%s/%s%u_%s%u", fifodir.c_str(), name.c_str(), addr.proc_idx,
            GetServiceName().c_str(), GetProcessIndex());
    DEBUG_LOG("Create write FIFO %s for dispatcher process.", temp);
    fifo->SetWritePath(temp);
    //fifo->SetOpenFlag(O_NONBLOCK);
    fifo->Open();
    ChannelOptions ops;
    ops.user_write_buffer_water_mark = 8192;
    ops.user_write_buffer_flush_timeout_mills = 1;
    ops.max_write_buffer_size =
            g_manager_proc->GetFrameworkOptions().max_ipc_buffer_size;
    fifo->Configure(ops);
    fifo->SetChannelPipelineInitializor(pipelineInit, &m_ipc_handler);
    fifo->SetChannelPipelineFinalizer(pipelineFinalize, NULL);
    return fifo;
}

bool DispatcherProcess::OnInit()
{
    string ipcdir = g_manager_proc->GetFrameworkOptions().home_dir + "/ipc";

    DispatcherProcessFactory* ownFactory =
            static_cast<DispatcherProcessFactory*>(m_factory);
    m_ipc_type = ownFactory->GetIPCType();

    m_ipc_handler.Init();
    DispatcherHandler* proc_handler = GetDispatcherHandler();
    proc_handler->SetDispatcherProcess(this);
    m_process_handler = proc_handler;
    m_ipc_handler.SetIOProcessHandler(m_process_handler);
    if (NULL != m_process_handler)
    {
        if (!m_process_handler->OnInit())
        {
            ERROR_LOG("Failed to init dispatcher handler");
            return false;
        }
    }

    string name = GetServiceName();
    char temp[100];
    sprintf(temp, "%s_%u", name.c_str(), GetProcessIndex());
    ngx_setproctitle(temp);

    if (m_ipc_type != IPC_FIFO)
    {
        //create unix socket server
        int ret = chdir(ipcdir.c_str());
        if (ret != 0)
        {
            ERROR_LOG("Failed to chdir to %s", ipcdir.c_str());
            return false;
        }
        char temp1[100];
        sprintf(
                temp1,
                "%s",
                DispatcherProcessFactory::GetServerUnixSocketFileName(
                        GetProcessIndex()).c_str());
        ServerSocketChannel* ss = GetChannelService().NewServerSocketChannel();
        ss->SetChannelPipelineInitializor(pipelineInit, &m_ipc_handler);
        ss->SetChannelPipelineFinalizer(pipelineFinalize, NULL);
        SocketUnixAddress address(temp1);
        //return false;
        bool retvalue = ss->Bind(&address);
        ChannelOptions ops;
        ops.user_write_buffer_water_mark = 8182;
        ops.user_write_buffer_flush_timeout_mills = 1;
        ops.max_write_buffer_size =
                g_manager_proc->GetFrameworkOptions().max_ipc_buffer_size;
        ss->Configure(ops);
        chdir(g_manager_proc->GetFrameworkOptions().home_dir.c_str());
        return retvalue;
    }
    else
    {
        std::list<ServiceProcessFactory*> proc_factory_list;
        GetManagerProcess()->GetAllServiceProcessFactory(proc_factory_list);
        std::list<ServiceProcessFactory*>::iterator it =
                proc_factory_list.begin();

        while (it != proc_factory_list.end())
        {
            ServiceProcessFactory* pfactory = *it;
            if (pfactory->GetType() != DISPATCHER_SERVICE_PROCESS_TYPE)
            {
                ServiceIndex count = pfactory->GetProcessCount();
                for (ServiceIndex i = 0; i < count; i++)
                {
                    ProcessAddress addr;
                    addr.proc_type = pfactory->GetType();
                    addr.proc_idx = i;
                    Channel* ch = CreateServiceProcessIPCFIFOChannel(addr);
                    m_ipc_handler.AddServiceIPCChannel(addr, ch);
                }
            }
            it++;
        }
    }
    return true;
}

void DispatcherProcess::OnRoutine()
{
    if (NULL != m_process_handler)
    {
        m_process_handler->OnRoutine();
    }
}

bool DispatcherProcess::OnDestroy()
{
    if (NULL != m_process_handler)
    {
        return m_process_handler->OnDestroy();
    }
    return true;
}

string DispatcherProcessFactory::GetServerUnixSocketFileName(ServiceIndex idx)
{
    char temp[100];
    sprintf(temp, "%s_%u.socket", NAME, idx);
    return temp;
}

