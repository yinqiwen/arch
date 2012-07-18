/*
 * InternalMessageDecoder.cpp
 *
 *  Created on: 2011-1-29
 *      Author: wqy
 */
#include "framework/handler/ipc_event_decoder.hpp"
#include "framework/event/ipc_event_factory.hpp"
#include "buffer/buffer_helper.hpp"
#include "logging/logger_macros.hpp"

using namespace arch::framework;
using namespace arch::buffer;
using namespace arch::channel;

static FrameDecodeResult<IPCEvent> emptyResult;
static void destroyIPCEvent(IPCEvent* obj)
{
    IPCEventFactory::DestoryIPCEvent(obj);
}

FrameDecodeResult<IPCEvent> IPCEventDecoder::Decode(ChannelHandlerContext& ctx,
        Channel* channel, Buffer& buffer)
{

    if (buffer.ReadableBytes() < sizeof(uint32))
    {
        return emptyResult;
    }
    size_t read_idx = buffer.GetReadIndex();
    uint32 len;
    BufferHelper::ReadFixUInt32(buffer, len);
    TRACE_LOG(
            "Enter IPCEventDecoder::decode with buffer size:%u and length header:%u",
            buffer.ReadableBytes(), len);
    if (buffer.ReadableBytes() < (len - sizeof(len)))
    {
        buffer.SetReadIndex(read_idx);
        return emptyResult;
    }
    char type_char;
    buffer.ReadByte(type_char);
    IPCEventType type = (IPCEventType) type_char;
    switch (type)
    {
        case SOCKET_MSG:
        {
            SocketMessageEvent* msg =
                    IPCEventFactory::CreateSocketMessageEvent();
            TRACE_LOG("Decode socket msg content length %d.", len - sizeof(len));
            bool ret = msg->Decode(&buffer);
            if (!ret)
            {
                IPCEventFactory::DestoryIPCEvent(msg);
                return emptyResult;
            }
            return FrameDecodeResult<IPCEvent> (msg, destroyIPCEvent);
        }
        case IPC_CTRL:
        {
            IPCCtrlEvent* ev = IPCEventFactory::CreateIPCCtrlEvent();
            bool ret = ev->Decode(&buffer);
            if (!ret)
            {
                IPCEventFactory::DestoryIPCEvent( ev);
                return emptyResult;
            }
            return FrameDecodeResult<IPCEvent> (ev, destroyIPCEvent);
        }
        case ADMIN:
        case IPC_MSG:
        {
            IPCMessageEvent* msg = IPCEventFactory::CreateIPCMessageEvent();
            TRACE_LOG("Decode ipc msg content length %d.", len - sizeof(len));
            msg->SetType(type);
            bool ret = msg->Decode(&buffer);
            if (!ret)
            {
                IPCEventFactory::DestoryIPCEvent(msg);
                return emptyResult;
            }
            return FrameDecodeResult<IPCEvent> (msg, destroyIPCEvent);
        }
        case SOCKET_CTRL:
        {
            SocketCtrlEvent* event = IPCEventFactory::CreateSocketCtrlEvent();
            bool ret = event->Decode(&buffer);
            if (!ret)
            {
                ERROR_LOG("Failed to decode socket ctrl event.");
                IPCEventFactory::DestoryIPCEvent(event);
                return emptyResult;
            }
            TRACE_LOG("Decoded socket ctrl event type %u.", event->GetSocketCtrlType());
            return FrameDecodeResult<IPCEvent> (event, destroyIPCEvent);
        }
        default:
        {
            ERROR_LOG("Unknown message type %u.", type);
            return emptyResult;
        }
    }
}
