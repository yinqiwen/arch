/*
 * http_encoder.cpp
 *
 *  Created on: 2012-3-15
 *      Author: qiyingwang
 */
#include "channel/http/http_encoder.hpp"
#include "util/string_helper.hpp"
#include "logging/logger_macros.hpp"
#include <ctype.h>

using namespace arch::channel::http;
using namespace arch::util;

bool HTTPMessageEncoder::WriteRequested(ChannelHandlerContext& ctx,
        MessageEvent<HTTPMessage>& e)
{
    static Buffer buffer(1024);
    buffer.Clear();
    HTTPMessage* msg = e.GetMessage();
    if (msg->Encode(buffer) == 0)
    {
        return ctx.GetChannel()->Write(buffer);
    }
    return true;
}
