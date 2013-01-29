/*
 * redis_protocol.hpp
 *
 *  Created on: 2011-7-14
 *      Author: qiyingwang
 */

#ifndef REDIS_PROTOCOL_HPP_
#define REDIS_PROTOCOL_HPP_
#include "common/base.hpp"
#include <string>
#include <map>
#include <vector>

#define MAX_REDIS_ERR_MSG_LEN  2048
namespace arch
{
	namespace redis
	{
		enum RedisCommandType
		{
			GET = 0,
			SET = 1,
			SETNX = 2,
			SETEX = 3,
			APPEND = 4,
			STRLEN = 5,
			DEL = 6,
			EXISTS = 7,
			SETBIT = 8,
			GETBIT = 9,
			SETRANGE = 10,
			GETRANGE = 11,
			SUBSTR = 12,
			INCR = 13,
			DECR = 14,
			MGET = 15,
			LPUSH = 16,
			RPUSH = 17,
			RPUSHX = 18,
			LPUSHX = 19,
			LINSERT = 20,
			RPOP = 21,
			LPOP = 22,
			BPOP = 23,
			BRPOPLPUSH = 24,
			BLPOP = 25,
			LLEN = 26,
			LINDEX = 27,
			LSET = 28,
			LRANGE = 29,
			LTRIM = 30,
			LREM = 31,
			RPOPLPUSH = 32,
			SADD = 33,
			SREM = 34,
			SMOVE = 35,
			SISMEMBER = 36,
			SCARD = 37,
			SPOP = 38,
			SRANDMEMBER = 39,
			SINTER = 40,
			SINTERSTORE = 42,
			SUNION = 43,
			SUNIONSTORE = 44,
			SDIFF = 45,
			SDIFFSTORE = 46,
			SMEMBERS = 47,
			ZADD = 48,
			ZINCRBY = 49,
			ZREM = 50,
			ZREMRANGEBYSCORE = 51,
			ZREMRANGEBYRANK = 52,
			ZUNIONSTORE = 53,
			ZINTERSTORE = 54,
			ZRANGE = 55,
			ZRANGEBYSCORE = 56,
			ZREVRANGEBYSCORE = 57,
			ZCOUNT = 58,
			ZREVRANGE = 59,
			ZCARD = 60,
			ZSCORE = 61,
			ZRANK = 62,
			ZREVRANK = 63,
			HSET = 64,
			HSETNX = 65,
			HGET = 66,
			HMSET = 67,
			HMGET = 68,
			HINCRBY = 69,
			HDEL = 70,
			HLEN = 71,
			HKEYS = 72,
			HVALS = 73,
			HGETALL = 74,
			HEXISTS = 75,
			INCRBY = 76,
			DECRBY = 77,
			GETSET = 78,
			MSET = 79,
			MSETNX = 80,
			RANDOMKEY = 81,
			SELECT = 82,
			MOVE = 83,
			RENAME = 84,
			RENAMENX = 85,
			EXPIRE = 86,
			EXPIREAT = 87,
			KEYS = 88,
			DBSIZE = 89,
			AUTH = 90,
			PING = 91,
			ECHO = 92,
			SAVE = 93,
			BGSAVE = 94,
			BGREWRITEEAOF = 95,
			SHUTDOWN = 96,
			LASTSAVE = 97,
			TYPE = 98,
			MULTI = 99,
			EXEC = 100,
			DISCARD = 101,
			SYNC = 102,
			FLUSHDB = 103,
			FLUSHALL = 104,
			SORT = 105,
			INFO = 106,
			MONITOR = 107,
			TTL = 108,
			PERSIST = 109,
			SLAVEOF = 110,
			DEBUG = 111,
			CONFIG =112,
			SUBSCRIBE = 113,
			UNSUBSCRIBE = 114,
			PSUBSCRIBE = 115,
			PUNSUBSCRIBE = 116,
			PUBLISH = 117,
			WATCH = 118,
			UNWATCH = 119,
			QUIT = 120
		};

		struct RedisCommandConfig
		{
				const char *name;
				RedisCommandType type;
				int arity;
				int flag;
		};

		class RedisCommandFrame;
		class RedisCommandHandler
		{
			public:
				virtual void OnRedisCommand(RedisCommandType type,
				        uint32 connection_id, RedisCommandFrame& command) = 0;
				virtual ~RedisCommandHandler()
				{
				}
		};

		class RedisCommandManager
		{
			private:
				typedef std::map<std::string, RedisCommandConfig>
				        RedisCommandConfigTable;
				typedef std::vector<RedisCommandHandler*>
				        RedisCommandHandlerArray;
				RedisCommandConfigTable m_cfg_table;
				RedisCommandHandlerArray m_handler_array;
				char m_szErrMsg[MAX_REDIS_ERR_MSG_LEN + 1];
			public:
				RedisCommandManager();
				int Init();
				void RegisterHandler(RedisCommandHandler* handler);
				int HandlerCommandFrame(RedisCommandFrame& frame,
				        uint32 commectionID);
				const char* GetErrorMessage()
				{
					return m_szErrMsg;
				}
		};

	}
}

#endif /* REDIS_PROTOCOL_HPP_ */
