/*
 * FIFOChannel.hpp
 *
 *  Created on: 2011-1-28
 *      Author: wqy
 */

#ifndef NOVA_FIFOCHANNEL_HPP_
#define NOVA_FIFOCHANNEL_HPP_
#include "channel/channel.hpp"
#include "ipc/fifo.hpp"
#include <string>
using arch::ipc::ReadFIFO;
using arch::ipc::WriteFIFO;
namespace arch
{
    namespace channel
    {
        namespace fifo
        {
            class FIFOChannel: public arch::channel::Channel
            {
                private:
                    string m_read_path;
                    string m_write_path;
                    ReadFIFO* m_read_fifo;
                    WriteFIFO* m_write_fifo;
                    int m_open_flag;
    				bool DoOpen();
    				bool DoBind(Address* local);
    				bool DoConnect(Address* remote);
    				bool DoClose();
    				int GetWriteFD();
    				int GetReadFD();
                public:
                    FIFOChannel(ChannelService& factory);
                    void SetReadPath(const string& path)
                    {
                        m_read_path = path;
                    }
                    void SetWritePath(const string& path)
                    {
                        m_write_path = path;
                    }
                    const string& GetReadPath()
                    {
                        return m_read_path;
                    }
                    const string& GetWritePath()
                    {
                        return m_write_path;
                    }
                    void SetOpenFlag(int flag);
                    virtual ~FIFOChannel();
            };
        }
    }
}

#endif /* FIFOCHANNEL_HPP_ */
