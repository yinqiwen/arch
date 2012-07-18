/*
 * http_encoder.hpp
 *
 *  Created on: 2011-12-22
 *      Author: qiyingwang
 */

#ifndef HTTP_ENCODER_HPP_
#define HTTP_ENCODER_HPP_

#include "channel/http/http_message.hpp"
#include "misc/http/http_parser.h"
#include "channel/all_includes.hpp"
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
            class HTTPMessageEncoder: public ChannelDownstreamHandler<
                    HTTPMessage>
            {
                private:
                    bool WriteRequested(ChannelHandlerContext& ctx,
                            MessageEvent<HTTPMessage>& e);
            };
        }
    }
}

#endif /* HTTP_ENCODER_HPP_ */
