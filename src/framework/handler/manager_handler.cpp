/*
 * manager_handler.cpp
 *
 *  Created on: 2011-2-28
 *      Author: qiyingwang
 */
#include "framework/handler/manager_handler.hpp"
#include "framework/process/manager_process.hpp"
using namespace arch::framework;

ManagerProcess& ManagerHandler::GetManagerProcess()
{
    return *m_proc;
}
