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

#include "groufix/internal.h"

/******************************************************/
static void _gfx_hardware_buffer_obj_free(GFX_Hardware_Object object, const GFX_Extensions* ext)
{
	GFX_Hardware_Buffer buffer = VOID_TO_UINT(object);

	ext->DeleteBuffers(1, &buffer);
}

/******************************************************/
/* vtable for hardware buffer object */
static GFX_Hardware_Funcs _gfx_hardware_buffer_obj_funcs =
{
	_gfx_hardware_buffer_obj_free,
	NULL,
	NULL
};

/******************************************************/
void _gfx_hardware_buffers_init(size_t num, GFX_Hardware_Buffer* buffers, GLenum target, GLenum usage, size_t size, const void* data, const GFX_Extensions* ext)
{
	ext->GenBuffers(num, buffers);

	/* Init all buffers */
	size_t i;
	for(i = 0; i < num; ++i)
	{
		ext->BindBuffer(target, buffers[i]);
		ext->BufferData(target, size, data, usage);

		/* Register as object */
		_gfx_hardware_object_register(UINT_TO_VOID(buffers[i]), &_gfx_hardware_buffer_obj_funcs);
	}
}

/******************************************************/
void _gfx_hardware_buffers_clear(size_t num, GFX_Hardware_Buffer* buffers, const GFX_Extensions* ext)
{
	ext->DeleteBuffers(num, buffers);

	/* Unregister all as object */
	size_t i;
	for(i = 0; i < num; ++i) _gfx_hardware_object_unregister(UINT_TO_VOID(buffers[i]));
}
