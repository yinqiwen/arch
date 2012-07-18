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
#include "util/regular_expression_helper.h"
#include <string>
using namespace arch::util;

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
}

TEST(RegularExpression, match)
{
    const char* expr = "abc.*tttt";
    const char* text = "abcfghttetttt";
    int ret = regular_expression_match(expr, text);
    EXPECT_EQ(ret, 1);
}

