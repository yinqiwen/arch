/*
 * object.cpp
 *
 *  Created on: 2011-2-25
 *      Author: qiyingwang
 */
#include "common/object.hpp"

namespace arch
{
    namespace common
    {
        void DefaultObjectDestructor(Object* obj)
        {
            delete obj;
        }
    }
}
