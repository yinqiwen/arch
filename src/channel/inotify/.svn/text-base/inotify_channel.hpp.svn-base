/*
 * inotify_channel.hpp
 *
 *  Created on: 2012-4-28
 *      Author: qiyingwang
 */

#ifndef INOTIFY_CHANNEL_HPP_
#define INOTIFY_CHANNEL_HPP_

#include "channel/channel.hpp"
#include <sys/inotify.h>
#include <vector>
#include <string>
#include <map>

namespace arch
{
    namespace channel
    {
        struct InotifyHandler
        {
                virtual void OnInotifyEvent(struct inotify_event * event) = 0;
                virtual ~InotifyHandler()
                {
                }
        };
        typedef void InotifyCallBack(struct inotify_event * event);

        class InotifyChannel: public Channel
        {
            private:
                typedef std::map<std::string, uint32> WatchDescTable;
                typedef std::map<uint32, std::string> WatchPathTable;
                WatchDescTable m_wd_table;
                WatchPathTable m_wp_table;
                InotifyHandler* m_handler;
                InotifyCallBack* m_callback;
                InotifyChannel(ChannelService& factory);
                void OnRead();
                bool DoOpen();
                bool DoClose();
                void ClearWatchTable(uint32 desc);
                void ClearWatchTable(const std::string& path);
                friend class arch::channel::ChannelService;
            public:
                int Watch(const std::string& path, uint32 mask = IN_ALL_EVENTS);
                int Unwatch(uint32 watch_desc);
                int Unwatch(const std::string& path);
                void RegisterHandler(InotifyHandler* handler, bool replace =
                        false);
                void RegisterCallBack(InotifyCallBack* cb,
                        bool replace = false);
                const char* GetWatchedPath(uint32 wd);
        };
    }
}

#endif /* INOTIFY_CHANNEL_HPP_ */
