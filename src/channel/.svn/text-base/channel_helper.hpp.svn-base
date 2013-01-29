/*
 * ChannelHelper.hpp
 *
 *  Created on: 2011-1-8
 *      Author: wqy
 */

#ifndef NOVA_CHANNELHELPER_HPP_
#define NOVA_CHANNELHELPER_HPP_
#include <string>
#include "common/base.hpp"
#include "exception/api_exception.hpp"
#include "net/socket_host_address.hpp"

using std::string;
using arch::exception::APIException;
using arch::net::SocketHostAddress;

namespace arch
{
    namespace channel
    {
        class Channel;
        class ChannelEvent;
        class Message;
        bool fire_channel_bound(Channel* channel);

        bool fire_channel_closed(Channel* channel);

        bool fire_channel_connected(Channel* channel);

        bool fire_channel_disconnected(Channel* channel);

        bool fire_channel_interest_changed(Channel* channel);

        bool fire_channel_open(Channel* channel);

        bool fire_channel_unbound(Channel* channel);

        bool
        fire_exception_caught(Channel* channel, const APIException& cause);

        //bool fireMessageReceived(Channel* channel, Object* message, ObjectDestructor destructor=NULL);

        bool fire_write_complete(Channel* channel);

        //bool openChannel(Channel* channel);
        //bool bindChannel(Channel* channel, Address* localAddress);
        //ChannelEvent* unbindChannel(Channel* channel);
        //bool writeChannel(Channel* channel, Object* message, ObjectDestructor destructor = NULL);

        template<typename T>
        bool write_channel(Channel* channel, T* message,
                typename Type<T>::Destructor* destructor)
        {
            MessageEvent<T> event(channel, message, destructor, false);
            return channel->GetPipeline().SendDownstream(event);
        }

        template<typename T>
        inline bool fire_message_received(Channel* channel, T* message,
                typename Type<T>::Destructor* destructor)
        {
            MessageEvent<T> event(channel, message, destructor, true);
            return channel->GetPipeline().SendUpstream(event);
        }

        template<typename T>
        inline bool fire_message_received(ChannelHandlerContext& ctx, T* message,
                typename Type<T>::Destructor* destructor)
        {
            MessageEvent<T> event(ctx.GetChannel(), message, destructor, true);
            return ctx.SendUpstream(event);
        }


        bool GetSocketRemoteAddress(Channel* ch, SocketHostAddress& address);
        bool GetSocketLocalAddress(Channel* channel, SocketHostAddress& address);

    }
}

#endif /* CHANNELHELPER_HPP_ */
