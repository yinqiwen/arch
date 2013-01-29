/*
 * InternalMessageFactory.cpp
 *
 *  Created on: 2011-2-12
 *      Author: qiyingwang
 */
#include "framework/event/ipc_event_factory.hpp"

using namespace arch::framework;

//boost::object_pool<IPCMessageEvent> IPCEventFactory::m_ipc_msg_pool;
//boost::object_pool<SocketMessageEvent> IPCEventFactory::m_socket_msg_pool;
//boost::object_pool<SocketCtrlEvent> IPCEventFactory::m_socket_ctrl_pool;

IPCMessageEvent* IPCEventFactory::CreateIPCMessageEvent()
{
    IPCMessageEvent* event = NULL;
    NEW(event, IPCMessageEvent);
    //return m_ipc_msg_pool.construct();
    return event;
}

SocketMessageEvent* IPCEventFactory::CreateSocketMessageEvent()
{
    //return m_socket_msg_pool.construct();
    SocketMessageEvent* event = NULL;
    NEW(event, SocketMessageEvent);
    return event;
}

SocketCtrlEvent* IPCEventFactory::CreateSocketCtrlEvent()
{
    //return m_socket_ctrl_pool.construct();
    SocketCtrlEvent* event = NULL;
    NEW(event, SocketCtrlEvent);
    return event;
}

IPCCtrlEvent* IPCEventFactory::CreateIPCCtrlEvent()
{
    IPCCtrlEvent* ev = NULL;
    NEW(ev, IPCCtrlEvent);
    return ev;
}

void IPCEventFactory::DestoryIPCEvent(IPCEvent* event)
{
    switch (event->GetType())
    {
        case SOCKET_MSG:
        {
            //m_socket_msg_pool.destroy((SocketMessageEvent*) event);
            DELETE(event);
            break;
        }
        case SOCKET_CTRL:
        {
            //m_socket_ctrl_pool.destroy((SocketCtrlEvent*) event);
            DELETE(event);
            break;
        }
        case IPC_CTRL:
        {
            DELETE(event);
            break;
        }
        case ADMIN:
        case IPC_MSG:
        {
            //m_ipc_msg_pool.destroy((IPCMessageEvent*) event);
            DELETE(event);
            break;
        }
        default:
        {
            break;
        }
    }
}

