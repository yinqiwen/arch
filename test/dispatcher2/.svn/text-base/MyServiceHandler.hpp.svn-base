/*
 * MyServiceHandler.hpp
 *
 *  Created on: 2011-2-7
 *      Author: wqy
 */

#ifndef MYSERVICEHANDLER_HPP_
#define MYSERVICEHANDLER_HPP_
#include "framework/handler/service_handler.hpp"
#include "ipc/shm_allocator.hpp"
#include <list>
using namespace arch::framework;
using namespace arch::ipc;
using namespace std;

class MyServiceHandler: public ServiceHandler
{
    private:
        bool OnInit();
        void OnRoutine();
        bool OnDestroy();
        void OnIPCMessage(ServiceType src_type, ServiceIndex src_idx,
                Buffer& content);
        void OnSocketMessage(uint32 socket_channel_id, Buffer& content);
        void OnSocketMessage(uint32 socket_channel_id, Buffer& content,
                const SocketInetAddress& remote_address);
        void OnSocketOpened(uint32 socket_channel_id);
        void OnSocketConnected(uint32 socket_channel_id,
                const SocketHostAddress& remote);
        void OnSocketConnectFailed(uint32 socket_channel_id,
                const SocketHostAddress& remote);
        void OnSocketClosed(uint32 socket_channel_id);
};

#endif /* MYSERVICEHANDLER_HPP_ */
