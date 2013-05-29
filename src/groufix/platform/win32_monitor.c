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

/*/*****************************************************/
void* _gfx_platform_get_monitor(int num)
{
	return NULL;
}

/*/*****************************************************/
int _gfx_platform_get_num_monitors(void)
{
	if(!_gfx_server) return 0;
	return _gfx_server->monitors;
}

/*/*****************************************************/
int _gfx_platform_monitor_get_width(void* handle)
{
	return 0;
}

/*/*****************************************************/
int _gfx_platform_monitor_get_height(void* handle)
{
	return 0;
}
