/*
 * MyFramework.hpp
 *
 *  Created on: 2011-2-1
 *      Author: wqy
 */

#include "MyManagerProcess.hpp"
#include "MyServiceHandler.hpp"
#include "MyServiceProcess.hpp"
#include "MyDispatcherProcess.hpp"
#include "util/proctitle_helper.h"

int main(int argc, char** argv)
{
	ngx_init_setproctitle(argc, argv);
	FrameworkOptions option;
	option.home_dir = "./";
	option.manager_routine_period = 5000;
	//option.process_routine_period = 1000;
	//option.service_timeout_count_limit = 5;
	MyManagerProcess manager(option);
	MyServiceProcessFactory factory;
	MyDispatcherProcessFactory disf;
	manager.RegisterServiceProcessFactory(&factory);
	manager.RegisterServiceProcessFactory(&disf);
	manager.Start();
}
