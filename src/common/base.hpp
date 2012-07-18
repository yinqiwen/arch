/*
 * base.hpp
 *
 *  Created on: 2010-12-25
 *      Author: wqy
 */

#ifndef NOVA_BASE_HPP_
#define NOVA_BASE_HPP_
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stddef.h>
#include <new>

#include "common/config.h"
#include "common/types.h"
#include "common/runnable.hpp"
#include "common/address.hpp"
#include "common/object.hpp"
#include "common/debug.h"
#include "common/version.hpp"
#include "common/helper.hpp"
//#include "logging/logging.hpp"

#define NEW(Ptr,Type) do\
    { \
        try                  \
        {                    \
           Ptr = new Type;   \
        }                    \
        catch(std::bad_alloc)\
        {                    \
            Ptr = NULL;      \
        }                    \
    }while(0)

#define DELETE(Ptr)     do\
    {                   \
        if(NULL != Ptr) \
        {               \
            delete Ptr; \
            Ptr = NULL; \
        }               \
    }while(0)

#define DELETE_A(Ptr) do\
    {                   \
        if(NULL != Ptr) \
        {               \
            delete[] Ptr; \
            Ptr = NULL; \
        }               \
    }while(0)

#define DELETE_R(Ref)     do\
    {                   \
        delete &Ref ;   \
    }while(0)

#define RETURN_FALSE_IF_NULL(x)  do\
    {                    \
        if(NULL == x)    \
           return false; \
    }while(0)

#define RETURN_NULL_IF_NULL(x)  do\
    {                    \
        if(NULL == x)    \
           return NULL; \
    }while(0)

#define RETURN_VALUE_IF_NOTNULL(x) do\
    {                    \
        if(NULL != x)    \
           return x; \
    }while(0)

#define RETURN_IF_NULL(x)  do\
    {                    \
        if(NULL == x)    \
           return; \
    }while(0)


using arch::common::Object;
using arch::common::Address;
using arch::common::Runnable;
using arch::common::RunnableDestructor;
using arch::common::make_fun_runnable;
using arch::common::InstanceOf;
using arch::common::Type;
using arch::common::StandardDestructor;


#endif /* BASE_HPP_ */
