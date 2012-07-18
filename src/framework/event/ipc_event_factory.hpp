/*
 * InternalMessageFactory.hpp
 *
 *  Created on: 2011-2-4
 *      Author: wqy
 */

#ifndef NOVA_INTERNALMESSAGEFACTORY_HPP_
#define NOVA_INTERNALMESSAGEFACTORY_HPP_
#include "buffer/buffer.hpp"
#include "channel/all_includes.hpp"
#include "framework/event/ipc_message_event.hpp"
#include "framework/event/socket_message_event.hpp"
#include "framework/event/socket_ctrl_event.hpp"
#include "framework/event/ipc_ctrl_event.hpp"
//#include <boost/pool/object_pool.hpp>

using arch::buffer::Buffer;
using arch::channel::Channel;

namespace arch
{
    namespace framework
    {
        class IPCEventFactory
        {
            private:
                //static boost::object_pool<IPCMessageEvent> m_ipc_msg_pool;
                //static boost::object_pool<SocketMessageEvent> m_socket_msg_pool;
                //static boost::object_pool<SocketCtrlEvent> m_socket_ctrl_pool;
            public:
                //static InternalMessage* decode(Buffer* content, uint32_t len);
                static IPCMessageEvent* CreateIPCMessageEvent();
                static SocketMessageEvent* CreateSocketMessageEvent();
                static SocketCtrlEvent* CreateSocketCtrlEvent();
                static IPCCtrlEvent* CreateIPCCtrlEvent();
                static void DestoryIPCEvent(IPCEvent* event);
        };
    }
}

#endif /* INTERNALMESSAGEFACTORY_HPP_ */
