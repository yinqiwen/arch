/*
 * IPCMessage.hpp
 *
 *  Created on: 2011-2-14
 *      Author: wqy
 */

#ifndef NOVA_IPCEVENT_HPP_
#define NOVA_IPCEVENT_HPP_

#include "common/base.hpp"
#include "buffer/buffer.hpp"
#include "framework/framework.hpp"

using arch::buffer::Buffer;

namespace arch
{
    namespace framework
    {
        enum IPCEventType
        {
            SOCKET_MSG, IPC_MSG, SOCKET_CTRL, IPC_CTRL, ADMIN
        };

        class IPCEvent: public Object
        {
            private:
                ServiceType m_src_type;
                ServiceIndex m_src_idx;
                friend class IPCEventFactory;
            protected:
                virtual bool OnDecode(Buffer* buffer) = 0;
                virtual bool OnEncode(Buffer* buffer) = 0;
            public:
                IPCEvent():m_src_type(0),m_src_idx(0){}
                void SetSrcType(ServiceType type)
                {
                	m_src_type = type;
                }
                void SetSrcIndex(ServiceIndex idx)
                {
                	m_src_idx = idx;
                }
                ServiceType GetSrcType()
                {
                	return m_src_type;
                }
                ServiceIndex GetSrcIndex()
                {
                	return m_src_idx;
                }
                virtual IPCEventType GetType() = 0;
                bool Encode(Buffer* buffer);
                bool Decode(Buffer* buffer);
                virtual ~IPCEvent(){}
        };
    }
}

#endif /* IPCMESSAGE_HPP_ */
