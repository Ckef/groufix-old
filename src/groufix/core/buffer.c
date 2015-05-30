/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#include "groufix/core/internal.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
/* Internal Buffer */
typedef struct GFX_Buffer
{
	/* Super class */
	GFXBuffer buffer;

	/* Hidden data */
	GFX_RenderObjectID  id;
	unsigned char       current; /* Current active buffer */
	GFXVector           handles; /* Stores GLuint */

} GFX_Buffer;


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

		const GFX_Buffer*  buffer,
		GFXVectorIterator  it,
		unsigned char      num,
		const void*        data,
		GFX_CONT_ARG)
{
	/* Allocate buffers */
	GFX_REND_GET.CreateBuffers(num, it);
	GLenum us = _gfx_buffer_get_usage(buffer->buffer.usage);

	/* Iterate over buffers */
	unsigned char i;
	for(i = 0; i < num; ++i)
	{
		/* Only write data to the first buffer */
		GFX_REND_GET.NamedBufferData(
			*(GLuint*)it,
			buffer->buffer.size,
			i ? NULL : data,
			us);

		it = gfx_vector_next(&buffer->handles, it);
	}
}

/******************************************************/
static void _gfx_buffer_delete_buffers(

		const GFX_Buffer*        buffer,
		const GFXVectorIterator  it,
		unsigned char            num,
		GFX_CONT_ARG)
{
	/* Iterate over buffers */
	unsigned char i;
	for(i = 0; i < num; ++i)
	{
		/* Make sure it is not currently bound */
		GLuint* handle = gfx_vector_advance(&buffer->handles, it, i);
		_gfx_gl_binder_unbind_uniform_buffer(*handle, GFX_CONT_AS_ARG);
	}

	/* And deallocate all buffers */
	GFX_REND_GET.DeleteBuffers(num, it);
}

/******************************************************/
static void _gfx_buffer_obj_free(

		void*               object,
		GFX_RenderObjectID  id)
{
	GFX_Buffer* buffer = (GFX_Buffer*)object;

	buffer->id = id;
	gfx_vector_clear(&buffer->handles);
}

/******************************************************/
static void _gfx_buffer_obj_save_restore(

		void*               object,
		GFX_RenderObjectID  id)
{
	GFX_Buffer* buffer = (GFX_Buffer*)object;
	buffer->id = id;
}

/******************************************************/
/* vtable for render object part of the buffer */
static GFX_RenderObjectFuncs _gfx_buffer_obj_funcs =
{
	_gfx_buffer_obj_free,
	_gfx_buffer_obj_save_restore,
	_gfx_buffer_obj_save_restore
};

/******************************************************/
GLuint _gfx_gl_buffer_get_handle(

		const GFXBuffer* buffer)
{
	const GFX_Buffer* internal = (const GFX_Buffer*)buffer;
	return *(GLuint*)gfx_vector_at(&internal->handles, internal->current);
}

/******************************************************/
GFXBuffer* gfx_buffer_create(

		GFXBufferUsage  usage,
		size_t          size,
		const void*     data,
		unsigned char   multi)
{
	GFX_CONT_INIT(NULL);

	/* Create new buffer */
	GFX_Buffer* buffer = calloc(1, sizeof(GFX_Buffer));
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
	buffer->id = _gfx_render_object_register(
		&GFX_CONT_GET.objects,
		buffer,
		&_gfx_buffer_obj_funcs
	);

	if(!buffer->id.id)
	{
		free(buffer);
		return NULL;
	}

	/* Force at least dynamic when multi buffering */
	if(!(usage & (GFX_BUFFER_STREAM | GFX_BUFFER_DYNAMIC)) && multi)
		usage |= GFX_BUFFER_DYNAMIC;

	buffer->buffer.usage  = usage;
	buffer->buffer.size   = size;
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
		_gfx_render_object_unregister(buffer->id);
		free(buffer);

		return NULL;
	}

	_gfx_buffer_alloc_buffers(
		buffer,
		buffer->handles.begin,
		multi,
		data,
		GFX_CONT_AS_ARG
	);

	return (GFXBuffer*)buffer;
}

/******************************************************/
GFXBuffer* gfx_buffer_create_copy(

		const GFXBuffer*  src,
		GFXBufferUsage    usage)
{
	/* Create buffer */
	GFXBuffer* buffer = gfx_buffer_create(
		usage,
		src->size,
		NULL,
		src->multi
	);

	if(!buffer) return NULL;

	/* Copy data */
	gfx_buffer_copy(buffer, src, 0, 0, src->size);

	return (GFXBuffer*)buffer;
}

/******************************************************/
void gfx_buffer_free(

		GFXBuffer* buffer)
{
	if(buffer)
	{
		GFX_CONT_INIT_UNSAFE;

		GFX_Buffer* internal = (GFX_Buffer*)buffer;

		/* Unregister as object */
		_gfx_render_object_unregister(internal->id);

		/* Delete all buffers */
		if(!GFX_CONT_EQ(NULL))
			_gfx_buffer_delete_buffers(
				internal,
				internal->handles.begin,
				buffer->multi + 1,
				GFX_CONT_AS_ARG
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
	GFX_CONT_INIT(0);

	if(!num) return 0;
	GFX_Buffer* internal = (GFX_Buffer*)buffer;

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
		NULL,
		GFX_CONT_AS_ARG
	);

	buffer->multi += num;

	return 1;
}

/******************************************************/
int gfx_buffer_shrink(

		GFXBuffer*     buffer,
		unsigned char  num)
{
	GFX_CONT_INIT(0);

	GFX_Buffer* internal = (GFX_Buffer*)buffer;

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
			aft,
			GFX_CONT_AS_ARG);

		gfx_vector_erase_range(&internal->handles, aft, it);
	}
	if(bef)
	{
		/* Erase handles at beginning */
		_gfx_buffer_delete_buffers(
			internal,
			internal->handles.begin,
			bef,
			GFX_CONT_AS_ARG);

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
	GFX_Buffer* internal = (GFX_Buffer*)buffer;

	/* Swap buffer */
	internal->current =
		(internal->current >= buffer->multi) ?
		0 : internal->current + 1;
}

/******************************************************/
void gfx_buffer_read(

		const GFXBuffer*  buffer,
		size_t            size,
		void*             data,
		size_t            offset)
{
	GFX_CONT_INIT();

	const GFX_Buffer* internal = (const GFX_Buffer*)buffer;

	GFX_REND_GET.GetNamedBufferSubData(
		*(GLuint*)gfx_vector_at(&internal->handles, internal->current),
		offset,
		size,
		data
	);
}

/******************************************************/
void gfx_buffer_write(

		const GFXBuffer*  buffer,
		size_t            size,
		const void*       data,
		size_t            offset)
{
	GFX_CONT_INIT();

	const GFX_Buffer* internal = (const GFX_Buffer*)buffer;

	GFX_REND_GET.NamedBufferSubData(
		*(GLuint*)gfx_vector_at(&internal->handles, internal->current),
		offset,
		size,
		data
	);
}

/******************************************************/
GFX_API void gfx_buffer_copy(

		const GFXBuffer*  dest,
		const GFXBuffer*  src,
		size_t            srcOffset,
		size_t            destOffset,
		size_t            size)
{
	GFX_CONT_INIT();

	const GFX_Buffer* intDest = (const GFX_Buffer*)dest;
	const GFX_Buffer* intSrc = (const GFX_Buffer*)src;

	/* Clip range */
	size = (destOffset + size > dest->size) ?
		dest->size - destOffset : size;
	size = (srcOffset + size > src->size) ?
		src->size - srcOffset : size;

	/* Copy data */
	GFX_REND_GET.CopyNamedBufferSubData(
		*(GLuint*)gfx_vector_at(&intSrc->handles, intSrc->current),
		*(GLuint*)gfx_vector_at(&intDest->handles, intDest->current),
		srcOffset,
		destOffset,
		size
	);
}

/******************************************************/
void* gfx_buffer_map(

		const GFXBuffer*  buffer,
		size_t            size,
		size_t            offset,
		GFXBufferUsage    access)
{
	GFX_CONT_INIT(NULL);

	const GFX_Buffer* internal = (const GFX_Buffer*)buffer;

	/* Strip access bits */
	access &= GFX_BUFFER_READ | GFX_BUFFER_WRITE;

	/* Do the actual mapping */
	return GFX_REND_GET.MapNamedBufferRange(
		*(GLuint*)gfx_vector_at(&internal->handles, internal->current),
		offset,
		size,
		access
	);
}

/******************************************************/
void gfx_buffer_unmap(

		const GFXBuffer* buffer)
{
	GFX_CONT_INIT();

	const GFX_Buffer* internal = (const GFX_Buffer*)buffer;

	GLboolean success = GFX_REND_GET.UnmapNamedBuffer(
		*(GLuint*)gfx_vector_at(&internal->handles, internal->current));

	if(!success) gfx_errors_push(
		GFX_ERROR_MEMORY_CORRUPTION,
		"Mapping a buffer might have corrupted its memory."
	);
}
