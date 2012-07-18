/*
 * lock_mode.hpp
 *
 *  Created on: 2011-4-4
 *      Author: wqy
 */

#ifndef LOCK_MODE_HPP_
#define LOCK_MODE_HPP_

namespace arch
{
	namespace concurrent
	{
		enum LockMode
		{
			INVALID_LOCK,
			READ_LOCK,
			WRITE_LOCK
		};
	}
}

#endif /* LOCK_MODE_HPP_ */
