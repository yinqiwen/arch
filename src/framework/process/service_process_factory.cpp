/*
 * service_process_factory.cpp
 *
 *  Created on: 2011-5-3
 *      Author: qiyingwang
 */
#include "framework/process/service_process_factory.hpp"
#include "framework/process/manager_process.hpp"
#include "framework/vcs/virtual_channel_helper.hpp"

using namespace arch::framework;

int ServiceProcessFactory::DelegateChannel(const std::string& name, Channel* ch)
{
	if (m_delegate_channel_table.count(name) > 0)
	{
		return -1;
	}
	ch->DetachFD();
	m_delegate_channel_table[name] = ch;
	g_manager_proc->SetControlProcess4FD(ch->GetReadFD(), 0);
	if(InstanceOf<ServerSocketChannel>(ch).Value)
	{
		ServerSocketChannel* sch = (ServerSocketChannel*)ch;
		sch->SetSocketAcceptedCallBack(VirtualChannelHelper::DefaultDelegateSocketAcceptedCallBack);
	}
	return 0;
}

ServiceProcess* ServiceProcessFactory::NewServiceProcess(ServiceIndex index)
{
	if (m_serv_proc_table.count(index) > 0)
	{
		return NULL;
	}
	ServiceProcess* proc = CreateServiceProcess();
	if (NULL != proc)
	{
		proc->m_factory = this;
		proc->m_index = index;
		proc->SetDelegateChannelTable(m_delegate_channel_table);
		m_serv_proc_table[index] = proc;
	}
	return proc;
}
void ServiceProcessFactory::DeleteServiceProcess(ServiceProcess* proc)
{
	if (NULL != proc)
	{
		if (m_serv_proc_table.count(proc->GetProcessIndex()) > 0)
		{
			m_serv_proc_table.erase(proc->GetProcessIndex());
			DestroyServiceProcess(proc);
		}
	}
}

ServiceProcess* ServiceProcessFactory::GetServiceProcessByPID(pid_t pid)
{
	ServiceProcessTable::iterator it = m_serv_proc_table.begin();
	while (it != m_serv_proc_table.end())
	{
		ServiceProcess* proc = it->second;
		if (proc->GetPID() == pid)
		{
			return proc;
		}
		it++;
	}
	return NULL;
}

size_t ServiceProcessFactory::GetActiveServiceProcessSize()
{
	return m_serv_proc_table.size();
}

void ServiceProcessFactory::GetActiveServiceProcessList(
        list<ServiceProcess*>& serv_list)
{
	ServiceProcessTable::iterator it = m_serv_proc_table.begin();
	while (it != m_serv_proc_table.end())
	{
		ServiceProcess* proc = it->second;
		serv_list.push_back(proc);
		it++;
	}
}

ServiceProcess* ServiceProcessFactory::GetServiceProcessByIndex(
        ServiceIndex idx)
{
	ServiceProcessTable::iterator it = m_serv_proc_table.find(idx);
	if (it != m_serv_proc_table.end())
	{
		return it->second;
	}
	return NULL;
}

void ServiceProcessFactory::DeleteAllServiceProcess()
{
	ServiceProcessTable::iterator it = m_serv_proc_table.begin();
	while (it != m_serv_proc_table.end())
	{
		ServiceProcess* proc = it->second;
		DestroyServiceProcess(proc);
		it++;
	}
	m_serv_proc_table.clear();
}

ServiceProcessFactory::~ServiceProcessFactory()
{
	DeleteAllServiceProcess();
}
