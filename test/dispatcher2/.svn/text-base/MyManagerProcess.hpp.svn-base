/*
 * MyMangerProcess.hpp
 *
 *  Created on: 2011-2-7
 *      Author: wqy
 */

#ifndef MYMANGERPROCESS_HPP_
#define MYMANGERPROCESS_HPP_
#include "framework/process/manager_process.hpp"
#include "channel/all_includes.hpp"
using namespace arch::framework;
using namespace arch::channel;
using namespace arch::channel::codec;
extern uint32 g_manager_recv_counts;
class MyManagerIPCHandler: public ManagerHandler
{
	private:
		bool OnInit();
		bool OnDestroy();
};

class MyManagerProcess: public ManagerProcess
{
	private:
		Channel* m_server;
		IntegerHeaderFrameDecoder m_decoder;
		MyManagerIPCHandler m_internal_handler;
		bool OnInit();
		void OnRoutine();
		ManagerHandler* GetManagerHandler()
		{
			return &m_internal_handler;
		}
	public:
		MyManagerProcess(FrameworkOptions option) :
			ManagerProcess(option), m_server(NULL)
		{
		}
};

#endif /* MYMANGERPROCESS_HPP_ */
