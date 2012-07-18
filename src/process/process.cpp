/*
 * process.cpp
 *
 *  Created on: 2010-10-11
 *      Author: qiyingwang
 */
#include "process/process.hpp"
#include <stdlib.h>
#include <unistd.h>

using namespace arch::process;

Process Process::m_currentProcessInstance;

Process& Process::CurrentProcess()
{
	return m_currentProcessInstance;
}

Process::Process()
{
	m_pid = getpid();
}

Process::Process(int pid) :
	m_pid(pid)
{
}

Process Process::Fork(ProcessRunner* runner, bool asDameon)
{
	if (NULL == runner)
	{
		return Process::CurrentProcess();
	}
	pid_t retPID = ::fork();
	if (asDameon)
	{
		if (0 == retPID)
		{
			setsid();
			m_pid = getpid();
			m_childrens.clear();
		}
		else
		{
			exit(0);
		}
	}
	else
	{
		if (retPID > 0)
		{
			m_childrens.push_back(retPID);
			Process p(retPID);
			return p;
		}
	}
	int ret = runner->Execute();
	exit(ret);
	//return Process::CurrentProcess();
}

vector<pair<Process, ProcessRunner*> > Process::ForkChildren(const vector<
        ProcessRunner*>& childRunners, bool asDameon)
{
	if (asDameon && getsid(m_pid) != m_pid)
	{
		pid_t retPID = ::fork();
		if (0 == retPID)
		{
			setsid();
			m_pid = getpid();
			m_childrens.clear();
		}
		else
		{
			exit(0);
		}
	}

	vector < pair<Process, ProcessRunner*> > childrens;
	vector<ProcessRunner*>::const_iterator it = childRunners.begin();
	while (it != childRunners.end())
	{
		ProcessRunner* runner = *it;
		if (NULL != runner)
		{
			pid_t retPID = ::fork();
			if (0 == retPID)
			{
				int ret = runner->Execute();
				exit(ret);
			}
			else
			{
				m_childrens.push_back(retPID);
				Process p(retPID);
				childrens.push_back(std::make_pair(p, runner));
			}
		}
		it++;
	}
	return childrens;
}

