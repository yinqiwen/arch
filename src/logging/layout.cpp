/*
 * layout.cpp
 *
 *  Created on: 2011-4-19
 *      Author: wqy
 */
#include "logging/logger.hpp"
#include "logging/appender.hpp"
#include "logging/layout.hpp"
#include "util/string_helper.hpp"
#include "util/time_helper.hpp"

using namespace arch::logging;
using namespace arch::util;

const std::string SimpleLayout::CLASS_NAME = "arch.logging.SimpleLayout";
const std::string VerboseLayout::CLASS_NAME = "arch.logging.VerboseLayout";

LoggerLayout* LoggerLayoutFactory::CreateLoggerLayout(const std::string& name)
{
	LoggerLayout* layout = DoCreateLoggerLayout(name);
	if (NULL != layout)
	{
		layout->m_factory = this;
	}
	return layout;
}

void LoggerLayoutFactory::DestroyLoggerLayout(LoggerLayout* layout)
{
	DoDestroyLoggerLayout(layout);
}

void SimpleLayout::FormatAndAppend(const LogRecord& record,
		LoggerAppender* appender)
{
	RETURN_IF_NULL(appender);
	uint32 mills = record.timestamp % 1000;
	char timetag[256];
	struct tm& tm = get_current_tm();
	sprintf(timetag, "%u-%02u-%02u %02u:%02u:%02u", tm.tm_year + 1900, tm.tm_mon,
			tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	appender->Append("[%s,%03u]%s\n", timetag, mills, record.content.c_str());
}

void VerboseLayout::FormatAndAppend(const LogRecord& record,
		LoggerAppender* appender)
{
	RETURN_IF_NULL(appender);
	uint32 mills = record.timestamp % 1000;
	char timetag[256];
	struct tm& tm = get_current_tm();
	sprintf(timetag, "%u-%02u-%02u %02u:%02u:%02u", tm.tm_year + 1900, tm.tm_mon,
			tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	appender->Append("[%s,%03u][%s][%u][%s:%u]%s\n", timetag, mills,
			record.level_str, getpid(), get_basename(record.file).c_str(),
			record.fileno, record.content.c_str());
}
