/*
 * math_helper.cpp
 *
 *  Created on: 2011-5-1
 *      Author: wqy
 */
#include "util/math_helper.hpp"
#include <stdlib.h>
#include <time.h>

namespace arch
{
    namespace util
    {
        uint32 upper_power_of_two(uint32 v)
        {
            v--;
            v |= v >> 1;
            v |= v >> 2;
            v |= v >> 4;
            v |= v >> 8;
            v |= v >> 16;
            v++;
            return v;
        }

        int32 random_int32()
        {
            srandom(time(NULL));
            return random();
        }
    }
}
