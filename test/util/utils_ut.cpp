/*
 * util_ut.cpp
 *
 *  Created on: 2011-3-14
 *      Author: qiyingwang
 */
#include <gtest/gtest.h>
#include "util/string_helper.hpp"
#include "util/config_helper.hpp"
#include "misc/crypto/md5.hpp"
#include "util/time_helper.hpp"
#include "util/math_helper.hpp"
#include "util/system_helper.hpp"
#include "util/lru.hpp"
#include "util/regular_expression_helper.h"
#include "channel/all_includes.hpp"
#include <string>
#include <fnmatch.h>
#include <map>
using namespace arch::util;
using namespace arch::channel;

struct TempTask: public Runnable
{
		void Run()
		{
		}
};

TEST(TimeHelper, GetHourMin)
{
	uint32 day = get_current_month_day();
	uint32 hour = get_current_hour();
	uint32 min = get_current_minute();
	printf("day=%u,hour=%u,minute=%u\n",day,hour,min);
}

TEST(StringHelper, Trim)
{
	char temp[100];
	sprintf(temp, "   what a fuck  \n \t ");
	char* ret = trim_str(temp, " \t\n");
	ASSERT_STREQ(ret, "what a fuck");

	std::string str = "   what a fuck  \n \t ";
	std::string ret_str = trim_string(str, " \t\n");
	EXPECT_EQ(ret_str, std::string("what a fuck"));
}

TEST(StringHelper, Split)
{
	char temp[100];
	sprintf(temp, "   what a fuck  \n \t ");
	char* ret = trim_str(temp, " \t\n");
	std::vector<char*> csret = split_str(ret, " ");
	EXPECT_EQ(3, csret.size());
	ASSERT_STREQ(csret[0], "what");
	ASSERT_STREQ(csret[1], "a");
	ASSERT_STREQ(csret[2], "fuck");

	std::string str = "   what a fuck  \n \t ";
	std::string ret_str = trim_string(str, " \t\n");
	std::vector<std::string> sret = split_string(ret_str, " ");
	EXPECT_EQ(3, sret.size());
	EXPECT_EQ(sret[0], std::string("what"));
	EXPECT_EQ(sret[1], std::string("a"));
	EXPECT_EQ(sret[2], std::string("fuck"));
}

TEST(ConfigHelper, Parse)
{
	Properties cfg_map;
	bool ret = parse_conf_file("cfg.txt", cfg_map);
	EXPECT_EQ(ret, true);
	EXPECT_EQ(3, cfg_map.size());
	EXPECT_EQ(cfg_map["ip"], std::string("10.10.10.10"));
	EXPECT_EQ(cfg_map["port"], std::string("48100"));
	EXPECT_EQ(cfg_map["LogEnable"], std::string("true"));

	INIProperties ini;
	parse_ini_conf_file("cfg.txt", ini);
	EXPECT_EQ(ini["Common"]["CommonItem"], std::string("123"));
}

TEST(crypto, MD5)
{
	MD5Context ctx;
	const char* content="hello,world! what a fuck!sadasfasgfsdgasdgasdgasdgasdgfasdgasdgasdgtusesfrhasfsjfhasjdfhasdjfhasdjkfasdjasdasfyu85678574574kgfasjkdg";
	size_t len = strlen(content);
	uint32 loop_count = 1000000;
	uint64 start = get_current_epoch_millis();
	uint32 i = 0;
	for(;i< loop_count;i++)
	{
		MD5Digest digest;
		MD5Init(&ctx);
		MD5Update(&ctx, content, len);
		MD5Final(&digest, &ctx);
		//uint64 x;
		//    memcpy(&x, digest.a+4, sizeof(x));
		//    printf("md5:%llu\n", x);
	}

	uint64 end = get_current_epoch_millis();
	printf("MD5 encode %u bytes %u times cost %u ms.\n", len, loop_count, (end - start));

	uint64 a, b;
	a= 0xffffffffffffffffLLU;
	b = 0xffffffffffffffffLLU;
	//char temp[100];
	//char temp[100];
	printf("%llx\n", a);

	char* kstr = "dz%E8%AE%BA%E5%9D%9B";
	//std::string ktemp =  string_tolower(kstr);
	std::string ktemp = kstr;
	uint64 md5sum = MD5DigestToUInt64(ktemp.c_str(), strlen(ktemp.c_str()));
	//uint64 md5sum = MD5DigestToUInt64(kstr, strlen(kstr));
	printf("##########%llu\n", md5sum);
}

TEST(RegularExpression, match)
{
	const char* expr = "abc.*tttt";
	const char* text = "abcfghttetttt";
	int ret = regular_expression_match(expr, text);
	EXPECT_EQ(ret, 1);
	uint64 start = get_current_epoch_millis();
	uint32 count =10000000;
	for(uint32 i = 0; i < count; i++)
	{
		ret = regular_expression_match(expr, text);
	}
	uint64 end = get_current_epoch_millis();
	printf("Cost %llums to regular_expression_match %u timer.\n", (end-start), count);

	const char* pattern = "abc*tttt";
	start = get_current_epoch_millis();

	for(uint32 i = 0; i < count; i++)
	{
		ret = fnmatch(pattern, text, 0);
	}
	end = get_current_epoch_millis();
	printf("Cost %llums to fnmatch %u timer.\n", (end-start), count);
}

TEST(SystemHelper, print_stacktrace)
{
	std::string buf;
	print_stacktrace(buf);
	printf("%s\n", buf.c_str());
}

TEST(LRU, LRUCache)
{
	LRUCache<uint32, uint32> cache(3);
	uint32 erased = 0;
	cache.Insert(1, 2, erased);
	cache.Insert(3, 4, erased);
	cache.Insert(4, 5, erased);
	cache.Insert(5, 6, erased);
	EXPECT_EQ(erased, 2);
	uint32 v;
	cache.Get(3, v);
	cache.Insert(6, 7, erased);
	EXPECT_EQ(erased, 5);
}

TEST(Timer, Schedule)
{
	arch::logging::Logger* logger =
	arch::logging::LoggerFactory::GetDevelopLogger();
	logger->SetLevel(arch::logging::INFO_LOG_LEVEL);
	ChannelService service;
	TempTask task;
	Timer& timer = service.GetTimer();
	uint64 start = get_current_epoch_millis();
	uint32 count =1000000;
	for(uint32 i = 0; i < count; i++)
	{
		timer.Schedule(&task, random_int32(), -1);
	}
	uint64 end = get_current_epoch_millis();
	printf("Cost %llums to schedule %u timer task.\n", (end-start), count);
}

class TempHandler: public ChannelUpstreamHandler<Buffer>
{
		void MessageReceived(ChannelHandlerContext& ctx,
				MessageEvent<Buffer>& e)
		{

		}
};

//TEST(Channel, CheckType)
//{
//    ChannelHandler* handler = new TempHandler;
//    std::map<ChannelHandler*, ChannelHandler*> xmap;
//    xmap[handler] = handler;
//    ChannelHandler* target = xmap[handler];
//    ChannelHandlerHelper<Buffer>::RegisterHandler(target);
//    uint64 start = get_current_epoch_millis();
//    uint32 count =10000000;
//    for(uint32 i = 0; i < count; i++)
//    {
//        ChannelUpstreamHandler<Buffer*> * base = dynamic_cast<ChannelUpstreamHandler<Buffer*>*>(target);
//    }
//    uint64 end = get_current_epoch_millis();
//    printf("[dynamic_cast]Cost %llums to verify %u types.\n", (end-start), count);
//
//    start = get_current_epoch_millis();
//
//    for(uint32 i = 0; i < count; i++)
//    {
//        if(ChannelHandlerHelper<Buffer*>::CanHandleUpMessageEvent(target))
//        {
//            //ChannelUpstreamHandler<char*> * base = static_cast<ChannelUpstreamHandler<Buffer>*>(handler);
//        }
//    }
//    end = get_current_epoch_millis();
//    printf("[template]Cost %llums to verify %u types.\n", (end-start), count);
//}

