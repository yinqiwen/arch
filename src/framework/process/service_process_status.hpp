/*
 * ServiceProcessStatus.hpp
 *
 *  Created on: 2011-2-7
 *      Author: wqy
 */

#ifndef NOVA_SERVICEPROCESSSTATUS_HPP_
#define NOVA_SERVICEPROCESSSTATUS_HPP_
#include <unistd.h>
#include "common/base.hpp"
namespace arch
{
    namespace framework
    {
        class ServiceProcessStatus
        {
            private:
                uint64 m_last_ping_time;
                uint32 m_timeout_count;
                uint32 m_timeout;
                pid_t m_pid;
                bool m_have_broadcasted;
            public:
                ServiceProcessStatus(uint32 timeout_count,
                        uint32 timeout);
                /*
                 * Invoke at service process
                 */
                void Ping(uint64 current_timestamp = 0);
                /**
                 * Invoke at manager process
                 */
                bool IsAlive(uint32 timeout_count_limit,
                        uint64 current_timestamp = 0);
                bool MarkDead();
                inline bool IsStarted()
                {
                	return m_last_ping_time > 0;
                }
                inline bool HaveBroadcasted()
                {
                    return m_have_broadcasted;
                }
                inline void SetBroadcastedFlag(bool flag)
                {
                    m_have_broadcasted = flag;
                }
                ~ServiceProcessStatus();
        };
    }
}

#endif /* SERVICEPROCESSSTATUS_HPP_ */
