/*
 * service_handler.cpp
 *
 *  Created on: 2011-2-28
 *      Author: qiyingwang
 */
#include "framework/handler/service_handler.hpp"
#include "framework/process/service_process.hpp"
using namespace arch::framework;

ServiceProcess& ServiceHandler::GetServiceProcess()
{
    return *m_proc;
}
//VirtualChannelManager& ServiceHandler::GetVirtualChannelManager()
//{
//    return GetServiceProcess().GetVirtualChannelManager();
//}
