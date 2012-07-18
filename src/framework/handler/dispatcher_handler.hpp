/*
 * dispatcher_handler.hpp
 *
 *  Created on: 2011-7-23
 *      Author: wqy
 */

#ifndef DISPATCHER_HANDLER_HPP_
#define DISPATCHER_HANDLER_HPP_
#include "channel/all_includes.hpp"
#include "buffer/buffer.hpp"
#include "framework/handler/io_process_handler.hpp"
#include "framework/handler/service_handler.hpp"

using arch::buffer::Buffer;
using arch::channel::Channel;

namespace arch
{
	namespace framework
	{
		class DispatcherProcess;
		class DispatcherHandler: public IOProcessHandler
		{
			protected:
				DispatcherProcess* m_proc;
				void  SetDispatcherProcess(DispatcherProcess*  proc);
				friend class DispatcherProcess;
			public:
				DispatcherHandler();
				DispatcherProcess& GetDispatcherProcess();
				virtual ~DispatcherHandler()
				{
				}
		};
	}
}

#endif /* DISPATCHER_HANDLER_HPP_ */
