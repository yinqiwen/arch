/*
 * BufferUnitTest.cpp
 *
 *  Created on: 2011-1-27
 *      Author: wqy
 */
#include <gtest/gtest.h>
#include "buffer/buffer_factory.hpp"
#include "util/time_helper.hpp"
#include "net/datagram_packet.hpp"
#include "buffer/buffer.hpp"
#include "buffer/struct_codec_macros.hpp"
using namespace arch::buffer;
using namespace arch::util;
using namespace arch::net;

struct TestStructBase
{
        uint32 k;
        uint64 kk;
        std::string str;
        std::list<uint64> klist;
        std::vector<std::string> kvec;
        std::map<std::string, uint64> kmap;ENCODE_DEFINE(str,k, kk, klist, kvec)DECODE_DEFINE(str,k, kk, klist, kvec)
};

struct TestStruct
{
        uint32 k;
        uint64 kk;
        std::string str;
        std::list<uint64> klist;
        std::vector<std::string> kvec;
        std::map<std::string, uint64> kmap;
        std::deque<TestStructBase> kdeq;
        TestStructBase base;ENCODE_DEFINE(str,k, kk, klist, kvec, base, kdeq)DECODE_DEFINE(str,k, kk, klist, kvec, base, kdeq)
};

TEST(BufferFactory, Factory)
{
    BufferFactory factory;
    Buffer* buf1 = factory.Acquire(256);
    Buffer* buf2 = factory.Acquire(253);
    factory.Release(buf1);
    factory.Release(buf2);
    Buffer* buf3 = factory.Acquire(257);
    Buffer* buf4 = factory.Acquire(252);
    EXPECT_EQ(512, buf3->Capacity())
    ;
    EXPECT_EQ(256, buf4->Capacity())
    ;
    factory.Release(buf3);
    factory.Release(buf4);
}

TEST(Buffer, writeBuffer)
{
    Buffer buf1(20);
    Buffer buf2(5);
    buf1.Write("hello,world", 11);
    uint32_t len = buf2.Write(&buf1, 5);
    printf("Length is %d\n", len);
    EXPECT_EQ(5, len)
    ;
    EXPECT_EQ(6, buf1.ReadableBytes())
    ;
}

TEST(Buffer, IndexOf)
{
    Buffer buf2(5);
    buf2.Write("hello,world\r\nA", 14);
    int index = buf2.IndexOf("wo", 2);
    EXPECT_EQ(6, index)
    ;
    index = buf2.IndexOf("fga", 3);
    EXPECT_EQ(-1, index)
    ;
    index = buf2.IndexOf("\n", 1);
    EXPECT_EQ(12, index)
    ;
    index = buf2.IndexOf("\r", 1);
    EXPECT_EQ(11, index)
    ;
    index = buf2.IndexOf("\r\n", 2);
    EXPECT_EQ(11, index)
    ;

    uint64 start = get_current_epoch_millis();
    uint32 i = 0;
    for (; i < 10000000; i++)
    {
        buf2.IndexOf("\r\n", 2);
    }
    uint64 end = get_current_epoch_millis();
    printf("Success %u IndexOf %llu ms\n",
            i, (end - start));
}

TEST(Object, InstanceOf)
{
    Buffer buf1(20);
    uint64 start = get_current_epoch_millis();
    uint32 i = 0;
    for (; i < 10000; i++)
    {
        if (InstanceOf<Buffer> (&buf1).Value)
        {
            continue;
        }
    }
    uint64 end = get_current_epoch_millis();
    printf(
            "Success %u InstanceOf Cost %llu ms\n",
            i, (end - start));

    start = get_current_epoch_millis();
    i = 0;
    for (; i < 10000; i++)
    {
        //if(InstanceOf<DatagramPacket>(&buf1).Value)

        {
            continue;
        }
    }
    end = get_current_epoch_millis();
    printf(
            "Fail %u InstanceOf Cost %llu ms\n",
            i, (end - start));

    start = get_current_epoch_millis();
    i = 0;
    for (; i < 100000000; i++)
    {

    }
    end = get_current_epoch_millis();
    printf("%u Noop Cost %llu ms\n", i,
            (end - start));

    TestStruct test;
    test.Encode(buf1);
}

