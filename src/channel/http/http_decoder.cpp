/*
 * http_decoder.cpp
 *
 *  Created on: 2011-12-26
 *      Author: qiyingwang
 */
#include "channel/http/http_decoder.hpp"
#include "util/string_helper.hpp"
#include "channel/all_includes.hpp"
#include "logging/logger_macros.hpp"
#include <ctype.h>

using namespace arch::channel::http;
using namespace arch::util;

static int on_header_field(http_parser *parser, const char *at, size_t len)
{
    HTTPDecodeContext* ctx = (HTTPDecodeContext*) parser->data;
    sds s = sdsnewlen(at, len);
    if (!ctx->AppendHeaderName(s))
    {
        ctx->Close();
        return -1;
    }
    return 0;
}

static int on_header_value(http_parser *parser, const char *at, size_t len)
{
    HTTPDecodeContext* ctx = (HTTPDecodeContext*) parser->data;
    sds s = sdsnewlen(at, len);
    if (!ctx->AppendHeaderValue(s))
    {
        ctx->Close();
        return -1;
    }
    return 0;
}

static int on_headers_complete(http_parser* parser)
{
    return 0;
}

static int on_message_begin(http_parser* parser)
{
    return 0;
}

static int on_url(http_parser *parser, const char *at, size_t len)
{
    HTTPDecodeContext* ctx = (HTTPDecodeContext*) parser->data;
    sds url = sdsnewlen(at, len);
    ctx->SetURL(url);
    return 0;
}

static int on_body(http_parser *parser, const char *at, size_t len)
{
    HTTPDecodeContext* ctx = (HTTPDecodeContext*) parser->data;
    //sds body = sdsnewlen(at, len);
    ctx->SetBody(at, len);
    return 0;
}

static int on_message_complete(http_parser* parser)
{
    HTTPDecodeContext* ctx = (HTTPDecodeContext*) parser->data;
    if (0 != parser->status_code)
    {
        ctx->message.SetStatusCode(parser->status_code);
    }
    else
    {
        ctx->message.SetMethod((HttpMethod) parser->method);
    }
    ctx->message.SetHttpMajor(parser->http_major);
    ctx->message.SetHttpMinor(parser->http_minor);
    ChannelHandlerContext* handler = ctx->ctx;
    fire_message_received<HTTPMessage>(*handler, &(ctx->message), NULL);
    ctx->Clear();
    return 0;
}

HTTPMessageDecoder::HTTPMessageDecoder(bool isHttpRequest)
{
    m_praser_setting.on_header_field = on_header_field;
    m_praser_setting.on_header_value = on_header_value;
    m_praser_setting.on_headers_complete = on_headers_complete;
    m_praser_setting.on_message_begin = on_message_begin;
    m_praser_setting.on_body = on_body;
    m_praser_setting.on_url = on_url;
    m_praser_setting.on_message_complete = on_message_complete;
    m_parser.data = &m_ctx;
    m_ctx.ctx = NULL;
    m_ctx.decoding = false;
    if(isHttpRequest)
    {
        http_parser_init(&m_parser, HTTP_REQUEST);
    }
    else
    {
        http_parser_init(&m_parser, HTTP_RESPONSE);
    }
}

void HTTPMessageDecoder::ChannelClosed(ChannelHandlerContext& ctx,
                            ChannelStateEvent& e)
{
    if(m_ctx.decoding)
    {
        on_message_complete(&m_parser);
    }
    ctx.SendUpstream(e);
}

void HTTPMessageDecoder::MessageReceived(ChannelHandlerContext& ctx,
        MessageEvent<Buffer>& e)
{
    m_ctx.ctx = &ctx;
    m_ctx.decoding = true;
    Buffer* msgbuf = e.GetMessage();
    uint32 bytes = msgbuf->ReadableBytes();
    if (m_ctx.cumulate.Readable())
    {
        m_ctx.cumulate.DiscardReadedBytes();
        m_ctx.cumulate.Write(msgbuf, bytes);
        msgbuf = &(m_ctx.cumulate);
        bytes = msgbuf->ReadableBytes();
    }

    uint32 len = http_parser_execute(&m_parser, &m_praser_setting,
            msgbuf->GetRawReadBuffer(), bytes);
    msgbuf->AdvanceReadIndex(len);
    if (len < bytes)
    {
        if(m_parser.http_errno != HPE_OK)
        {
            ERROR_LOG("Failed to parse http message for cause:%s", http_errno_name((http_errno)m_parser.http_errno));
            ctx.GetChannel()->Close();
            return;
        }
        if (msgbuf != &(m_ctx.cumulate))
        {
            m_ctx.cumulate.Write(msgbuf, (bytes - len));
        }
    }
}

HTTPMessageDecoder::~HTTPMessageDecoder()
{

}

