/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
 *
 * Groufix is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Groufix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Groufix.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "groufix/core/platform/win32.h"

#include <process.h>
#include <stdlib.h>

/******************************************************/
/* Internal thread arguments */
struct GFX_ThreadArgs
{
	GFX_ThreadAddress  addr;
	void*              args;
};

/******************************************************/
static unsigned int __stdcall _gfx_win32_thread_addr(

		void* arg)
{
	struct GFX_ThreadArgs data = *(struct GFX_ThreadArgs*)arg;
	free(arg);

	return data.addr(data.args);
}

/******************************************************/
int _gfx_platform_thread_init(

		GFX_PlatformThread*  thread,
		GFX_ThreadAddress    func,
		void*                arg,
		int                  joinable)
{
	/* Create arguments */
	struct GFX_ThreadArgs* data = malloc(sizeof(struct GFX_ThreadArgs));
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
void _gfx_platform_thread_detach(

		GFX_PlatformThread handle)
{
	CloseHandle(handle);
}

/******************************************************/
int _gfx_platform_thread_join(

		GFX_PlatformThread  handle,
		unsigned int*       ret)
{
	if(WaitForSingleObject(handle, INFINITE) == WAIT_FAILED)
		return 0;

	if(ret)
	{
		DWORD val = 0;
		GetExitCodeThread(handle, &val);

		*ret = val;
	}

	CloseHandle(handle);

	return 1;
}

/******************************************************/
void _gfx_platform_thread_exit(

		unsigned int ret)
{
	_endthreadex(ret);
}

/******************************************************/
int _gfx_platform_mutex_init(

		GFX_PlatformMutex* mutex)
{
	InitializeCriticalSection(mutex);

	return 1;
}

/******************************************************/
void _gfx_platform_mutex_clear(

		GFX_PlatformMutex* mutex)
{
	DeleteCriticalSection(mutex);
}

/******************************************************/
int _gfx_platform_mutex_lock(

		GFX_PlatformMutex* mutex)
{
	EnterCriticalSection(mutex);

	return 1;
}

/******************************************************/
int _gfx_platform_mutex_try_lock(

		GFX_PlatformMutex* mutex)
{
	return TryEnterCriticalSection(mutex);
}

/******************************************************/
void _gfx_platform_mutex_unlock(

		GFX_PlatformMutex* mutex)
{
	LeaveCriticalSection(mutex);
}

/******************************************************/
int _gfx_platform_cond_init(

		GFX_PlatformCond* cond)
{
	InitializeConditionVariable(cond);

	return 1;
}

/******************************************************/
void _gfx_platform_cond_clear(

		GFX_PlatformCond* cond)
{
	/* No-op on windows */
}

/******************************************************/
int _gfx_platform_cond_wait(

		GFX_PlatformCond*   cond,
		GFX_PlatformMutex*  mutex)
{
	return SleepConditionVariableCS(cond, mutex, INFINITE);
}

/******************************************************/
int _gfx_platform_cond_wait_time(

		GFX_PlatformCond*   cond,
		GFX_PlatformMutex*  mutex,
		uint64_t            nsec)
{
	/* Round up so nsec is a minimum */
	DWORD time = nsec ? (nsec -1) / 1000000 + 1 : 0;

	if(!SleepConditionVariableCS(cond, mutex, time))
		return (GetLastError() == ERROR_TIMEOUT) ? -1 : 0;

	return 1;
}

/******************************************************/
void _gfx_platform_cond_signal(

		GFX_PlatformCond* cond)
{
	WakeConditionVariable(cond);
}

/******************************************************/
void _gfx_platform_cond_broadcast(

		GFX_PlatformCond* cond)
{
	WakeAllConditionVariable(cond);
}
