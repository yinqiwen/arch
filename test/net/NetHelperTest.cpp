/*
 * NetHelperTest.cpp
 *
 *  Created on: 2011-2-28
 *      Author: qiyingwang
 */
#include <gtest/gtest.h>
#include <arpa/inet.h>
#include <errno.h>
#include "net/network_helper.hpp"
using namespace arch::net;

TEST(network_helper, getRemoteHostAddress)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    SocketInetAddress address = getInetAddress("127.0.0.1", 48100);
    if(0 == connect(fd,&(address.GetRawSockAddr()), sizeof(struct sockaddr)))
    {
        SocketHostAddress host = getRemoteHostAddress(fd);
        EXPECT_EQ(48100, host.GetPort());
        string str = "127.0.0.1";
        EXPECT_EQ(str, host.GetHost());
        socklen_t socklen;
        if (0 == getpeername(fd, (sockaddr*)&addr, &socklen))
        {
        	const char* adstr = inet_ntoa(addr.sin_addr);
        	printf("adstr is %s\n", adstr);
        }

    }
    else
    {
        SocketHostAddress host = getRemoteHostAddress(fd);
        const char* cause = strerror(errno);
        printf("%s\n", cause);
        printf("%s:%u\n", host.GetHost().c_str(), host.GetPort());
    }

}
