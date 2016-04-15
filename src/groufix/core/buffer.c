/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel.
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
/** Internal renderer handle */
#if defined(GFX_RENDERER_GL)
typedef GLuint GFX_BufferHandle;

#elif defined(GFX_RENDERER_VK)
typedef void* GFX_BufferHandle;

#endif


/** Internal Buffer */
typedef struct GFX_Buffer
{
	/* Super class */
	GFXBuffer buffer;

	/* Hidden data */
	GFX_RenderObjectID  id;
	unsigned char       current;

} GFX_Buffer;


/******************************************************/
static inline GFX_BufferHandle* _gfx_buffer_get_handle(

		const GFX_Buffer*  buffer,
		unsigned char      index)
{
	return ((GFX_BufferHandle*)(buffer + 1)) + index;
}

/******************************************************/
static inline int _gfx_buffer_ref(

		const GFX_Buffer* buffer,
		GFX_CONT_ARG)
{
	return _gfx_render_object_id_reference(
		&((GFX_Buffer*)buffer)->id,
		buffer->buffer.object,
		&GFX_CONT_GET.objects
	);
}

#if defined(GFX_RENDERER_GL)

/******************************************************/
static inline GLenum _gfx_buffer_from_usage(

		GFXBufferUsage usage)
{
	return

		/* Draw */
		usage & GFX_BUFFER_WRITE ?

		(usage & GFX_BUFFER_MAP_WRITE ? GL_STREAM_DRAW :
		usage & GFX_BUFFER_MAP_READ ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW) :

		/* Read */
		usage & GFX_BUFFER_READ ?

		(usage & GFX_BUFFER_MAP_WRITE ? GL_STREAM_READ :
		usage & GFX_BUFFER_MAP_READ ? GL_DYNAMIC_READ : GL_STATIC_READ) :

		/* Copy */
		(usage & GFX_BUFFER_MAP_WRITE ? GL_STREAM_COPY :
		usage & GFX_BUFFER_MAP_READ ? GL_DYNAMIC_COPY : GL_STATIC_COPY);
}

#endif

/******************************************************/
static int _gfx_buffer_init(

		GFX_Buffer*    buffer,
		unsigned char  num,
		const void**   data,
		GFX_CONT_ARG)
{
#if defined(GFX_RENDERER_GL)

	/* Allocate buffers */
	GFX_REND_GET.CreateBuffers(
		buffer->buffer.count, _gfx_buffer_get_handle(buffer, 0));
	GLenum us = _gfx_buffer_from_usage(
		buffer->buffer.usage);

	unsigned char i;

	/* Check if any buffers weren't created */
	for(i = 0; i < buffer->buffer.count; ++i)
		if(!(*_gfx_buffer_get_handle(buffer, i)))
		{
			GFX_REND_GET.DeleteBuffers(
				buffer->buffer.count, _gfx_buffer_get_handle(buffer, 0));

			return 0;
		}

	/* Only write data to a subset of buffers */
	for(i = 0; i < buffer->buffer.count; ++i)
	{
		const void* d = NULL;
		if(i < num) d = data[i];

		GFX_REND_GET.NamedBufferData(
			*_gfx_buffer_get_handle(buffer, i), buffer->buffer.size, d, us);
	}

#endif

	return 1;
}

/******************************************************/
static void _gfx_buffer_clear(

		GFX_Buffer* buffer,
		GFX_CONT_ARG)
{
#if defined(GFX_RENDERER_GL)

	GFX_REND_GET.DeleteBuffers(
		buffer->buffer.count,
		_gfx_buffer_get_handle(buffer, 0)
	);

#endif

	memset(
		_gfx_buffer_get_handle(buffer, 0),
		0,
		buffer->buffer.count * sizeof(GFX_BufferHandle)
	);
}

/******************************************************/
static void _gfx_buffer_obj_destruct(

		GFX_RenderObjectIDArg arg)
{
	/* Check if the context actually exists */
	/* It may not exist if the buffer was freed after all contexts were destroyed */
	/* But not to worry, if that is the case, this callback was called before */
	GFX_CONT_INIT();

	GFX_Buffer* buffer = GFX_PTR_SUB_BYTES(arg, offsetof(GFX_Buffer, id));
	_gfx_buffer_clear(buffer, GFX_CONT_AS_ARG);
}

/******************************************************/
static void _gfx_buffer_obj_prepare(

		GFX_RenderObjectIDArg  arg,
		void**                 temp,
		int                    shared)
{
	if(shared) return;

	GFX_CONT_INIT_UNSAFE;
	GFX_Buffer* buffer = GFX_PTR_SUB_BYTES(arg, offsetof(GFX_Buffer, id));

	/* Create temporary storage */
	*temp = malloc(buffer->buffer.size * buffer->buffer.count);
	if(!*temp)
	{
		/* Out of memory error */
		gfx_errors_output(
			"[GFX Out Of Memory]: Buffer ran out of memory during preparation."
		);
	}

	else
	{
		/* Copy all data */
#if defined(GFX_RENDERER_GL)

		unsigned char i;
		for(i = 0; i < buffer->buffer.count; ++i)
		{
			GFX_REND_GET.GetNamedBufferSubData(
				*_gfx_buffer_get_handle(buffer, i),
				0,
				buffer->buffer.size,
				GFX_PTR_ADD_BYTES(*temp, buffer->buffer.size * i)
			);
		}

#endif
	}

	/* Clear actual buffers */
	_gfx_buffer_clear(buffer, GFX_CONT_AS_ARG);
}

/******************************************************/
static void _gfx_buffer_obj_transfer(

		GFX_RenderObjectIDArg  arg,
		void**                 temp,
		int                    shared)
{
	if(shared) return;

	GFX_CONT_INIT_UNSAFE;
	GFX_Buffer* buffer = GFX_PTR_SUB_BYTES(arg, offsetof(GFX_Buffer, id));

	/* Get pointers */
	const void* data[buffer->buffer.count];
	unsigned char i;

	for(i = 0; i < buffer->buffer.count; ++i) data[i] = *temp ?
		GFX_PTR_ADD_BYTES(*temp, buffer->buffer.size * i) : NULL;

	/* Create the actual buffer contents */
	if(!_gfx_buffer_init(buffer, buffer->buffer.count, data, GFX_CONT_AS_ARG))
	{
		/* Out of memory error */
		gfx_errors_output(
			"[GFX Out Of Memory]: Buffer ran out of memory during transferring."
		);
	}

	free(*temp);
}

/******************************************************/
/* vtable for render object part of the buffer */
static const GFX_RenderObjectFuncs _gfx_buffer_obj_funcs =
{
	.destruct = _gfx_buffer_obj_destruct,
	.prepare  = _gfx_buffer_obj_prepare,
	.transfer = _gfx_buffer_obj_transfer
};

/******************************************************/
GFXBuffer* gfx_buffer_create(

		GFXBufferUsage  usage,
		size_t          size,
		const void*     data,
		unsigned char   count)
{
	GFX_CONT_INIT(NULL);

	/* Create new buffer, append handles at the end of the struct */
	size_t alloc = sizeof(GFX_Buffer) + count * sizeof(GFX_BufferHandle);

	GFX_Buffer* buffer = calloc(1, alloc);
	if(!buffer)
	{
		/* Out of memory error */
		gfx_errors_output(
			"[GFX Out Of Memory]: Buffer could not be allocated."
		);
		return NULL;
	}

	/* Initialize as object */
#if defined(GFX_RENDERER_GL)

	buffer->buffer.object =
		GFX_OBJECT_NEEDS_REFERENCE |
		GFX_OBJECT_CAN_SHARE;

#endif

	if(!_gfx_render_object_id_init(
		&buffer->id,
		buffer->buffer.object,
		&_gfx_buffer_obj_funcs,
		&GFX_CONT_GET.objects))
	{
		free(buffer);
		return NULL;
	}

	buffer->buffer.usage = usage;
	buffer->buffer.size = size;
	buffer->buffer.count = count;

	/* Create the actual buffer contents */
	if(!_gfx_buffer_init(buffer, 1, &data, GFX_CONT_AS_ARG))
	{
		_gfx_render_object_id_clear(&buffer->id);
		free(buffer);

		return NULL;
	}

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
		src->count
	);

	if(!buffer) return NULL;

	/* Copy data */
	gfx_buffer_copy(buffer, src, 0, 0, src->size);

	return buffer;
}

/******************************************************/
void gfx_buffer_free(

		GFXBuffer* buffer)
{
	if(buffer)
	{
		/* Clear as object */
		/* Object clearing will call the destruct callback */
		_gfx_render_object_id_clear(&((GFX_Buffer*)buffer)->id);
		free(buffer);
	}
}

/******************************************************/
void gfx_buffer_swap(

		GFXBuffer* buffer)
{
	GFX_CONT_INIT();

	GFX_Buffer* internal = (GFX_Buffer*)buffer;
	if(_gfx_buffer_ref(internal, GFX_CONT_AS_ARG))
	{
		/* Get new current index */
		internal->current = (internal->current + 1) % buffer->count;
	}
}

/******************************************************/
size_t gfx_buffer_read(

		const GFXBuffer*  buffer,
		size_t            size,
		void*             data,
		size_t            offset)
{
	/* Derp */
	if(offset >= buffer->size) return 0;

	GFX_CONT_INIT(0);

	const GFX_Buffer* internal = (const GFX_Buffer*)buffer;
	if(!_gfx_buffer_ref(internal, GFX_CONT_AS_ARG)) return 0;

	/* Clip offset and size */
	size = (offset + size > buffer->size) ?
		buffer->size - offset : size;

#if defined(GFX_RENDERER_GL)

	GFX_REND_GET.GetNamedBufferSubData(
		*_gfx_buffer_get_handle(internal, internal->current),
		offset,
		size,
		data
	);

#endif

	return size;
}

/******************************************************/
size_t gfx_buffer_write(

		GFXBuffer*   buffer,
		size_t       size,
		const void*  data,
		size_t       offset)
{
	/* Derp */
	if(offset >= buffer->size) return 0;

	GFX_CONT_INIT(0);

	GFX_Buffer* internal = (GFX_Buffer*)buffer;
	if(!_gfx_buffer_ref(internal, GFX_CONT_AS_ARG)) return 0;

	/* Clip size */
	size = (offset + size > buffer->size) ?
		buffer->size - offset : size;

#if defined(GFX_RENDERER_GL)

	GFX_REND_GET.NamedBufferSubData(
		*_gfx_buffer_get_handle(internal, internal->current),
		offset,
		size,
		data
	);

#endif

	return size;
}

/******************************************************/
size_t gfx_buffer_copy(

		GFXBuffer*        dest,
		const GFXBuffer*  src,
		size_t            srcOffset,
		size_t            destOffset,
		size_t            size)
{
	/* Derp */
	if(srcOffset >= src->size || destOffset >= dest->size) return 0;

	GFX_CONT_INIT(0);

	GFX_Buffer* intDest = (GFX_Buffer*)dest;
	const GFX_Buffer* intSrc = (const GFX_Buffer*)src;

	if(
		!_gfx_buffer_ref(intDest, GFX_CONT_AS_ARG) ||
		!_gfx_buffer_ref(intSrc, GFX_CONT_AS_ARG))
	{
		return 0;
	}

	/* Clip range */
	size = (destOffset + size > dest->size) ?
		dest->size - destOffset : size;
	size = (srcOffset + size > src->size) ?
		src->size - srcOffset : size;

#if defined(GFX_RENDERER_GL)

	GFX_REND_GET.CopyNamedBufferSubData(
		*_gfx_buffer_get_handle(intSrc, intSrc->current),
		*_gfx_buffer_get_handle(intDest, intDest->current),
		srcOffset,
		destOffset,
		size
	);

#endif

	return size;
}

/******************************************************/
size_t gfx_buffer_copy_same(

		GFXBuffer*     buffer,
		unsigned char  dest,
		size_t         srcOffset,
		size_t         destOffset,
		size_t         size)
{
	/* Derp */
	if(srcOffset >= buffer->size || destOffset >= buffer->size) return 0;

	GFX_CONT_INIT(0);

	GFX_Buffer* internal = (GFX_Buffer*)buffer;
	if(!_gfx_buffer_ref(internal, GFX_CONT_AS_ARG)) return 0;

	/* Clip range */
	size = (destOffset + size > buffer->size) ?
		buffer->size - destOffset : size;
	size = (srcOffset + size > buffer->size) ?
		buffer->size - srcOffset : size;

	/* Get backbuffer to copy to */
	unsigned char copyTo = (internal->current + dest) % buffer->count;

#if defined(GFX_RENDERER_GL)

	GFX_REND_GET.CopyNamedBufferSubData(
		*_gfx_buffer_get_handle(internal, internal->current),
		*_gfx_buffer_get_handle(internal, copyTo),
		srcOffset,
		destOffset,
		size
	);

#endif

	return size;
}

/******************************************************/
void gfx_buffer_orphan(

		GFXBuffer* buffer)
{
	GFX_CONT_INIT();

	GFX_Buffer* internal = (GFX_Buffer*)buffer;
	if(!_gfx_buffer_ref(internal, GFX_CONT_AS_ARG)) return;

#if defined(GFX_RENDERER_GL)

	GFX_REND_GET.NamedBufferData(
		*_gfx_buffer_get_handle(internal, internal->current),
		buffer->size,
		NULL,
		_gfx_buffer_from_usage(buffer->usage)
	);

#endif
}

/******************************************************/
void* gfx_buffer_map(

		GFXBuffer*  buffer,
		size_t*     size,
		size_t      offset)
{
	/* Herpaderp, cannot map */
	if(
		offset >= buffer->size ||
		(!(buffer->usage & GFX_BUFFER_MAP_READ) &&
		!(buffer->usage & GFX_BUFFER_MAP_WRITE)))
	{
		return NULL;
	}

	GFX_CONT_INIT(NULL);

	GFX_Buffer* internal = (GFX_Buffer*)buffer;
	if(!_gfx_buffer_ref(internal, GFX_CONT_AS_ARG)) return NULL;

	/* Clip size */
	*size = (*size + offset > buffer->size) ?
		buffer->size - offset : *size;

	void* ptr = NULL;

#if defined(GFX_RENDERER_GL)

	/* Get access */
	GLbitfield access =
		(buffer->usage & GFX_BUFFER_MAP_READ ? GL_MAP_READ_BIT : 0) |
		(buffer->usage & GFX_BUFFER_MAP_WRITE ? GL_MAP_WRITE_BIT : 0);

	ptr = GFX_REND_GET.MapNamedBufferRange(
		*_gfx_buffer_get_handle(internal, internal->current),
		offset,
		*size,
		access
	);

#endif

	return ptr;
}

/******************************************************/
void gfx_buffer_unmap(

		const GFXBuffer* buffer)
{
	GFX_CONT_INIT();

	const GFX_Buffer* internal = (const GFX_Buffer*)buffer;
	if(!_gfx_buffer_ref(internal, GFX_CONT_AS_ARG)) return;

#if defined(GFX_RENDERER_GL)

	GLboolean success = GFX_REND_GET.UnmapNamedBuffer(
		*_gfx_buffer_get_handle(internal, internal->current));

	if(!success) gfx_errors_push(
		GFX_ERROR_MEMORY_CORRUPTION,
		"Mapping a buffer might have corrupted its memory."
	);

#endif
}
