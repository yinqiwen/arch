/*
 * admin_handler.cpp
 *
 *  Created on: 2011-5-16
 *      Author: qiyingwang
 */
#include "net/socket_host_address.hpp"
#include "framework/admin/admin_message_handler.hpp"
#include "framework/process/manager_process.hpp"
#include "framework/process/service_process.hpp"
#include "channel/all_includes.hpp"
#include "logging/logger_macros.hpp"
#include "util/string_helper.hpp"
#include <stdarg.h>
#include <vector>
using namespace arch::framework;
using namespace arch::net;
using namespace arch::channel;
using namespace arch::util;

static const char* ALL = "all";

void AdminMessageHandler::GetRemoteHostPort(ChannelHandlerContext& ctx,
        std::string& result)
{
	const Address* addr = ctx.GetChannel()->GetRemoteAddress();
	if (NULL != addr)
	{
		Address* address = const_cast<Address*> (addr);
		if (InstanceOf<SocketHostAddress> (address).OK)
		{
			SocketHostAddress* host_addr =
			        static_cast<SocketHostAddress*> (address);
			char temp[256];
			sprintf(temp, "%s:%u", host_addr->GetHost().c_str(),
			        host_addr->GetPort());
			result = temp;
			return;
		}
	}
	result = "Unknown";
}

void AdminMessageHandler::ChannelConnected(ChannelHandlerContext& ctx,
        ChannelStateEvent& e)
{
	std::string hostport;
	GetRemoteHostPort(ctx, hostport);
	INFO_LOG("Admin client connected from %s", hostport.c_str());
	WriteResult(ctx, "admin@%s ~\n$ ", hostport.c_str());

}
void AdminMessageHandler::ChannelClosed(ChannelHandlerContext& ctx,
        ChannelStateEvent& e)
{

}
void AdminMessageHandler::ExceptionCaught(ChannelHandlerContext& ctx,
        ExceptionEvent& e)
{
	ERROR_LOG("Exception(%s) caught in admin handler.", e.GetException().GetCause().c_str());
}

void AdminMessageHandler::WriteResult(ChannelHandlerContext& ctx,
        const char* format, ...)
{
	Buffer buf;
	buf.EnsureWritableBytes(64);
	va_list args;
	va_start(args, format);
	buf.VPrintf(format, args);
	va_end(args);
	ctx.GetChannel()->Write(buf);

}

void AdminMessageHandler::MessageReceived(ChannelHandlerContext& ctx,
        MessageEvent<Buffer>& e)
{
	std::string hostport;
	GetRemoteHostPort(ctx, hostport);
	Buffer* msg = e.GetMessage();
	std::string msgstr = trim_string(msg->AsString(), " \t\r\n");
	INFO_LOG("Received command string:%s with size:%u", msgstr.c_str(), msgstr.size());
	if (msgstr.empty())
	{
		WriteResult(ctx, "admin@%s ~\n$ ", hostport.c_str());
		return;
	}
	std::vector < std::string > cmds = split_string(msgstr, " ");

	const std::string& name = cmds[0];
	std::vector<std::string>::iterator first = cmds.begin() + 1;
	std::vector < std::string > args(first, cmds.end());


	/**
	 * 23456.<cmd> means the <cmd> should dispatch to sub process with pid 23456.
	 */
	AdminCommand cmd;
	cmd.admin_channel_id = ctx.GetChannel()->GetID();
	std::vector < pid_t > dest_pids;
	size_t found = name.find('.');
	if (found != std::string::npos)
	{
		cmd.name = name.substr(found + 1);
		std::string deststr = name.substr(0, found);
		if (deststr == ALL)
		{
			Process& current = Process::CurrentProcess();
			dest_pids = current.GetChildren();
			dest_pids.push_back(getpid());
		}
		else
		{
			uint32 pid;
			if (string_touint32(deststr, pid))
			{
				dest_pids.push_back(pid);
			}
			else
			{
				WriteResult(
				        ctx,
				        "Invalid destination indicator(%s). Indicator should be 'all' or valid PID.\r\n",
				        deststr.c_str());
				WriteResult(ctx, "admin@%s ~\n$ ", hostport.c_str());
				return;
			}
		}
	}
	else
	{
		cmd.name = name;
		dest_pids.push_back(getpid());
	}

	cmd.args = args;

	std::vector<pid_t>::iterator it = dest_pids.begin();
	while (it != dest_pids.end())
	{
		pid_t target_pid = *it;
		if (target_pid == getpid())
		{
			std::string result;
			int ret = AdminCommandHandler::HandleCommand(cmd, result);
			if (ret < 0)
			{
				ctx.GetChannel()->Close();
			}
			else
			{
				WriteResult(ctx, "%s\r\n", result.c_str());
				WriteResult(ctx, "admin@%s ~\n$ ", hostport.c_str());
			}
		}
		else
		{
			Buffer buf;
			buf.EnsureWritableBytes(64);
			if (0 == AdminCommandHandler::EncodeCommand(cmd, &buf))
			{
				ServiceProcess* proc = g_manager_proc->GetServiceProcess(
				        target_pid);
				if (NULL == proc)
				{
					WriteResult(ctx, "Process with pid(%u) not found.\r\n",
					        target_pid);
					WriteResult(ctx, "admin@%s ~\n$ ", hostport.c_str());
					return;
				}
				g_manager_proc->GetIPCEventHandler().DispatchIPCMsg(
				        proc->GetServiceType(), proc->GetProcessIndex(), buf,
				        ADMIN);
			}
			else
			{
				WriteResult(ctx, "Failed to send command to sub-process.\r\n");
				WriteResult(ctx, "admin@%s ~\n$ ", hostport.c_str());
			}
		}
		it++;
	}

}
