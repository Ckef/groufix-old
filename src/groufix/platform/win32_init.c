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
GFX_Win32_Instance* _gfx_instance = NULL;

/*/*****************************************************/
int _gfx_platform_init(void)
{
	if(!_gfx_instance)
	{
		/* Get module handle */
		HINSTANCE hInst = GetModuleHandle(NULL);
		if(!hInst) return 0;

		/* Allocate */
		_gfx_instance = (GFX_Win32_Instance*)calloc(1, sizeof(GFX_Win32_Instance));
		_gfx_instance->handle = (void*)hInst;
	}
	return 1;
}

/*/*****************************************************/
int _gfx_platform_is_initialized(void)
{
	return (size_t)_gfx_instance;
}

/*/*****************************************************/
void _gfx_platform_terminate(void)
{
	if(_gfx_instance)
	{
		/* Deallocate server */
		free(_gfx_instance);
		_gfx_instance = NULL;
	}
}
