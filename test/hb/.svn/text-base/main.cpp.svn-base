/*
 * main.cpp
 *
 *  Created on: 2013-1-22
 *      Author: wqy
 */

#include <unistd.h>
#include <stdlib.h>
#include "logging/logger.hpp"
#include "util/file_helper.hpp"
#include "util/string_helper.hpp"
#include "http_benchmark.hpp"

using namespace arch::util;
using namespace arch::logging;

static const char *optString = "H:h:p:f:c:n:m:u:t:r:x:k";
static void processs_args(int argc, char** argv, HttpBenchmarkOptions& options,
        Buffer& buffer)
{
    int opt = 0;

    /* Initialize globalArgs before we get to work. */
    options.concurrentConns = 1;
    options.keepAlive = false;
    options.limit = 0;
    options.ratio = 5;
    std::string input_file;
    std::string method;
    std::string url;
    std::string headers;
    std::string xvars;
    opt = getopt(argc, argv, optString);
    while (opt != -1)
    {
        switch (opt)
        {
            case 'k':
                options.keepAlive = true; /* true */
                break;

            case 'f':
                input_file = optarg;
                break;
            case 'u':
                url = optarg;
                break;
            case 'c':
                options.concurrentConns = atoi(optarg);
                break;
            case 'r':
                options.ratio = atoi(optarg);
                break;
            case 'n':
                //globalArgs.concurentConns = atoi(optarg);
                options.limit = atoi(optarg);
                break;
            case 't':
                options.tps = atoi(optarg);
                break;
            case 'h':
                options.host = optarg;
                break;
            case 'p':
                options.port = atoi(optarg);
                break;
            case 'H':
                headers = optarg;
                break;
            case 'm':
                method = optarg;
                break;
            case 'x':
                xvars = optarg;
                break;
            default:
                /* You won't actually get here. */
                break;
        }

        opt = getopt(argc, argv, optString);
    }

    if (url.empty())
    {
        ERROR_LOG("No -u para");
        exit(-1);
    }

    if (method.empty())
    {
        ERROR_LOG("No -m para");
        exit(-1);
    }

    HTTPMessage request;
    request.SetMethod(POST);
    request.SetHttpMajor(1);
    request.SetHttpMinor(1);
    request.setUrl(url.c_str());
    if (strcasecmp(method.c_str(), "GET") == 0)
    {
        request.SetMethod(GET);
    }
    else if (strcasecmp(method.c_str(), "POST") == 0)
    {
        request.SetMethod(POST);
    }
    else if (strcasecmp(method.c_str(), "PUT") == 0)
    {
        request.SetMethod(PUT);
    }
    else if (strcasecmp(method.c_str(), "HEAD") == 0)
    {
        request.SetMethod(HEAD);
    }
    else if (strcasecmp(method.c_str(), "DELETE") == 0)
    {
        request.SetMethod(DELETE);
    }
    char tmp[1024];
    sprintf(tmp, "%s:%d", options.host.c_str(), options.port);
    request.SetHeader("Host", tmp);

    if (!input_file.empty())
    {
        file_read_full(input_file, request.GetBody());
    }
    sprintf(tmp, "%d", request.GetBody().ReadableBytes());
    request.SetHeader("Content-Length", tmp);
    if (!headers.empty())
    {
        std::vector<std::string> ss = split_string(headers, ";");
        for (int i = 0; i < ss.size(); i++)
        {
            std::string hv = ss[i];
            std::vector<std::string> hvss = split_string(hv, ":");
            if (hvss.size() >= 2)
            {
                request.AddHeader(hvss[0].c_str(), hvss[1].c_str());
            }
        }
    }
    if (options.keepAlive)
    {
        request.SetHeader("Connection", "keep-alive");
    }
    else
    {
        request.SetHeader("Connection", "close");
    }
    request.Encode(buffer);

    if (!xvars.empty())
    {
        std::vector<std::string> ss = split_string(xvars, ";");
        for (int i = 0; i < ss.size(); i++)
        {
            std::string hv = ss[i];
            std::vector<std::string> hvss = split_string(hv, "=");
            if (hvss.size() >= 2)
            {
                options.vars[hvss[0]] = hvss[1];
            }
        }
    }
}

int main(int argc, char** argv)
{
    HttpBenchmarkOptions options;
    Buffer buffer;
    processs_args(argc, argv, options, buffer);
    LoggerFactory::GetDevelopLogger()->SetLevel(INFO_LOG_LEVEL);
    INFO_LOG("Start http benchmark.");
    start_benchmark(options, buffer);
    return 0;
}
