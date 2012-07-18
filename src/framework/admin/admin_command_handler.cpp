/*
 * admin_command_handler.cpp
 *
 *  Created on: 2011-5-17
 *      Author: qiyingwang
 */
#include "net/socket_host_address.hpp"
#include "framework/admin/admin_command_handler.hpp"
#include "channel/all_includes.hpp"
#include "logging/logger_macros.hpp"
#include "buffer/buffer_helper.hpp"
#include "framework/process/manager_process.hpp"
#include <sstream>

static const char* kExit = "exit";
static const char* kQuit = "quit";
static const char* kList = "ps";
static const char* kHelp = "help";
static const char* kLogLvel = "loglevel";

using namespace arch::framework;
using namespace arch::buffer;

AdminCommandHandler::UserAdminCommandHandlerTable
        AdminCommandHandler::m_handler_table;

bool AdminCommandHandler::RegisterHandler(UserAdminCommandHandler* handler)
{
    RETURN_FALSE_IF_NULL(handler);
    std::vector<std::string> cmdlist;
    handler->GetSupportedCommands(cmdlist);
    std::vector<std::string>::iterator it = cmdlist.begin();
    while (it != cmdlist.end())
    {
        if (m_handler_table.count(*it) > 0)
        {
            return false;
        }
        it++;
    }
    it = cmdlist.begin();
    while (it != cmdlist.end())
    {
        m_handler_table[*it] = handler;
        it++;
    }
    return true;
}

int AdminCommandHandler::DecodeCommand(AdminCommand& cmd, Buffer* buf)
{
    if (NULL == buf)
    {
        return -1;
    }
    if (BufferHelper::ReadFixUInt32(*buf, cmd.admin_channel_id)
            && BufferHelper::ReadFixString(*buf, cmd.name))
    {
        uint32 argsize;
        if (!BufferHelper::ReadFixUInt32(*buf, argsize))
        {
            return -1;
        }
        uint32 i = 0;
        for (; i < argsize; i++)
        {
            std::string arg;
            if (!BufferHelper::ReadFixString(*buf, arg))
            {
                return -1;
            }
            cmd.args.push_back(arg);
        }
        return 0;
    }
    else
    {
        return -1;
    }

}

int AdminCommandHandler::EncodeCommand(AdminCommand& cmd, Buffer* buf)
{
    if (NULL == buf)
    {
        return -1;
    }
    if (BufferHelper::WriteFixUInt32(*buf, cmd.admin_channel_id)
            && BufferHelper::WriteFixString(*buf, cmd.name))
    {
        if (!BufferHelper::WriteFixUInt32(*buf, cmd.args.size()))
        {
            return -1;
        }
        uint32 i = 0;
        for (; i < cmd.args.size(); i++)
        {
            if (!BufferHelper::WriteFixString(*buf, cmd.args[i]))
            {
                return -1;
            }
        }
        return 0;
    }
    else
    {
        return -1;
    }
}

int AdminCommandHandler::HandleCommand(const AdminCommand& cmd,
        std::string& result)
{
    if (cmd.name == kExit || cmd.name == kQuit)
    {
        return -1;
    }
    else if (cmd.name == kHelp)
    {
        std::stringstream ss(std::stringstream::in | std::stringstream::out);
        ss << "Command line format:[all|<pid>].<COMMAND> [arg0] [arg1] ... [argn] \n";
        ss << "Supported commands:\n";
        ss << kList << "\n";
        ss << kLogLvel << " <TRACE|DEBUG|INFO|WARN|ERROR|FATAL>\n";
        UserAdminCommandHandlerTable::iterator it = m_handler_table.begin();
        while(it != m_handler_table.end())
        {
            UserAdminCommandHandler* handler = it->second;
            std::vector<std::string> cmdlist;
            handler->GetSupportedCommands(cmdlist);
            for(std::vector<std::string>::const_iterator iter = cmdlist.begin(); iter != cmdlist.end(); ++iter)
            {
                ss << *iter << "\n";
            }
            it++;
        }
        ss << kExit << "|" << kQuit << "\n";
        ss << kHelp << "\n";
        result = ss.str();
        return 0;
    }
    else if (cmd.name == kList)
    {
        Buffer buf;
        buf.EnsureWritableBytes(1024);
        g_manager_proc->PrintPid4Processes(buf);
        result = buf.AsString();
        return 0;
    }
    else if (cmd.name == kLogLvel)
    {
        arch::logging::Logger* logger =
                arch::logging::LoggerFactory::GetDevelopLogger();
        if (cmd.args.empty())
        {
            //write current loglevel back

            if (NULL != logger)
            {
                std::string levelstr = arch::logging::LoggerFactory::ToString(
                        logger->GetLvel());
                char temp[1024];
                snprintf(
                        temp,
                        sizeof(temp) - 1,
                        "Process(%u)'s develop logger current loglvel is [%s].",getpid(),
                        levelstr.c_str());
                result = temp;
                return 0;
            }
        }
        else
        {
            std::string levelstr = cmd.args[0];
            char temp[1024];
            arch::logging::LogLevel level =
                    arch::logging::LoggerFactory::FromString(levelstr);
            if (level == arch::logging::INVALID_LOG_LEVEL)
            {
                snprintf(
                        temp,
                        sizeof(temp) - 1,
                        "Invalid loglvel string[%s].",
                        levelstr.c_str());
                result = temp;
                return 0;
            }
            if (NULL != logger)
            {
                logger->SetLevel(level);
                snprintf(
                        temp,
                        sizeof(temp) - 1,
                        "Process(%u)'s develop logger switch loglvel to [%s].",getpid(),
                        levelstr.c_str());
                result = temp;
            }
        }
        return 0;
    }
    else
    {
        if (m_handler_table.count(cmd.name) > 0)
        {
            return m_handler_table[cmd.name]->HandleCommand(cmd, result);
        }
        else
        {
            result = "Not supported this command:";
            result += cmd.name ;
        }
    }
    return 0;
}

