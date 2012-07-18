/*
 * appender.cpp
 *
 *  Created on: 2011-4-19
 *      Author: wqy
 */
#include "logging/logger.hpp"
#include "logging/appender.hpp"
#include "logging/layout.hpp"
#include "concurrent/lock_guard.hpp"
#include "util/string_helper.hpp"
#include "util/proctitle_helper.h"
#include "util/time_helper.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <sstream>
#include <utility>

using namespace arch::logging;
using namespace arch::concurrent;
using namespace arch::util;

const char* RollingFileAppender::CLASS_NAME = "arch.logging.RollingFileAppender";
const char* DailyRollyFileAppender::CLASS_NAME =
		"arch.logging.DailyRollingFileAppender";
const char* FileAppender::CLASS_NAME = "arch.logging.FileAppender";
const char* ConsoleAppender::CLASS_NAME = "arch.logging.ConsoleAppender";

static VerboseLayout k_default_verbose_layout;
static SimpleLayout k_default_simple_layout;

//static const uint32 kMaxLogLineSize = 2048;
//static const uint32 kDefaultAIOTaskBufferSize = 256;

SingletonThreadingAppenderWorker * SingletonThreadingAppender::m_singleton_worker =
		NULL;

LoggerAppender* LoggerAppenderFactory::CreateLoggerAppender(
		const std::string& name)
{
	LoggerAppender* appender = DoCreateLoggerAppender(name);
	if (NULL != appender)
	{
		appender->m_factory = this;
	}
	return appender;
}

void LoggerAppenderFactory::DestroyLoggerAppender(LoggerAppender* appender)
{
	DoDestroyLoggerAppender(appender);
}

void LoggerAppender::SetLayout(LoggerLayout* layout, bool delegate)
{
	if (m_self_managed)
	{
		if (NULL != m_layout)
		{
			if (NULL != m_layout->GetFactory())
			{
				m_layout->GetFactory()->DestroyLoggerLayout(m_layout);
				m_layout = NULL;
			}
			else
			{
				DELETE(m_layout);
			}
		}
	}
	m_layout = layout;
	m_self_managed = delegate;
}

LoggerLayout& LoggerAppender::GetLayout()
{
	if (NULL == m_layout)
	{
		SetLayout(&k_default_verbose_layout, false);
	}
	return *m_layout;
}

LoggerAppender::~LoggerAppender()
{
	SetLayout(NULL, false);
}

void LoggerAppender::Append(const LogRecord& record)
{
	GetLayout().FormatAndAppend(record, this);
}

void LoggerAppender::Append(const char* format, ...)
{
	assert(1 == 0 && "Not implemented in sub-class.");
}

bool LoggerAppender::Configure(const Properties& props)
{
	Properties::const_iterator found = props.find("Layout");
	if (found != props.end())
	{
		std::string layout_class = found->second;
		if (layout_class == VerboseLayout::CLASS_NAME)
		{
			SetLayout(&k_default_verbose_layout, false);
		}
		else if (layout_class == SimpleLayout::CLASS_NAME)
		{
			SetLayout(&k_default_simple_layout, false);
		}
		else
		{
			LoggerLayoutFactory* factory = LoggerFactory::GetLayoutFactory(
					layout_class);
			if (NULL == factory)
			{
				fprintf(stderr, "Invalid layout class name:%s",
						layout_class.c_str());
				return false;
			}
			LoggerLayout* layout = factory->CreateLoggerLayout(layout_class);
			if (NULL == layout)
			{
				fprintf(stderr,
						"Invalid layout class name:%s to create layout instance",
						layout_class.c_str());
				return false;
			}
			SetLayout(layout, true);
		}
	}
	return true;
}

void LoggerAppender::Run()
{
	Flush();
}

bool LoggerAppender::Init()
{
	return true;
}

void ConsoleAppender::Append(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stdout, format, args);
	va_end(args);
}

FileAppender::FileAppender() :
		m_write_buffer(NULL), m_write_buffer_size(512 * 1024), m_flush_period(
				1000), m_flush_task_id(-1), m_stream(NULL), m_immediateFlush(
				false)
{
	std::string base = getproctitle();
	std::stringstream namess(std::stringstream::in | std::stringstream::out);
	namess << base << "_" << getpid() << ".log";
	m_base_file_name = namess.str();
}

void FileAppender::ReOpen()
{
	if (NULL != m_stream)
	{
		fclose(m_stream);
		m_stream = NULL;
	}
	m_stream = fopen(m_base_file_name.c_str(), "a+");
}

bool FileAppender::Flush()
{
	if (NULL != m_stream)
	{
		fflush(m_stream);
	}
	return true;
}

void FileAppender::VAppend(const char *format, va_list ap)
{
	if (NULL != m_stream)
	{
		vfprintf(m_stream, format, ap);
		if (m_immediateFlush)
		{
			Flush();
		}
	}
}

void FileAppender::Append(const char* format, ...)
{
	//struct stat st;
	if (NULL == m_stream)
	{
		ReOpen();
	}
	if (NULL == m_stream)
	{
		return;
	}

	va_list args;
	va_start(args, format);
	VAppend(format, args);
	va_end(args);
}

bool FileAppender::Configure(const Properties& props)
{
	if (!LoggerAppender::Configure(props))
	{
		return false;
	}
	Properties::const_iterator found = props.find("File");
	if (found != props.end())
	{
		m_base_file_name = found->second;
	}

	found = props.find("ImmediateFlush");
	if (found != props.end())
	{
		std::string enable_flush_str = string_tolower(found->second);
		m_immediateFlush = (enable_flush_str == "true");
		if (m_immediateFlush)
		{
			found = props.find("WriteBufferSize");
			if (found != props.end())
			{
				string_touint32(found->second, m_write_buffer_size);
			}
			found = props.find("FlushPeriod");
			if (found != props.end())
			{
				string_touint32(found->second, m_flush_period);
			}
		}
	}

	return true;
}

bool FileAppender::Init()
{
	if (NULL == m_stream && !m_base_file_name.empty())
	{
		m_stream = fopen(m_base_file_name.c_str(), "a+");

	}
	if (NULL == m_stream)
	{
		fprintf(stderr, "Can NOT open log file:%s for logging!\n",
				m_base_file_name.c_str());
		return false;
	}
	if (!m_immediateFlush)
	{
		DELETE_A(m_write_buffer);
		NEW(m_write_buffer, char[m_write_buffer_size]);
		setvbuf(m_stream, m_write_buffer, _IOFBF, m_write_buffer_size);
		if (-1 == m_flush_task_id && NULL != LoggerFactory::GetLoggerTimer())
		{
			LoggerFactory::GetLoggerTimer()->Schedule(this, m_flush_period,
					m_flush_period);
		}
	}
	return true;
}

FileAppender::~FileAppender()
{
	if (NULL != m_stream)
	{
		fclose(m_stream);
		m_stream = NULL;
	}
	DELETE_A(m_write_buffer);
	if (-1 != m_flush_task_id && NULL != LoggerFactory::GetLoggerTimer())
	{
		LoggerFactory::GetLoggerTimer()->Cancel(m_flush_task_id);
	}
}

RollingFileAppender::RollingFileAppender() :
		m_max_file_size(20000000), m_max_rolling_index(10)
{

}

void RollingFileAppender::VAppend(const char *format, va_list ap)
{
	long file_size = -1;
	if (NULL != m_stream)
	{
		file_size = ftell(m_stream);
	}
	//    if (0 == fstat(m_fd, &sb))
	//    {
	//        file_size = sb.st_size;
	//    }
	if (file_size < 0)
	{
		ReOpen();
	}
	else if (file_size >= m_max_file_size)
	{
		RollOver();
		ReOpen();
	}
	if (NULL == m_stream)
	{
		return;
	}
	FileAppender::VAppend(format, ap);
}

//void RollingFileAppender::Append(const char* format, ...)
//{
//	va_list args;
//	va_start(args, format);
//	this->VAppend(format, args);
//	va_end(args);
//}

bool RollingFileAppender::Configure(const Properties& props)
{
	if (!FileAppender::Configure(props))
	{
		return false;
	}
	Properties::const_iterator found = props.find("MaxFileSize");
	if (found != props.end())
	{
		std::string max_size_str = string_toupper(found->second);
		m_max_file_size = atoll(max_size_str.c_str());
		if (max_size_str.find("MB") == (max_size_str.size() - 2))
		{
			m_max_file_size *= (1024 * 1024); // convert to megabytes
		}
		if (max_size_str.find("KB") == (max_size_str.size() - 2))
		{
			m_max_file_size *= 1024; // convert to kilobytes
		}
	}
	found = props.find("MaxBackupIndex");
	if (found != props.end())
	{
		string_touint32(found->second, m_max_rolling_index);
	}
	return true;
}

void RollingFileAppender::RollOver()
{
	if (NULL != m_stream)
	{
		fclose(m_stream);
		m_stream = NULL;
	}
	std::stringstream oldest_file(
			std::stringstream::in | std::stringstream::out);
	oldest_file << m_base_file_name << "." << m_max_rolling_index;
	remove(oldest_file.str().c_str());

	for (int i = m_max_rolling_index - 1; i >= 1; --i)
	{
		std::stringstream source_oss(
				std::stringstream::in | std::stringstream::out);
		std::stringstream target_oss(
				std::stringstream::in | std::stringstream::out);

		source_oss << m_base_file_name << "." << i;
		target_oss << m_base_file_name << "." << (i + 1);
		remove(target_oss.str().c_str());
		rename(source_oss.str().c_str(), target_oss.str().c_str());
		//loglog_renaming_result(*loglog, source, target, ret);
	}
	std::stringstream ss(std::stringstream::in | std::stringstream::out);
	ss << m_base_file_name << ".1";
	string path = ss.str();
	rename(m_base_file_name.c_str(), path.c_str());
}

RollingFileAppender::~RollingFileAppender()
{

}

DailyRollyFileAppender::DailyRollyFileAppender() :
		m_day(0)
{
}

bool DailyRollyFileAppender::Configure(const Properties& props)
{
	if (!RollingFileAppender::Configure(props))
	{
		return false;
	}
	m_origin_file_name = m_base_file_name;
	SetDailyFileName();
	return true;
}

void DailyRollyFileAppender::SetDailyFileName()
{
	time_t now;
	time(&now);
	struct tm temp;
	localtime_r(&now, &temp);
	char buf[100];
	strftime(buf, arraysize(buf), "-%Y-%m-%d", &temp);
	m_base_file_name = m_origin_file_name;
	m_base_file_name += buf;
}

void DailyRollyFileAppender::VAppend(const char *format, va_list ap)
{
	time_t now;
	time(&now);
	struct tm temp;
	localtime_r(&now, &temp);
	if (temp.tm_mday != m_day)
	{
		m_day = temp.tm_mday;
		SetDailyFileName();
		ReOpen();
	}
	RollingFileAppender::VAppend(format, ap);
}

void ThreadingAppender::Run()
{
	std::deque<LogRecord*> ready_list;
	while (!m_stoped)
	{
		do
		{
			LockGuard<ThreadMutexLock> guard(m_mutex);
			if (m_record_queue.empty())
			{
				m_mutex.Wait(1000);
			}
			while (!m_record_queue.empty())
			{
				LogRecord* record = m_record_queue.front();
				ready_list.push_back(record);
				m_record_queue.pop_front();
			}
		} while (0);
		while (!ready_list.empty())
		{
			LogRecord* record = ready_list.front();
			ready_list.pop_front();
			if (NULL != m_appender && NULL != record)
			{
				m_appender->Append(*record);
			}
			DELETE(record);
		}
	}
}

bool ThreadingAppender::Configure(const Properties& props)
{
	if (NULL != m_appender)
	{
		return m_appender->Configure(props);
	}
	return false;
}

ThreadingAppender::~ThreadingAppender()
{
	m_stoped = true;
	LockGuard<ThreadMutexLock> guard(m_mutex);
	m_mutex.Notify();
	if (m_delegate)
	{
		if (NULL != m_appender->GetFactory())
		{
			m_appender->GetFactory()->DestroyLoggerAppender(m_appender);
			m_appender = NULL;
		}
		else
		{
			DELETE(m_appender);
		}
	}
}

void ThreadingAppender::Append(const LogRecord& record)
{
	RETURN_IF_NULL(m_appender);
	if (!m_started)
	{
		Start();
		m_started = true;
	}
	LockGuard<ThreadMutexLock> guard(m_mutex);
	LogRecord* new_record = LogRecord::Clone(record);
	m_record_queue.push_back(new_record);
	m_mutex.Notify();
}
void SingletonThreadingAppenderWorker::Hang(uint64 timeout)
{
	m_hang_timeout = timeout;
	LockGuard<ThreadMutexLock> guard(m_mutex);
	m_mutex.Notify();
}

void SingletonThreadingAppenderWorker::Stop()
{
	m_stoped = true;
	LockGuard<ThreadMutexLock> guard(m_mutex);
	m_mutex.Notify();
}

void SingletonThreadingAppenderWorker::Run()
{
	std::map<LoggerAppender*, std::deque<LogRecord*> > ready_map;
	while (!m_stoped)
	{
		if (m_hang_timeout > 0)
		{
			usleep(m_hang_timeout * 1000);
			m_hang_timeout = 0;
		}
		//printf("Start at pid:%d %llu\n", getpid(), getCurrentEpochTimeMillis());
		do
		{
			LockGuard<ThreadMutexLock> guard(m_mutex);
			if (m_record_queue.empty())
			{
				m_mutex.Wait(1000);
			}
			while (!m_record_queue.empty())
			{
				std::pair<LoggerAppender*, LogRecord*>& record =
						m_record_queue.front();
				ready_map[record.first].push_back(record.second);
				m_record_queue.pop_front();
			}
		} while (0);
		if (!ready_map.empty())
		{
			std::map<LoggerAppender*, std::deque<LogRecord*> >::iterator it =
					ready_map.begin();
			while (it != ready_map.end())
			{
				LoggerAppender* appender = it->first;
				std::deque<LogRecord*>& ready_list = it->second;
				while (!ready_list.empty())
				{
					LogRecord* record = ready_list.front();
					ready_list.pop_front();
					if (NULL != appender && NULL != record)
					{
						appender->Append(*record);
					}
					DELETE(record);
				}
				it++;
			}
		}
		ready_map.clear();
	}

}

//void SingletonThreadingAppender::ChildAtFork()
//{
//    DELETE(m_singleton_worker);
//}

void SingletonThreadingAppenderWorker::Append(LoggerAppender* appender,
		const LogRecord& record)
{
	RETURN_IF_NULL(appender);
	if (!m_started)
	{
		Start();
		m_started = true;
	}
	LockGuard<ThreadMutexLock> guard(m_mutex);
	LogRecord* new_record = LogRecord::Clone(record);
	m_record_queue.push_back(std::make_pair(appender, new_record));
	m_mutex.Notify();
}

SingletonThreadingAppenderWorker* SingletonThreadingAppender::GetSingletonWorker()
{
	if (NULL == m_singleton_worker)
	{
		NEW(m_singleton_worker, SingletonThreadingAppenderWorker);
		//pthread_atfork(NULL, HangSingletonWorker, NULL);
	}
	return m_singleton_worker;
}

bool SingletonThreadingAppender::Configure(const Properties& props)
{
	if (NULL != m_appender)
	{
		return m_appender->Configure(props);
	}
	return false;
}

void SingletonThreadingAppender::Append(const LogRecord& record)
{
	GetSingletonWorker()->Append(m_appender, record);
}

SingletonThreadingAppender::~SingletonThreadingAppender()
{
	//DELETE(m_appender);
	if (m_delegate)
	{
		if (NULL != m_appender->GetFactory())
		{
			m_appender->GetFactory()->DestroyLoggerAppender(m_appender);
			m_appender = NULL;
		}
		else
		{
			DELETE(m_appender);
		}
	}
}

void SingletonThreadingAppender::HangSingletonWorker()
{
	if (NULL != m_singleton_worker)
	{
		m_singleton_worker->Hang(1000);
	}
}

void SingletonThreadingAppender::StopSingletonWorker()
{
	if (NULL != m_singleton_worker)
	{
		m_singleton_worker->Stop();
		//DELETE(m_singleton_worker);
		m_singleton_worker = NULL;
	}
}
