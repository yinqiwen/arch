/*
 * message.hpp
 *
 *  Created on: 2011-12-22
 *      Author: qiyingwang
 */

#ifndef HTTP_CHANNEL_MESSAGE_HPP_
#define HTTP_CHANNEL_MESSAGE_HPP_
#include "common/base.hpp"
#include "buffer/buffer.hpp"
#include "misc/sds/sds.h"
#include <utility>
#include <vector>
#include <string>

using arch::buffer::Buffer;
namespace arch
{
    namespace channel
    {
        namespace http
        {
            enum HttpMethod
            {
                DELETE = 0,
                GET = 1,
                HEAD = 2,
                POST = 3,
                PUT = 4,
                CONNECT = 5,
                OPTIONS = 6,
                TRACE = 7,
                COPY = 8,
                LOCK = 9,
                MKCOL = 10,
                MOVE = 11,
                PROPFIND = 12,
                PROPPATCH = 13,
                SEARCH = 14,
                UNLOCK = 15,
                REPORT = 16,
                MKACTIVITY = 17,
                CHECKOUT = 18,
                MERGE = 19,
                MSEARCH = 20,
                NOTIFY = 21,
                SUBSCRIBE = 22,
                UNSUBSCRIBE = 23,
                PATCH = 24,
                PURGE = 25
            };
            struct HTTPHeader
            {
                    sds name;
                    sds value;
                    HTTPHeader(sds n, sds v) :
                            name(n), value(v)
                    {
                    }
            };
            typedef std::vector<HTTPHeader> HTTPHeaderArray;

            class HTTPDecodeContext;
            class HTTPMessage
            {
                protected:
                    uint8 m_http_major;
                    uint8 m_http_minor;
                    uint16 m_method_or_status_code;
                    sds m_url;
                    Buffer m_body;
                    sds m_current_header_name;
                    HTTPHeaderArray m_headers;
                    friend class HTTPDecodeContext;
                    void SetURL(sds url)
                    {
                        m_url = url;
                    }
                    bool AppendHeaderName(sds s);
                    bool AppendHeaderValue(sds s);
                    HTTPHeader* GetHeaderEntry(const char* name);
                public:
                    HTTPMessage();
                    void Clear();
                    void SetHttpMajor(uint8 major);
                    void SetHttpMinor(uint8 minor);
                    bool SetStatusCode(uint16 code);
                    bool SetMethod(HttpMethod method);
                    bool IsRequest();
                    uint8 GetHttpMajor()
                    {
                        return m_http_major;
                    }
                    uint8 GetHttpMinor()
                    {
                        return m_http_minor;
                    }
                    const char* GetUrl()
                    {
                        return m_url;
                    }
                    void setUrl(const char* url);
                    uint16 GetStatusCode();
                    HttpMethod GetMethod();
                    const char* GetMethodString();
                    const char* GetResponseReasonPhrase();
                    const char* GetHeader(const char* name);
                    void AddHeader(const char* name, const char* value);
                    void SetHeader(const char* name, const char* value);
                    void RemoveHeader(const char* name);
                    void SetContentLength(uint64 len);
                    Buffer& GetBody()
                    {
                        return m_body;
                    }
                    std::string ToString();
                    int Encode(Buffer& buffer);
                    ~HTTPMessage();
            };
        }
    }
}

#endif /* MESSAGE_HPP_ */
