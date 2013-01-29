/*
 * admin_command_handler.hpp
 *
 *  Created on: 2011-5-17
 *      Author: qiyingwang
 */

#ifndef ADMIN_COMMAND_HANDLER_HPP_
#define ADMIN_COMMAND_HANDLER_HPP_

#include "channel/all_includes.hpp"
#include "buffer/buffer.hpp"
#include <string>
#include <vector>
#include <map>

using namespace arch::channel;
using arch::buffer::Buffer;

namespace arch
{
    namespace framework
    {
        struct AdminCommand
        {
                uint32 admin_channel_id;
                std::string name;
                std::vector<std::string> args;
        };

        struct UserAdminCommandHandler
        {
        		virtual int HandleCommand(const AdminCommand& args, std::string& result) = 0;
        		virtual void GetSupportedCommands(std::vector<std::string>& cmdlist) = 0;
        		virtual void PrintHelp(Buffer& output) = 0;
        		virtual ~UserAdminCommandHandler(){}
        };

        class AdminCommandHandler
        {
            public:
        		typedef std::map<std::string, UserAdminCommandHandler*> UserAdminCommandHandlerTable;
        		static UserAdminCommandHandlerTable m_handler_table;
                static int DecodeCommand(AdminCommand& cmd, Buffer* buf);
                static int EncodeCommand(AdminCommand& cmd, Buffer* buf);
                static int HandleCommand(const AdminCommand& args, std::string& result);
                static bool RegisterHandler(UserAdminCommandHandler* handler);
        };
    }
}

#endif /* ADMIN_COMMAND_HANDLER_HPP_ */
