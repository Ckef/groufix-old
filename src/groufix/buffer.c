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
#include "groufix/containers/vector.h"
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
	GFXVector      handles; /* Stores GLuint */
};

/** Internal Buffer Segment */
struct GFX_Internal_Buffer_Segment
{
	/* Super class */
	GFXBufferSegment segment;

	/* Hidden data */
	unsigned char  firstBuffer; /* First used multi buffer */
	size_t         current;     /* Current fence to be created */
	GFXVector      fences;      /* Stores GLsync */
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
	
	ext->DeleteBuffers(num, buffer->handles.begin);
	memset(buffer->handles.begin, 0, sizeof(GLuint) * num);
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

	return *(GLuint*)gfx_vector_at(&internal->handles, internal->current);
}

/******************************************************/
GFXBuffer* gfx_buffer_create(GFXBufferUsage usage, GFXBufferTarget target, size_t size, const void* data, unsigned char multi)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return NULL;

	/* Create new buffer */
	struct GFX_Internal_Buffer* buffer = calloc(1, sizeof(struct GFX_Internal_Buffer));
	if(!buffer) return NULL;

	/* Force stream when multi buffering */
	usage |= multi ? GFX_BUFFER_STREAM : 0;

	buffer->buffer.size = size;
	buffer->buffer.usage = usage;
	buffer->buffer.target = target;
	buffer->buffer.multi = multi;

	/* Increment, as we need a front buffer too! */
	gfx_vector_init_from_buffer(&buffer->handles, sizeof(GLuint), ++multi, NULL);
	if(buffer->handles.begin == buffer->handles.end)
	{
		free(buffer);
		return NULL;
	}
	window->extensions.GenBuffers(multi, buffer->handles.begin);

	/* Allocate buffers */
	GLenum us = _gfx_buffer_get_usage(usage);
	while(multi > 0)
	{
		window->extensions.BindBuffer(target, *(GLuint*)gfx_vector_at(&buffer->handles, --multi));
		window->extensions.BufferData(target, size, multi ? NULL : data, us);
	}

	/* Register as object */
	_gfx_hardware_object_register(buffer, &_gfx_buffer_obj_funcs);

	return (GFXBuffer*)buffer;
}

/******************************************************/
GFXBuffer* gfx_buffer_create_copy(GFXBuffer* src, GFXBufferUsage usage, GFXBufferTarget target)
{
	/* Map buffer to copy data and create */
	void* data = gfx_buffer_map(src, src->size, 0, GFX_ACCESS_READ);
	if(!data) return NULL;

	GFXBuffer* buffer = gfx_buffer_create(usage, target, src->size, data, src->multi);
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
		if(window) window->extensions.DeleteBuffers(buffer->multi + 1, internal->handles.begin);

		gfx_vector_clear(&internal->handles);
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
int gfx_buffer_expand(GFXBuffer* buffer, unsigned char num)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return 0;

	struct GFX_Internal_Buffer* internal = (struct GFX_Internal_Buffer*)buffer;

	/* Allocate new handles */
	GFXVectorIterator it = gfx_vector_insert_range(&internal->handles, num, NULL, internal->handles.end);
	if(it == internal->handles.end) return 0;

	window->extensions.GenBuffers(num, it);
	buffer->multi += num;

	/* Allocate buffers */
	GLenum us = _gfx_buffer_get_usage(buffer->usage);
	for(; it != internal->handles.end; it = gfx_vector_next(&internal->handles, it))
	{
		window->extensions.BindBuffer(buffer->target, *(GLuint*)it);
		window->extensions.BufferData(buffer->target, buffer->size, NULL, us);
	}

	return 1;
}

/******************************************************/
int gfx_buffer_shrink(GFXBuffer* buffer, unsigned char num)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return 0;

	struct GFX_Internal_Buffer* internal = (struct GFX_Internal_Buffer*)buffer;

	/* Get where to remove buffers */
	unsigned char rem = num > buffer->multi ? buffer->multi : num;
	unsigned char aft = buffer->multi - internal->current;

	aft = aft > rem ? rem : aft;
	unsigned char bef = rem - aft;

	if(aft)
	{
		/* Erase handles after current */
		GFXVectorIterator it = gfx_vector_at(&internal->handles, internal->current + 1);

		window->extensions.DeleteBuffers(aft, it);
		gfx_vector_erase_range(&internal->handles, aft, it);
	}
	if(bef)
	{
		/* Erase handles at beginning */
		window->extensions.DeleteBuffers(bef, internal->handles.begin);
		gfx_vector_erase_range(&internal->handles, bef, internal->handles.begin);
	}

	/* Adjust values */
	buffer->multi -= rem;
	internal->current -= bef;

	return rem;
}

/******************************************************/
void gfx_buffer_write(GFXBuffer* buffer, size_t size, const void* data, size_t offset)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return;

	struct GFX_Internal_Buffer* internal = (struct GFX_Internal_Buffer*)buffer;

	window->extensions.BindBuffer(GL_ARRAY_BUFFER, *(GLuint*)gfx_vector_at(&internal->handles, internal->current));
	window->extensions.BufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

/******************************************************/
void gfx_buffer_read(GFXBuffer* buffer, size_t size, void* data, size_t offset)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return;

	struct GFX_Internal_Buffer* internal = (struct GFX_Internal_Buffer*)buffer;

	window->extensions.BindBuffer(GL_ARRAY_BUFFER, *(GLuint*)gfx_vector_at(&internal->handles, internal->current));
	window->extensions.GetBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

/******************************************************/
void* gfx_buffer_map(GFXBuffer* buffer, size_t size, size_t offset, GFXBufferAccess access)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return NULL;

	struct GFX_Internal_Buffer* internal = (struct GFX_Internal_Buffer*)buffer;

	window->extensions.BindBuffer(GL_ARRAY_BUFFER, *(GLuint*)gfx_vector_at(&internal->handles, internal->current));

	return window->extensions.MapBufferRange(GL_ARRAY_BUFFER, offset, size, access);
}

/******************************************************/
void gfx_buffer_unmap(GFXBuffer* buffer)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return;

	struct GFX_Internal_Buffer* internal = (struct GFX_Internal_Buffer*)buffer;

	window->extensions.BindBuffer(GL_ARRAY_BUFFER, *(GLuint*)gfx_vector_at(&internal->handles, internal->current));

	if(!window->extensions.UnmapBuffer(GL_ARRAY_BUFFER)) gfx_errors_push(
		GFX_ERROR_MEMORY_CORRUPTION,
		"Mapping a buffer might have corrupted its memory."
	);
}

/******************************************************/
GFXBufferSegment* gfx_buffer_segment_create(GFXBuffer* buffer, size_t size)
{
	/* Allocate segment */
	struct GFX_Internal_Buffer_Segment* segment = calloc(1, sizeof(struct GFX_Internal_Buffer_Segment));
	if(!segment) return NULL;

	gfx_vector_init(&segment->fences, sizeof(GLsync));

	segment->firstBuffer = ((struct GFX_Internal_Buffer*)buffer)->current;
	segment->segment.buffer = buffer;
	segment->segment.size = size > buffer->size ? buffer->size : size;

	return (GFXBufferSegment*)segment;
}

/******************************************************/
void gfx_buffer_segment_free(GFXBufferSegment* segment)
{
	if(segment)
	{
		struct GFX_Internal_Buffer_Segment* internal = (struct GFX_Internal_Buffer_Segment*)segment;

		gfx_vector_clear(&internal->fences);
		free(segment);
	}
}

/******************************************************/
int gfx_buffer_segment_swap(GFXBufferSegment* segment)
{
	struct GFX_Internal_Buffer_Segment* internal = (struct GFX_Internal_Buffer_Segment*)segment;
	struct GFX_Internal_Buffer* buffer = (struct GFX_Internal_Buffer*)segment->buffer;

	/* Advance to next block */
	size_t curr = internal->current;
	size_t off = segment->offset + segment->size;
	++internal->current;

	if(off + segment->size > segment->buffer->size)
	{
		/* Swap buffer if out of bounds */
		segment->offset = 0;
		gfx_buffer_swap(segment->buffer);

		if(buffer->current == internal->firstBuffer) internal->current = 0;
	}
	else segment->offset = off;

	/* If either is not zero, a new segment was selected */
	return curr | internal->current;
}
