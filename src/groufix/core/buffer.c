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

#include "groufix/core/utils.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
/** Object flags associated with all buffers */
#if defined(GFX_RENDERER_GL)
const GFXRenderObjectFlags GFX_BUFFER_OBJECT_FLAGS =
	GFX_OBJECT_NEEDS_REFERENCE |
	GFX_OBJECT_CAN_SHARE;

#elif defined(GFX_RENDERER_VK)
const GFXRenderObjectFlags GFX_BUFFER_OBJECT_FLAGS =
	GFX_OBJECT_NEEDS_REFERENCE |
	GFX_OBJECT_CAN_SHARE;

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
static inline GFX_BufferHandle* _gfx_buffer_get(

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
		GFX_BUFFER_OBJECT_FLAGS,
		&GFX_CONT_GET.objects
	);
}

/******************************************************/
static inline int _gfx_buffer_check(

		const GFX_Buffer* buffer,
		GFX_CONT_ARG)
{
	return *_gfx_buffer_get(buffer, 0) &&
		_gfx_buffer_ref(buffer, GFX_CONT_AS_ARG);
}

#if defined(GFX_RENDERER_GL)

/******************************************************/
static inline GLbitfield _gfx_buffer_from_usage(

		GFXBufferUsage usage,
		GFX_CONT_ARG)
{
	return
		/* Some flags so GL fallback functions will be able to do stuff */
		(GFX_REND_GET.intExt[GFX_INT_EXT_BUFFER_INVALIDATION] ?
			0 : GL_MAP_WRITE_BIT) |
		(GFX_REND_GET.intExt[GFX_INT_EXT_BUFFER_READ] ?
			0 : GL_MAP_READ_BIT) |

		/* Actual flags */
		(usage & GFX_BUFFER_CLIENT_STORAGE ? GL_CLIENT_STORAGE_BIT : 0) |
		(usage & GFX_BUFFER_WRITE ? GL_DYNAMIC_STORAGE_BIT : 0) |
		(usage & GFX_BUFFER_MAP_READ ? GL_MAP_READ_BIT : 0) |
		(usage & GFX_BUFFER_MAP_WRITE ? GL_MAP_WRITE_BIT : 0) |
		(usage & GFX_BUFFER_MAP_PERSISTENT ? GL_MAP_PERSISTENT_BIT : 0);
}

/******************************************************/
static inline GLbitfield _gfx_buffer_from_usage_to_access(

		GFXBufferUsage usage)
{
	return
		(usage & GFX_BUFFER_MAP_READ ? GL_MAP_READ_BIT : 0) |
		(usage & GFX_BUFFER_MAP_WRITE ? GL_MAP_WRITE_BIT : 0) |
		(usage & GFX_BUFFER_MAP_PERSISTENT ? GL_MAP_PERSISTENT_BIT : 0);
}

/******************************************************/
static inline GLenum _gfx_buffer_from_usage_to_usage(

		GFXBufferUsage usage)
{
	return

		/* Client storage */
		usage & GFX_BUFFER_CLIENT_STORAGE ?

		((usage & GFX_BUFFER_WRITE || usage & GFX_BUFFER_MAP_WRITE) ?
			GL_STREAM_DRAW :
		(usage & GFX_BUFFER_READ || usage & GFX_BUFFER_MAP_READ) ?
			GL_STREAM_READ : GL_STREAM_COPY) :

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
static void _gfx_buffer_clear(

		GFX_Buffer* buffer,
		GFX_CONT_ARG)
{
#if defined(GFX_RENDERER_GL)

	GFX_REND_GET.DeleteBuffers(
		buffer->buffer.count,
		_gfx_buffer_get(buffer, 0));

#endif

	memset(
		_gfx_buffer_get(buffer, 0),
		0,
		buffer->buffer.count * sizeof(GFX_BufferHandle));
}

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
		buffer->buffer.count, _gfx_buffer_get(buffer, 0));
	GLbitfield bf = _gfx_buffer_from_usage(
		buffer->buffer.usage, GFX_CONT_AS_ARG);
	GLenum us = _gfx_buffer_from_usage_to_usage(
		buffer->buffer.usage);

	unsigned char i;

	/* Check if any buffers weren't created */
	for(i = 0; i < buffer->buffer.count; ++i)
		if(!(*_gfx_buffer_get(buffer, i)))
		{
			_gfx_buffer_clear(buffer, GFX_CONT_AS_ARG);
			return 0;
		}

	/* Only write data to a subset of buffers */
	for(i = 0; i < buffer->buffer.count; ++i)
	{
		const void* d = NULL;
		if(i < num) d = data[i];

		/* Check if we can use buffer storage */
		if(
			GFX_REND_GET.intExt[GFX_INT_EXT_BUFFER_STORAGE] ||
			GFX_REND_GET.intExt[GFX_INT_EXT_DIRECT_STATE_ACCESS])
		{
			GFX_REND_GET.NamedBufferStorage(
				*_gfx_buffer_get(buffer, i),
				buffer->buffer.size,
				d,
				bf);
		}
		else
		{
			GFX_REND_GET.NamedBufferData(
				*_gfx_buffer_get(buffer, i),
				buffer->buffer.size,
				d,
				us);
		}
	}

#endif

	return 1;
}

/******************************************************/
static void _gfx_buffer_obj_destruct(

		GFX_RenderObjectIDArg arg)
{
	GFX_CONT_INIT_UNSAFE;

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

	/* Create temporary storage */
	GFX_Buffer* buffer = GFX_PTR_SUB_BYTES(arg, offsetof(GFX_Buffer, id));
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
				*_gfx_buffer_get(buffer, i),
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

	/* Get pointers */
	GFX_Buffer* buffer = GFX_PTR_SUB_BYTES(arg, offsetof(GFX_Buffer, id));

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
/** vtable for render object part of the buffer */
static const GFX_RenderObjectFuncs _gfx_buffer_obj_funcs =
{
	.destruct = _gfx_buffer_obj_destruct,
	.prepare  = _gfx_buffer_obj_prepare,
	.transfer = _gfx_buffer_obj_transfer
};

/******************************************************/
unsigned char _gfx_buffer_get_current(

		const GFXBuffer* buffer)
{
	return ((GFX_Buffer*)buffer)->current;
}

/******************************************************/
GFX_BufferHandle _gfx_buffer_get_handle(

		const GFXBuffer*  buffer,
		unsigned char     index)
{
	return *_gfx_buffer_get((const GFX_Buffer*)buffer, index);
}

/******************************************************/
GFXBuffer* gfx_buffer_create(

		GFXBufferUsage  usage,
		size_t          size,
		const void*     data,
		unsigned char   count)
{
	/* Herpaderp */
	if(
		!size ||
		((usage & GFX_BUFFER_MAP_PERSISTENT) &&
		!(usage & GFX_BUFFER_MAP_READ) && !(usage & GFX_BUFFER_MAP_WRITE)))
	{
		return NULL;
	}

	GFX_CONT_INIT(NULL);

	/* Always create at least one buffer */
	count = count ? count : 1;

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

	/* Create the actual buffer contents */
	buffer->buffer.usage = usage;
	buffer->buffer.size = size;
	buffer->buffer.count = count;

	if(!_gfx_buffer_init(buffer, 1, &data, GFX_CONT_AS_ARG))
	{
		free(buffer);
		return NULL;
	}

	/* Initialize as object */
	if(!_gfx_render_object_id_init(
		&buffer->id,
		GFX_BUFFER_OBJECT_ORDER,
		GFX_BUFFER_OBJECT_FLAGS,
		&_gfx_buffer_obj_funcs,
		&GFX_CONT_GET.objects))
	{
		_gfx_buffer_clear(buffer, GFX_CONT_AS_ARG);
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
	gfx_buffer_copy(buffer, (GFXBuffer*)src, 0, 0, src->size);

	return buffer;
}

/******************************************************/
void gfx_buffer_free(

		GFXBuffer* buffer)
{
	if(buffer)
	{
		if(*_gfx_buffer_get((GFX_Buffer*)buffer, 0))
		{
			/* Check context */
			GFX_CONT_INIT();

			/* Check if sharing context */
			if(!_gfx_buffer_ref((GFX_Buffer*)buffer, GFX_CONT_AS_ARG))
				return;
		}

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
	/* Check context */
	GFX_CONT_INIT();

	GFX_Buffer* internal = (GFX_Buffer*)buffer;
	if(!_gfx_buffer_check(internal, GFX_CONT_AS_ARG)) return;

	/* Get new current index */
	internal->current = (internal->current + 1) % buffer->count;
}

/******************************************************/
size_t gfx_buffer_read(

		const GFXBuffer*  buffer,
		size_t            size,
		void*             data,
		size_t            offset)
{
	/* Derp */
	if(!size || offset >= buffer->size) return 0;

	/* Check context */
	GFX_CONT_INIT(0);

	const GFX_Buffer* internal = (const GFX_Buffer*)buffer;
	if(!_gfx_buffer_check(internal, GFX_CONT_AS_ARG)) return 0;

	/* Clip offset and size */
	size = (offset + size > buffer->size) ?
		buffer->size - offset : size;

#if defined(GFX_RENDERER_GL)

	GFX_REND_GET.GetNamedBufferSubData(
		*_gfx_buffer_get(internal, internal->current),
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
	if(!size || offset >= buffer->size) return 0;

	/* Check context */
	GFX_CONT_INIT(0);

	GFX_Buffer* internal = (GFX_Buffer*)buffer;
	if(!_gfx_buffer_check(internal, GFX_CONT_AS_ARG)) return 0;

	/* Clip size */
	size = (offset + size > buffer->size) ?
		buffer->size - offset : size;

#if defined(GFX_RENDERER_GL)

	GFX_REND_GET.NamedBufferSubData(
		*_gfx_buffer_get(internal, internal->current),
		offset,
		size,
		data
	);

#endif

	return size;
}

/******************************************************/
size_t gfx_buffer_copy(

		GFXBuffer*  dest,
		GFXBuffer*  src,
		size_t      srcOffset,
		size_t      destOffset,
		size_t      size)
{
	/* Derp */
	if(!size || srcOffset >= src->size || destOffset >= dest->size)
		return 0;

	/* Check context */
	GFX_CONT_INIT(0);

	GFX_Buffer* intDest = (GFX_Buffer*)dest;
	GFX_Buffer* intSrc = (GFX_Buffer*)src;

	if(
		!_gfx_buffer_check(intDest, GFX_CONT_AS_ARG) ||
		!_gfx_buffer_check(intSrc, GFX_CONT_AS_ARG))
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
		*_gfx_buffer_get(intSrc, intSrc->current),
		*_gfx_buffer_get(intDest, intDest->current),
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
	if(!size || srcOffset >= buffer->size || destOffset >= buffer->size)
		return 0;

	/* Check context */
	GFX_CONT_INIT(0);

	GFX_Buffer* internal = (GFX_Buffer*)buffer;
	if(!_gfx_buffer_check(internal, GFX_CONT_AS_ARG)) return 0;

	/* Clip range */
	size = (destOffset + size > buffer->size) ?
		buffer->size - destOffset : size;
	size = (srcOffset + size > buffer->size) ?
		buffer->size - srcOffset : size;

	/* Get backbuffer to copy to */
	unsigned char copyTo = (internal->current + dest) % buffer->count;

#if defined(GFX_RENDERER_GL)

	GFX_REND_GET.CopyNamedBufferSubData(
		*_gfx_buffer_get(internal, internal->current),
		*_gfx_buffer_get(internal, copyTo),
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
	/* Check context */
	GFX_CONT_INIT();

	GFX_Buffer* internal = (GFX_Buffer*)buffer;
	if(!_gfx_buffer_check(internal, GFX_CONT_AS_ARG)) return;

#if defined(GFX_RENDERER_GL)

	GFX_REND_GET.InvalidateBufferSubData(
		*_gfx_buffer_get(internal, internal->current),
		0,
		buffer->size
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
		!size ||
		offset >= buffer->size ||
		(!(buffer->usage & GFX_BUFFER_MAP_READ) &&
		!(buffer->usage & GFX_BUFFER_MAP_WRITE)))
	{
		return NULL;
	}

	/* Check context */
	GFX_CONT_INIT(NULL);

	GFX_Buffer* internal = (GFX_Buffer*)buffer;
	if(!_gfx_buffer_check(internal, GFX_CONT_AS_ARG)) return NULL;

	/* Clip size */
	*size = (*size + offset > buffer->size) ?
		buffer->size - offset : *size;

	void* ptr = NULL;

#if defined(GFX_RENDERER_GL)

	ptr = GFX_REND_GET.MapNamedBufferRange(
		*_gfx_buffer_get(internal, internal->current),
		offset,
		*size,
		_gfx_buffer_from_usage_to_access(buffer->usage)
	);

#endif

	return ptr;
}

/******************************************************/
int gfx_buffer_unmap(

		const GFXBuffer* buffer)
{
	/* Check context */
	GFX_CONT_INIT(0);

	const GFX_Buffer* internal = (const GFX_Buffer*)buffer;
	if(!_gfx_buffer_check(internal, GFX_CONT_AS_ARG)) return 0;

#if defined(GFX_RENDERER_GL)

	GLboolean success = GFX_REND_GET.UnmapNamedBuffer(
		*_gfx_buffer_get(internal, internal->current));

#endif

	return success;
}
