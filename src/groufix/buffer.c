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

#include "groufix/buffer.h"
#include "groufix/internal.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
/** Internal Buffer */
struct GFX_Internal_Buffer
{
	/* Super class */
	GFXBuffer buffer;

	/* Hidden data */
	GLuint*  handles;
	GLenum   usage; /* Applies to all handles */
};

/******************************************************/
static void _gfx_buffer_obj_free(GFX_Hardware_Object object, const GFX_Extensions* ext)
{
	struct GFX_Internal_Buffer* buffer = (struct GFX_Internal_Buffer*)object;

	unsigned char num = buffer->buffer.backbuffers + 1;

	ext->DeleteBuffers(num, buffer->handles);
	memset(buffer->handles, 0, sizeof(GLuint) * num);
}

/******************************************************/
/* vtable for hardware part of the layout */
static GFX_Hardware_Funcs _gfx_buffer_obj_funcs =
{
	_gfx_buffer_obj_free,
	NULL,
	NULL
};

/******************************************************/
GFXBuffer* gfx_buffer_create(size_t size, unsigned char back)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return NULL;

	/* Create new buffer */
	struct GFX_Internal_Buffer* buffer = malloc(sizeof(struct GFX_Internal_Buffer));
	if(!buffer) return NULL;

	buffer->buffer.size = size;
	buffer->buffer.backbuffers = back;

	/* Increment, as we need a front buffer too! */
	buffer->handles = malloc(sizeof(GLuint) * ++back);
	if(!buffer->handles)
	{
		free(buffer);
		return NULL;
	}
	window->extensions.GenBuffers(back, buffer->handles);

	/* Register as object */
	_gfx_hardware_object_register(buffer, &_gfx_buffer_obj_funcs);

	return (GFXBuffer*)buffer;
}

/******************************************************/
void gfx_buffer_free(GFXBuffer* buffer)
{
	if(buffer)
	{
		struct GFX_Internal_Buffer* internal = (struct GFX_Internal_Buffer*)buffer;

		/* Get current window and context */
		GFX_Internal_Window* window = _gfx_window_get_current();
		if(window) window->extensions.DeleteBuffers(buffer->backbuffers + 1, internal->handles);

		free(internal->handles);
		free(buffer);

		/* Unregister as object */
		_gfx_hardware_object_unregister(buffer);
	}
}
