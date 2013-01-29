/*
 * http_benchmark.hpp
 *
 *  Created on: 2013-1-22
 *      Author: wqy
 */

#ifndef HTTP_BENCHMARK_HPP_
#define HTTP_BENCHMARK_HPP_

#include "channel/all_includes.hpp"
#include <string>
#include <map>

using namespace arch::channel;
using namespace arch::channel::http;

struct HttpBenchmarkOptions
{
        bool keepAlive;
        uint32 totalRequests;
        uint32 concurrentConns;
        uint32 ratio;
        std::string host;
        uint32 port;
        uint32 tps;
        uint32 limit;
        std::map<std::string, std::string> vars;
};

class HttpClientHandler: public ChannelUpstreamHandler<HTTPMessage>
{
    protected:

        void ChannelBound(ChannelHandlerContext& ctx, ChannelStateEvent& e)
        {

        }
        void ChannelConnected(ChannelHandlerContext& ctx, ChannelStateEvent& e);
        void ChannelOpen(ChannelHandlerContext& ctx, ChannelStateEvent& e)
        {
        }
        void ChannelClosed(ChannelHandlerContext& ctx, ChannelStateEvent& e);
        void MessageReceived(ChannelHandlerContext& ctx, MessageEvent<
                HTTPMessage>& e);

        void ExceptionCaught(ChannelHandlerContext& ctx, ExceptionEvent& e)
        {
        }
    public:
        bool m_request_sent;
        bool m_response_recved;
        bool m_connected;
        Channel* m_client;
        HttpClientHandler();
};

int start_benchmark(HttpBenchmarkOptions& options, Buffer& requestData);

#endif /* HTTP_BENCHMARK_HPP_ */
