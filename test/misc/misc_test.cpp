/*
 * misc_test.cpp
 *
 *  Created on: 2011-8-7
 *      Author: wqy
 */
#include "util/proctitle_helper.h"
#include "common/base.hpp"
#include <stdlib.h>

int main(int argc, char** argv)
{
	ngx_init_setproctitle(argc, argv);
	ngx_setproctitle("what");
    char* kk = getenv("TENV");
    if(NULL != kk)
    {
    	printf("###%s##\n", kk);
    }
    return 1;
}
