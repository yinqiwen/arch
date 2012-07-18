/*
 * MACAddress.hpp
 *
 *  Created on: 2011-1-8
 *      Author: wqy
 */

#ifndef NOVA_MACADDRESS_HPP_
#define NOVA_MACADDRESS_HPP_

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <string>
#include "common/base.hpp"

namespace arch
{
    namespace net
    {
        class MACAddress
        {
            private:
                bool m_isValid;
                unsigned char m_address[6];
                MACAddress() :
                        m_isValid(false)
                {
                    memset(m_address, 0, sizeof(m_address));
                }
                void SetRawAddr(const unsigned char* addr, int len)
                {
                    memcpy(m_address, addr, sizeof(m_address));
                    m_isValid = true;
                }
                friend MACAddress arch::net::getMACAddress(
                        const std::string& nicName);
            public:
                uint64_t ToUInt64();
                bool IsValid()
                {
                    return m_isValid;
                }
                std::string ToHexStr();
        };
    }
}

#endif /* MACADDRESS_HPP_ */
