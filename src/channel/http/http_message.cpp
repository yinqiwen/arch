/*
 * http_message.cpp
 *
 *  Created on: 2012-3-14
 *      Author: qiyingwang
 */
#include "channel/http/http_message.hpp"
#include "util/string_helper.hpp"
#include "misc/http/http_parser.h"
#include "logging/logger_macros.hpp"
#include <ctype.h>
#include <algorithm>

using namespace arch::channel::http;

HTTPMessage::HTTPMessage() :
        m_http_major(1), m_http_minor(0), m_method_or_status_code(0), m_url(
                NULL),  m_current_header_name(NULL)
{
}

bool HTTPMessage::AppendHeaderName(sds s)
{
    if (NULL != m_current_header_name)
    {
    	sdscat(m_current_header_name, s);
    	sdsfree(s);
//        ERROR_LOG("No value set for header:%s", m_current_header_name);
//        sdsfree(s);
        return true;
    }
    m_current_header_name = s;
    return true;
}

bool HTTPMessage::AppendHeaderValue(sds s)
{
    if (NULL == m_current_header_name)
    {
    	if(m_headers.empty())
    	{
    		ERROR_LOG("Empty header name for value:%s", s);
    		return false;
    	}
    	HTTPHeader& header = *(m_headers.rbegin());
    	header.value = sdscat(header.value, s);
    	sdsfree(s);
//        ERROR_LOG("Empty header name for value:%s", s);
//        sdsfree(s);
        return true;
    }
    HTTPHeader header(m_current_header_name, s);
    m_headers.push_back(header);
    m_current_header_name = NULL;
    return true;
}

bool HTTPMessage::IsRequest()
{
    return m_url != NULL;
}

uint16 HTTPMessage::GetStatusCode()
{
    if (IsRequest())
    {
        return 0;
    }
    return m_method_or_status_code;
}

HttpMethod HTTPMessage::GetMethod()
{
    if (IsRequest())
    {
        return (HttpMethod) m_method_or_status_code;
    }
    return (HttpMethod) -1;
}

void HTTPMessage::SetHttpMajor(uint8 major)
{
    m_http_major = major;
}
void HTTPMessage::SetHttpMinor(uint8 minor)
{
    m_http_minor = minor;
}

bool HTTPMessage::SetMethod(HttpMethod method)
{
    m_method_or_status_code = method;
    return true;
}

bool HTTPMessage::SetStatusCode(uint16 code)
{
    m_method_or_status_code = code;
    return true;
}

HTTPHeader* HTTPMessage::GetHeaderEntry(const char* name)
{
    HTTPHeaderArray::iterator it = m_headers.begin();
    while (it != m_headers.end())
    {
        if (strcasecmp(name, it->name) == 0)
        {
            return &(*it);
        }
        it++;
    }
    return NULL;
}

void HTTPMessage::AddHeader(const char* name, const char* value)
{
    sds n = sdsnew(name);
    sds v = sdsnew(value);
    HTTPHeader header(n, v);
    m_headers.push_back(header);
}

void HTTPMessage::SetHeader(const char* name, const char* value)
{
    HTTPHeader* header = GetHeaderEntry(name);
    if (NULL == header)
    {
        AddHeader(name, value);
    }
    else
    {
        sdsfree(header->value);
        header->value = sdsnew(value);
    }
}

static bool is_null_entry(HTTPHeader& header)
{
    return header.name == NULL && header.value == NULL;
}

void HTTPMessage::RemoveHeader(const char* name)
{
    HTTPHeaderArray::iterator it = m_headers.begin();
    while (it != m_headers.end())
    {
        if (strcasecmp(name, it->name) == 0)
        {
            sdsfree(it->name);
            sdsfree(it->value);
            it->name = NULL;
            it->value = NULL;
        }
        it++;
    }
    std::remove_if (m_headers.begin(), m_headers.end(), is_null_entry);
}

void HTTPMessage::Clear()
{
	m_body.Clear();
    sdsfree(m_current_header_name);
    m_current_header_name = NULL;
    sdsfree(m_url);
    m_url = NULL;
    m_method_or_status_code = 0;
    HTTPHeaderArray::iterator it = m_headers.begin();
    while (it != m_headers.end())
    {
        sdsfree(it->name);
        sdsfree(it->value);
        it++;
    }
    m_headers.clear();
}

void HTTPMessage::setUrl(const char* url)
{
    RETURN_IF_NULL(url);
    m_url = sdsnew(url);
}

const char* HTTPMessage::GetMethodString()
{
    return http_method_str((http_method) m_method_or_status_code);
}

const char* HTTPMessage::GetResponseReasonPhrase()
{
    switch (m_method_or_status_code)
    {
        case 100:
            return "Continue";
        case 101:
            return "Switching Protocols";
        case 102:
            return "Processing";
        case 200:
            return "OK";
        case 201:
            return "Created";

            /**
             * 202 Accepted
             */
        case 202:
            return "Accepted";

            /**
             * 203 Non-Authoritative Information (since HTTP/1.1)
             */
        case 203:
            return "Non-Authoritative Information";

            /**
             * 204 No Content
             */
        case 204:
            return "No Content";

            /**
             * 205 Reset Content
             */
        case 205:
            return "Reset Content";

            /**
             * 206 Partial Content
             */
        case 206:
            return "Partial Content";

            /**
             * 207 Multi-Status (WebDAV, RFC2518)
             */
        case 207:
            return "Multi-Status";

            /**
             * 300 Multiple Choices
             */
        case 300:
            return "Multiple Choices";

            /**
             * 301 Moved Permanently
             */
        case 301:
            return "Moved Permanently";

            /**
             * 302 Found
             */
        case 302:
            return "Found";

            /**
             * 303 See Other (since HTTP/1.1)
             */
        case 303:
            return "See Other";

            /**
             * 304 Not Modified
             */
        case 304:
            return "Not Modified";

            /**
             * 305 Use Proxy (since HTTP/1.1)
             */
        case 305:
            return "Use Proxy";

            /**
             * 307 Temporary Redirect (since HTTP/1.1)
             */
        case 307:
            return "Temporary Redirect";

            /**
             * 400 Bad Request
             */
        case 400:
            return "Bad Request";

            /**
             * 401 Unauthorized
             */
        case 401:
            return "Unauthorized";

            /**
             * 402 Payment Required
             */
        case 402:
            return "Payment Required";

            /**
             * 403 Forbidden
             */
        case 403:
            return "Forbidden";

            /**
             * 404 Not Found
             */
        case 404:
            return "Not Found";

            /**
             * 405 Method Not Allowed
             */
        case 405:
            return "Method Not Allowed";

            /**
             * 406 Not Acceptable
             */
        case 406:
            return "Not Acceptable";

            /**
             * 407 Proxy Authentication Required
             */
        case 407:
            return "Proxy Authentication Required";

            /**
             * 408 Request Timeout
             */
        case 408:
            return "Request Timeout";

            /**
             * 409 Conflict
             */
        case 409:
            return "Conflict";

            /**
             * 410 Gone
             */
        case 410:
            return "Gone";

            /**
             * 411 Length Required
             */
        case 411:
            return "Length Required";

            /**
             * 412 Precondition Failed
             */
        case 412:
            return "Precondition Failed";

            /**
             * 413 Request Entity Too Large
             */
        case 413:
            return "Request Entity Too Large";

            /**
             * 414 Request-URI Too Long
             */
        case 414:
            return "Request-URI Too Long";

            /**
             * 415 Unsupported Media Type
             */
        case 415:
            return "Unsupported Media Type";

            /**
             * 416 Requested Range Not Satisfiable
             */
        case 416:
            return "Requested Range Not Satisfiable";

            /**
             * 417 Expectation Failed
             */
        case 417:
            return "Expectation Failed";

            /**
             * 422 Unprocessable Entity (WebDAV, RFC4918)
             */
        case 422:
            return "Unprocessable Entity";

            /**
             * 423 Locked (WebDAV, RFC4918)
             */
        case 423:
            return "Locked";

            /**
             * 424 Failed Dependency (WebDAV, RFC4918)
             */
        case 424:
            return "Failed Dependency";

            /**
             * 425 Unordered Collection (WebDAV, RFC3648)
             */
        case 425:
            return "Unordered Collection";

            /**
             * 426 Upgrade Required (RFC2817)
             */
        case 426:
            return "Upgrade Required";

            /**
             * 500 Internal Server Error
             */
        case 500:
            return "Internal Server Error";

            /**
             * 501 Not Implemented
             */
        case 501:
            return "Not Implemented";

            /**
             * 502 Bad Gateway
             */
        case 502:
            return "Bad Gateway";

            /**
             * 503 Service Unavailable
             */
        case 503:
            return "Service Unavailable";

            /**
             * 504 Gateway Timeout
             */
        case 504:
            return "Gateway Timeout";

            /**
             * 505 HTTP Version Not Supported
             */
        case 505:
            return "HTTP Version Not Supported";

            /**
             * 506 Variant Also Negotiates (RFC2295)
             */
        case 506:
            return "Variant Also Negotiates";

            /**
             * 507 Insufficient Storage (WebDAV, RFC4918)
             */
        case 507:
            return "Insufficient Storage";

            /**
             * 510 Not Extended (RFC2774)
             */
        case 510:
            return "Not Extended";
        default:
            return "";
    }
}

const char* HTTPMessage::GetHeader(const char* name)
{
    RETURN_NULL_IF_NULL(name);
    HTTPHeaderArray::iterator it = m_headers.begin();
    while (it != m_headers.end())
    {
        if (strcasecmp(name, it->name) == 0)
        {
            return it->value;
        }
        it++;
    }
    return NULL;
}

void HTTPMessage::SetContentLength(uint64 len)
{
    char tmp[32];
    sprintf(tmp, "%llu", len);
    SetHeader("Content-Length", tmp);
}

int HTTPMessage::Encode(Buffer& buffer)
{
    if (IsRequest())
    {
        buffer.Printf("%s %s HTTP/%u.%u\r\n", GetMethodString(), m_url,
                m_http_major, m_http_minor);
    }
    else
    {
        buffer.Printf("HTTP/%u.%u %u %s\r\n", m_http_major, m_http_minor,
                m_method_or_status_code, GetResponseReasonPhrase());
    }
    HTTPHeaderArray::iterator it = m_headers.begin();
    while (it != m_headers.end())
    {
        buffer.Printf("%s: %s\r\n", it->name, it->value);
        it++;
    }
    buffer.Printf("\r\n");
    if (m_body.Readable())
    {
        buffer.Write(m_body.GetRawReadBuffer(), m_body.ReadableBytes());
    }
    return 0;
}

std::string HTTPMessage::ToString()
{
    char prover[16];
    sprintf(prover, "HTTP/%u.%u", m_http_major, m_http_minor);

    std::string str;
    if (IsRequest())
    {
        str += GetMethodString();
        str += " ";
        str += m_url;
        str += " ";
        str += prover;
        str += "\r\n";
    }
    else
    {
        str += prover;
        char tmp[10];
        sprintf(tmp, " %u\r\n", m_method_or_status_code);
        str += tmp;
    }
    HTTPHeaderArray::iterator it = m_headers.begin();
    while (it != m_headers.end())
    {
        str += it->name;
        str += ":";
        str += it->value;
        str += "\r\n";
        it++;
    }
    str += "\r\n";
    return str;
}

HTTPMessage::~HTTPMessage()
{
    Clear();
}
