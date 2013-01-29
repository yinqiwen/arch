/*
 * ThreadMutex.hpp
 *
 *  Created on: 2011-1-12
 *      Author: wqy
 */

#ifndef NOVA_THREADMUTEX_HPP_
#define NOVA_THREADMUTEX_HPP_
#include <pthread.h>

namespace arch
{
	namespace concurrent
	{
		class ThreadMutex
		{
			protected:
				pthread_mutex_t m_mutex;
			public:
				ThreadMutex();
				virtual ~ThreadMutex();
				pthread_mutex_t& GetRawMutex();
		};
	}
}

#endif /* THREADMUTEX_HPP_ */
