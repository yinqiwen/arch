/*
 * NetworkHelper.cpp
 *
 *  Created on: 2011-1-8
 *      Author: wqy
 */
#include "net/network_helper.hpp"
#include "net/mac_address.hpp"
#include "exception/api_exception.hpp"
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <net/if.h>
#include <string.h>

using namespace arch::net;
using arch::exception::APIException;

namespace arch
{
    namespace net
    {
        static const uint16 kMaxNICCount = 16;
        SocketInetAddress getInetAddress(const SocketHostAddress& addr)
        {
            return getInetAddress(addr.GetHost().c_str(), addr.GetPort());
        }

        SocketInetAddress getInetAddress(const SocketUnixAddress& unix_addr)
        {
            struct sockaddr_un addr;
            memset(&addr, 0, sizeof(addr));

            addr.sun_family = AF_UNIX;
            strcpy(addr.sun_path, unix_addr.GetPath().c_str());
            //SocketInetAddress ret(addr);
            return SocketInetAddress(addr);
        }

        SocketInetAddress getInetAddress(const string& host, uint16 port)
        {
            struct addrinfo hints;
            struct addrinfo* res = NULL;
            //struct sockaddr addr;
            //memset(&addr, 0, sizeof(addr));
            memset(&hints, 0, sizeof(hints));
            hints.ai_flags = 0;
            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = 0;
            hints.ai_protocol = 0;

            char szPort[10];
            sprintf(szPort, "%d", port);
            int retvalue = getaddrinfo(host.c_str(), szPort, &hints, &res);
            if (0 != retvalue)
            {
                freeaddrinfo(res);
                throw APIException(errno);
            }
            SocketInetAddress ret;
            if (NULL != res)
            {
                struct sockaddr* addr = res->ai_addr;
                if (addr->sa_family == AF_INET)
                {
                    sockaddr_in* in = (sockaddr_in*) (addr);
                    ret = SocketInetAddress(*in);
                }
                else if (addr->sa_family == AF_INET6)
                {
                    sockaddr_in6* in6 = (sockaddr_in6*) (addr);
                    ret = SocketInetAddress(*in6);
                }
                else if (addr->sa_family == AF_UNIX)
                {
                    sockaddr_un* un = (sockaddr_un*) (addr);
                    ret = SocketInetAddress(*un);
                }
            }
            freeaddrinfo(res);
            return ret;
        }

        SocketHostAddress getHostAddress(const sockaddr& addr)
        {
            SocketHostAddress invalid("", 0);
            int size;
            if (addr.sa_family == AF_INET || addr.sa_family != AF_INET6)
            {
                //ipv4
                char host[INET_ADDRSTRLEN + 1];
                size = sizeof(sockaddr_in);
                sockaddr_in* paddr = (sockaddr_in*) (&addr);
                if (NULL != inet_ntop(AF_INET, &(paddr->sin_addr), host, size))
                {
                    return SocketHostAddress(host, ntohs(paddr->sin_port));
                }
                return invalid;
            }
            else
            {
                //ipv6
                char host[INET6_ADDRSTRLEN + 1];
                size = sizeof(sockaddr_in6);
                sockaddr_in6* paddr = (sockaddr_in6*) (&addr);
                if (NULL
                        != inet_ntop(AF_INET6, &(paddr->sin6_addr), host, size))
                {
                    sockaddr_in6* paddr = (sockaddr_in6*) (&addr);
                    return SocketHostAddress(host, ntohs(paddr->sin6_port));
                }
                return invalid;
            }
        }

        SocketUnixAddress getUnixAddress(const sockaddr& addr)
        {
            struct sockaddr_un* pun = (struct sockaddr_un*) &addr;
            const char* path = pun->sun_path;
            return SocketUnixAddress(path);
        }
        SocketUnixAddress getUnixAddress(const SocketInetAddress& addr)
        {
            return getUnixAddress(addr.GetRawSockAddr());
        }

        SocketHostAddress getHostAddress(const SocketInetAddress& addr)
        {
            return getHostAddress(addr.GetRawSockAddr());
        }

        SocketInetAddress getSocketInetAddress(int32 fd)
        {
            char temp[256];
            memset(&temp, 0, sizeof(temp));
            sockaddr* addr = (sockaddr*) temp;
            socklen_t socklen = sizeof(temp);
            if (0 == getsockname(fd, addr, &socklen))
            {
                if (addr->sa_family == AF_INET)
                {
                    sockaddr_in* in = (sockaddr_in*) addr;
                    return SocketInetAddress(*in);
                }
                else if (addr->sa_family == AF_INET6)
                {
                    sockaddr_in6* in6 = (sockaddr_in6*) addr;
                    return SocketInetAddress(*in6);
                }
                else if (addr->sa_family == AF_UNIX)
                {
                    sockaddr_un* un = (sockaddr_un*) addr;
                    return SocketInetAddress(*un);
                }
            }
            return SocketInetAddress();
        }

        SocketHostAddress getHostAddress(int32 fd)
        {
            SocketInetAddress inetaddr = getSocketInetAddress(fd);
            return getHostAddress(inetaddr);
        }

        SocketInetAddress getRemoteInetAddress(int32 fd)
        {

            char temp[256];
            memset(&temp, 0, sizeof(temp));
            sockaddr* addr = (sockaddr*) temp;
            socklen_t socklen = sizeof(temp);
            if (0 == getpeername(fd, addr, &socklen))
            {
                if (addr->sa_family == AF_INET)
                {
                    sockaddr_in* in = (sockaddr_in*) addr;
                    return SocketInetAddress(*in);
                }
                else if (addr->sa_family == AF_INET6)
                {
                    sockaddr_in6* in6 = (sockaddr_in6*) addr;
                    return SocketInetAddress(*in6);
                }
                else if (addr->sa_family == AF_UNIX)
                {
                    sockaddr_un* un = (sockaddr_un*) addr;
                    return SocketInetAddress(*un);
                }

            }
            return SocketInetAddress();
        }

        SocketHostAddress getRemoteHostAddress(int32 fd)
        {
            SocketInetAddress inetaddr = getRemoteInetAddress(fd);
            return getHostAddress(inetaddr);
        }

        uint64 htonll(uint64 v)
        {
            int num = 42;
            //big or little
            if (*(char *) &num == 42)
            {
                uint64_t temp = htonl(v & 0xFFFFFFFF);
                temp <<= 32;
                return temp | htonl(v >> 32);
            }
            else
            {
                return v;
            }

        }

        uint64 ntohll(uint64 v)
        {
            return htonll(v);
        }

        MACAddress getMACAddress(const std::string& nicName)
        {
            struct ifreq ifreq;
            memset(&ifreq, 0, sizeof(ifreq));
            int sock;
            MACAddress addr;
            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
                return addr;
            }
            strcpy(ifreq.ifr_name, nicName.c_str());
            if (ioctl(sock, SIOCGIFHWADDR, &ifreq) < 0)
            {
                ::close(sock);
                return addr;
            }
            ::close(sock);
            unsigned char* raw = (unsigned char*) (ifreq.ifr_hwaddr.sa_data);
            addr.SetRawAddr(raw, 6);
            return addr;
        }

        list<MACAddress> getAllMACAddresses()
        {
            list<MACAddress> resultList;
            struct ifconf ifc;
            memset(&ifc, 0, sizeof(ifc));
            struct ifreq ifreqs[kMaxNICCount];
            memset(ifreqs, 0, sizeof(ifreqs));
            ifc.ifc_len = sizeof ifreqs;
            ifc.ifc_req = ifreqs;

            int sock;
            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
                return resultList;
            }
            if (0 != ioctl(sock, SIOCGIFCONF, (char *) &ifc))
            {
                ::close(sock);
                return resultList;
            }
            ::close(sock);
            uint32_t nicCount = ifc.ifc_len / sizeof(struct ifreq);
            for (uint32_t i = 0; i < nicCount; ++i)
            {
                MACAddress addr = getMACAddress(ifreqs[i].ifr_name);
                resultList.push_back(addr);
            }
            return resultList;
        }

        int getIPByNICName(const std::string& ifName, std::string& ip)
        {
            int fd;
            int intrface;
            struct ifconf ifc;
            struct ifreq ifr[kMaxNICCount];

            if (-1 == (fd = socket(AF_INET, SOCK_DGRAM, 0)))
            {
                return -1;
            }

            ifc.ifc_len = sizeof(ifr);
            ifc.ifc_buf = (caddr_t) ifr;

            if (-1 == ioctl(fd, SIOCGIFCONF, (char *) &ifc))
            {
                ::close(fd);
                return -1;
            }

            intrface = ifc.ifc_len / sizeof(struct ifreq);
            while (intrface-- > 0)
            {
                /*Get IP of the net card */
                if (-1 == ioctl(fd, SIOCGIFADDR, (char *) &ifr[intrface]))
                    continue;
                if (NULL == ifr[intrface].ifr_name)
                    continue;

                if (0 == strcmp(ifName.c_str(), ifr[intrface].ifr_name))
                {
                    SocketHostAddress addr = getHostAddress(
                            ifr[intrface].ifr_addr);
                    ip = addr.GetHost();
                    ::close(fd);
                    return 0;
                }
            }

            close(fd);
            return -1;
        }

        int getLocalHostIPList(std::vector<std::string>& iplist)
        {
            int fd;
            int intrface;
            struct ifconf ifc;
            struct ifreq ifr[kMaxNICCount];

            if (-1 == (fd = socket(AF_INET, SOCK_DGRAM, 0)))
            {
                return -1;
            }

            ifc.ifc_len = sizeof(ifr);
            ifc.ifc_buf = (caddr_t) ifr;

            if (-1 == ioctl(fd, SIOCGIFCONF, (char *) &ifc))
            {
                ::close(fd);
                return -1;
            }

            intrface = ifc.ifc_len / sizeof(struct ifreq);
            while (intrface-- > 0)
            {
                /*Get IP of the net card */
                if (-1 == ioctl(fd, SIOCGIFADDR, (char *) &ifr[intrface]))
                    continue;
                if (NULL == ifr[intrface].ifr_name)
                    continue;

                SocketHostAddress addr = getHostAddress(ifr[intrface].ifr_addr);
                std::string ip = addr.GetHost();
                iplist.push_back(ip);
            }

            close(fd);
            return -1;
        }
    }
}

