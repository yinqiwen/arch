/*
 * perftools_helper.c
 *
 *  Created on: 2011-5-22
 *      Author: wqy
 */
#include "common/config.h"
//#include "util/perftools_helper.h"
#include "util/proctitle_helper.h"
#if HAVE_GOOGLE_PROFILER == 1
#include <google/profiler.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void adjust_google_profiler()
{
#if HAVE_GOOGLE_PROFILER == 1
	if (getenv("CPUPROFILE"))
	{
		/* disable inherited Profiler enabled in master process */
		ProfilerStop();
	}
	else
	{
		return;
	}
//	struct ProfilerState state;
//	ProfilerGetCurrentState(&state);
	char fname[256];
	sprintf(fname, "%s.cpu.%u.prof", "GooglePerfTool", getpid());
	//sprintf(fname, "%s.%u", getenv("CPUPROFILE"), getpid());
	if (ProfilerStart(fname))
	{
		ProfilerRegisterThread();
	}
	else
	{
		//
	}
#endif
}
