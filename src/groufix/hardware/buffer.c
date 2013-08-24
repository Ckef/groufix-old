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
GFX_Hardware_Buffer _gfx_hardware_buffer_create(GLenum target, GLenum usage, size_t size, const void* data, const GFX_Extensions* ext)
{
	GLuint buffer;
	ext->GenBuffers(1, &buffer);
	ext->BindBuffer(target, buffer);
	ext->BufferData(target, size, data, usage);

	/* Register as object */
	_gfx_hardware_object_register(UINT_TO_VOID(buffer), &_gfx_hardware_buffer_obj_funcs);

	return buffer;
}

/******************************************************/
void _gfx_hardware_buffer_free(GFX_Hardware_Buffer buffer, const GFX_Extensions* ext)
{
	_gfx_hardware_buffer_obj_free(UINT_TO_VOID(buffer), ext);

	/* Unregister as object */
	_gfx_hardware_object_unregister(UINT_TO_VOID(buffer));
}
