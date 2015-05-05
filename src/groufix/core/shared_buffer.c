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

		size_t minSize,
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
		gfx_vector_erase(_gfx_shared_buffers, it);
		free(buff);

		return NULL;
	}

	/* Initialize buffer */
	buff->size = (_gfx_shared_buffer_size < minSize) ?
		minSize : _gfx_shared_buffer_size;

	GFX_REND_GET.CreateBuffers(1, &buff->handle);
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
		unsigned char      align,
		size_t*            offset)
{
	GFX_Segment new =
	{
		.offset = buffer->size,
		.size = size
	};

	/* Iterate through segments and find a big enough empty spot */
	/* Iterate back to front as it's likely to find space at the end */
	GFXVectorIterator it = buffer->segments.end;

	while(it != buffer->segments.begin)
	{
		GFX_Segment* seg = gfx_vector_previous(&buffer->segments, it);

		/* Continue if it does not fit */
		size_t off = seg->offset + seg->size;
		off += (align - (off % align)) % align;

		if(off >= new.offset || size > (new.offset - off))
		{
			new.offset = seg->offset;
			it = seg;

			continue;
		}

		/* Great, try to insert the segment */
		new.offset = off;
		it = gfx_vector_insert(&buffer->segments, &new, it);

		if(it == buffer->segments.end) return 0;
		*offset = new.offset;

		return 1;
	}

	/* Check the leading empty space */
	if(size <= new.offset)
	{
		/* Try to insert at the beginning */
		new.offset = 0;
		it = gfx_vector_insert(&buffer->segments, &new, it);

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
void gfx_shared_buffer_cleanup(void)
{
	if(_gfx_shared_buffers)
	{
		GFX_WIND_INIT_UNSAFE;

		/* Find any empty buffers and free them */
		/* Iterate from back to front again, because erasal efficiency */
		GFXVectorIterator it = _gfx_shared_buffers->end;

		while(it != _gfx_shared_buffers->begin)
		{
			it = gfx_vector_previous(_gfx_shared_buffers, it);
			GFX_SharedBuffer* buff = *(GFX_SharedBuffer**)it;

			if(buff->segments.begin == buff->segments.end)
			{
				/* Also bail if all buffers are freed */
				_gfx_shared_buffer_free(it, GFX_WIND_AS_ARG);
				if(!_gfx_shared_buffers) break;
			}
		}
	}
}

/******************************************************/
int gfx_shared_buffer_init(

		GFXSharedBuffer*  buffer,
		size_t            size,
		const void*       data,
		unsigned char     align)
{
	GFX_WIND_INIT(0);

	/* Validate alignment */
	align = align ? align : 1;

	/* Create vector if it doesn't exist yet */
	if(!_gfx_shared_buffers)
	{
		_gfx_shared_buffers =
			gfx_vector_create(sizeof(GFX_SharedBuffer*));

		if(!_gfx_shared_buffers) return 0;
	}

	/* Iterate through all shared buffers */
	/* Iterate from back to front as back likely contains empty space */
	GFXVectorIterator it = _gfx_shared_buffers->end;

	while(it != _gfx_shared_buffers->begin)
	{
		/* Try to insert */
		it = gfx_vector_previous(_gfx_shared_buffers, it);
		GFX_SharedBuffer* buff = *(GFX_SharedBuffer**)it;

		size_t offset;
		if(_gfx_shared_buffer_insert_segment(buff, size, align, &offset))
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
	it = _gfx_shared_buffer_create(size, GFX_WIND_AS_ARG);

	if(it)
	{
		GFX_SharedBuffer* buff = *(GFX_SharedBuffer**)it;

		size_t offset;
		if(_gfx_shared_buffer_insert_segment(buff, size, align, &offset))
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
int gfx_shared_buffer_init_align(

		GFXSharedBuffer*  buffer,
		size_t            size,
		const void*       data,
		GFXDataType       type)
{
	return gfx_shared_buffer_init(
		buffer,
		size,
		data,
		_gfx_sizeof_data_type(type)
	);
}

/******************************************************/
void gfx_shared_buffer_clear(

		GFXSharedBuffer*  buffer,
		int               keep)
{
	GFX_WIND_INIT_UNSAFE;

	GFX_SharedBuffer* buff = buffer->reference;
	_gfx_shared_buffer_erase_segment(buff, buffer->offset);

	/* If already terminated, ignore keep */
	if(GFX_WIND_EQ(NULL)) keep = 0;

	/* If empty and no keep flag, free it */
	if(!keep && buff->segments.begin == buff->segments.end)
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
