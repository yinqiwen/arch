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

using arch::buffer::Buffer;
namespace arch
{
    namespace channel
    {
        namespace http
        {
        	   class HttpClient{
        		   private:
        			   Channel* GetIdleConnection(const char* host);
        		   public:
        			   bool Do(HTTPMessage& req);
        			   bool Get(const std::string& url);
        	   };
        }
    }
}


#endif /* HTTP_CLIENT_HPP_ */
