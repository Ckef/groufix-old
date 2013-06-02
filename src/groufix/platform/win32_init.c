/**
 * Groufix  :  Graphics Engine produced by Ckef Worx
 * www      :  http://www.ejb.ckef-worx.com
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "groufix/platform.h"

#include <windows.h>
#include <stdlib.h>

//******************************************************/
GFX_Win32_Instance* _gfx_instance = NULL;

//******************************************************/
static BOOL CALLBACK _gfx_win32_monitor_proc(HMONITOR handle, HDC hdc, LPRECT rect, LPARAM data)
{
	/* Simply store the monitor handle */
	++_gfx_instance->numMonitors;
	_gfx_instance->monitors = (void**)realloc(_gfx_instance->monitors, sizeof(void*) * _gfx_instance->numMonitors);
	_gfx_instance->monitors[_gfx_instance->numMonitors - 1] = (void*)handle;

	return 1;
}

//******************************************************/
int _gfx_platform_init(void)
{
	if(!_gfx_instance)
	{
		/* Allocate */
		_gfx_instance = (GFX_Win32_Instance*)calloc(1, sizeof(GFX_Win32_Instance));

		/* Enumerate all monitors */
		if(!EnumDisplayMonitors(NULL, NULL, _gfx_win32_monitor_proc, 0))
		{
			_gfx_platform_terminate();
			return 0;
		}
	}
	return 1;
}

//******************************************************/
int _gfx_platform_is_initialized(void)
{
	return (size_t)_gfx_instance;
}

//******************************************************/
void _gfx_platform_terminate(void)
{
	if(_gfx_instance)
	{
		/* Deallocate instance */
		free(_gfx_instance->monitors);
		free(_gfx_instance);
		_gfx_instance = NULL;
	}
}
