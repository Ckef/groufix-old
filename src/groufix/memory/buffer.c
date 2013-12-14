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

#include "groufix/containers/vector.h"
#include "groufix/internal.h"
#include "groufix/errors.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
/* Internal Buffer */
struct GFX_Internal_Buffer
{
	/* Super class */
	GFXBuffer buffer;

	/* Hidden data */
	unsigned char  current;     /* Current active buffer */
	unsigned char  currentSeg;  /* Current active segment */
	GFXVector      handles;     /* Stores GLuint + (GLsync * numSegments) */
};

/******************************************************/
static inline GLenum _gfx_buffer_get_usage(GFXBufferUsage usage)
{
	/* Return appropriate OpenGL usage hint */
	return usage & GFX_BUFFER_STREAM ?

		(usage & GFX_BUFFER_WRITE) ? GL_STREAM_DRAW :
		(usage & GFX_BUFFER_READ) ? GL_STREAM_READ : GL_STREAM_COPY :

		(usage & GFX_BUFFER_WRITE) ? GL_STATIC_DRAW :
		(usage & GFX_BUFFER_READ) ? GL_STATIC_READ : GL_STATIC_COPY;
}

/******************************************************/
static inline void _gfx_buffer_sync(GLsync sync, const GFX_Extensions* ext)
{
	/* Force a synchronization */
	GLenum status = GL_TIMEOUT_EXPIRED;
	while(status == GL_TIMEOUT_EXPIRED) status = ext->ClientWaitSync(sync, 0, 0);
}

/******************************************************/
/* Create buffers in existing vector */
static void _gfx_buffer_alloc_buffers(struct GFX_Internal_Buffer* buffer, GFXVectorIterator it, unsigned char num, const void* data, const GFX_Extensions* ext)
{	
	/* Allocate buffers */
	GLuint handles[num];
	ext->GenBuffers(num, handles);

	GLenum us = _gfx_buffer_get_usage(buffer->buffer.usage);
	memset(it, 0, buffer->handles.elementSize * num);

	/* Iterate over buffers */
	unsigned char i;
	for(i = 0; i < num; ++i)
	{
		/* Only write data to the first buffer */
		*(GLuint*)it = handles[i];
		ext->BindBuffer(buffer->buffer.target, *(GLuint*)it);
		ext->BufferData(buffer->buffer.target, buffer->buffer.size, i ? NULL : data, us);

		it = gfx_vector_next(&buffer->handles, it);
	}
}

/******************************************************/
/* Entirely delete buffers, but don't remove vector entry */
static void _gfx_buffer_delete_buffers(struct GFX_Internal_Buffer* buffer, GFXVectorIterator it, unsigned char num, const GFX_Extensions* ext)
{
	GLuint handles[num];
	unsigned char segs = buffer->buffer.size / buffer->buffer.segSize;

	/* Iterate over buffers */
	unsigned char i, s;
	for(i = 0; i < num; ++i)
	{
		/* Remove sync objects */
		GLsync* sync = (GLsync*)(((GLuint*)it) + 1);
		for(s = 0; s < segs; ++s) ext->DeleteSync(*(sync++));

		/* Copy handle and advance */
		handles[i] = *(GLuint*)it;
		it = gfx_vector_next(&buffer->handles, it);
	}

	/* And deallocate all buffers */
	ext->DeleteBuffers(num, handles);
}

/******************************************************/
static void _gfx_buffer_obj_free(void* object, GFX_Extensions* ext)
{
	struct GFX_Internal_Buffer* buffer = (struct GFX_Internal_Buffer*)object;
	
	unsigned char num = buffer->buffer.multi + 1;

	/* Delete buffers and reset memory */
	_gfx_buffer_delete_buffers(buffer, buffer->handles.begin, num, ext);
	memset(buffer->handles.begin, 0, buffer->handles.elementSize * num);

	buffer->buffer.id = 0;
}

/******************************************************/
/* vtable for hardware part of the buffer */
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
GFXBuffer* gfx_buffer_create(GFXBufferUsage usage, GFXBufferTarget target, size_t size, const void* data, unsigned char multi, unsigned char segments)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return NULL;

	/* Create new buffer */
	struct GFX_Internal_Buffer* buffer = calloc(1, sizeof(struct GFX_Internal_Buffer));
	if(!buffer) return NULL;

	/* Register as object */
	buffer->buffer.id = _gfx_hardware_object_register(buffer, &_gfx_buffer_obj_funcs);
	if(!buffer->buffer.id)
	{
		free(buffer);
		return NULL;
	}

	/* Force stream when multi buffering and/or segmenting */
	usage |= (multi | segments) ? GFX_BUFFER_STREAM : 0;

	/* Calculate segment size, force at least one segment */
	buffer->buffer.segSize = size / ++segments;
	if(!buffer->buffer.segSize)
	{
		buffer->buffer.segSize = size;
		segments = 1;
	}

	buffer->buffer.size   = size;
	buffer->buffer.usage  = usage;
	buffer->buffer.target = target;
	buffer->buffer.multi  = multi;

	/* Increment multi, as we need a front buffer too! */
	gfx_vector_init_from_buffer(&buffer->handles, sizeof(GLuint) + sizeof(GLsync) * segments, ++multi, NULL);
	if(buffer->handles.begin == buffer->handles.end)
	{
		free(buffer);
		return NULL;
	}
	_gfx_buffer_alloc_buffers(buffer, buffer->handles.begin, multi, data, &window->extensions);

	return (GFXBuffer*)buffer;
}

/******************************************************/
GFXBuffer* gfx_buffer_create_copy(GFXBuffer* src, GFXBufferUsage usage, GFXBufferTarget target)
{
	/* Map buffer to copy data and create */
	void* data = gfx_buffer_map(src, src->size, 0, GFX_BUFFER_READ);
	if(!data) return NULL;

	GFXBuffer* buffer = gfx_buffer_create(usage, target, src->size, data, src->multi, src->size / src->segSize);
	gfx_buffer_unmap(src);

	return buffer;
}

/******************************************************/
void gfx_buffer_free(GFXBuffer* buffer)
{
	if(buffer)
	{
		struct GFX_Internal_Buffer* internal = (struct GFX_Internal_Buffer*)buffer;

		/* Unregister as object */
		_gfx_hardware_object_unregister(buffer->id);

		/* Get current window and context */
		GFX_Internal_Window* window = _gfx_window_get_current();
		if(window) _gfx_buffer_delete_buffers(internal, internal->handles.begin, buffer->multi + 1, &window->extensions);

		gfx_vector_clear(&internal->handles);
		free(buffer);
	}
}

/******************************************************/
int gfx_buffer_expand(GFXBuffer* buffer, unsigned char num)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window || !num) return 0;

	struct GFX_Internal_Buffer* internal = (struct GFX_Internal_Buffer*)buffer;

	/* Allocate new handles */
	GFXVectorIterator it = gfx_vector_insert_range(&internal->handles, num, NULL, internal->handles.end);
	if(it == internal->handles.end) return 0;

	/* Allocate buffers */
	_gfx_buffer_alloc_buffers(internal, it, num, NULL, &window->extensions);
	buffer->multi += num;

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

		_gfx_buffer_delete_buffers(internal, it, aft, &window->extensions);
		gfx_vector_erase_range(&internal->handles, aft, it);
	}
	if(bef)
	{
		/* Erase handles at beginning */
		_gfx_buffer_delete_buffers(internal, internal->handles.begin, bef, &window->extensions);
		gfx_vector_erase_range(&internal->handles, bef, internal->handles.begin);
	}

	/* Adjust values */
	buffer->multi -= rem;
	internal->current -= bef;

	return rem;
}

/******************************************************/
int gfx_buffer_swap(GFXBuffer* buffer)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return 0;

	struct GFX_Internal_Buffer* internal = (struct GFX_Internal_Buffer*)buffer;

	/* Create a sync object */
	GFXVectorIterator it = gfx_vector_at(&internal->handles, internal->current);
	GLsync* sync = ((GLsync*)(((GLuint*)it) + 1)) + internal->currentSeg;

	window->extensions.DeleteSync(*sync);
	*sync = window->extensions.FenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

	/* Advance segment */
	++internal->currentSeg;
	size_t offset = internal->currentSeg * buffer->segSize;

	if(offset + buffer->segSize > buffer->size)
	{
		/* Swap buffer if out of bounds */
		internal->currentSeg = 0;

		++internal->current;
		internal->current = internal->current > buffer->multi ? 0 : internal->current;

		return buffer->multi;
	}
	return 1;
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
void* gfx_buffer_map(GFXBuffer* buffer, size_t size, size_t offset, GFXBufferUsage access)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return NULL;

	struct GFX_Internal_Buffer* internal = (struct GFX_Internal_Buffer*)buffer;

	/* Strip access bitfield */
	access &= GFX_BUFFER_READ | GFX_BUFFER_WRITE;
	access |= !(access & GFX_BUFFER_WRITE) ? GL_MAP_UNSYNCHRONIZED_BIT : 0;

	window->extensions.BindBuffer(GL_ARRAY_BUFFER, *(GLuint*)gfx_vector_at(&internal->handles, internal->current));

	return window->extensions.MapBufferRange(GL_ARRAY_BUFFER, offset, size, access);
}

/******************************************************/
void* gfx_buffer_map_segment(GFXBuffer* buffer, size_t size, size_t offset, GFXBufferUsage access)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return NULL;

	struct GFX_Internal_Buffer* internal = (struct GFX_Internal_Buffer*)buffer;

	/* Sync the client with the previous fence object */
	GFXVectorIterator it = gfx_vector_at(&internal->handles, internal->current);
	GLsync* sync = ((GLsync*)(((GLuint*)it) + 1)) + internal->currentSeg;

	if(*sync && (access & GFX_BUFFER_WRITE)) _gfx_buffer_sync(*sync, &window->extensions);

	/* Do the actual mapping */
	access &= GFX_BUFFER_READ | GFX_BUFFER_WRITE;
	window->extensions.BindBuffer(GL_ARRAY_BUFFER, *(GLuint*)it);

	return window->extensions.MapBufferRange(
		GL_ARRAY_BUFFER,
		internal->currentSeg * buffer->segSize + offset,
		size,
		access | GL_MAP_UNSYNCHRONIZED_BIT
	);
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
