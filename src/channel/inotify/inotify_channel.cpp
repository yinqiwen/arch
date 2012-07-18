/*
 * inotify_channel.cpp
 *
 *  Created on: 2012-4-28
 *      Author: qiyingwang
 */
#include "inotify_channel.hpp"
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include "logging/logger_macros.hpp"


using namespace arch::channel;

InotifyChannel::InotifyChannel(ChannelService& factory) :
        Channel(NULL, factory), m_handler(NULL), m_callback(NULL)
{

}

void InotifyChannel::RegisterHandler(InotifyHandler* handler, bool replace)
{
    if (NULL != handler && !replace)
    {
        ERROR_LOG("Inotify Handler is already set.");
        return;
    }
    m_handler = handler;
}

void InotifyChannel::RegisterCallBack(InotifyCallBack* cb, bool replace)
{
    if (NULL != m_callback && !replace)
    {
        ERROR_LOG("Inotify Handler is already set.");
        return;
    }
    m_callback = cb;
}

void InotifyChannel::OnRead()
{
    int bufsize;
    if (::ioctl(0, FIONREAD, &bufsize) < 0)
    {
        ERROR_LOG("Failed to get readed bytes.");
        return;
    }
    char buffer[bufsize];
    char * offset = NULL;
    struct inotify_event* event;
    int len;
    while ((len = ::read(m_fd, buffer, bufsize)) > 0)
    {
        offset = buffer;
        event = (struct inotify_event *) buffer;
        while (((char *) event - buffer) < len)
        {
            int tmp_len = sizeof(struct inotify_event) + event->len;
            event = (struct inotify_event *) (offset + tmp_len);
            offset += tmp_len;
            if (NULL != m_callback)
            {
                m_callback(event);
            }
            if (NULL != m_handler)
            {
                m_handler->OnInotifyEvent(event);
            }
        }
    }
}

bool InotifyChannel::DoOpen()
{
    m_fd = inotify_init();
    if (m_fd == -1)
    {
        int err = errno;
        ERROR_LOG("Failed to init inotify for reason:%s", strerror(err));
        return false;
    }
    return true;
}

bool InotifyChannel::DoClose()
{
    ::close(m_fd);
    m_fd = -1;
    return true;
}

int InotifyChannel::Watch(const std::string& path, uint32 mask)
{
    if (-1 == m_fd)
    {
        if (!Open())
        {
            return -1;
        }
    }
    if (m_wd_table.count(path) > 0)
    {
        ERROR_LOG("%s is already watched.", path.c_str());
        return -1;
    }
    int wd = inotify_add_watch(m_fd, path.c_str(), mask);
    if (wd > 0)
    {
        m_wd_table[path] = wd;
        m_wp_table[wd] = path;
    }
    else
    {
        int err = errno;
        ERROR_LOG(
                "Failed to watch %s for reason:%s", path.c_str(), strerror(err));
    }
    return wd;
}

void InotifyChannel::ClearWatchTable(uint32 desc)
{
    WatchPathTable::iterator found = m_wp_table.find(desc);
    if (found != m_wp_table.end())
    {
        std::string& path = found->second;
        WatchDescTable::iterator found2 = m_wd_table.find(path);
        if (found2 != m_wd_table.end())
        {
            m_wd_table.erase(found2);
        }
        m_wp_table.erase(found);
    }
}
void InotifyChannel::ClearWatchTable(const std::string& path)
{
    WatchDescTable::iterator found2 = m_wd_table.find(path);
    if (found2 != m_wd_table.end())
    {
        uint32 wd = found2->second;
        m_wd_table.erase(found2);
        WatchPathTable::iterator found = m_wp_table.find(wd);
        if (found != m_wp_table.end())
        {
            m_wp_table.erase(found);
        }
    }
}

int InotifyChannel::Unwatch(uint32 watch_desc)
{
    int ret = inotify_rm_watch(m_fd, watch_desc);
    if (ret != 0)
    {
        int err = errno;
        ERROR_LOG(
                "Failed to unwatch %u for reason:%s", watch_desc, strerror(err));
        return -1;
    }
    ClearWatchTable(watch_desc);
    return 0;

}

int InotifyChannel::Unwatch(const std::string& path)
{
    WatchDescTable::iterator found2 = m_wd_table.find(path);
    if (found2 == m_wd_table.end())
    {
        ERROR_LOG( "%s is not under watch.", path.c_str());
        return -1;
    }
    Unwatch(found2->second);
    return 0;
}

const char* InotifyChannel::GetWatchedPath(uint32 wd)
{
    WatchPathTable::iterator found = m_wp_table.find(wd);
    if (found != m_wp_table.end())
    {
        return found->second.c_str();
    }
    return NULL;
}
