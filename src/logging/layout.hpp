/*
 * layout.hpp
 *
 *  Created on: 2011-4-19
 *      Author: wqy
 */

#ifndef LOGGING_LAYOUT_HPP_
#define LOGGING_LAYOUT_HPP_

#include "logging/logger.hpp"
namespace arch
{
    namespace logging
    {
        class SimpleLayout: public LoggerLayout
        {
            private:
                void
                FormatAndAppend(const LogRecord& record,
                        LoggerAppender* appender);
            public:
                static const std::string CLASS_NAME;
        };

        class VerboseLayout: public LoggerLayout
        {
            private:
                void
                FormatAndAppend(const LogRecord& record,
                        LoggerAppender* appender);
            public:
                static const std::string CLASS_NAME;
        };
    }
}

#endif /* LAYOUT_HPP_ */
