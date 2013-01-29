/*
 * redis_command_manager.cpp
 *
 *  Created on: 2011-7-14
 *      Author: qiyingwang
 */
#include "protocol/redis/redis_command_manager.hpp"
#include "protocol/redis/redis_frame_decoder.hpp"
#include "channel/all_includes.hpp"
#include "logging/logger_macros.hpp"
#include "util/string_helper.hpp"
#include <limits.h>
#include <string.h>

using namespace arch::redis;
using namespace arch::util;

RedisCommandManager::RedisCommandManager()
{
	memset(m_szErrMsg, 0, MAX_REDIS_ERR_MSG_LEN + 1);
}

int RedisCommandManager::Init()
{
//	RedisCommandConfig getcfg = { GET, 2, 0 };
//	m_cfg_table["get"] = getcfg;
//	RedisCommandConfig setcfg = { SET, 3, 0 };
//	m_cfg_table["set"] = setcfg;
//	RedisCommandConfig setnxcfg = { SETNX, 3, 0 };
//	m_cfg_table["setnx"] = setnxcfg;
//	RedisCommandConfig setexcfg = { SETEX, 3, 0 };
//	m_cfg_table["setex"] = setexcfg;
//	RedisCommandConfig appendcfg = { APPEND, 3, 0 };
//	m_cfg_table["append"] = appendcfg;
//	RedisCommandConfig strlencfg = { STRLEN, 2, 0 };
//	m_cfg_table["strlen"] = strlencfg;
//	RedisCommandConfig delcfg = { DEL, 2, 0 };
//	m_cfg_table["del"] = delcfg;
//	RedisCommandConfig existscfg = { DEL, -2, 0 };
//	m_cfg_table["exists"] = existscfg;
//	RedisCommandConfig setbitcfg = { SETBIT, 4, 0 };
//	m_cfg_table["setbit"] = setbitcfg;
//	RedisCommandConfig getbitcfg = { GETBIT, 3, 0 };
//	m_cfg_table["getbit"] = getbitcfg;
//	RedisCommandConfig setrangebitcfg = { SETRANGE, 3, 0 };
//	m_cfg_table["setrange"] = setrangebitcfg;
//	RedisCommandConfig getrangebitcfg = { GETRANGE, 3, 0 };
//	m_cfg_table["getrange"] = getrangebitcfg;
//	RedisCommandConfig substrcfg = {SUBSTR,4,0};
//	m_cfg_table["substr"] = substrcfg;
//	RedisCommandConfig incrcfg ={INCR,2,0};
//	m_cfg_table["incr"] = incrcfg;
//	RedisCommandConfig decrcfg ={DECR,2,0};
//	m_cfg_table["decr"] = decrcfg;
	struct RedisCommandConfig commandTable[] = {
	    {"get",GET,2,0},
	    {"set",SET,3,0},
	    {"setnx",SETNX,3,0},
	    {"setex",SETEX,4,0},
	    {"append",APPEND,3,1},
	    {"strlen",STRLEN,2,1},
	    {"del",DEL,-2,0},
	    {"exists",EXISTS,2,0},
	    {"setbit",SETBIT,4,1},
	    {"getbit",GETBIT,3,0},
	    {"setrange",SETRANGE,4,1},
	    {"getrange",GETRANGE,4,0},
	    {"substr",SUBSTR,4,0},
	    {"incr",INCR,2,1},
	    {"decr",DECR,2,1},
	    {"mget",MGET,-2,0},
	    {"rpush",RPUSH,3,1},
	    {"lpush",LPUSH,3,1},
	    {"rpushx",RPUSHX,3,1},
	    {"lpushx",LPUSHX,3,1},
	    {"linsert",LINSERT,5,1},
	    {"rpop",RPOP,2,0},
	    {"lpop",LPOP,2,0},
	    {"brpop",BPOP,-3,0},
	    {"brpoplpush",BRPOPLPUSH,4,1},
	    {"blpop",BLPOP,-3,0},
	    {"llen",LLEN,2,0},
	    {"lindex",LINDEX,3,0},
	    {"lset",LSET,4,1},
	    {"lrange",LRANGE,4,0},
	    {"ltrim",LTRIM,4,0},
	    {"lrem",LREM,4,0},
	    {"rpoplpush",RPOPLPUSH,3,1},
	    {"sadd",SADD,3,1},
	    {"srem",SREM,3,0},
	    {"smove",SMOVE,4,0},
	    {"sismember",SISMEMBER,3,0},
	    {"scard",SCARD,2,0},
	    {"spop",SPOP,2,0},
	    {"srandmember",SRANDMEMBER,2,0},
	    {"sinter",SINTER,-2,1},
	    {"sinterstore",SINTERSTORE,-3,1},
	    {"sunion",SUNION,-2,1},
	    {"sunionstore",SUNIONSTORE,-3,1},
	    {"sdiff",SDIFF,-2,1},
	    {"sdiffstore",SDIFFSTORE,-3,1},
	    {"smembers",SMEMBERS,2,0},
	    {"zadd",ZADD,4,1},
	    {"zincrby",ZINCRBY,4,1},
	    {"zrem",ZREM,3,0},
	    {"zremrangebyscore",ZREMRANGEBYSCORE,4,0},
	    {"zremrangebyrank",ZREMRANGEBYRANK,4,0},
	    {"zunionstore",ZUNIONSTORE,-4,0},
	    {"zinterstore",ZINTERSTORE,-4,0},
	    {"zrange",ZRANGE,-4,0},
	    {"zrangebyscore",ZRANGEBYSCORE,-4,0},
	    {"zrevrangebyscore",ZREVRANGEBYSCORE,-4,0},
	    {"zcount",ZCOUNT,4,0},
	    {"zrevrange",ZREVRANGE,-4,0},
	    {"zcard",ZCARD,2,0},
	    {"zscore",ZSCORE,3,0},
	    {"zrank",ZRANK,3,0},
	    {"zrevrank",ZREVRANK,3,0},
	    {"hset",HSET,4,1},
	    {"hsetnx",HSETNX,4,1},
	    {"hget",HGET,3,0},
	    {"hmset",HMSET,-4,1},
	    {"hmget",HMGET,-3,0},
	    {"hincrby",HINCRBY,4},
	    {"hdel",HDEL,3,0},
	    {"hlen",HLEN,2,0},
	    {"hkeys",HKEYS,2,0},
	    {"hvals",HVALS,2,0},
	    {"hgetall",HGETALL,2,0},
	    {"hexists",HEXISTS,3,0},
	    {"incrby",INCRBY,3,1},
	    {"decrby",DECRBY,3,1},
	    {"getset",GETSET,3,1},
	    {"mset",MSET,-3,2},
	    {"msetnx",MSETNX,-3,2},
	    {"randomkey",RANDOMKEY,1,0},
	    {"select",SELECT,2,0},
	    {"move",MOVE,3,0},
	    {"rename",RENAME,3,0},
	    {"renamenx",RENAMENX,3,0},
	    {"expire",EXPIRE,3,0},
	    {"expireat",EXPIREAT,3,0},
	    {"keys",KEYS,2,0},
	    {"dbsize",DBSIZE,1,0},
	    {"auth",AUTH,2,0},
	    {"ping",PING,1,0},
	    {"echo",ECHO,2,0},
	    {"save",SAVE,1,0},
	    {"bgsave",BGSAVE,1,0},
	    {"bgrewriteaof",BGREWRITEEAOF,1,0},
	    {"shutdown",SHUTDOWN,1,0},
	    {"lastsave",LASTSAVE,1,0},
	    {"type",TYPE,2,0},
	    {"multi",MULTI,1,0},
	    {"exec",EXEC,1,0},
	    {"discard",DISCARD,1,0},
	    {"sync",SYNC,1,0},
	    {"flushdb",FLUSHDB,1,0},
	    {"flushall",FLUSHALL,1,0},
	    {"sort",SORT,-2,1},
	    {"info",INFO,1,0},
	    {"monitor",MONITOR,1},
	    {"ttl",TTL,2,0},
	    {"persist",PERSIST,2,0},
	    {"slaveof",SLAVEOF,3,0},
	    {"debug",DEBUG,-2,0},
	    {"config",CONFIG,-2,0},
	    {"subscribe",SUBSCRIBE,-2,0},
	    {"unsubscribe",UNSUBSCRIBE,-1,0},
	    {"psubscribe",PSUBSCRIBE,-2,0},
	    {"punsubscribe",PUNSUBSCRIBE,-1,0},
	    {"publish",PUBLISH,3,0},
	    {"watch",WATCH,-2,0},
	    {"unwatch",UNWATCH,1,0},
	    {"quit",QUIT,1,0},
	};

	uint32 arraylen = arraysize(commandTable);
	for(uint32 i = 0; i<arraylen; i++)
	{
		m_cfg_table[commandTable[i].name] = commandTable[i];
	}
	return 0;
}

void RedisCommandManager::RegisterHandler(RedisCommandHandler* handler)
{
	m_handler_array.push_back(handler);
}

int RedisCommandManager::HandlerCommandFrame(RedisCommandFrame& frame,
        uint32 connectionID)
{
	Buffer* name = frame.GetArgument(0);
	if (NULL == name)
	{
		snprintf(m_szErrMsg, MAX_REDIS_ERR_MSG_LEN, "NULL command name.");
		return -1;
	}
	std::string namestr = name->AsString();
	namestr = string_tolower(namestr);
	RedisCommandConfigTable::iterator cfgfound = m_cfg_table.find(namestr);
	if (cfgfound == m_cfg_table.end())
	{
		snprintf(m_szErrMsg, MAX_REDIS_ERR_MSG_LEN, "not supported command:%s",
		        namestr.c_str());
		return -1;
	}
	RedisCommandConfig cfg = cfgfound->second;
	if (cfg.arity > 0 && cfg.arity != frame.GetArguments().size())
	{
		snprintf(m_szErrMsg, MAX_REDIS_ERR_MSG_LEN,
		        "wrong number of arguments for '%s' command", namestr.c_str());
		return -1;
	}

	RedisCommandHandlerArray::iterator it = m_handler_array.begin();
	while (it != m_handler_array.end())
	{
		RedisCommandHandler* handler = *it;
		if (NULL != handler)
		{
			handler->OnRedisCommand(cfg.type, connectionID, frame);
		}
		it++;
	}
	return 0;
}
