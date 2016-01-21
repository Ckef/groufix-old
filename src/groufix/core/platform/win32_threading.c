/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#include "groufix/core/platform/win32.h"
#include "groufix/core/threading.h"

#include <limits.h>
#include <stdlib.h>

#define GFX_WIN32_MASK_MSB ((ULONG_PTR)1 << (sizeof(ULONG_PTR) * CHAR_BIT - 1))

/******************************************************/
/* Internal thread arguments */
typedef struct GFX_ThreadArgs
{
	GFX_ThreadAddress  addr;
	void*              args;

} GFX_ThreadArgs;


/******************************************************/
static unsigned int __stdcall _gfx_win32_thread_addr(

		void* arg)
{
	GFX_ThreadArgs data = *(GFX_ThreadArgs*)arg;
	free(arg);

	return data.addr(data.args);
}

/******************************************************/
unsigned long _gfx_platform_get_num_cores(void)
{
	/* Get buffer of data */
	DWORD size = 0;
	GetLogicalProcessorInformation(NULL, &size);

	void* buff = malloc(size);
	GetLogicalProcessorInformation(buff, &size);

	/* Count logical cores */
	unsigned long cnt = 0;

	while(size > 0)
	{
		size -=
			sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
		SYSTEM_LOGICAL_PROCESSOR_INFORMATION* info =
			GFX_PTR_ADD_BYTES(buff, size);

		if(info->Relationship == RelationProcessorCore)
		{
			ULONG_PTR mask;
			for(mask = GFX_WIN32_MASK_MSB; mask > 0; mask >>= 1)
				cnt += info->ProcessorMask & mask ? 1 : 0;
		}
	}

	free(buff);

	return cnt;
}

/******************************************************/
int _gfx_platform_thread_init(

		GFX_PlatformThread*  thread,
		GFX_ThreadAddress    func,
		void*                arg,
		int                  joinable)
{
	/* Create arguments */
	GFX_ThreadArgs* data = malloc(sizeof(GFX_ThreadArgs));
	if(!data) return 0;

	data->addr = func;
	data->args = arg;

	/* Create thread */
	*thread = (HANDLE)_beginthreadex(
		NULL, 0,
		_gfx_win32_thread_addr,
		data, 0,
		NULL
	);

	if(!*thread)
	{
		free(data);
		return 0;
	}

	if(!joinable) CloseHandle(*thread);

	return 1;
}

/******************************************************/
int _gfx_platform_thread_join(

		GFX_PlatformThread  thread,
		unsigned int*       ret)
{
	if(WaitForSingleObject(thread, INFINITE) == WAIT_FAILED)
		return 0;

	if(ret)
	{
		DWORD val = 0;
		GetExitCodeThread(thread, &val);

		*ret = val;
	}

	CloseHandle(thread);

	return 1;
}

/******************************************************/
int _gfx_platform_cond_wait_time(

		GFX_PlatformCond*   cond,
		GFX_PlatformMutex*  mutex,
		uint64_t            nsec)
{
	/* Round up so nsec is a minimum */
	DWORD time = nsec ? (nsec - 1) / 1000000 + 1 : 0;

	if(!SleepConditionVariableCS(cond, mutex, time))
		return (GetLastError() == ERROR_TIMEOUT) ? -1 : 0;

	return 1;
}
