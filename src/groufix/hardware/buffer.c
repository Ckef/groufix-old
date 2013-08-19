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

#include <stdlib.h>

/******************************************************/
static void _gfx_hardware_buffer_obj_free(GFX_Hardware_Object object, const GFX_Extensions* ext)
{
	_gfx_hardware_buffer_clear((GFX_Hardware_Buffer*)object, ext);
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
GFX_Hardware_Buffer* _gfx_hardware_buffer_create(GLenum target, GLenum usage, size_t size, const void* data, const GFX_Extensions* ext)
{
	GFX_Hardware_Buffer* buff = malloc(sizeof(GFX_Hardware_Buffer));
	if(!buff) return NULL;

	_gfx_hardware_buffer_init(buff, target, usage, size, data, ext);

	return buff;
}

/******************************************************/
void _gfx_hardware_buffer_free(GFX_Hardware_Buffer* buffer, const GFX_Extensions* ext)
{
	if(buffer)
	{
		_gfx_hardware_buffer_clear(buffer, ext);
		free(buffer);
	}
}

/******************************************************/
void _gfx_hardware_buffer_init(GFX_Hardware_Buffer* buffer, GLenum target, GLenum usage, size_t size, const void* data, const GFX_Extensions* ext)
{
	ext->GenBuffers(1, &buffer->handle);
	ext->BindBuffer(target, buffer->handle);
	ext->BufferData(target, size, data, usage);

	buffer->usage = usage;
	buffer->size = size;

	/* Register as object */
	_gfx_hardware_object_register(buffer, &_gfx_hardware_buffer_obj_funcs);
}

/******************************************************/
void _gfx_hardware_buffer_clear(GFX_Hardware_Buffer* buffer, const GFX_Extensions* ext)
{
	ext->DeleteBuffers(1, &buffer->handle);

	buffer->handle = 0;
	buffer->size = 0;

	/* Unregister as object */
	_gfx_hardware_object_unregister(buffer);
}
