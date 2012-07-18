/*
 * IPCMessage.hpp
 *
 *  Created on: 2011-2-14
 *      Author: wqy
 */

#ifndef NOVA_IPCMESSAGE_EVENT_HPP_
#define NOVA_IPCMESSAGE_EVENT_HPP_

#include "common/base.hpp"
#include "buffer/buffer.hpp"
#include "framework/event/ipc_event.hpp"

using arch::buffer::Buffer;

namespace arch
{
    namespace framework
    {
        class IPCMessageEvent: public IPCEvent
        {
            private:
                IPCEventType m_type;
                ServiceType m_dst_type;
                ServiceIndex m_dst_idx;
                Buffer* m_content;
                bool m_is_self_decode_buf;
                bool OnDecode(Buffer* buffer);
                bool OnEncode(Buffer* buffer);
                friend class IPCEventFactory;
            public:
                inline IPCMessageEvent() :m_type(IPC_MSG),
                    m_dst_type(0), m_dst_idx(0), m_content(NULL),
                            m_is_self_decode_buf(false)
                {
                }
                inline void SetDstType(ServiceType type)
                {
                    m_dst_type = type;
                }
                inline void SetDstIndex(ServiceIndex idx)
                {
                    m_dst_idx = idx;
                }
                inline ServiceType GetDstType()
                {
                    return m_dst_type;
                }
                inline ServiceIndex GetDstIndex()
                {
                    return m_dst_idx;
                }
                inline Buffer* GetContent()
                {
                    return m_content;
                }
                inline void SetContent(Buffer* content)
                {
                    m_content = content;
                    m_is_self_decode_buf = false;
                }
                IPCEventType GetType()
                {
                    return m_type;
                }
                inline void SetType(IPCEventType type)
                {
                    m_type = type;
                }
                ~IPCMessageEvent();
        };
    }
}

#endif /* IPCMESSAGE_HPP_ */
