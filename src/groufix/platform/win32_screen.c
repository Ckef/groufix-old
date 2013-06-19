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

#include "groufix/platform/win32.h"

/******************************************************/
unsigned int _gfx_platform_get_num_screens(void)
{
	if(!_gfx_win32) return 0;
	return _gfx_win32->numMonitors;
}

/******************************************************/
GFX_Platform_Screen _gfx_platform_get_screen(unsigned int num)
{
	if(!_gfx_win32) return NULL;

	/* Validate the number first */
	if(num >= _gfx_win32->numMonitors) return NULL;
	return _gfx_win32->monitors[num];
}

/******************************************************/
GFX_Platform_Screen _gfx_platform_get_default_screen(void)
{
	POINT zero = { 0,0 };
	return MonitorFromPoint(zero, MONITOR_DEFAULTTOPRIMARY);
}

/******************************************************/
void _gfx_platform_screen_get_size(GFX_Platform_Screen handle, unsigned int* width, unsigned int* height)
{
	MONITORINFO info;
	ZeroMemory(&info, sizeof(MONITORINFO));
	info.cbSize = sizeof(MONITORINFO);

	GetMonitorInfo(handle, &info);
	*width = info.rcMonitor.right - info.rcMonitor.left;
	*height = info.rcMonitor.bottom - info.rcMonitor.top;
}
