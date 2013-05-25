/*
 * http_decoder.hpp
 *
 *  Created on: 2011-12-22
 *      Author: qiyingwang
 */

#ifndef HTTP_DECODER_HPP_
#define HTTP_DECODER_HPP_

#include "channel/http/http_message.hpp"
#include "misc/http/http_parser.h"
#include "channel/channel_upstream_handler.hpp"
#include "channel/channel.hpp"
#include "buffer/buffer.hpp"

using namespace arch::channel;
using namespace arch::buffer;
#include <utility>

namespace arch
{
    namespace channel
    {
        namespace http
        {

            struct HTTPDecodeContext
            {
                    bool decoding;
                    ChannelHandlerContext* ctx;
                    Buffer cumulate;
                    HTTPMessage message;
                    void Close()
                    {
                        if (NULL != ctx)
                        {
                            ctx->GetChannel()->Close();
                        }
                        Clear();
                    }
                    void Clear()
                    {
                        decoding = false;
                        //ctx = NULL;
                        cumulate.Clear();
                        message.Clear();
                    }
                    void SetBody(const char *at, size_t len)
                    {
                    	Buffer& buf = message.GetBody();
                    	buf.Write(at, len);
                        //message.SetBody(body);
                    }
                    void SetURL(sds url)
                    {
                        message.SetURL(url);
                    }
                    bool AppendHeaderName(sds s)
                    {
                        return message.AppendHeaderName(s);
                    }
                    bool AppendHeaderValue(sds s)
                    {
                        return message.AppendHeaderValue(s);
                    }
            };

            class HTTPMessageDecoder: public ChannelUpstreamHandler<Buffer>
            {
                protected:
                    http_parser_settings m_praser_setting;
                    http_parser m_parser;
                    HTTPDecodeContext m_ctx;
                    void MessageReceived(ChannelHandlerContext& ctx,
                            MessageEvent<Buffer>& e);
                    void ChannelClosed(ChannelHandlerContext& ctx,
                            ChannelStateEvent& e);
                public:
                    HTTPMessageDecoder(bool isHttpRequest);
                    ~HTTPMessageDecoder();
            };

        }
    }
}

#endif /* HTTP_DECODER_HPP_ */
