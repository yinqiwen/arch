/*
 * ReadFIFOChannel.cpp
 *
 *  Created on: 2011-2-1
 *      Author: wqy
 */
#include "channel/all_includes.hpp"
#include "exception/api_exception.hpp"
#include "logging/logger_macros.hpp"
#include "util/file_helper.hpp"
#include "exception/api_exception.hpp"

using namespace arch::channel;
using namespace arch::channel::fifo;
using namespace arch::exception;
using namespace arch::util;

FIFOChannel::FIFOChannel(ChannelService& factory) :
        Channel(NULL, factory), m_read_fifo(NULL), m_write_fifo(NULL), m_open_flag(
                -1)
{

}

bool FIFOChannel::DoOpen()
{
    try
    {
        if (!m_read_path.empty() && NULL == m_read_fifo)
        {
            NEW(m_read_fifo, ReadFIFO(m_read_path));
            int ret = -1;
            if (m_open_flag != -1)
            {
                ret = m_read_fifo->Open(m_open_flag);
            }
            else
            {
                ret = m_read_fifo->Open();
            }
            if (-1 == ret)
            {
                int err = errno;
                WARN_LOG(
                        "Failed to open read fifo(%u:%s) for cause:%s", m_id, m_read_path.c_str(), strerror(err));
                DELETE(m_read_fifo);
                return false;
            }
            make_fd_nonblocking(m_read_fifo->GetFD());
            aeCreateFileEvent(m_service.GetRawEventLoop(), m_read_fifo->GetFD(),
                    AE_READABLE, Channel::IOEventCallback, this);

        }
        if (!m_write_path.empty() && NULL == m_write_fifo)
        {
            NEW(m_write_fifo, WriteFIFO(m_write_path));
            int ret = -1;
            if (m_open_flag != -1)
            {
                ret = m_write_fifo->Open(m_open_flag);
            }
            else
            {
                ret = m_write_fifo->Open();
            }
            if (-1 == ret)
            {
                int err = errno;
                WARN_LOG(
                        "Failed to open write fifo(%u:%s) for cause:%s", m_id, m_write_path.c_str(), strerror(err));
                DELETE(m_write_fifo);
                return false;
            }
            make_fd_nonblocking(m_write_fifo->GetFD());
        }
    }
    catch (APIException& e)
    {
        ERROR_LOG("Open FIFO error cause:%s", e.GetCause().c_str());
        return false;
    }

    return true;
}

bool FIFOChannel::DoBind(Address* local)
{
    return false;
}
bool FIFOChannel::DoConnect(Address* remote)
{
    return false;
}
bool FIFOChannel::DoClose()
{
    if (NULL != m_read_fifo)
    {
        m_read_fifo->Close();
        DELETE(m_read_fifo);
    }
    if (NULL != m_write_fifo)
    {
        m_write_fifo->Close();
        DELETE(m_write_fifo);
    }
    //fireChannelClosed(this);
    return true;
}
int FIFOChannel::GetWriteFD()
{
    if (NULL != m_write_fifo)
    {
        return m_write_fifo->GetFD();
    }
    return -1;
}

int FIFOChannel::GetReadFD()
{
    if (NULL != m_read_fifo)
    {
        return m_read_fifo->GetFD();
    }
    return -1;
}

void FIFOChannel::SetOpenFlag(int flag)
{
    m_open_flag = flag;
}

FIFOChannel::~FIFOChannel()
{
    DELETE(m_read_fifo);
    DELETE(m_write_fifo);
}

PipeChannel::PipeChannel(ChannelService& factory, int readFd, int writeFd) :
		Channel(NULL, factory), m_read_fd(readFd), m_write_fd(writeFd)
{
	if (-1 != m_read_fd)
	{
		make_fd_nonblocking(m_read_fd);
		aeCreateFileEvent(GetService().GetRawEventLoop(), m_read_fd,
						AE_READABLE, Channel::IOEventCallback, this);

	}
	if (-1 != m_write_fd)
	{
		make_fd_nonblocking(m_write_fd);
	}
}

bool PipeChannel::DoOpen()
{
	return true;
}

bool PipeChannel::DoBind(Address* local)
{
	return false;
}
bool PipeChannel::DoConnect(Address* remote)
{
	return false;
}
bool PipeChannel::DoClose()
{
	close(m_read_fd);
	close(m_write_fd);
	m_read_fd = -1;
	m_write_fd = -1;
	//fireChannelClosed(this);
	return true;
}
int PipeChannel::GetWriteFD()
{
	return m_write_fd;
}

int PipeChannel::GetReadFD()
{
	return m_read_fd;
}

PipeChannel::~PipeChannel()
{

}
