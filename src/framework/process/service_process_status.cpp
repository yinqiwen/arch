/*
 * service_process_status.cpp
 *
 *  Created on: 2011-3-25
 *      Author: qiyingwang
 */
#include "framework/process/service_process_status.hpp"
#include "util/time_helper.hpp"
#include "logging/logger_macros.hpp"
#include <limits.h>

using namespace arch::framework;
using namespace arch::util;

ServiceProcessStatus::ServiceProcessStatus(uint32 timeout_count, uint32 timeout) :
        m_last_ping_time(0), m_timeout_count(timeout_count), m_timeout(timeout), m_pid(
                0), m_have_broadcasted(false)
{

}

void ServiceProcessStatus::Ping(uint64 current_timestamp)
{
    if (0 == m_pid)
    {
        m_pid = getpid();
    }
    if (current_timestamp == 0)
    {
        current_timestamp = get_current_monotonic_millis();
    }

    /**
     * No  lock on 'm_last_ping_time' since we do not care
     */
    m_last_ping_time = current_timestamp;
    m_timeout_count = 0;
}

//bool ServiceProcessStatus::IsStarted()
//{
//    return m_init_time > 0;
//}

bool ServiceProcessStatus::MarkDead()
{
    m_last_ping_time = 0;
    m_timeout_count = LONG_MAX;
    return true;
}

bool ServiceProcessStatus::IsAlive(uint32 timeout_count_limit,
        uint64 current_timestamp)
{
    if (current_timestamp == 0)
    {
        current_timestamp = get_current_monotonic_millis();
    }
    //means service process not started
    if (m_last_ping_time == 0)
    {
        return true;
    }
    if ((current_timestamp - m_last_ping_time) > m_timeout)
    {
        if ((current_timestamp - m_last_ping_time)
                > (m_timeout * (m_timeout_count + 1)))
        {
            m_timeout_count++;
        }
    }
    else
    {
        m_timeout_count = 0;
    }
    if (m_timeout_count >= timeout_count_limit
            && (current_timestamp - m_last_ping_time)
                    >= timeout_count_limit * m_timeout_count)
    {
        ERROR_LOG(
                "Service process(pid:%u) timeout %u times & hanged for a long while(%llums)", m_pid, timeout_count_limit, current_timestamp - m_last_ping_time);
        return false;
    }
    return true;
}

ServiceProcessStatus::~ServiceProcessStatus()
{

}
