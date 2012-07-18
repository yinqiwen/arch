/*
 * system_helper.hpp
 *
 *  Created on: 2011-3-1
 *      Author: qiyingwang
 */

#ifndef SYSTEM_HELPER_HPP_
#define SYSTEM_HELPER_HPP_
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include "common/base.hpp"
#include <string>

namespace arch
{
    namespace util
    {

        uint32 available_processors();
        int print_stacktrace(std::string& buf);
    }
}

#endif /* SYSTEM_HELPER_HPP_ */
