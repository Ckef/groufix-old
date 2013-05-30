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

/*/*****************************************************/
void* _gfx_platform_get_screen(int num)
{
	if(!_gfx_instance) return NULL;

	/* Allocate new display device */
	if(!_gfx_instance->lastDisplay) _gfx_instance->lastDisplay = malloc(sizeof(DISPLAY_DEVICE));
	DISPLAY_DEVICE* disp = (DISPLAY_DEVICE*)_gfx_instance->lastDisplay;

	/* Fetch its actual display device */
	ZeroMemory(disp, sizeof(DISPLAY_DEVICE));
	disp->cb = sizeof(DISPLAY_DEVICE);
	if(!EnumDisplayDevices(NULL, _gfx_instance->displayNumbers[num], disp, 0)) return NULL;
	EnumDisplayDevices(disp->DeviceName, 0, disp, 0);

	return (void*)disp;
}

/*/*****************************************************/
int _gfx_platform_get_num_screens(void)
{
	if(!_gfx_instance) return 0;
	return _gfx_instance->numDisplays;
}

/*/*****************************************************/
int _gfx_platform_screen_get_width(void* handle)
{
	/* Create Device Context */
	DISPLAY_DEVICE* disp = (DISPLAY_DEVICE*)handle;
	HDC dc = CreateDC(L"DISPLAY", disp->DeviceString, NULL, NULL);

	int width = GetDeviceCaps(dc, HORZRES);
	DeleteDC(dc);

	return width;
}

/*/*****************************************************/
int _gfx_platform_screen_get_height(void* handle)
{
	/* Create Device Context */
	DISPLAY_DEVICE* disp = (DISPLAY_DEVICE*)handle;
	HDC dc = CreateDC(L"DISPLAY", disp->DeviceString, NULL, NULL);

	int height = GetDeviceCaps(dc, VERTRES);
	DeleteDC(dc);

	return height;
}
