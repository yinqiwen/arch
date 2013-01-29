/*
 * logger_appender.hpp
 *
 *  Created on: 2011-4-11
 *      Author: qiyingwang
 */

#ifndef LOGGER_APPENDER_HPP_
#define LOGGER_APPENDER_HPP_
#include "logging/logger.hpp"
#include "util/config_helper.hpp"
#include "concurrent/thread_mutex_lock.hpp"
#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <deque>

using arch::util::Properties;
namespace arch
{
	namespace logging
	{
		class FileAppender: public LoggerAppender
		{
			protected:
			    char* m_write_buffer;
			    uint32 m_write_buffer_size;
			    uint32 m_flush_period;
			    int32 m_flush_task_id;
				FILE* m_stream;
				bool m_immediateFlush;
				string m_base_file_name;
				typedef std::deque<LogRecord*> LogRecordQueue;
				virtual void ReOpen();
				//void CheckWritingQuque();
				virtual void VAppend(const char *format, va_list ap);
				virtual void Append(const char* format, ...);
				virtual bool Flush();
			public:
				static const char* CLASS_NAME;
				FileAppender();
				bool Configure(const Properties& props);
				bool Init();
				~FileAppender();
		};

		class ConsoleAppender: public LoggerAppender
		{
			private:
				void Append(const char* format, ...);
			public:
				static const char* CLASS_NAME;

		};

		class RollingFileAppender: public FileAppender
		{
			protected:
				uint32 m_max_file_size;
				uint32 m_max_rolling_index;
				void RollOver();
				void VAppend(const char *format, va_list ap);
				//void Append(const char* format, ...);
			public:
				static const char* CLASS_NAME;
				RollingFileAppender();
				bool Configure(const Properties& props);
				virtual ~RollingFileAppender();
		};

		class DailyRollyFileAppender: public RollingFileAppender
		{
			protected:
				string m_origin_file_name;
				uint32 m_day;
				void VAppend(const char *format, va_list ap);
				void SetDailyFileName();
			public:
				static const char* CLASS_NAME;
				DailyRollyFileAppender();
				bool Configure(const Properties& props);
		};

		class ThreadingAppender: public LoggerAppender,
		        public arch::thread::Thread
		{
			private:
				bool m_started;
				bool m_stoped;
				LoggerAppender* m_appender;
				bool m_delegate;
				std::deque<LogRecord*> m_record_queue;
				arch::concurrent::ThreadMutexLock m_mutex;
				void Run();
			public:
				ThreadingAppender(LoggerAppender* appender, bool delegate) :
					m_started(false), m_stoped(false), m_appender(appender),
					        m_delegate(delegate)
				{
				}
				bool Configure(const Properties& props);
				void Append(const LogRecord& record);
				~ThreadingAppender();
		};

		class SingletonThreadingAppenderWorker: public arch::thread::Thread
		{
			private:
				bool m_started;
				bool m_stoped;
				uint64 m_hang_timeout;
				arch::concurrent::ThreadMutexLock m_mutex;
				typedef std::deque<std::pair<LoggerAppender*, LogRecord*> >
				        LogRecordQueue;
				LogRecordQueue m_record_queue;
				void Run();
			public:
				SingletonThreadingAppenderWorker() :
					m_started(false), m_stoped(false), m_hang_timeout(0)
				{
				}
				void Append(LoggerAppender* appender, const LogRecord& record);
				void Stop();
				void Hang(uint64 timeout);
		};

		class SingletonThreadingAppender: public LoggerAppender
		{
			private:
				static SingletonThreadingAppenderWorker* m_singleton_worker;
				static SingletonThreadingAppenderWorker* GetSingletonWorker();
				LoggerAppender* m_appender;
				bool m_delegate;

			public:
				SingletonThreadingAppender(LoggerAppender* appender,
				        bool delegate) :
					m_appender(appender), m_delegate(delegate)
				{
				}
				bool Configure(const Properties& props);
				void Append(const LogRecord& record);
				static void HangSingletonWorker();
				static void StopSingletonWorker();
				~SingletonThreadingAppender();
		};
	}
}

#endif /* LOGGER_APPENDER_HPP_ */
