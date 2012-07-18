/*
 * MyFramework.hpp
 *
 *  Created on: 2011-2-1
 *      Author: wqy
 */

#include "MyManagerProcess.hpp"
#include "MyManagerHandler.hpp"
#include "MyServiceHandler.hpp"
#include "MyServiceProcess.hpp"
#include "util/proctitle_helper.h"

int main(int argc, char** argv)
{
	ngx_init_setproctitle(argc, argv);
	FrameworkOptions option;
	option.home_dir = "./";
	//option.use_shm_fifo = false;
	option.manager_routine_period = 5000;
	//option.process_routine_period = 1000;
	//option.service_timeout_count_limit = 5;
	MyManagerProcess manager(option);
	MyServiceProcessFactory factory;
	manager.RegisterServiceProcessFactory(&factory);
	manager.Start();
}
