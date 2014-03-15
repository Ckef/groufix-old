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

/******************************************************/
int _gfx_platform_thread_create(GFX_PlatformThread* thread, GFX_ThreadAddress func, void* arg, int joinable)
{
	return 0;
}

/******************************************************/
void _gfx_platform_thread_exit(void* ret)
{
}

/******************************************************/
int _gfx_platform_thread_join(GFX_PlatformThread handle, void** ret)
{
	return 0;
}
