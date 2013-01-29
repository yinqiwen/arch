/*
 * logger.h
 *
 *  Created on: 2011-4-11
 *      Author: qiyingwang
 */

#ifndef ARCH_LOGGER_HPP_
#define ARCH_LOGGER_HPP_
#include "common/base.hpp"
#include "thread/thread.hpp"
#include "util/config_helper.hpp"
#include "timer/timer.hpp"
#include <map>
#include <vector>
#include <deque>
#include <string>
#include <queue>
#include <sys/time.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

using arch::util::Properties;
using arch::util::INIProperties;
using arch::timer::Timer;

namespace arch
{
	namespace logging
	{
		enum LogLevel
		{
			NONE_LOG_LEVEL = 0,
			FATAL_LOG_LEVEL = 1,
			ERROR_LOG_LEVEL = 2,
			WARN_LOG_LEVEL = 3,
			INFO_LOG_LEVEL = 4,
			DEBUG_LOG_LEVEL = 5,
			TRACE_LOG_LEVEL = 6,
			ALL_LOG_LEVEL = 7,

			INVALID_LOG_LEVEL = 100
		};

		struct LogRecord
		{
				//unsigned int logger_id;
				uint64 timestamp;
				const char* level_str;
				const char* file;
				unsigned int fileno;
				const char* function;
				pthread_t tid;
				std::string content;

				static LogRecord* Clone(const LogRecord& record)
				{
					LogRecord* new_record = NULL;
					NEW(new_record, LogRecord);
					RETURN_NULL_IF_NULL(new_record);
					new_record->timestamp = record.timestamp;
					new_record->file = record.file;
					new_record->fileno = record.fileno;
					new_record->function = record.function;
					new_record->tid = record.tid;
					new_record->content = record.content;
					new_record->level_str = record.level_str;
					return new_record;
				}
		};

		class Logger;
		struct LoggerAppender;
		class LoggerLayoutFactory;
		struct LoggerLayout
		{
			private:
				LoggerLayoutFactory* m_factory;
				//friend class LoggerAppender;
				friend class LoggerLayoutFactory;
			public:
				LoggerLayout() :
						m_factory(NULL)
				{
				}
				inline LoggerLayoutFactory* GetFactory()
				{
					return m_factory;
				}
				virtual void FormatAndAppend(const LogRecord& record,
						LoggerAppender* appender) = 0;
				virtual ~LoggerLayout()
				{
				}
		};

		class LoggerLayoutFactory
		{
			protected:
				virtual LoggerLayout* DoCreateLoggerLayout(
						const std::string& name) = 0;
				virtual void DoDestroyLoggerLayout(LoggerLayout* layout) = 0;
			public:
				LoggerLayout* CreateLoggerLayout(const std::string& name);
				void DestroyLoggerLayout(LoggerLayout* layout);
				virtual ~LoggerLayoutFactory()
				{
				}
		};
		class LoggerFactory;
		class LoggerAppenderFactory;
		struct LoggerAppender:public Runnable
		{
			protected:
				//virtual void DoAppend(const char* format, ...) = 0;
				LoggerAppenderFactory* m_factory;
				LoggerLayout* m_layout;
				bool m_self_managed;
				//friend class LoggerFactory;
				friend class LoggerAppenderFactory;
				void Run();
			public:
				LoggerAppender() :
						m_factory(NULL), m_layout(NULL), m_self_managed(false)
				{
				}
				LoggerAppenderFactory* GetFactory()
				{
					return m_factory;
				}
				LoggerLayout& GetLayout();
				void SetLayout(LoggerLayout* layout, bool delegate = false);
				virtual void Append(const LogRecord& record);
				virtual void Append(const char* format, ...);
				//virtual void Flush();
				virtual bool Configure(const Properties& props);
				virtual bool Init();
				virtual bool Flush()
				{
					return true;
				}
				virtual ~LoggerAppender();
		};

		class LoggerAppenderFactory
		{
			protected:
				virtual LoggerAppender* DoCreateLoggerAppender(
						const std::string& name) = 0;
				virtual void
				DoDestroyLoggerAppender(LoggerAppender* appender) = 0;
			public:
				LoggerAppender* CreateLoggerAppender(const std::string& name);
				void DestroyLoggerAppender(LoggerAppender* appender);
				virtual ~LoggerAppenderFactory()
				{
				}
		};

		class Logger
		{
			private:
				LogLevel m_level;
				Logger();
				friend class LoggerFactory;
			protected:
				typedef std::vector<std::pair<LoggerAppender*, bool> > AppenderArray;
				AppenderArray m_appenders;
			public:
				void Log(LogLevel level, const char* file, const char* function,
						unsigned int fileno, const char* format, ...);
				inline bool IsEnable(LogLevel level)
				{
					return m_level >= level;
				}
				inline void SetLevel(LogLevel level)
				{
					m_level = level;
				}
				inline LogLevel GetLvel()
				{
					return m_level;
				}
				void AddAppender(LoggerAppender* appender,
						bool delegate = false);
				void FlushAllAppenders();
				~Logger();
		};

		class LoggerFactory
		{
			public:
				typedef LoggerAppender
				* AppenderCreater(const std::string& name);
			private:
				typedef std::map<std::string, Logger*> LoggerTable;
				typedef std::map<std::string, LoggerAppenderFactory*> AppenderFactoryTable;
				typedef std::map<std::string, LoggerLayoutFactory*> LayoutFactoryTable;
				static LoggerTable m_loggr_table;
				static AppenderFactoryTable m_appender_factory_table;
				static LayoutFactoryTable m_layout_factory_table;
				static Logger* m_developer_logger;
				static Logger* m_monitor_logger;
				static Timer* m_timer;
			public:
				static bool InitLogger(const std::string& name,
						const Properties& props);
				static Logger* GetLogger(const std::string& name);
				static std::string ToString(LogLevel level);
				static LogLevel FromString(const std::string& levelstr);
				static inline Logger* GetDevelopLogger()
				{
					return NULL != m_developer_logger ?
							m_developer_logger : GetLogger("Develop");
				}
				static inline Logger* GetMonitorLogger()
				{
					return NULL != m_monitor_logger ?
							m_monitor_logger : GetLogger("Monitor");
				}
				static void SetLoggerTimer(Timer* timer)
				{
					m_timer = timer;
				}
				static Timer* GetLoggerTimer()
				{
					return m_timer;
				}
				static bool RegisterAppenderFactory(const std::string& name,
						LoggerAppenderFactory* factory);
				static bool RegisterLayoutFactory(const std::string& name,
						LoggerLayoutFactory* factory);
				static LoggerAppenderFactory* GetAppenderFactory(
						const std::string& name);
				static LoggerLayoutFactory* GetLayoutFactory(
						const std::string& name);
				static bool Configure(const std::string& conf);
				static bool Configure(const INIProperties& props);
				static void Destroy();
		};

	}
}

#endif /* LOGGER_H_ */
