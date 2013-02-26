/*
 * virtual_channel_helper.cpp
 *
 *  Created on: 2011-2-27
 *      Author: wqy
 */
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include "framework/process/service_process.hpp"
#include "framework/process/manager_process.hpp"
#include "framework/vcs/virtual_channel_helper.hpp"
#include "framework/event/ipc_event_factory.hpp"
#include "channel/channel.hpp"
#include "logging/logger_macros.hpp"
#include <utility>
using namespace arch::framework;

#define CONTROLLEN  CMSG_LEN(sizeof(int))

static bool has_dispatcher = false;
static uint32 dispatcher_count = 0;
static uint32 dispatcher_cursor = 0;
static uint32 virtual_id_seed = 1;

TransportChannelIDTable VirtualChannelHelper::m_transport_table;
VirtualSocketIDTable VirtualChannelHelper::m_virtual_id_table;

int VirtualChannelHelper::Init()
{
	if (NULL != g_service_proc)
	{
		dispatcher_count =
				g_service_proc->GetManagerProcess()->GetDispatcherCount();
		has_dispatcher = dispatcher_count > 0;

	}
	return 0;
}

Channel* VirtualChannelHelper::GetIPCChannel()
{
	Channel* ch = NULL;
	if (has_dispatcher)
	{
		uint32 loop_count = 0;
		while (NULL == ch)
		{
			if (loop_count > dispatcher_count)
			{
				return g_service_proc->GetIPCChannel();
			}
			ch = g_service_proc->GetDispatcherIPCChannel(dispatcher_cursor++);
			if (dispatcher_cursor >= dispatcher_count)
			{
				dispatcher_cursor = 0;
			}
			loop_count++;
		}
		return ch;
	} else
	{
		return g_service_proc->GetIPCChannel();
	}
}

void VirtualChannelHelper::ClearTable(Channel* src_ch)
{
	if (has_dispatcher)
	{
		TransportChannelIDTable::iterator found = m_transport_table.find(
				src_ch->GetID());
		if (found != m_transport_table.end())
		{
			std::map<uint32, uint32>& temp = found->second;
			std::map<uint32, uint32>::iterator it = temp.begin();
			while (it != temp.end())
			{
				uint32 virtual_id = it->second;
				VirtualSocketIDTable::iterator vfound = m_virtual_id_table.find(
						virtual_id);
				if (vfound != m_virtual_id_table.end())
				{
					m_virtual_id_table.erase(vfound);
				}
				it++;
			}
			m_transport_table.erase(found);
		}
	}
}

void VirtualChannelHelper::ClearTable(Channel* src_ch, uint32 realID)
{
	if (has_dispatcher)
	{
		TransportChannelIDTable::iterator found = m_transport_table.find(
				src_ch->GetID());
		if (found != m_transport_table.end())
		{
			std::map<uint32, uint32>& temp = found->second;
			std::map<uint32, uint32>::iterator it = temp.find(realID);
			if (it != temp.end())
			{
				uint32 virtual_id = it->second;
				VirtualSocketIDTable::iterator vfound = m_virtual_id_table.find(
						virtual_id);
				if (vfound != m_virtual_id_table.end())
				{
					m_virtual_id_table.erase(vfound);
				}
				temp.erase(it);
			}
		}
	}
}

int VirtualChannelHelper::GetVirtualSocketID(Channel* src_ch, uint32 realID,
		uint32& virtual_id)
{
	if (!has_dispatcher)
	{
		virtual_id = realID;
		return 0;
	} else
	{
		std::map<uint32, uint32>& temp = m_transport_table[src_ch->GetID()];
		std::map<uint32, uint32>::iterator it = temp.find(realID);
		if (it != temp.end())
		{
			virtual_id = it->second;
			return 0;
		}
		uint32 generated_id = virtual_id_seed;
		virtual_id_seed++;
		if (0 == virtual_id_seed)
		{
			virtual_id_seed = 1;
		}
		temp.insert(std::make_pair(realID, generated_id));
		ChannelAndSocketID transport;
		transport.real_socket_ID = realID;
		transport.transport_channel_ID = src_ch->GetID();
		m_virtual_id_table.insert(std::make_pair(generated_id, transport));
		virtual_id = generated_id;
		return 0;
	}
}

int VirtualChannelHelper::GetChannelAndScoketID(uint32 virtual_sock_id,
		Channel*& dst_ch, uint32& realID)
{
	if (!has_dispatcher)
	{
		realID = virtual_sock_id;
		dst_ch = g_service_proc->GetIPCChannel();
		return 0;
	} else
	{
		VirtualSocketIDTable::iterator it = m_virtual_id_table.find(
				virtual_sock_id);
		if (it != m_virtual_id_table.end())
		{
			dst_ch = g_service_proc->GetChannelService().GetChannel(
					it->second.transport_channel_ID);
			realID = it->second.real_socket_ID;
			if (NULL == dst_ch)
			{
				m_virtual_id_table.erase(it);
				ERROR_LOG(
						"SocketChannelID %u is already invalid.", virtual_sock_id);
				return -1;
			}
			return 0;
		}
		return -1;
	}
}

int VirtualChannelHelper::SocketOpenRequest(bool is_tcp, uint64 addon)
{
	if (NULL != g_service_proc)
	{
		Channel* dst_ch = GetIPCChannel();
		SocketCtrlEvent event;
		IPCEvent& ev = event;
		event.SetAddonInfo(addon);
		event.SetSrcType(g_service_proc->GetServiceType());
		event.SetSrcIndex(g_service_proc->GetProcessIndex());
		event.SetSocketCtrlType(SOCKET_OPEN);
		event.SetSocketChannelID(
				is_tcp ?
						TCP_CLIENT_SOCKET_CHANNEL_ID_BIT_MASK :
						UDP_SOCKET_CHANNEL_ID_BIT_MASK);
		return (dst_ch->Write(ev) == true) ? 0 : -1;
	}
	return -1;
}

int VirtualChannelHelper::SocketConnectRequest(uint32 socket_channel_id,
		const SocketHostAddress& remote, uint64 addon)
{
	if (NULL != g_service_proc)
	{
		Channel* dst_channel = NULL;
		uint32 real_sock_id = 0;
		if (0
				!= GetChannelAndScoketID(socket_channel_id, dst_channel,
						real_sock_id))
		{
			return -1;
		}
		SocketCtrlEvent event;
		IPCEvent& ev = event;
		event.SetAddonInfo(addon);
		event.SetSrcType(g_service_proc->GetServiceType());
		event.SetSrcIndex(g_service_proc->GetProcessIndex());
		event.SetSocketChannelID(real_sock_id);
		event.SetSocketCtrlType(SOCKET_CONNECT);
		event.SetAddress(const_cast<SocketHostAddress*>(&remote));
		return dst_channel->Write(ev) == true ? 0 : -1;
	}
	return -1;
}

int VirtualChannelHelper::SocketConnectRequest(const SocketUnixAddress& remote,
		uint64 addon)
{
	if (NULL != g_service_proc)
	{
		Channel* dst_ch = GetIPCChannel();
		if (NULL == dst_ch)
		{
			//ERROR_LOG("Failed to get IPC channel to send IPC message.");
			return -1;
		}
		SocketCtrlEvent event;
		IPCEvent& ev = event;
		event.SetAddonInfo(addon);
		event.SetSrcType(g_service_proc->GetServiceType());
		event.SetSrcIndex(g_service_proc->GetProcessIndex());
		event.SetSocketCtrlType(SOCKET_CONNECT);
		event.SetUnixAddress(const_cast<SocketUnixAddress*>(&remote));
		event.SetSocketChannelID(TCP_CLIENT_SOCKET_CHANNEL_ID_BIT_MASK);
		return dst_ch->Write(ev) == true ? 0 : -1;
	}
	return -1;
}

int VirtualChannelHelper::WriteSocket(uint32 socket_channel_id, Buffer& content)
{
	if (NULL != g_service_proc)
	{
		Channel* dst_channel = NULL;
		uint32 real_sock_id = 0;
		if (0
				!= GetChannelAndScoketID(socket_channel_id, dst_channel,
						real_sock_id))
		{
			return -1;
		}
		SocketMessageEvent event;
		IPCEvent& ev = event;
		event.SetSrcType(g_service_proc->GetServiceType());
		event.SetSrcIndex(g_service_proc->GetProcessIndex());
		event.SetChannelID(real_sock_id);
		event.SetContent(&content);
		return dst_channel->Write(ev) == true ? 0 : -1;
	}
	return -1;
}

int VirtualChannelHelper::WriteSocket(uint32 socket_channel_id, Buffer& content,
		const SocketInetAddress& remote)
{
	if (NULL != g_service_proc)
	{
		Channel* dst_channel = NULL;
		uint32 real_sock_id = 0;
		if (0
				!= GetChannelAndScoketID(socket_channel_id, dst_channel,
						real_sock_id))
		{
			return -1;
		}
		SocketMessageEvent event;
		IPCEvent& ev = event;
		event.SetSrcType(g_service_proc->GetServiceType());
		event.SetSrcIndex(g_service_proc->GetProcessIndex());
		event.SetChannelID(real_sock_id);
		event.SetContent(&content);
		event.SetSocketInetAddress(const_cast<SocketInetAddress*>(&remote));
		return dst_channel->Write(ev) == true ? 0 : -1;
	}
	return -1;
}

int VirtualChannelHelper::SocketConnectRequest(const SocketHostAddress& remote,
		bool is_tcp, uint64 addon)
{
	if (NULL != g_service_proc)
	{
		Channel* dst_ch = GetIPCChannel();
		if (NULL == dst_ch)
		{
			//ERROR_LOG("Failed to get IPC channel to send IPC message.");
			return -1;
		}
		SocketCtrlEvent event;
		IPCEvent& ev = event;
		event.SetAddonInfo(addon);
		event.SetSrcType(g_service_proc->GetServiceType());
		event.SetSrcIndex(g_service_proc->GetProcessIndex());
		event.SetSocketCtrlType(SOCKET_CONNECT);
		event.SetAddress(const_cast<SocketHostAddress*>(&remote));
		event.SetSocketChannelID(
				is_tcp ?
						TCP_CLIENT_SOCKET_CHANNEL_ID_BIT_MASK :
						UDP_SOCKET_CHANNEL_ID_BIT_MASK);
		//dst_ch->Write(ev);
		return dst_ch->Write(ev) == true ? 0 : -1;
	}
	return -1;

}

int VirtualChannelHelper::CloseSocket(uint32 socket_channel_id, uint64 addon)
{
	if (NULL != g_service_proc)
	{
		Channel* dst_channel = NULL;
		uint32 real_sock_id = 0;
		if (0
				!= GetChannelAndScoketID(socket_channel_id, dst_channel,
						real_sock_id))
		{
			return -1;
		}
		SocketCtrlEvent event;
		IPCEvent& ev = event;
		event.SetAddonInfo(addon);
		event.SetSrcType(g_service_proc->GetServiceType());
		event.SetSrcIndex(g_service_proc->GetProcessIndex());
		event.SetSocketCtrlType(SOCKET_CLOSE);
		event.SetSocketChannelID(real_sock_id);
		//dst_channel->Write(ev);
		return dst_channel->Write(ev) == true ? 0 : -1;
	}
	return -1;
}

int VirtualChannelHelper::WriteIPCMessage(ServiceType service_type,
		ServiceIndex index, Buffer& content)
{
	Channel* dst_ch = NULL;
	ServiceType src_type = 0;
	ServiceIndex src_idx = 0;
	if (NULL != g_service_proc)
	{
		if (service_type == MANAGER_SERVICE_PROCESS_TYPE)
		{
			dst_ch = g_service_proc->GetIPCChannel();
		} else
		{
			//dst_ch = GetIPCChannel();
			dst_ch = g_service_proc->GetIPCChannel();
		}
		src_type = g_service_proc->GetServiceType();
		src_idx = g_service_proc->GetProcessIndex();
	} else
	{
		ServiceProcess* proc = g_manager_proc->GetServiceProcess(service_type,
				index);
		if (NULL != proc)
		{
			dst_ch = proc->GetIPCChannel();
		}
	}
	if (NULL == dst_ch)
	{
		ERROR_LOG("Failed to get IPC channel to send IPC message.");
		return -1;
	}
	IPCMessageEvent event;
	IPCEvent& ev = event;
	event.SetContent(&content);
	event.SetDstIndex(index);
	event.SetDstType(service_type);
	event.SetSrcType(src_type);
	event.SetSrcIndex(src_idx);
	//dst_ch->Write(ev);
	return dst_ch->Write(ev) == true ? 0 : -1;
	//return -1;
}

int VirtualChannelHelper::SendClientSocketChannelFD(ServiceType service_type,
		ServiceIndex index, ClientSocketChannel* ch)
{
	DEBUG_LOG(
			"Send client socket FD:%u to process %u:%u", ch->GetReadFD(), service_type, index);
	if (ch->GetReadFD() != ch->GetWriteFD())
	{
		ERROR_LOG("Read FD is not equal with write FD.");
		return -1;
	}
	Channel* dst_ch = NULL;
	Channel* ctrl_ch = NULL;
	ServiceType src_type = 0;
	ServiceIndex src_idx = 0;
	if (NULL != g_service_proc)
	{
		dst_ch = g_service_proc->GetIPCChannel();
		src_type = g_service_proc->GetServiceType();
		src_idx = g_service_proc->GetProcessIndex();
		ctrl_ch = g_service_proc->GetCtrlChannel();
	} else
	{
		ServiceProcess* proc = g_manager_proc->GetServiceProcess(service_type,
				index);
		if (NULL != proc)
		{
			dst_ch = proc->GetIPCChannel();
			ctrl_ch = proc->GetCtrlChannel();
		}
	}
	if (NULL == dst_ch || NULL == ctrl_ch)
	{
		ERROR_LOG(
				"Failed to get IPC channel to send IPC message %p %p", dst_ch, ctrl_ch);
		return -1;
	}
	struct msghdr msg;
	struct iovec iov[1];
	uint32 iov_len = sizeof(ServiceType) + sizeof(ServiceIndex) + 1;
	char tmp[iov_len];
	//Since currently only manager process can tranfer FD to child, service_type & service_idx should be 0
	memset(tmp, 0, sizeof(tmp));
	tmp[0] = TRANSMIT_CLIENT_SOCKET_FD;
	memcpy(tmp + 1, &src_type, sizeof(ServiceType));
	memcpy(tmp + 1 + sizeof(ServiceType), &src_idx, sizeof(ServiceIndex));
	iov[0].iov_base = tmp;
	iov[0].iov_len = iov_len;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	int fd_to_send = ch->GetReadFD();
//	char temp[CONTROLLEN];
//	struct cmsghdr* cmptr = (struct cmsghdr*) temp;
//	cmptr->cmsg_level = SOL_SOCKET;
//	cmptr->cmsg_type = SCM_RIGHTS;
//	cmptr->cmsg_len = CONTROLLEN;
//	msg.msg_control = cmptr;
//	msg.msg_controllen = CONTROLLEN;

	union
	{
			struct cmsghdr cm;
			char control[CMSG_SPACE(sizeof(int))];
	} control_un;
	struct cmsghdr *cmptr;
	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);
	cmptr = CMSG_FIRSTHDR(&msg);
	if (NULL == cmptr)
	{
		return -1;
	}
	cmptr->cmsg_len = CMSG_LEN(sizeof(int));
	cmptr->cmsg_level = SOL_SOCKET;
	cmptr->cmsg_type = SCM_RIGHTS;
	*(int*) CMSG_DATA(cmptr) = fd_to_send;
	if (::sendmsg(ctrl_ch->GetWriteFD(), &msg, 0) < 0)
	{
		int err = errno;
		ERROR_LOG("Failed to write IPC channel ioctl:%s", strerror(err));
		return -1;
	}
//    IPCCtrlEvent event;
//    IPCEvent& ev = event;
//    event.SetSrcType(src_type);
//    event.SetSrcIndex(src_idx);
//    event.SetDstType(service_type);
//    event.SetDstIndex(index);
//    event.SetIPCCtrlType(TRANSMIT_CLIENT_SOCKET_FD);
//    if (!dst_ch->Write(ev))
//    {
//        return -1;
//    }
//    dst_ch->Flush();
	ch->Close();
	return 0;
}

void VirtualChannelHelper::CtrlChannelIOEventCallback(
		struct aeEventLoop *eventLoop, int fd, void *clientData, int mask)
{
	Channel* ch = (Channel*) clientData;
	struct msghdr msg;
	struct iovec iov[1];
	uint32 iov_len = sizeof(ServiceType) + sizeof(ServiceIndex) + 1;
	char tmp[iov_len];
	iov[0].iov_base = tmp;
	iov[0].iov_len = iov_len;
	union
	{
			struct cmsghdr cm;
			char control[CMSG_SPACE(sizeof(int))];
	} control_un;
	struct cmsghdr *cmptr;
	do
	{
		msg.msg_iov = iov;
		msg.msg_iovlen = 1;
		msg.msg_name = NULL;
		msg.msg_namelen = 0;
		msg.msg_flags = 0;
		msg.msg_control = control_un.control;
		msg.msg_controllen = sizeof(control_un.control);
		int ret = ::recvmsg(fd, &msg, 0);
		if (ret < 0)
		{
			int err = errno;
			if (IO_ERR_RW_RETRIABLE(err))
			{
				DEBUG_LOG("##Continue recvmsg for transferd FD.");
				continue;
			}
			ERROR_LOG(
					"Failed to read FD from ctrl channel:%s.", strerror(err));
			ch->Close();
			return;
		}
		if (ret == 0)
		{
			ch->Close();
			return;
		}
		break;
	} while (1);
	cmptr = CMSG_FIRSTHDR(&msg);
	RETURN_IF_NULL(cmptr);
	if (cmptr->cmsg_len != CMSG_LEN(sizeof(int)))
	{
		ERROR_LOG(
				"No FD read from recvmsg, msg.msg_controllen = %u while expected %u", cmptr->cmsg_len, CMSG_LEN(sizeof(int)));
		return;
	}
	int transferfd = *(int*) CMSG_DATA(cmptr);
	ServiceType src_type;
	ServiceIndex src_idx;
	memcpy(&src_type, tmp + 1, sizeof(ServiceType));
	memcpy(&src_idx, tmp + 1 + sizeof(ServiceType), sizeof(ServiceIndex));
	DEBUG_LOG(
			"Received fd:%d from other process %u:%u.", transferfd, src_type, src_idx);
	if (tmp[0] == TRANSMIT_CLIENT_SOCKET_FD)
	{
		ClientSocketChannel* ch =
				g_service_proc->GetChannelService().NewClientSocketChannel();
		ch->AttachFD(transferfd);
		if (NULL != g_service_proc)
		{
			g_service_proc->GetServiceHandler()->OnTransferFD(src_type, src_idx,
					ch);
			fire_channel_connected(ch);
			return;
		} else
		{
			g_manager_proc->GetManagerHandler()->OnTransferFD(src_type, src_idx,
					ch);
			fire_channel_connected(ch);
		}
	}

	return;
}

//ClientSocketChannel* VirtualChannelHelper::ReadClientSocketChannelFD()
//{
//    if (NULL != g_service_proc)
//    {
//        Channel* ctrl_channel = g_service_proc->GetCtrlChannel();
//        int fifoFD = ctrl_channel->GetReadFD();
//
//        struct msghdr msg;
//        struct iovec iov[1];
//        char c = 1;
//        iov[0].iov_base = &c;
//        iov[0].iov_len = 1;
//        msg.msg_iov = iov;
//        msg.msg_iovlen = 1;
//        msg.msg_name = NULL;
//        msg.msg_namelen = 0;
//        char temp[CONTROLLEN];
//        struct cmsghdr* cmptr = (struct cmsghdr*) temp;
//        cmptr->cmsg_len = CONTROLLEN;
//        msg.msg_control = cmptr;
//        msg.msg_controllen = CONTROLLEN;
//        do
//        {
//            int ret = ::recvmsg(fifoFD, &msg, 0);
//            if (ret < 0)
//            {
//                int err = errno;
//                if (IO_ERR_RW_RETRIABLE(err))
//                {
//                    continue;
//                }ERROR_LOG("Failed to read FD from FIFO:%s.", strerror(err));
//                return NULL;
//            }
//            break;
//        } while (1);
//        if (msg.msg_controllen != CONTROLLEN)
//        {
//            ERROR_LOG(
//                    "No FD read from recvmsg, msg.msg_controllen = %u while expected %u", msg.msg_controllen, CONTROLLEN);
//            return NULL;
//        }
//        int fd = *(int*) CMSG_DATA(cmptr);
//        DEBUG_LOG("Received fd:%d from other process.", fd);
//        ClientSocketChannel* ch =
//                g_service_proc->GetChannelService().NewClientSocketChannel();
//        ch->AttachFD(fd);
//        //fireChannelConnected(ch);
//        return ch;
//    }
//    return NULL;
//}

