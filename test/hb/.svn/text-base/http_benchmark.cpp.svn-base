/*
 * http_benchmark.cpp
 *
 *  Created on: 2013-1-22
 *      Author: wqy
 */

#include "http_benchmark.hpp"
#include "util/time_helper.hpp"
#include "util/string_helper.hpp"
#include "util/math_helper.hpp"
#include <set>

using namespace arch::util;

static ChannelService* g_serv = NULL;

static uint32 g_count_in_10msperiod = 0;
static uint32 g_count_in_1speriod = 0;
static uint64 g_10ms_transc_start_time = 0;
static uint64 g_1s_transc_start_time = 0;
static uint32 g_sent_count = 0;
static uint32 g_ratio_sent_req = 0;
static uint32 g_limit = 0;
static uint32 g_tp10ms = 0;
static uint32 g_tps = 0;

static uint32 g_recved_res = 0;
static uint32 g_ratio_recved_res = 0;
static uint32 g_unanwsered_requests = 0;
static uint64 g_start_time = 0;

static HttpBenchmarkOptions g_options;
static Buffer g_request_data;
static std::string g_orig_request_str;
static Buffer g_var_request_data;

typedef std::set<HttpClientHandler*> ChannelSet;
static ChannelSet g_all_channels;
static ChannelSet g_ready_channels;
static int32 g_period_task_id = -1;

static int http_client_connect(const char* host, int port);
static void start_10ms_period_task(bool);
static void start_1s_period_task(bool);
static int try_send_request();

static void period_log_stat(bool flag)
{
    INFO_LOG("Total: %u requests sent.", g_sent_count);
    INFO_LOG("Total: %u responses recv.", g_recved_res);
    INFO_LOG("Period(%usecs): %u requests sent.", g_options.ratio, g_ratio_sent_req);
    INFO_LOG("Period(%usecs): %u responses recv.", g_options.ratio,g_ratio_recved_res);
    g_ratio_sent_req = 0;
    g_ratio_recved_res = 0;
}

static Buffer& get_request_data()
{
    static uint32 seq = 0;
    if (g_options.vars.empty())
    {
        return g_request_data;
    }
    else
    {
        g_var_request_data.Clear();
        std::map<std::string, std::string>::iterator it =
                g_options.vars.begin();
        std::string newstr = g_orig_request_str;

        while (it != g_options.vars.end())
        {
            const std::string& key = it->first;
            const std::string& value = it->second;

            if (strcasecmp(value.c_str(), "${SEQ}"))
            {
                char tmp[256];
                sprintf(tmp, "%u", seq++);
                replace(newstr, key, tmp);
            }
            else if (strcasecmp(value.c_str(), "${RANDOM}"))
            {
                char tmp[256];
                sprintf(tmp, "%u", random_int32());
                replace(newstr, key, tmp);
            }
            else
            {
                replace(newstr, key, value);
            }
            it++;
        }
        uint32 index = newstr.find("\r\n\r\n");
        uint32 bodylen = newstr.size() - index - 4;
        uint32 startidx = newstr.find("Content-Length:");
        uint32 endidx = newstr.find("\r\n", startidx);
        char tmp[256];
        sprintf(tmp, "Content-Length:%d", bodylen);
        newstr.replace(startidx, (endidx - startidx), tmp);
        g_var_request_data.Write(newstr.c_str(), newstr.size());
        // INFO_LOG("Send new req:%s", newstr.c_str());
        return g_var_request_data;
    }
}

static void init_serv()
{
    if (NULL == g_serv)
    {
        g_serv = new ChannelService(102400);
    }
}

static void stop_serv()
{
    if ((g_recved_res + g_unanwsered_requests) < g_sent_count)
    {
        return;
    }
    uint64 now = get_current_monotonic_millis();
    if (NULL != g_serv)
    {
        g_serv->Stop();
        g_serv = NULL;
        INFO_LOG("Finish: %u requests sent, %u responses recv.", g_sent_count, g_recved_res);
        INFO_LOG("Cost %llums to execute benchmark test.", (now - g_start_time));
        double x = g_sent_count;
        INFO_LOG("Average %f req/s", (x/(now - g_start_time))*1000);
    }

}

HttpClientHandler::HttpClientHandler() :
    m_request_sent(false), m_connected(false), m_response_recved(false),
            m_client(NULL)
{
}

void HttpClientHandler::MessageReceived(ChannelHandlerContext& ctx,
        MessageEvent<HTTPMessage>& e)
{
    g_recved_res++;
    g_ratio_recved_res++;
    m_request_sent = false;
    if (!g_options.keepAlive)
    {
        ctx.GetChannel()->Close();
        g_all_channels.erase(this);
    }
    else
    {
        g_ready_channels.insert(this);
    }
    try_send_request();
}

void HttpClientHandler::ChannelConnected(ChannelHandlerContext& ctx,
        ChannelStateEvent& e)
{
    m_connected = true;
    m_client = ctx.GetChannel();
    g_ready_channels.insert(this);
    try_send_request();
}

void HttpClientHandler::ChannelClosed(ChannelHandlerContext& ctx,
        ChannelStateEvent& e)
{
    g_ready_channels.erase(this);
    g_all_channels.erase(this);
    if (!m_connected)
    {
        WARN_LOG("Connect %s:%u failed!", g_options.host.c_str(), g_options.port);
    }
    try_send_request();
    if (m_request_sent)
    {
        g_unanwsered_requests++;
        if (g_unanwsered_requests % 10000 == 0)
        {
            INFO_LOG("Recv %d xres.", g_unanwsered_requests);
        }
    }
}

static void http_pipeline_init(ChannelPipeline* pipeline, void* data)
{
    pipeline->AddLast("encoder", new HTTPMessageEncoder());
    pipeline->AddLast("decoder", new HTTPMessageDecoder(false));
    HttpClientHandler* handler = new HttpClientHandler;
    handler->m_client = (Channel*) data;
    pipeline->AddLast("handler", handler);
    g_all_channels.insert(handler);
}

static void http_pipeline_finallize(ChannelPipeline* pipeline, void* data)
{
    ChannelHandler* handler = pipeline->Get("decoder");
    DELETE(handler);
    handler = pipeline->Get("encoder");
    DELETE(handler);
    handler = pipeline->Get("handler");
    DELETE(handler);
}

static int http_client_connect(const char* host, int port)
{
    ChannelService service;
    ClientSocketChannel* client = g_serv->NewClientSocketChannel();
    SocketHostAddress address(host, port);
    client->Connect(&address);
    client->SetChannelPipelineInitializor(http_pipeline_init, client);
    client->SetChannelPipelineFinalizer(http_pipeline_finallize, NULL);
    return 0;
}

static int try_send_request()
{
    if (g_limit > 0 && g_sent_count >= g_limit)
    {
        stop_serv();
        return 0;
    }
    uint64 now = get_current_monotonic_millis();

    if (g_tp10ms > 0 && g_count_in_10msperiod >= g_tp10ms)
    {

        if (g_period_task_id == -1)
        {
            if (g_count_in_1speriod >= g_options.tps)
            {
                uint64 delay_1s = 0;
                if (now >= g_1s_transc_start_time + 1000)
                {
                    delay_1s = 0;
                }
                else
                {
                    delay_1s = (g_1s_transc_start_time + 1000) - now;
                }
                g_period_task_id = g_serv->GetTimer().ScheduleHeapTask(
                        make_fun_runnable(start_1s_period_task, true),
                        delay_1s, -1);
            }
            else
            {
                uint64 delay_10ms = 0;
                if (now >= g_10ms_transc_start_time + 10)
                {
                    delay_10ms = 0;
                }
                else
                {
                    delay_10ms = (g_10ms_transc_start_time + 10) - now;
                }
                g_period_task_id = g_serv->GetTimer().ScheduleHeapTask(
                        make_fun_runnable(start_10ms_period_task, true),
                        delay_10ms, -1);
            }
        }

        return 0;
    }
    if (!g_ready_channels.empty())
    {
        ChannelSet::iterator it = g_ready_channels.begin();
        HttpClientHandler* client = *it;
        client->m_request_sent = true;
        Buffer& data = get_request_data();
        client->m_client->Write(data);
        data.SetReadIndex(0);
        g_ready_channels.erase(it);
        g_count_in_10msperiod++;
        g_sent_count++;
        g_ratio_sent_req++;
        g_count_in_1speriod++;
        return 1;
    }
    else
    {
        if (g_all_channels.size() < g_options.concurrentConns)
        {
            http_client_connect(g_options.host.c_str(), g_options.port);
        }
    }
    return 0;
}

static void start_10ms_period_task(bool flag)
{
    g_count_in_10msperiod = 0;
    g_period_task_id = -1;
    g_10ms_transc_start_time = get_current_monotonic_millis();
    for (int i = 0; i < g_tp10ms; i++)
    {
        if (try_send_request() == 0)
        {
            break;
        }
    }
}

static void start_1s_period_task(bool flag)
{
    g_count_in_1speriod = 0;
    g_1s_transc_start_time = get_current_monotonic_millis();
    start_10ms_period_task(true);
}

int start_benchmark(HttpBenchmarkOptions& options, Buffer& requestData)
{
    init_serv();
    g_options = options;
    if (!g_options.vars.empty())
    {
        g_orig_request_str = requestData.AsString();
    }

    Buffer* data = &(requestData);
    g_request_data.Clear();
    g_request_data.Write(data, data->ReadableBytes());
    g_tp10ms = options.tps / 100;
    if (g_tp10ms == 0)
    {
        g_tp10ms = options.tps;
    }
    g_tps = options.tps;
    g_limit = options.limit;
    for (int i = 0; i < g_options.concurrentConns; i++)
    {
        http_client_connect(g_options.host.c_str(), g_options.port);
    }
    start_10ms_period_task(true);
    g_start_time = get_current_monotonic_millis();
    g_serv->GetTimer().ScheduleHeapTask(make_fun_runnable(period_log_stat,
            false), options.ratio * 1000, options.ratio * 1000);
    g_serv->Start();
    return 0;
}
