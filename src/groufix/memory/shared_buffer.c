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
#include "groufix/memory/internal.h"

#include <limits.h>
#include <stdlib.h>

#define GFX_SHARED_BUFFER_MSB (~(ULONG_MAX >> 1))

/******************************************************/
/* Size of a new shared buffer */
static unsigned long _gfx_shared_buffer_size = GFX_SHARED_BUFFER_SIZE_DEFAULT;

/* All shared buffers */
static GFXVector* _gfx_shared_buffers = NULL;

/* Internal Shared Buffer */
struct GFX_SharedBuffer
{
	size_t           id;       /* Hardware Object ID */
	GLuint           handle;   /* OpenGL handle */

	GFXBufferTarget  target;
	size_t           size;     /* In bytes */
	GFXVector        segments; /* Taken segments of the buffer */
};

/* Internal Segment */
struct GFX_Segment
{
	size_t offset; /* Sort key */
	size_t size;
};

/******************************************************/
static void _gfx_shared_buffer_free_obj(void* object, GFX_Extensions* ext)
{
	struct GFX_SharedBuffer* buff = (struct GFX_SharedBuffer*)object;

	ext->DeleteBuffers(1, &buff->handle);
	buff->handle = 0;

	buff->size = 0;
	buff->id = 0;
}

/******************************************************/
static GFX_HardwareFuncs _gfx_shared_buffer_obj_funcs =
{
	_gfx_shared_buffer_free_obj,
	NULL,
	NULL
};

/******************************************************/
static GFXVectorIterator _gfx_shared_buffer_create(GFXBufferTarget target, GFX_Extensions* ext)
{
	/* Create a new shared buffer */
	struct GFX_SharedBuffer* buff = malloc(sizeof(struct GFX_SharedBuffer));
	if(!buff) return NULL;

	/* Insert the buffer */
	GFXVectorIterator it = gfx_vector_insert(_gfx_shared_buffers, &buff, _gfx_shared_buffers->end);
	if(it == _gfx_shared_buffers->end)
	{
		free(buff);
		return NULL;
	}

	/* Register as object */
	buff->id = _gfx_hardware_object_register(buff, &_gfx_shared_buffer_obj_funcs);
	if(!buff->id)
	{
		free(buff);
		return NULL;
	}

	/* Initialize buffer */
	buff->target = target;
	buff->size = _gfx_shared_buffer_size;

	ext->GenBuffers(1, &buff->handle);
	ext->BindBuffer(target, buff->handle);
	ext->BufferData(target, buff->size, NULL, GL_STATIC_DRAW);

	gfx_vector_init(&buff->segments, sizeof(struct GFX_Segment));

	return it;
}

/******************************************************/
static void _gfx_shared_buffer_free(GFXVectorIterator it, GFX_Extensions* ext)
{
	if(it)
	{
		struct GFX_SharedBuffer* buff = *(struct GFX_SharedBuffer**)it;

		/* Remove from le vector */
		gfx_vector_erase(_gfx_shared_buffers, it);

		/* Unregister as object */
		_gfx_hardware_object_unregister(buff->id);

		if(ext) ext->DeleteBuffers(1, &buff->handle);
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
static int _gfx_shared_buffer_insert_segment(struct GFX_SharedBuffer* buffer, size_t size, size_t* offset)
{
	struct GFX_Segment new;
	new.offset = 0;
	new.size = size;

	/* Iterate through segments and find a big enough empty spot */
	GFXVectorIterator it;
	for(it = buffer->segments.begin; it != buffer->segments.end; it = gfx_vector_next(&buffer->segments, it))
	{
		struct GFX_Segment* seg = it;
		if(size > (seg->offset - new.offset))
		{
			new.offset = seg->offset + seg->size;
			continue;
		}

		/* Great, try to insert the segment */
		if(gfx_vector_insert(&buffer->segments, &new, it) == buffer->segments.end) return 0;
		*offset = new.offset;

		return 1;
	}

	/* Check the trailing empty space */
	if(size <= (buffer->size - new.offset))
	{
		/* Try to insert at the end */
		if(gfx_vector_insert(&buffer->segments, &new, buffer->segments.end) != buffer->segments.end)
		{
			*offset = new.offset;
			return 1;
		}
	}

	return 0;
}

/******************************************************/
static GFXVectorIterator _gfx_shared_buffer_find_segment(struct GFX_SharedBuffer* buffer, size_t offset)
{
	/* Binary search for the offset */
	size_t min = 0;
	size_t max = gfx_vector_get_size(&buffer->segments);
	GFXVectorIterator it = buffer->segments.end;

	while(max > min)
	{
		/* Get mid point */
		size_t mid = min + ((max - min) >> 1);

		it = gfx_vector_at(&buffer->segments, mid);
		size_t found = ((struct GFX_Segment*)it)->offset;

		/* Compare against key */
		if(found < offset)
			min = mid + 1;
		else if(found > offset)
			max = mid;

		else return it;
	}
	return it;
}

/******************************************************/
static void _gfx_shared_buffer_erase_segment(struct GFX_SharedBuffer* buffer, size_t offset)
{
	/* Retrieve segment */
	GFXVectorIterator it = _gfx_shared_buffer_find_segment(buffer, offset);
	if(it != buffer->segments.end)
	{
		struct GFX_Segment* seg = it;
		if(seg->offset == offset) gfx_vector_erase(&buffer->segments, it);
	}
}

/******************************************************/
GLuint _gfx_shared_buffer_get_handle(const GFXSharedBuffer* buffer)
{
	return ((struct GFX_SharedBuffer*)buffer->reference)->handle;
}

/******************************************************/
void gfx_shared_buffer_request_size(unsigned long size)
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
int gfx_shared_buffer_init(GFXSharedBuffer* buffer, GFXBufferTarget target, size_t size, const void* data)
{
	/* Get current window and context & validate target */
	GFX_Window* window = _gfx_window_get_current();
	if(!window) return 0;

	/* Create vector if it doesn't exist yet */
	if(!_gfx_shared_buffers)
	{
		_gfx_shared_buffers = gfx_vector_create(sizeof(struct GFX_SharedBuffer*));
		if(!_gfx_shared_buffers) return 0;
	}

	/* Iterate through all shared buffers */
	GFXVectorIterator it;
	for(it = _gfx_shared_buffers->begin; it != _gfx_shared_buffers->end; it = gfx_vector_next(_gfx_shared_buffers, it))
	{
		/* Validate buffer and try to insert */
		struct GFX_SharedBuffer* buff = *(struct GFX_SharedBuffer**)it;
		if(buff->target != target) continue;

		size_t offset;
		if(_gfx_shared_buffer_insert_segment(buff, size, &offset))
		{
			buffer->reference = buff;
			buffer->offset = offset;

			window->extensions.BindBuffer(buff->target, buff->handle);
			window->extensions.BufferSubData(buff->target, offset, size, data);

			return 1;
		}
	}

	/* Create new shared buffer */
	it = _gfx_shared_buffer_create(target, &window->extensions);
	if(it)
	{
		struct GFX_SharedBuffer* buff = *(struct GFX_SharedBuffer**)it;
		size_t offset;

		if(_gfx_shared_buffer_insert_segment(buff, size, &offset))
		{
			buffer->reference = buff;
			buffer->offset = offset;

			window->extensions.BindBuffer(buff->target, buff->handle);
			window->extensions.BufferSubData(buff->target, offset, size, data);

			return 1;
		}
	}

	/* Nope, destroy again */
	_gfx_shared_buffer_free(it, &window->extensions);

	return 0;
}

/******************************************************/
void gfx_shared_buffer_clear(GFXSharedBuffer* buffer)
{
	struct GFX_SharedBuffer* buff = (struct GFX_SharedBuffer*)buffer->reference;
	_gfx_shared_buffer_erase_segment(buff, buffer->offset);

	/* If empty, free it */
	if(buff->segments.begin == buff->segments.end)
	{
		/* Get current window and context */
		GFX_Window* window = _gfx_window_get_current();
		GFX_Extensions* ext = NULL;

		if(window) ext = &window->extensions;

		/* Find buffer iterator and free it */
		GFXVectorIterator it;
		for(it = _gfx_shared_buffers->begin; it != _gfx_shared_buffers->end; it = gfx_vector_next(_gfx_shared_buffers, it))
			if(*(struct GFX_SharedBuffer**)it == buff)
			{
				_gfx_shared_buffer_free(it, ext);
				break;
			}
	}

	buffer->reference = NULL;
	buffer->offset = 0;
}