/*
 * http_client.hpp
 *
 *  Created on: 2013-3-18
 *      Author: wqy
 */

#ifndef HTTP_CLIENT_HPP_
#define HTTP_CLIENT_HPP_

#include "common/base.hpp"
#include "buffer/buffer.hpp"
#include <utility>
#include <vector>
#include <string>
#include "http_message.hpp"
#include "channel/channel_service.hpp"

using arch::buffer::Buffer;
using namespace arch::channel;
namespace arch
{
	namespace channel
	{
		namespace http
		{
			struct HttpClientOptions
			{
					int max_idle_conns_per_host;
			};
			struct HttpClientCallback
			{
					virtual void OnResponse(HTTPMessage& response) = 0;
					virtual void OnBody(Buffer& content) = 0;
					virtual void OnComplete() = 0;
					virtual void OnError(int errorCode, const std::string& cause) = 0;
					virtual ~HttpClientCallback()
					{
					}
			};
			class HttpClient
			{
				private:
					HttpClientOptions m_options;
					ChannelService& m_serv;
					Channel* GetIdleConnection(const char* host);
				public:
					HttpClient(ChannelService& serv);
					bool Do(HTTPMessage& req, HttpClientCallback* cb);
					bool Get(const std::string& url, HttpClientCallback* cb);
			};
		}
	}
}

#endif /* HTTP_CLIENT_HPP_ */
