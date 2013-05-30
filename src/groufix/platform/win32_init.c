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
int _gfx_platform_init(void)
{
	if(!_gfx_instance)
	{
		/* Allocate */
		_gfx_instance = (GFX_Win32_Instance*)calloc(1, sizeof(GFX_Win32_Instance));

		/* Get number of display devices */
		DISPLAY_DEVICE display;
		DWORD i = 0;
		while(1)
		{
			ZeroMemory(&display, sizeof(DISPLAY_DEVICE));
			display.cb = sizeof(DISPLAY_DEVICE);

			/* Validate adapter and increase */
			if(!EnumDisplayDevices(NULL, i++, &display, 0)) break;
			if(display.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) continue;
			++_gfx_instance->numDevices;

			/* Create the device context pointer */
			EnumDisplayDevices(display.DeviceName, 0, &display, 0);
			_gfx_instance->deviceContexts = (void**)realloc(
				_gfx_instance->deviceContexts,
				sizeof(void*) * _gfx_instance->numDevices);

			/* Create a context for the adapter/display */
			HDC context = CreateDC(L"DISPLAY", display.DeviceString, NULL, NULL);
			_gfx_instance->deviceContexts[_gfx_instance->numDevices - 1] = (void*)context;
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
		/* Destroy all device contexts */
		unsigned int i;
		for(i = 0; i < _gfx_instance->numDevices; ++i)
			DeleteDC((HDC)_gfx_instance->deviceContexts[i]);

		/* Deallocate instance */
		free(_gfx_instance->deviceContexts);
		free(_gfx_instance);
		_gfx_instance = NULL;
	}
}
