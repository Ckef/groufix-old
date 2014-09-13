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
#include "groufix/core/shading/internal.h"
#include "groufix/core/errors.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
/* Internal Buffer */
struct GFX_Buffer
{
	/* Super class */
	GFXBuffer buffer;

	/* Hidden data */
	unsigned char  current; /* Current active buffer */
	GFXVector      handles; /* Stores GLuint */
};

/******************************************************/
static int _gfx_buffer_eval_target(

		GFXBufferTarget target)
{
	switch(target)
	{
		/* GFX_EXT_BUFFER_TEXTURE */
		case GFX_TEXTURE_BUFFER :

			if(!(GFX_WND)->flags[GFX_EXT_BUFFER_TEXTURE])
			{
				gfx_errors_push(
					GFX_ERROR_INCOMPATIBLE_CONTEXT,
					"GFX_EXT_BUFFER_TEXTURE is incompatible with this context."
				);
				return 0;
			}
			return 1;

		/* Everything else */
		default : return 1;
	}
}

/******************************************************/
static inline GLenum _gfx_buffer_get_usage(

		GFXBufferUsage usage)
{
	return

		/* Dynamic */
		usage & GFX_BUFFER_DYNAMIC ?

		((usage & GFX_BUFFER_WRITE) ? GL_DYNAMIC_DRAW :
		((usage & GFX_BUFFER_READ) ? GL_DYNAMIC_READ : GL_DYNAMIC_COPY)) :

		/* Stream */
		usage & GFX_BUFFER_STREAM ?

		((usage & GFX_BUFFER_WRITE) ? GL_STREAM_DRAW :
		((usage & GFX_BUFFER_READ) ? GL_STREAM_READ : GL_STREAM_COPY)) :

		/* Static */
		((usage & GFX_BUFFER_WRITE) ? GL_STATIC_DRAW :
		((usage & GFX_BUFFER_READ) ? GL_STATIC_READ : GL_STATIC_COPY));
}

/******************************************************/
static void _gfx_buffer_alloc_buffers(

		struct GFX_Buffer*  buffer,
		GFXVectorIterator   it,
		unsigned char       num,
		const void*         data)
{
	/* Allocate buffers */
	(GFX_RND).CreateBuffers(num, it);
	GLenum us = _gfx_buffer_get_usage(buffer->buffer.usage);

	/* Iterate over buffers */
	unsigned char i;
	for(i = 0; i < num; ++i)
	{
		/* Bind just so to hint the buffer type */
		(GFX_RND).BindBuffer(
			buffer->buffer.target,
			*(GLuint*)it);

		/* Only write data to the first buffer */
		(GFX_RND).NamedBufferData(
			*(GLuint*)it,
			buffer->buffer.size,
			i ? NULL : data,
			us);

		it = gfx_vector_next(&buffer->handles, it);
	}
}

/******************************************************/
static void _gfx_buffer_delete_buffers(

		struct GFX_Buffer*  buffer,
		GFXVectorIterator   it,
		unsigned char       num)
{
	/* Iterate over buffers */
	unsigned char i;
	for(i = 0; i < num; ++i)
	{
		/* Make sure it is not currently bound */
		GLuint* handle = gfx_vector_advance(&buffer->handles, it, i);
		_gfx_binder_unbind_uniform_buffer(*handle);
	}

	/* And deallocate all buffers */
	(GFX_RND).DeleteBuffers(num, it);
}

/******************************************************/
static void _gfx_buffer_obj_free(

		void* object)
{
	struct GFX_Buffer* buffer = (struct GFX_Buffer*)object;
	unsigned char num = buffer->buffer.multi + 1;

	/* Reset memory */
	memset(
		buffer->handles.begin,
		0,
		buffer->handles.elementSize * num
	);
	buffer->buffer.id = 0;
}

/******************************************************/
/* vtable for hardware part of the buffer */
static GFX_HardwareFuncs _gfx_buffer_obj_funcs =
{
	_gfx_buffer_obj_free,
	NULL,
	NULL
};

/******************************************************/
GLuint _gfx_buffer_get_handle(

		const GFXBuffer* buffer)
{
	struct GFX_Buffer* internal = (struct GFX_Buffer*)buffer;
	return *(GLuint*)gfx_vector_at(&internal->handles, internal->current);
}

/******************************************************/
GFXBuffer* gfx_buffer_create(

		GFXBufferUsage   usage,
		GFXBufferTarget  target,
		size_t           size,
		const void*      data,
		unsigned char    multi)
{
	if(!GFX_WND) return NULL;

	/* Validate target */
	if(!_gfx_buffer_eval_target(target))
		return NULL;

	/* Create new buffer */
	struct GFX_Buffer* buffer = calloc(1, sizeof(struct GFX_Buffer));
	if(!buffer)
	{
		/* Out of memory error */
		gfx_errors_push(
			GFX_ERROR_OUT_OF_MEMORY,
			"Buffer could not be allocated."
		);
		return NULL;
	}

	/* Register as object */
	buffer->buffer.id = _gfx_hardware_object_register(
		buffer,
		&_gfx_buffer_obj_funcs
	);

	if(!buffer->buffer.id)
	{
		free(buffer);
		return NULL;
	}

	/* Force at least dynamic when multi buffering */
	if(!(usage & (GFX_BUFFER_STREAM | GFX_BUFFER_DYNAMIC)) && multi)
		usage |= GFX_BUFFER_DYNAMIC;

	buffer->buffer.size   = size;
	buffer->buffer.usage  = usage;
	buffer->buffer.target = target;
	buffer->buffer.multi  = multi;

	/* Increment multi, as we need a front buffer too! */
	gfx_vector_init_from_buffer(
		&buffer->handles,
		sizeof(GLuint),
		++multi,
		NULL
	);

	if(buffer->handles.begin == buffer->handles.end)
	{
		free(buffer);
		return NULL;
	}

	_gfx_buffer_alloc_buffers(
		buffer,
		buffer->handles.begin,
		multi,
		data
	);

	return (GFXBuffer*)buffer;
}

/******************************************************/
GFXBuffer* gfx_buffer_create_copy(

		GFXBuffer*       src,
		GFXBufferUsage   usage,
		GFXBufferTarget  target)
{
	/* Map buffer to copy data and create */
	void* data = gfx_buffer_map(src, src->size, 0, GFX_BUFFER_READ);
	if(!data) return NULL;

	GFXBuffer* buffer = gfx_buffer_create(
		usage,
		target,
		src->size,
		data,
		src->multi
	);

	gfx_buffer_unmap(src);

	return buffer;
}

/******************************************************/
void gfx_buffer_free(

		GFXBuffer* buffer)
{
	if(buffer)
	{
		struct GFX_Buffer* internal = (struct GFX_Buffer*)buffer;

		/* Unregister as object */
		_gfx_hardware_object_unregister(buffer->id);

		if(GFX_WND) _gfx_buffer_delete_buffers(
			internal,
			internal->handles.begin,
			buffer->multi + 1
		);

		gfx_vector_clear(&internal->handles);
		free(buffer);
	}
}

/******************************************************/
int gfx_buffer_expand(

		GFXBuffer*     buffer,
		unsigned char  num)
{
	if(!GFX_WND || !num) return 0;
	struct GFX_Buffer* internal = (struct GFX_Buffer*)buffer;

	/* Allocate new handles */
	GFXVectorIterator it = gfx_vector_insert_range(
		&internal->handles,
		num,
		NULL,
		internal->handles.end
	);
	if(it == internal->handles.end) return 0;

	/* Allocate buffers */
	_gfx_buffer_alloc_buffers(
		internal,
		it,
		num,
		NULL
	);

	buffer->multi += num;

	return 1;
}

/******************************************************/
int gfx_buffer_shrink(

		GFXBuffer*     buffer,
		unsigned char  num)
{
	if(!GFX_WND) return 0;
	struct GFX_Buffer* internal = (struct GFX_Buffer*)buffer;

	/* Get where to remove buffers */
	num = num > buffer->multi ? buffer->multi : num;
	unsigned char aft = buffer->multi - internal->current;

	aft = aft > num ? num : aft;
	unsigned char bef = num - aft;

	if(aft)
	{
		/* Erase handles after current */
		GFXVectorIterator it = gfx_vector_at(
			&internal->handles,
			internal->current + 1);

		_gfx_buffer_delete_buffers(
			internal,
			it,
			aft);

		gfx_vector_erase_range(&internal->handles, aft, it);
	}
	if(bef)
	{
		/* Erase handles at beginning */
		_gfx_buffer_delete_buffers(
			internal,
			internal->handles.begin,
			bef);

		gfx_vector_erase_range(
			&internal->handles,
			bef,
			internal->handles.begin);
	}

	buffer->multi -= num;
	internal->current -= bef;

	return num;
}

/******************************************************/
void gfx_buffer_swap(

		GFXBuffer* buffer)
{
	struct GFX_Buffer* internal = (struct GFX_Buffer*)buffer;

	/* Swap buffer */
	internal->current =
		(internal->current >= buffer->multi) ?
		0 : internal->current + 1;
}

/******************************************************/
void gfx_buffer_write(

		GFXBuffer*   buffer,
		size_t       size,
		const void*  data,
		size_t       offset)
{
	if(!GFX_WND) return;
	struct GFX_Buffer* internal = (struct GFX_Buffer*)buffer;

	(GFX_RND).NamedBufferSubData(
		*(GLuint*)gfx_vector_at(&internal->handles, internal->current),
		offset,
		size,
		data);
}

/******************************************************/
void gfx_buffer_read(

		GFXBuffer*  buffer,
		size_t      size,
		void*       data,
		size_t      offset)
{
	if(!GFX_WND) return;
	struct GFX_Buffer* internal = (struct GFX_Buffer*)buffer;

	(GFX_RND).GetNamedBufferSubData(
		*(GLuint*)gfx_vector_at(&internal->handles, internal->current),
		offset,
		size,
		data);
}

/******************************************************/
void* gfx_buffer_map(

		GFXBuffer*      buffer,
		size_t          size,
		size_t          offset,
		GFXBufferUsage  access)
{
	if(!GFX_WND) return NULL;
	struct GFX_Buffer* internal = (struct GFX_Buffer*)buffer;

	/* Strip access bits */
	access &= GFX_BUFFER_READ | GFX_BUFFER_WRITE;

	/* Do the actual mapping */
	return (GFX_RND).MapNamedBufferRange(
		*(GLuint*)gfx_vector_at(&internal->handles, internal->current),
		offset,
		size,
		access
	);
}

/******************************************************/
void gfx_buffer_unmap(

		GFXBuffer* buffer)
{
	if(!GFX_WND) return;
	struct GFX_Buffer* internal = (struct GFX_Buffer*)buffer;

	GLboolean success = (GFX_RND).UnmapNamedBuffer(
		*(GLuint*)gfx_vector_at(&internal->handles, internal->current));

	if(!success) gfx_errors_push(
		GFX_ERROR_MEMORY_CORRUPTION,
		"Mapping a buffer might have corrupted its memory."
	);
}
