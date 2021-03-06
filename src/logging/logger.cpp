/*
 * logger.cpp
 *
 *  Created on: 2011-4-11
 *      Author: qiyingwang
 */
#include "logging/logger.hpp"
#include "logging/appender.hpp"
#include "logging/layout.hpp"
#include "util/time_helper.hpp"
#include "util/string_helper.hpp"
#include <stdarg.h>
#include <utility>
#include <sstream>

#define THREAD_MODE_NO 0
#define THREAD_MODE_SINGLETON 1
#define THREAD_MODE_PERAPPENDER 2

using namespace arch::logging;
using namespace arch::util;

static const unsigned int k_default_log_line_buf_size = 256;

static const char* kLogLevelNames[] =
{ "FATAL", "ERROR", "WARN", "INFO", "DEBUG", "TRACE" };

static ConsoleAppender k_default_console_appender;

LoggerFactory::LoggerTable LoggerFactory::m_loggr_table;
LoggerFactory::AppenderFactoryTable LoggerFactory::m_appender_factory_table;
LoggerFactory::LayoutFactoryTable LoggerFactory::m_layout_factory_table;
Logger* LoggerFactory::m_developer_logger = NULL;
Logger* LoggerFactory::m_monitor_logger = NULL;
Timer* LoggerFactory::m_timer = NULL;

void Logger::AddAppender(LoggerAppender* appender, bool delegate)
{
	RETURN_IF_NULL(appender);
	m_appenders.push_back(std::make_pair(appender, delegate));
}

void Logger::FlushAllAppenders()
{
	AppenderArray::iterator it = m_appenders.begin();
	while (it != m_appenders.end())
	{
		LoggerAppender* appender = it->first;
		if (NULL != appender)
		{
			appender->Flush();
		}
		it++;
	}
}

void Logger::Log(LogLevel level, const char* file, const char* function,
		unsigned int fileno, const char* format, ...)
{
	//
	LogRecord record;
	pthread_t tid = pthread_self();
	record.file = file;
	record.function = function;
	record.fileno = fileno;
	record.tid = tid;
	record.timestamp = get_current_epoch_millis();
	if (level > 0 && level < ALL_LOG_LEVEL)
	{
		record.level_str = kLogLevelNames[level - 1];
	}
	else
	{
		record.level_str = "???";
	}
	size_t log_line_size = k_default_log_line_buf_size;
	va_list args;
	va_start(args, format);
	for (;;)
	{
		//log_line_size may be too large to overflow stack
		char content[log_line_size + 1];
#ifndef va_copy
#define va_copy(dst, src)   memcpy(&(dst), &(src), sizeof(va_list))
#endif
		va_list aq;
		va_copy(aq, args);
		int sz = vsnprintf(content, log_line_size, format, aq);
		va_end(aq);
		if (sz < 0)
		{
			return;
		}
		if ((size_t) sz < log_line_size || log_line_size >= 1024*1024)
		{
			content[log_line_size] = 0;
			record.content = content;
			break;
		}
		log_line_size <<= 1;
	}

	//vsnprintf(content, k_max_log_line_buf_size, format, args);
	//va_end(args);
	//record.content = content;

	if (m_appenders.empty())
	{
		LoggerAppender& console_appender = k_default_console_appender;
		console_appender.Append(record);
	}
	else
	{
		AppenderArray::iterator it = m_appenders.begin();
		while (it != m_appenders.end())
		{
			LoggerAppender* appender = it->first;
			if (NULL != appender)
			{
				appender->Append(record);
			}
			it++;
		}
	}
}

Logger::Logger() :
		m_level(ALL_LOG_LEVEL)
{

}

Logger::~Logger()
{
	AppenderArray::iterator it = m_appenders.begin();
	while (it != m_appenders.end())
	{
		LoggerAppender* appender = it->first;
		if (NULL != appender && it->second)
		{
			if (appender->GetFactory() != NULL)
			{
				appender->GetFactory()->DestroyLoggerAppender(appender);
			}
			else
			{
				DELETE(appender);
			}
		}
		it++;
	}
}

Logger* LoggerFactory::GetLogger(const std::string& name)
{
	LoggerTable::iterator found = m_loggr_table.find(name);
	if (found != m_loggr_table.end())
	{
		return found->second;
	}
	Logger* logger = NULL;
	NEW(logger, Logger);
	if (NULL != logger)
	{
		m_loggr_table[name] = logger;
	}
	if (NULL == m_developer_logger && name == "Develop")
	{
		m_developer_logger = logger;
	}
	if (NULL == m_monitor_logger && name == "Monitor")
	{
		m_monitor_logger = logger;
	}
	return logger;
}

bool LoggerFactory::InitLogger(const std::string& name, const Properties& props)
{
	Logger* logger = GetLogger(name);
	RETURN_FALSE_IF_NULL(logger);
	Properties::const_iterator fount_it = props.find("Level");
	uint32 i = 0;
	if (fount_it != props.end())
	{
		std::string level_str = string_toupper(fount_it->second);
		for (; (i + 1) < ALL_LOG_LEVEL; i++)
		{
			if (level_str == kLogLevelNames[i])
			{
				logger->SetLevel((LogLevel) (i + 1));
				break;
			}
		}
	}
	uint32 thread_mode = THREAD_MODE_NO;
	fount_it = props.find("ThreadMode");
	if (fount_it != props.end())
	{
		std::string mode = fount_it->second;
		if (!string_touint32(mode, thread_mode) || thread_mode
		> THREAD_MODE_PERAPPENDER)
		{
			fprintf(stderr, "Invalid thread mode value:%s", mode.c_str());
			return false;
		}
	}
	i = 0;
	char key[256];
	for (;; i++)
	{
		snprintf(key, sizeof(key) - 1, "Appender[%u].Class", i);
		Properties::const_iterator found = props.find(key);
		bool delegate = true;
		if (found != props.end())
		{
			const std::string& appender_class = found->second;
			LoggerAppender* base_appender = NULL;
			if (appender_class == RollingFileAppender::CLASS_NAME)
			{
				NEW(base_appender, RollingFileAppender);
			}
			else if (appender_class == DailyRollyFileAppender::CLASS_NAME)
			{
				NEW(base_appender, DailyRollyFileAppender);
			}
			else if (appender_class == FileAppender::CLASS_NAME)
			{
				NEW(base_appender, FileAppender);
			}
			else if (appender_class == ConsoleAppender::CLASS_NAME)
			{
				base_appender = &k_default_console_appender;
				delegate = false;
			}
			else
			{
				LoggerAppenderFactory* factory =
						LoggerFactory::GetAppenderFactory(appender_class);
				if (NULL == factory)
				{
					fprintf(stderr, "Invalid appender class name:%s",
							appender_class.c_str());
					return false;
				}
				base_appender = factory->CreateLoggerAppender(appender_class);
				if (NULL == base_appender)
				{
					fprintf(stderr, "Invalid appender class name:%s",
							appender_class.c_str());
					return false;
				}
			}
			LoggerAppender* final_appender = NULL;
			switch (thread_mode)
			{
				case THREAD_MODE_PERAPPENDER:
				{
					NEW(final_appender,
							ThreadingAppender(base_appender, delegate));
					delegate = true;
					break;
				}
				case THREAD_MODE_SINGLETON:
				{
					NEW(final_appender,
							SingletonThreadingAppender(base_appender, delegate));
					delegate = true;
					break;
				}
				case THREAD_MODE_NO:
				default:
				{
					final_appender = base_appender;
					break;
				}

			}
			Properties::const_iterator begin = props.begin();
			Properties appender_props;
			snprintf(key, sizeof(key) - 1, "Appender[%u].", i);
			while (begin != props.end())
			{
				const std::string& prop_key = begin->first;
				const std::string& prop_value = begin->second;
				if (prop_key.find(key) != std::string::npos)
				{
					std::string appender_prop_key = prop_key.substr(
							strlen(key));
					appender_props[appender_prop_key] = prop_value;
				}
				begin++;
			}
			if (final_appender->Configure(appender_props)
					&& final_appender->Init())
			{
				logger->AddAppender(final_appender, delegate);
			}
			else
			{
				return false;
			}
		}
		else
		{
			break;
		}
	}
	return true;
}

bool LoggerFactory::Configure(const INIProperties& ini_props)
{
	INIProperties props = ini_props;
	replace_env_var(props);
	Properties rootProps = props[""];

	Properties::const_iterator found = rootProps.find("Loggers");
	if (found == rootProps.end())
	{
		fprintf(stderr, "No logger defined in configure properties!\n");
		return false;
	}

	std::string names = found->second;
	std::vector<std::string> logger_names = split_string(names, ",");
	std::vector<std::string>::iterator it = logger_names.begin();
	while (it != logger_names.end())
	{
		std::string loggername = *it;
		if (!InitLogger(loggername, props[loggername]))
		{
			return false;
		}
		it++;
	}
	return true;
}

std::string LoggerFactory::ToString(LogLevel level)
{
	if (level > 0 && level < ALL_LOG_LEVEL)
	{
		return kLogLevelNames[level - 1];
	}
	else
	{
		return "???";
	}
}

LogLevel LoggerFactory::FromString(const std::string& levelstr)
{
	for (uint32 i = 1; i < ALL_LOG_LEVEL; i++)
	{
		if (levelstr == kLogLevelNames[i - 1])
		{
			return (LogLevel) i;
		}
	}
	return INVALID_LOG_LEVEL;
}

bool LoggerFactory::Configure(const std::string& conf)
{
	INIProperties props;
	bool ret = parse_ini_conf_file(conf, props);
	if (!ret)
	{
		fprintf(stderr, "Failed to parse conf file:%s for logging!\n",
				conf.c_str());
		return false;
	}
	return Configure(props);
}

bool LoggerFactory::RegisterAppenderFactory(const std::string& name,
		LoggerAppenderFactory* factory)
{
	RETURN_FALSE_IF_NULL(factory);
	if (m_appender_factory_table.find(name) == m_appender_factory_table.end())
	{
		m_appender_factory_table[name] = factory;
		return true;
	}
	else
	{
		return false;
	}
}
bool LoggerFactory::RegisterLayoutFactory(const std::string& name,
		LoggerLayoutFactory* factory)
{
	RETURN_FALSE_IF_NULL(factory);
	if (m_layout_factory_table.find(name) == m_layout_factory_table.end())
	{
		m_layout_factory_table[name] = factory;
		return true;
	}
	else
	{
		return false;
	}
}
LoggerAppenderFactory* LoggerFactory::GetAppenderFactory(
		const std::string& name)
{
	AppenderFactoryTable::iterator found = m_appender_factory_table.find(name);
	if (found != m_appender_factory_table.end())
	{
		return found->second;
	}
	return NULL;
}
LoggerLayoutFactory* LoggerFactory::GetLayoutFactory(const std::string& name)
{
	LayoutFactoryTable::iterator found = m_layout_factory_table.find(name);
	if (found != m_layout_factory_table.end())
	{
		return found->second;
	}
	return NULL;
}

void LoggerFactory::Destroy()
{
	m_developer_logger = NULL;
	m_monitor_logger = NULL;
	LoggerTable::iterator it = m_loggr_table.begin();
	while (it != m_loggr_table.end())
	{
		Logger* logger = it->second;
		DELETE(logger);
		it++;
	}
	m_loggr_table.clear();
	SingletonThreadingAppender::StopSingletonWorker();
	SetLoggerTimer(NULL);
}

