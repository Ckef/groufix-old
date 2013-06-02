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

//******************************************************/
void* _gfx_platform_get_screen(int num)
{
	if(!_gfx_instance) return NULL;

	/* Validate the number first */
	if(num < 0 || num >= _gfx_instance->numMonitors) return NULL;
	return _gfx_instance->monitors[num];
}

//******************************************************/
void* _gfx_platform_get_default_screen()
{
	POINT zero = { 0,0 };
	return (void*)MonitorFromPoint(zero, MONITOR_DEFAULTTOPRIMARY);
}

//******************************************************/
int _gfx_platform_get_num_screens(void)
{
	if(!_gfx_instance) return 0;
	return _gfx_instance->numMonitors;
}

//******************************************************/
int _gfx_platform_screen_get_width(void* handle)
{
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);

	if(!GetMonitorInfo((HMONITOR)handle, &info)) return 0;
	return info.rcMonitor.right - info.rcMonitor.left;
}

//******************************************************/
int _gfx_platform_screen_get_height(void* handle)
{
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);

	if(!GetMonitorInfo((HMONITOR)handle, &info)) return 0;
	return info.rcMonitor.bottom - info.rcMonitor.top;
}
