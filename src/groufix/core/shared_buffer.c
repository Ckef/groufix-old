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

#include "groufix/core/errors.h"
#include "groufix/core/renderer.h"

#include <limits.h>
#include <stdlib.h>

#define GFX_SHARED_BUFFER_MSB (~(ULONG_MAX >> 1))

/******************************************************/
/* Size of a new shared buffer */
static unsigned long _gfx_shared_buffer_size = GFX_SHARED_BUFFER_SIZE_DEFAULT;


/* All shared buffers */
static GFXVector* _gfx_shared_buffers = NULL;


/* Internal Shared Buffer */
typedef struct GFX_SharedBuffer
{
	GFX_RenderObjectID  id;
	GLuint              handle;   /* OpenGL handle */

	GFXBufferTarget     target;
	size_t              size;     /* In bytes */
	GFXVector           segments; /* Taken segments of the buffer */

} GFX_SharedBuffer;


/* Internal Segment */
typedef struct GFX_Segment
{
	size_t offset; /* Sort key */
	size_t size;

} GFX_Segment;


/******************************************************/
static void _gfx_shared_buffer_obj_free(

		void*               object,
		GFX_RenderObjectID  id)
{
	GFX_SharedBuffer* buff = (GFX_SharedBuffer*)object;

	buff->id = id;
	buff->handle = 0;

	buff->size = 0;
	gfx_vector_clear(&buff->segments);
}

/******************************************************/
static void _gfx_shared_buffer_obj_save_restore(

		void*               object,
		GFX_RenderObjectID  id)
{
	GFX_SharedBuffer* buff = (GFX_SharedBuffer*)object;
	buff->id = id;
}

/******************************************************/
/* vtable for render object part of the shared buffer */
static GFX_RenderObjectFuncs _gfx_shared_buffer_obj_funcs =
{
	_gfx_shared_buffer_obj_free,
	_gfx_shared_buffer_obj_save_restore,
	_gfx_shared_buffer_obj_save_restore
};

/******************************************************/
static GFXVectorIterator _gfx_shared_buffer_create(

		GFXBufferTarget  target,
		size_t           minSize,
		GFX_WIND_ARG)
{
	/* Create a new shared buffer */
	GFX_SharedBuffer* buff = malloc(sizeof(GFX_SharedBuffer));
	if(!buff)
	{
		/* Out of memory error */
		gfx_errors_push(
			GFX_ERROR_OUT_OF_MEMORY,
			"Shared Buffer could not be allocated."
		);
		return NULL;
	}

	/* Insert the buffer */
	GFXVectorIterator it = gfx_vector_insert(
		_gfx_shared_buffers,
		&buff,
		_gfx_shared_buffers->end
	);

	if(it == _gfx_shared_buffers->end)
	{
		free(buff);
		return NULL;
	}

	/* Register as object */
	buff->id = _gfx_render_object_register(
		&GFX_WIND_GET.objects,
		buff,
		&_gfx_shared_buffer_obj_funcs
	);

	if(!buff->id.id)
	{
		free(buff);
		return NULL;
	}

	/* Initialize buffer */
	buff->target = target;
	buff->size = (_gfx_shared_buffer_size < minSize) ?
		minSize : _gfx_shared_buffer_size;

	GFX_REND_GET.CreateBuffers(1, &buff->handle);
	if(!GFX_WIND_GET.ext[GFX_EXT_DIRECT_STATE_ACCESS])
		GFX_REND_GET.BindBuffer(target, buff->handle);

	GFX_REND_GET.NamedBufferData(buff->handle, buff->size, NULL, GL_STATIC_DRAW);

	gfx_vector_init(&buff->segments, sizeof(GFX_Segment));

	return it;
}

/******************************************************/
static void _gfx_shared_buffer_free(

		GFXVectorIterator it,
		GFX_WIND_ARG)
{
	if(it)
	{
		GFX_SharedBuffer* buff = *(GFX_SharedBuffer**)it;

		/* Unregister as object */
		_gfx_render_object_unregister(buff->id);

		/* Delete buffer */
		if(!GFX_WIND_EQ(NULL))
			GFX_REND_GET.DeleteBuffers(1, &buff->handle);

		/* Remove from le vector */
		gfx_vector_erase(_gfx_shared_buffers, it);
		gfx_vector_clear(&buff->segments);

		free(buff);
	}

	/* Free vector if empty */
	if(_gfx_shared_buffers->begin == _gfx_shared_buffers->end)
	{
		gfx_vector_free(_gfx_shared_buffers);
		_gfx_shared_buffers = NULL;
	}
}

/******************************************************/
static int _gfx_shared_buffer_insert_segment(

		GFX_SharedBuffer*  buffer,
		size_t             size,
		size_t*            offset)
{
	GFX_Segment new =
	{
		.offset = 0,
		.size = size
	};

	/* Iterate through segments and find a big enough empty spot */
	GFXVectorIterator it;
	for(
		it = buffer->segments.begin;
		it != buffer->segments.end;
		it = gfx_vector_next(&buffer->segments, it))
	{
		GFX_Segment* seg = it;
		if(size > (seg->offset - new.offset))
		{
			new.offset = seg->offset + seg->size;
			continue;
		}

		/* Great, try to insert the segment */
		it = gfx_vector_insert(&buffer->segments, &new, it);
		if(it == buffer->segments.end) return 0;

		*offset = new.offset;
		return 1;
	}

	/* Check the trailing empty space */
	if(size <= (buffer->size - new.offset))
	{
		/* Try to insert at the end */
		it = gfx_vector_insert(
			&buffer->segments,
			&new,
			buffer->segments.end
		);

		if(it != buffer->segments.end)
		{
			*offset = new.offset;
			return 1;
		}
	}

	return 0;
}

/******************************************************/
static int _gfx_shared_buffer_segment_comp(

		const void*  key,
		const void*  elem)
{
	size_t offset = GFX_VOID_TO_UINT(key);
	size_t found = ((GFX_Segment*)elem)->offset;

	if(found < offset) return 1;
	if(found > offset) return -1;

	return 0;
}

/******************************************************/
static void _gfx_shared_buffer_erase_segment(

		GFX_SharedBuffer*  buffer,
		size_t             offset)
{
	/* Retrieve segment */
	GFXVectorIterator it = bsearch(
		GFX_UINT_TO_VOID(offset),
		buffer->segments.begin,
		gfx_vector_get_size(&buffer->segments),
		sizeof(GFX_Segment),
		_gfx_shared_buffer_segment_comp
	);

	if(it) gfx_vector_erase(&buffer->segments, it);
}

/******************************************************/
GLuint _gfx_shared_buffer_get_handle(

		const GFXSharedBuffer* buffer)
{
	return ((const GFX_SharedBuffer*)buffer->reference)->handle;
}

/******************************************************/
void gfx_shared_buffer_request_size(

		unsigned long size)
{
	if(size <= GFX_SHARED_BUFFER_MSB)
	{
		unsigned long cap = 1;
		while(cap < size) cap <<= 1;

		_gfx_shared_buffer_size = cap;
	}

	else _gfx_shared_buffer_size = size;
}

/******************************************************/
int gfx_shared_buffer_init(

		GFXSharedBuffer*  buffer,
		GFXBufferTarget   target,
		size_t            size,
		const void*       data)
{
	GFX_WIND_INIT(0);

	/* Create vector if it doesn't exist yet */
	if(!_gfx_shared_buffers)
	{
		_gfx_shared_buffers =
			gfx_vector_create(sizeof(GFX_SharedBuffer*));

		if(!_gfx_shared_buffers) return 0;
	}

	/* Iterate through all shared buffers */
	GFXVectorIterator it;
	for(
		it = _gfx_shared_buffers->begin;
		it != _gfx_shared_buffers->end;
		it = gfx_vector_next(_gfx_shared_buffers, it))
	{
		/* Validate buffer and try to insert */
		GFX_SharedBuffer* buff = *(GFX_SharedBuffer**)it;
		if(buff->target != target) continue;

		size_t offset;
		if(_gfx_shared_buffer_insert_segment(buff, size, &offset))
		{
			buffer->reference = buff;
			buffer->offset = offset;

			GFX_REND_GET.NamedBufferSubData(
				buff->handle,
				offset,
				size,
				data);

			return 1;
		}
	}

	/* Create new shared buffer */
	it = _gfx_shared_buffer_create(target, size, GFX_WIND_AS_ARG);

	if(it)
	{
		GFX_SharedBuffer* buff = *(GFX_SharedBuffer**)it;
		size_t offset;

		if(_gfx_shared_buffer_insert_segment(buff, size, &offset))
		{
			buffer->reference = buff;
			buffer->offset = offset;

			GFX_REND_GET.NamedBufferSubData(
				buff->handle,
				offset,
				size,
				data);

			return 1;
		}
	}

	/* Nope, destroy again */
	_gfx_shared_buffer_free(it, GFX_WIND_AS_ARG);

	return 0;
}

/******************************************************/
void gfx_shared_buffer_clear(

		GFXSharedBuffer* buffer)
{
	GFX_WIND_INIT_UNSAFE;

	GFX_SharedBuffer* buff = (GFX_SharedBuffer*)buffer->reference;
	_gfx_shared_buffer_erase_segment(buff, buffer->offset);

	/* If empty, free it */
	if(buff->segments.begin == buff->segments.end)
	{
		/* Find buffer iterator and free it */
		GFXVectorIterator it;
		for(
			it = _gfx_shared_buffers->begin;
			it != _gfx_shared_buffers->end;
			it = gfx_vector_next(_gfx_shared_buffers, it))
		{
			if(*(GFX_SharedBuffer**)it == buff)
			{
				_gfx_shared_buffer_free(it, GFX_WIND_AS_ARG);
				break;
			}
		}
	}

	buffer->reference = NULL;
	buffer->offset = 0;
}
