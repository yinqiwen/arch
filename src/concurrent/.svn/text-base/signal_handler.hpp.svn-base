/*
 * signal_handler.hpp
 *
 *  Created on: 2011-4-23
 *      Author: wqy
 */

#ifndef SIGNAL_HANDLER_HPP_
#define SIGNAL_HANDLER_HPP_
#include "common/base.hpp"
#include <signal.h>
namespace arch
{
    namespace concurrent
    {
        struct SignalHandler
        {
                virtual void OnSignal(int signo, siginfo_t& info) = 0;
                virtual ~SignalHandler()
                {
                }
        };

        struct SoftSignalHandler
        {
                virtual void OnSoftSignal(uint32 soft_signo, uint32 appendinfo) = 0;
                virtual ~SoftSignalHandler()
                {
                }
        };
    }
}

#endif /* SIGNAL_HANDLER_HPP_ */
