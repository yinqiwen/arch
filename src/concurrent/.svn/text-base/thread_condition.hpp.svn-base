/*
 * ThreadCondition.hpp
 *
 *  Created on: 2011-1-12
 *      Author: wqy
 */

#ifndef NOVA_THREADCONDITION_HPP_
#define NOVA_THREADCONDITION_HPP_
#include <pthread.h>

namespace arch
{
	namespace concurrent
	{
		class ThreadCondition
		{
			protected:
				pthread_cond_t m_cond;
			public:
				ThreadCondition();
				virtual ~ThreadCondition();
				pthread_cond_t& GetRawCondition()
				{
					return m_cond;
				}

		};
	}
}

#endif /* THREADCONDITION_HPP_ */
