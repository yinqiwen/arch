/*
 * logger_macros.hpp
 *
 *  Created on: 2011-4-20
 *      Author: qiyingwang
 */

#ifndef LOGGER_MACROS_HPP_
#define LOGGER_MACROS_HPP_
#include "logging/logger.hpp"

#define RUN_LOG(...) do {\
        arch::logging::Logger* logger = arch::logging::LoggerFactory::GetMonitorLogger();\
        if(NULL != logger)\
        {                 \
           if(logger->IsEnable(arch::logging::INFO_LOG_LEVEL)) { \
               logger->Log(arch::logging::INFO_LOG_LEVEL, __FILE__, __FUNCTION__, __LINE__,__VA_ARGS__); \
           }\
        }\
}while(0)

#define DEBUG_ENABLED() (NULL != arch::logging::LoggerFactory::GetDevelopLogger() && \
                       arch::logging::LoggerFactory::GetDevelopLogger()->IsEnable(arch::logging::DEBUG_LOG_LEVEL))
#define TRACE_ENABLED() (NULL != arch::logging::LoggerFactory::GetDevelopLogger() && \
                       arch::logging::LoggerFactory::GetDevelopLogger()->IsEnable(arch::logging::TRACE_LOG_LEVEL))
#define ERROR_ENABLED() (NULL != arch::logging::LoggerFactory::GetDevelopLogger() && \
                       arch::logging::LoggerFactory::GetDevelopLogger()->IsEnable(arch::logging::ERROR_LOG_LEVEL))
#define INFO_ENABLED() (NULL != arch::logging::LoggerFactory::GetDevelopLogger() && \
                       arch::logging::LoggerFactory::GetDevelopLogger()->IsEnable(arch::logging::INFO_LOG_LEVEL))
#define FATAL_ENABLED() (NULL != arch::logging::LoggerFactory::GetDevelopLogger() && \
                       arch::logging::LoggerFactory::GetDevelopLogger()->IsEnable(arch::logging::FATAL_LOG_LEVEL))

#define DEBUG_LOG( ...) do {\
        arch::logging::Logger* logger = arch::logging::LoggerFactory::GetDevelopLogger();\
        if(NULL != logger)\
        {                 \
           if(logger->IsEnable(arch::logging::DEBUG_LOG_LEVEL)) { \
               logger->Log(arch::logging::DEBUG_LOG_LEVEL, __FILE__, __FUNCTION__, __LINE__,__VA_ARGS__); \
           }                  \
        }\
}while(0)

#define WARN_LOG(...) do {\
        arch::logging::Logger* logger = arch::logging::LoggerFactory::GetDevelopLogger();\
        if(NULL != logger)\
        {                 \
           if(logger->IsEnable(arch::logging::WARN_LOG_LEVEL)) { \
               logger->Log(arch::logging::WARN_LOG_LEVEL, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); \
           }                  \
        }\
}while(0)

#define TRACE_LOG(...) do {\
        arch::logging::Logger* logger = arch::logging::LoggerFactory::GetDevelopLogger();\
        if(NULL != logger)\
        {                 \
           if(logger->IsEnable(arch::logging::TRACE_LOG_LEVEL)) { \
               logger->Log(arch::logging::TRACE_LOG_LEVEL, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); \
           }                  \
        }\
}while(0)

#define ERROR_LOG(...) do {\
        arch::logging::Logger* logger = arch::logging::LoggerFactory::GetDevelopLogger();\
        if(NULL != logger)\
        {                 \
           if(logger->IsEnable(arch::logging::ERROR_LOG_LEVEL)) { \
               logger->Log(arch::logging::ERROR_LOG_LEVEL, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); \
           }                  \
        }\
}while(0)

#define FATAL_LOG(...) do {\
        arch::logging::Logger* logger = arch::logging::LoggerFactory::GetDevelopLogger();\
        if(NULL != logger)\
        {                 \
           if(logger->IsEnable(arch::logging::FATAL_LOG_LEVEL)) { \
               logger->Log(arch::logging::FATAL_LOG_LEVEL, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); \
           }                  \
        }\
}while(0)

#define INFO_LOG(...) do {\
        arch::logging::Logger* logger = arch::logging::LoggerFactory::GetDevelopLogger();\
        if(NULL != logger)\
        {                 \
           if(logger->IsEnable(arch::logging::INFO_LOG_LEVEL)) { \
               logger->Log(arch::logging::INFO_LOG_LEVEL, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); \
           }                  \
        }\
}while(0)

#define LOGGER_INFO(logger, ...) do {\
        if(NULL != logger)\
        {                 \
           if(logger->IsEnable(arch::logging::INFO_LOG_LEVEL)) { \
               logger->Log(arch::logging::INFO_LOG_LEVEL, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); \
           }                  \
        }\
}while(0)

#define LOGGER_DEBUG(logger, ...) do {\
        if(NULL != logger)\
        {                 \
           if(logger->IsEnable(arch::logging::DEBUG_LOG_LEVEL)) { \
               logger->Log(arch::logging::DEBUG_LOG_LEVEL, __FILE__, __FUNCTION__, __LINE__,__VA_ARGS__); \
           }                  \
        }\
}while(0)

#define LOGGER_WARN(logger, ...) do {\
        if(NULL != logger)\
        {                 \
           if(logger->IsEnable(arch::logging::WARN_LOG_LEVEL)) { \
               logger->Log(arch::logging::WARN_LOG_LEVEL, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); \
           }                  \
        }\
}while(0)

#define LOGGER_TRACE(logger, ...) do {\
        if(NULL != logger)\
        {                 \
           if(logger->IsEnable(arch::logging::TRACE_LOG_LEVEL)) { \
               logger->Log(arch::logging::TRACE_LOG_LEVEL, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); \
           }                  \
        }\
}while(0)

#define LOGGER_ERROR(logger,...) do {\
        if(NULL != logger)\
        {                 \
           if(logger->IsEnable(arch::logging::ERROR_LOG_LEVEL)) { \
               logger->Log(arch::logging::ERROR_LOG_LEVEL, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); \
           }                  \
        }\
}while(0)

#define LOGGER_FATAL(logger,...) do {\
        if(NULL != logger)\
        {                 \
           if(logger->IsEnable(arch::logging::FATAL_LOG_LEVEL)) { \
               logger->Log(arch::logging::FATAL_LOG_LEVEL, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); \
           }                  \
        }\
}while(0)



#endif /* LOGGER_MACROS_HPP_ */
