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

	if(num < 0 || num >= _gfx_instance->numDisplays) return NULL;

	/* Get display device */
	DISPLAY_DEVICE display;
	ZeroMemory(&display, sizeof(DISPLAY_DEVICE));
	display.cb = sizeof(DISPLAY_DEVICE);

	EnumDisplayDevices(NULL, _gfx_instance->displayNumbers[num], &display, 0);
	EnumDisplayDevices(display.DeviceName, 0, &display, 0);

	/* Allocate the device context */
	DeleteDC((HDC)_gfx_instance->lastContext);
	_gfx_instance->lastContext = (void*)CreateDC(L"DISPLAY", display.DeviceString, NULL, NULL);

	return _gfx_instance->lastContext;
}

//******************************************************/
void* _gfx_platform_get_default_screen()
{
	/* Don't save it as last context as it is not created by the application */
	return (void*)GetDC(NULL);
}

//******************************************************/
int _gfx_platform_get_num_screens(void)
{
	if(!_gfx_instance) return 0;
	return _gfx_instance->numDisplays;
}

//******************************************************/
int _gfx_platform_screen_get_width(void* handle)
{
	return GetDeviceCaps((HDC)handle, HORZRES);
}

//******************************************************/
int _gfx_platform_screen_get_height(void* handle)
{
	return GetDeviceCaps((HDC)handle, VERTRES);
}
