/*
 * MyServiceHandler.cpp
 *
 *  Created on: 2011-2-13
 *      Author: wqy
 */
#include "MyServiceHandler.hpp"
#include "framework/process/service_process.hpp"
#include "ipc/shm_allocator.hpp"
#include "logging/logger_macros.hpp"
#include "util/proctitle_helper.h"
#include <vector>
using namespace arch::ipc;
using namespace std;
static uint32_t k_temp_sock_id;
static uint32 kcount = 0;
//SHMMap<uint32, SHMString>::map* g_test_map = NULL;
bool MyServiceHandler::OnInit()
{
	string name = GetServiceProcess().GetServiceName();
	char temp[100];
	sprintf(temp, "%s_%u_%u", name.c_str(),GetServiceProcess().GetServiceType(),GetServiceProcess().GetProcessIndex());
	//ngx_setproctitle(temp);
	DEBUG_LOG("OnInit Invoked!");
    return true;
}

bool MyServiceHandler::OnDestroy()
{
    DEBUG_LOG("OnDestroy Invoked!");
    return true;
}

void MyServiceHandler::OnRoutine()
{
	//INFO_LOG("Recv/send %u packets", kcount);
//    if (NULL != g_test_map)
//    {
//        printf("=======Iterator list content  Begin=======\n");
//        mm_lock(STLSharedMemoryPool::GetSharedMemoryPool(), MM_LOCK_RD);
//        SHMMap<uint32, SHMString>::map::iterator it = g_test_map->begin();
//        while (it != g_test_map->end())
//        {
//            printf("%d->%s\n", it->first, it->second.c_str());
//            it++;
//        }
//        mm_unlock(STLSharedMemoryPool::GetSharedMemoryPool());
//        g_test_map->clear();
//        printf("=======Iterator list content  END=======\n");
//    }

    //sleep(200000000);

}
void MyServiceHandler::OnIPCMessage(ServiceType src_type, ServiceIndex src_idx,
        Buffer& content)
{
    DEBUG_LOG("OnIPCMessage Invoked!");
}
void MyServiceHandler::OnSocketMessage(uint32 socket_channel_id,
        Buffer& content)
{
	kcount++;
    char temp[content.ReadableBytes() + 1];
    temp[content.ReadableBytes()] = 0;
    content.Read(temp, content.ReadableBytes());
    DEBUG_LOG("####Message received:%s for channel(%u)", temp, socket_channel_id);
    Buffer send(7);
    send.Write("Hello\r\n", 7);
    VirtualChannelHelper::WriteSocket(socket_channel_id, send);
    //k_temp_sock_id = socket_channel_id;
    //SocketHostAddress remote("127.0.0.1", 12345);
    //VirtualChannelManager::SocketConnectRequest(remote, true);
}

void MyServiceHandler::OnSocketMessage(uint32 socket_channel_id,
        Buffer& content, const SocketInetAddress& remote_address)
{
    char temp[content.ReadableBytes() + 1];
    temp[content.ReadableBytes()] = 0;
    content.Read(temp, content.ReadableBytes());
    DEBUG_LOG("####UDP Message received:  %s", temp);
}

void MyServiceHandler::OnSocketOpened(uint32 socket_channel_id)
{
    DEBUG_LOG("OnSocketOpened with id:%u", socket_channel_id);
}
void MyServiceHandler::OnSocketConnected(uint32 socket_channel_id,
        const SocketHostAddress& remote)
{
    INFO_LOG("OnSocketConnected with id:%u and address %s:%u", socket_channel_id, remote.GetHost().c_str(), remote.GetPort());
}
void MyServiceHandler::OnSocketConnectFailed(uint32 socket_channel_id,
        const SocketHostAddress& remote)
{
    DEBUG_LOG("#####OnSocketConnectFailed with address %s:%u", remote.GetHost().c_str(), remote.GetPort());
    VirtualChannelHelper::CloseSocket(k_temp_sock_id);
}
void MyServiceHandler::OnSocketClosed(uint32 socket_channel_id)
{
    DEBUG_LOG("OnSocketClosed with id:%u", socket_channel_id);
}

