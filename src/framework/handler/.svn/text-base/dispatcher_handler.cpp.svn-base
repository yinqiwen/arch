/*
 * dispatcher_handler.cpp
 *
 *  Created on: 2011-7-23
 *      Author: wqy
 */
#include "framework/handler/dispatcher_handler.hpp"
#include "framework/handler/dispatcher_ipc_event_handler.hpp"
using namespace arch::framework;

DispatcherHandler::DispatcherHandler() :
	m_proc(NULL)
{

}

void DispatcherHandler::SetDispatcherProcess(DispatcherProcess* proc)
{
	m_proc = proc;
}

DispatcherProcess& DispatcherHandler::GetDispatcherProcess()
{
	return *m_proc;
}
