/*
 * types.hpp
 *
 *  Created on: 2011-7-27
 *      Author: wqy
 */

#ifndef IPC_TYPES_HPP_
#define IPC_TYPES_HPP_
#include "common/base.hpp"

namespace arch
{
    namespace ipc
    {
    	enum IPCType
    	{
    		IPC_FIFO = 1,
    		IPC_UnixDomainSocket = 2,
    		IPC_SharedMemory = 3
    	};
    }
}

#endif /* TYPES_HPP_ */
