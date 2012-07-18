/*
 * MyDispatcherHandler.hpp
 *
 *  Created on: 2011-7-23
 *      Author: wqy
 */

#ifndef MYDISPATCHERHANDLER_HPP_
#define MYDISPATCHERHANDLER_HPP_
#include "framework/handler/dispatcher_handler.hpp"
using namespace arch::framework;



class MyDispatcherHandler: public DispatcherHandler
{
		bool OnInit();
		bool OnDestroy();
		/**
		 * 例行检查回调
		 */
		void OnRoutine();

};

#endif /* MYDISPATCHERHANDLER_HPP_ */
