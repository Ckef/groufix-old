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
#include "groufix/errors.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
/** Internal Buffer */
struct GFX_Internal_Buffer
{
	/* Super class */
	GFXBuffer buffer;

	/* Hidden data */
	unsigned char  current; /* Current active buffer */
	GLuint*        handles;
};

/******************************************************/
static GLenum _gfx_buffer_get_usage(GFXBufferUsage usage)
{
	/* Return appropriate OpenGL usage hint */
	return usage & GFX_BUFFER_STREAM ?

		(usage & GFX_BUFFER_WRITE) ? GL_STREAM_DRAW :
		(usage & GFX_BUFFER_READ) ? GL_STREAM_READ : GL_STREAM_COPY :

		(usage & GFX_BUFFER_WRITE) ? GL_STATIC_DRAW :
		(usage & GFX_BUFFER_READ) ? GL_STATIC_READ : GL_STATIC_COPY;
}

/******************************************************/
static void _gfx_buffer_obj_free(GFX_Hardware_Object object, const GFX_Extensions* ext)
{
	struct GFX_Internal_Buffer* buffer = (struct GFX_Internal_Buffer*)object;

	unsigned char num = buffer->buffer.multi + 1;

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
GLuint _gfx_buffer_get_handle(const GFXBuffer* buffer)
{
	struct GFX_Internal_Buffer* internal = (struct GFX_Internal_Buffer*)buffer;

	return internal->handles[internal->current];
}

/******************************************************/
GFXBuffer* gfx_buffer_create(GFXBufferUsage usage, GFXBufferTarget target, size_t size, const void* data, unsigned char multi)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return NULL;

	/* Create new buffer */
	struct GFX_Internal_Buffer* buffer = malloc(sizeof(struct GFX_Internal_Buffer));
	if(!buffer) return NULL;

	/* Force stream when multi buffering */
	usage |= multi ? GFX_BUFFER_STREAM : 0;

	buffer->buffer.size  = size;
	buffer->buffer.usage = usage;
	buffer->buffer.multi = multi;

	/* Increment, as we need a front buffer too! */
	buffer->handles = malloc(sizeof(GLuint) * ++multi);
	if(!buffer->handles)
	{
		free(buffer);
		return NULL;
	}
	window->extensions.GenBuffers(multi, buffer->handles);

	/* Allocate buffers */
	GLenum us = _gfx_buffer_get_usage(usage);
	while(multi > 0)
	{
		window->extensions.BindBuffer(target, buffer->handles[--multi]);
		window->extensions.BufferData(target, size, multi ? NULL : data, us);
	}

	/* Register as object */
	_gfx_hardware_object_register(buffer, &_gfx_buffer_obj_funcs);

	return (GFXBuffer*)buffer;
}

/******************************************************/
GFXBuffer* gfx_buffer_create_copy(GFXBuffer* src, GFXBufferTarget target)
{
	/* Map buffer to copy data and create */
	void* data = gfx_buffer_map(src, src->size, 0, GFX_ACCESS_READ);
	if(!data) return NULL;

	GFXBuffer* buffer = gfx_buffer_create(src->usage, target, src->size, data, src->multi);
	gfx_buffer_unmap(src);

	return buffer;
}

/******************************************************/
void gfx_buffer_free(GFXBuffer* buffer)
{
	if(buffer)
	{
		struct GFX_Internal_Buffer* internal = (struct GFX_Internal_Buffer*)buffer;

		/* Get current window and context */
		GFX_Internal_Window* window = _gfx_window_get_current();
		if(window) window->extensions.DeleteBuffers(buffer->multi + 1, internal->handles);

		free(internal->handles);
		free(buffer);

		/* Unregister as object */
		_gfx_hardware_object_unregister(buffer);
	}
}

/******************************************************/
int gfx_buffer_swap(GFXBuffer* buffer)
{
	struct GFX_Internal_Buffer* internal = (struct GFX_Internal_Buffer*)buffer;

	++internal->current;
	internal->current = internal->current > buffer->multi ? 0 : internal->current;

	return buffer->multi;
}

/******************************************************/
void gfx_buffer_write(GFXBuffer* buffer, size_t size, const void* data, size_t offset)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return;

	struct GFX_Internal_Buffer* internal = (struct GFX_Internal_Buffer*)buffer;

	window->extensions.BindBuffer(GL_ARRAY_BUFFER, internal->handles[internal->current]);
	window->extensions.BufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

/******************************************************/
void gfx_buffer_read(GFXBuffer* buffer, size_t size, void* data, size_t offset)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return;

	struct GFX_Internal_Buffer* internal = (struct GFX_Internal_Buffer*)buffer;

	window->extensions.BindBuffer(GL_ARRAY_BUFFER, internal->handles[internal->current]);
	window->extensions.GetBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

/******************************************************/
void* gfx_buffer_map(GFXBuffer* buffer, size_t size, size_t offset, GFXBufferAccess access)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return NULL;

	struct GFX_Internal_Buffer* internal = (struct GFX_Internal_Buffer*)buffer;

	window->extensions.BindBuffer(GL_ARRAY_BUFFER, internal->handles[internal->current]);

	return window->extensions.MapBufferRange(GL_ARRAY_BUFFER, offset, size, access);
}

/******************************************************/
void gfx_buffer_unmap(GFXBuffer* buffer)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return;

	struct GFX_Internal_Buffer* internal = (struct GFX_Internal_Buffer*)buffer;

	window->extensions.BindBuffer(GL_ARRAY_BUFFER, internal->handles[internal->current]);

	if(!window->extensions.UnmapBuffer(GL_ARRAY_BUFFER)) gfx_errors_push(
		GFX_ERROR_MEMORY_CORRUPTION,
		"Mapping a buffer might have corrupted its memory."
	);
}
