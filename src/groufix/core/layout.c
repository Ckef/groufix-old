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
/** Object flags associated with all layouts */
#if defined(GFX_RENDERER_GL)
const GFXRenderObjectFlags GFX_VERTEX_LAYOUT_OBJECT_FLAGS = 0;

#elif defined(GFX_RENDERER_VK)
const GFXRenderObjectFlags GFX_VERTEX_LAYOUT_OBJECT_FLAGS =
	GFX_OBJECT_CAN_SHARE;

#endif


/** Internal Layout */
typedef struct GFX_Layout
{
	/* Super class */
	GFXVertexLayout layout;

	/* Hidden data */
	GFX_RenderObjectID  id;
	GFX_LayoutHandle    handle;

	const GFXBuffer*    indexBuffer;
	unsigned char       index;       /* Index of the handle at the index buffer */
	size_t              indexOffset; /* Byte offset to start reading at in the index buffer */

} GFX_Layout;


/** Internal vertex attribute */
typedef struct GFX_Attribute
{
	GFXVertexAttribute  attrib;
	unsigned char       buffer; /* Index into the buffers of the layout (actually index + 1, 0 means not set) */

} GFX_Attribute;


/** Internal vertex buffer */
typedef struct GFX_Buffer
{
	const GFXBuffer*  buffer;
	unsigned char     index; /* Index of the handle at the buffer */

	size_t            offset;
	size_t            stride;
	unsigned int      divisor;

} GFX_Buffer;


/******************************************************/
static inline GFX_Buffer* _gfx_layout_get_buffer(

		const GFX_Layout*  layout,
		unsigned char      index)
{
	return ((GFX_Buffer*)(layout + 1)) + index;
}

/******************************************************/
static inline GFX_Attribute* _gfx_layout_get_attribute(

		const GFX_Layout*  layout,
		unsigned char      index)
{
	void* attr = _gfx_layout_get_buffer(layout, layout->layout.buffers);
	return ((GFX_Attribute*)attr) + index;
}

/******************************************************/
static inline GFXVertexSource* _gfx_layout_get_source(

		const GFX_Layout*  layout,
		unsigned char      index)
{
	void* src = _gfx_layout_get_attribute(layout, layout->layout.attributes);
	return ((GFXVertexSource*)src) + index;
}

/******************************************************/
static inline int _gfx_layout_ref(

		const GFX_Layout* layout,
		GFX_CONT_ARG)
{
	return _gfx_render_object_id_reference(
		&((GFX_Layout*)layout)->id,
		GFX_VERTEX_LAYOUT_OBJECT_FLAGS,
		&GFX_CONT_GET.objects
	);
}

/******************************************************/
static inline int _gfx_layout_check(

		const GFX_Layout* layout,
		GFX_CONT_ARG)
{
	return !layout->handle ||
		(!GFX_CONT_EQ(NULL) && _gfx_layout_ref(layout, GFX_CONT_AS_ARG));
}

#if defined(GFX_RENDERER_GL)

/******************************************************/
static void _gfx_layout_set_attribute_combined(

		GFX_Layout*     layout,
		unsigned char   index,
		GFX_Attribute*  attribute,
		GFX_Buffer*     buffer,
		GFX_CONT_ARG)
{
	int shaderType;
	GLint size;
	GLenum type;
	GLboolean normalized;

	if(!_gfx_gl_format_to_vertex(
		&attribute->attrib,
		&shaderType,
		&size,
		&type,
		&normalized,
		GFX_CONT_AS_ARG))
	{
		return;
	}

	/* The below call will also bind the VAO, as we do not have DSA */
	GFX_REND_GET.EnableVertexArrayAttrib(
		layout->handle,
		index);

	/* Now set the actual values of the attribute */
	GFX_REND_GET.VertexAttribDivisor(
		index,
		buffer->divisor);

	GFX_REND_GET.BindBuffer(
		GL_ARRAY_BUFFER,
		_gfx_buffer_get_handle(buffer->buffer, buffer->index));

	/* Compute total offset into the buffer */
	/* Next set values using the correct combined vertex function */
	size_t offset = buffer->offset + attribute->attrib.offset;

	switch(shaderType)
	{
	case 0 :
		GFX_REND_GET.VertexAttribIPointer(
			index,
			size,
			type,
			buffer->stride,
			GFX_UINT_TO_VOID(offset)
		);
		break;

	case 1 :
		GFX_REND_GET.VertexAttribPointer(
			index,
			size,
			type,
			normalized,
			buffer->stride,
			GFX_UINT_TO_VOID(offset)
		);
		break;

	case 2 :
		GFX_REND_GET.VertexAttribLPointer(
			index,
			size,
			type,
			buffer->stride,
			GFX_UINT_TO_VOID(offset)
		);
		break;
	}
}

#endif

/******************************************************/
static void _gfx_layout_set_index_buffer(

		GFX_Layout* layout,
		GFX_CONT_ARG)
{
#if defined(GFX_RENDERER_GL)

	GLuint buffer = 0;

	if(layout->indexBuffer) buffer = _gfx_buffer_get_handle(
		layout->indexBuffer, layout->index);

	GFX_REND_GET.VertexArrayElementBuffer(
		layout->handle, buffer);

#endif
}

/******************************************************/
static void _gfx_layout_set_vertex_buffer(

		GFX_Layout*    layout,
		unsigned char  index,
		GFX_CONT_ARG)
{
	GFX_Buffer* buffer = _gfx_layout_get_buffer(layout, index);

	/* Check extensions & limits */
	if(buffer->divisor && !GFX_CONT_GET.ext[GFX_EXT_VERTEX_INSTANCING])
	{
		gfx_errors_push(
			GFX_ERROR_INCOMPATIBLE_CONTEXT,
			"GFX_EXT_VERTEX_INSTANCING is incompatible with this context."
		);
		return;
	}

	if(buffer->stride > (size_t)GFX_CONT_GET.lim[GFX_LIM_MAX_VERTEX_STRIDE])
	{
		gfx_errors_push(
			GFX_ERROR_INCOMPATIBLE_CONTEXT,
			"Vertex stride at a layout exceeded GFX_LIM_MAX_VERTEX_STRIDE."
		);
		return;
	}

#if defined(GFX_RENDERER_GL)

	unsigned int i;

	if(!buffer->buffer)
	{
		/* Disable all attributes associated with this buffer */
		for(i = 0; i < layout->layout.attributes; ++i)
		{
			GFX_Attribute* attrib = _gfx_layout_get_attribute(layout, i);

			if(attrib->buffer && attrib->buffer - 1 == index)
				GFX_REND_GET.DisableVertexArrayAttrib(layout->handle, i);
		}
	}
	else
	{
		/* Directly set the state of the vertex buffer */
		if(
			GFX_REND_GET.intExt[GFX_INT_EXT_VERTEX_ATTRIB_BINDING] ||
			GFX_REND_GET.intExt[GFX_INT_EXT_DIRECT_STATE_ACCESS])
		{
			GFX_REND_GET.VertexArrayVertexBuffer(
				layout->handle,
				index,
				_gfx_buffer_get_handle(buffer->buffer, buffer->index),
				buffer->offset,
				buffer->stride);

			GFX_REND_GET.VertexArrayBindingDivisor(
				layout->handle,
				index,
				buffer->divisor);
		}

		/* Enable and set all combined attributes associated with this buffer */
		/* For each attribute, check if it is also set */
		else for(i = 0; i < layout->layout.attributes; ++i)
		{
			GFX_Attribute* attrib = _gfx_layout_get_attribute(layout, i);

			if(
				attrib->buffer &&
				attrib->buffer - 1 == index &&
				gfx_format_is_valid(attrib->attrib.format))
			{
				_gfx_layout_set_attribute_combined(
					layout,
					i,
					attrib,
					buffer,
					GFX_CONT_AS_ARG
				);
			}
		}
	}

#endif
}

/******************************************************/
static void _gfx_layout_set_attribute(

		GFX_Layout*    layout,
		unsigned char  index,
		GFX_CONT_ARG)
{
	GFX_Attribute* attrib = _gfx_layout_get_attribute(layout, index);
	GFX_Buffer* buffer = _gfx_layout_get_buffer(layout, attrib->buffer - 1);

	/* Check extensions & limits */
	if(
		(attrib->attrib.format.type == GFX_DOUBLE || attrib->attrib.type == GFX_DOUBLE) &&
		!GFX_CONT_GET.ext[GFX_EXT_VERTEX_DOUBLE_PRECISION])
	{
		gfx_errors_push(
			GFX_ERROR_INCOMPATIBLE_CONTEXT,
			"GFX_EXT_VERTEX_DOUBLE_PRECISION is incompatible with this context."
		);
		return;
	}

	if(attrib->attrib.offset > (size_t)GFX_CONT_GET.lim[GFX_LIM_MAX_VERTEX_OFFSET])
	{
		gfx_errors_push(
			GFX_ERROR_INCOMPATIBLE_CONTEXT,
			"Vertex offset at a layout exceeded GFX_LIM_MAX_VERTEX_OFFSET."
		);
		return;
	}

#if defined(GFX_RENDERER_GL)

	if(!gfx_format_is_valid(attrib->attrib.format))
		GFX_REND_GET.DisableVertexArrayAttrib(layout->handle, index);
	else
	{
		/* Directly set the state of the vertex attribute */
		if(
			GFX_REND_GET.intExt[GFX_INT_EXT_VERTEX_ATTRIB_BINDING] ||
			GFX_REND_GET.intExt[GFX_INT_EXT_DIRECT_STATE_ACCESS])
		{
			int shaderType;
			GLint size;
			GLenum type;
			GLboolean normalized;

			if(!_gfx_gl_format_to_vertex(
				&attrib->attrib,
				&shaderType,
				&size,
				&type,
				&normalized,
				GFX_CONT_AS_ARG))
			{
				return;
			}

			GFX_REND_GET.EnableVertexArrayAttrib(layout->handle, index);

			/* Use the correct function to set the format */
			switch(shaderType)
			{
			case 0 :
				GFX_REND_GET.VertexArrayAttribIFormat(
					layout->handle,
					index,
					size,
					type,
					attrib->attrib.offset
				);
				break;

			case 1 :
				GFX_REND_GET.VertexArrayAttribFormat(
					layout->handle,
					index,
					size,
					type,
					normalized,
					attrib->attrib.offset
				);
				break;

			case 2 :
				GFX_REND_GET.VertexArrayAttribLFormat(
					layout->handle,
					index,
					size,
					type,
					attrib->attrib.offset
				);
				break;
			}
		}

		/* Enable and set the combined attribute */
		/* Check if the buffer is set also */
		else if(attrib->buffer && buffer->buffer)
		{
			/* Check more extensions & limits */
			/* We check it here because when there is no DSA, init only calls this function */
			if(buffer->divisor && !GFX_CONT_GET.ext[GFX_EXT_VERTEX_INSTANCING])
			{
				gfx_errors_push(
					GFX_ERROR_INCOMPATIBLE_CONTEXT,
					"GFX_EXT_VERTEX_INSTANCING is incompatible with this context."
				);
				return;
			}

			if(buffer->stride > (size_t)GFX_CONT_GET.lim[GFX_LIM_MAX_VERTEX_STRIDE])
			{
				gfx_errors_push(
					GFX_ERROR_INCOMPATIBLE_CONTEXT,
					"Vertex stride at a layout exceeded GFX_LIM_MAX_VERTEX_STRIDE."
				);
				return;
			}

			_gfx_layout_set_attribute_combined(
				layout,
				index,
				attrib,
				buffer,
				GFX_CONT_AS_ARG
			);
		}
	}

#endif
}

/******************************************************/
static void _gfx_layout_set_attribute_buffer(

		GFX_Layout*    layout,
		unsigned char  index,
		GFX_CONT_ARG)
{
	GFX_Attribute* attrib = _gfx_layout_get_attribute(layout, index);
	GFX_Buffer* buffer = _gfx_layout_get_buffer(layout, attrib->buffer - 1);

#if defined(GFX_RENDERER_GL)

	if(attrib->buffer)
	{
		/* Directly bind the buffer */
		if(
			GFX_REND_GET.intExt[GFX_INT_EXT_VERTEX_ATTRIB_BINDING] ||
			GFX_REND_GET.intExt[GFX_INT_EXT_DIRECT_STATE_ACCESS])
		{
			GFX_REND_GET.VertexArrayAttribBinding(
				layout->handle,
				index,
				attrib->buffer - 1);
		}

		/* Set the combined attribute */
		/* First check both the buffer and attribute are set */
		else if(buffer->buffer && gfx_format_is_valid(attrib->attrib.format))
		{
			_gfx_layout_set_attribute_combined(
				layout,
				index,
				attrib,
				buffer,
				GFX_CONT_AS_ARG);
		}
	}

#endif
}

/******************************************************/
static void _gfx_layout_clear(

		GFX_Layout* layout,
		GFX_CONT_ARG)
{
#if defined(GFX_RENDERER_GL)

	GFX_REND_GET.DeleteVertexArrays(1, &layout->handle);

#endif

	memset(&layout->handle, 0, sizeof(GFX_LayoutHandle));
}

/******************************************************/
static int _gfx_layout_init(

		GFX_Layout* layout,
		GFX_CONT_ARG)
{
	/* Check limits */
	if(layout->layout.buffers > GFX_CONT_GET.lim[GFX_LIM_MAX_VERTEX_BUFFERS])
	{
		gfx_errors_push(
			GFX_ERROR_INCOMPATIBLE_CONTEXT,
			"Number of buffers at a layout exceeded GFX_LIM_MAX_VERTEX_BUFFERS."
		);
		return 0;
	}

	if(layout->layout.attributes > GFX_CONT_GET.lim[GFX_LIM_MAX_VERTEX_ATTRIBS])
	{
		gfx_errors_push(
			GFX_ERROR_INCOMPATIBLE_CONTEXT,
			"Number of attributes at a layout exceeded GFX_LIM_MAX_VERTEX_ATTRIBS."
		);
		return 0;
	}

#if defined(GFX_RENDERER_GL)

	/* Allocate layout */
	GFX_REND_GET.CreateVertexArrays(1, &layout->handle);
	if(!layout->handle)
	{
		_gfx_layout_clear(layout, GFX_CONT_AS_ARG);
		return 0;
	}

#endif

	unsigned char i;

	/* Set all buffers and attributes */
	_gfx_layout_set_index_buffer(layout, GFX_CONT_AS_ARG);

	for(i = 0; i < layout->layout.attributes; ++i)
		_gfx_layout_set_attribute(layout, i, GFX_CONT_AS_ARG);

#if defined(GFX_RENDERER_GL)

	/* Skip vertex and attribute buffer if no DSA */
	if(
		!GFX_REND_GET.intExt[GFX_INT_EXT_VERTEX_ATTRIB_BINDING] &&
		!GFX_REND_GET.intExt[GFX_INT_EXT_DIRECT_STATE_ACCESS])
	{
		return 1;
	}

#endif

	for(i = 0; i < layout->layout.buffers; ++i)
		_gfx_layout_set_vertex_buffer(layout, i, GFX_CONT_AS_ARG);

	for(i = 0; i < layout->layout.attributes; ++i)
		_gfx_layout_set_attribute_buffer(layout, i, GFX_CONT_AS_ARG);

	return 1;
}

/******************************************************/
static void _gfx_layout_obj_destruct(

		GFX_RenderObjectIDArg arg)
{
	GFX_CONT_INIT_UNSAFE;

	GFX_Layout* layout = GFX_PTR_SUB_BYTES(arg, offsetof(GFX_Layout, id));
	_gfx_layout_clear(layout, GFX_CONT_AS_ARG);
}

/******************************************************/
static void _gfx_layout_obj_prepare(

		GFX_RenderObjectIDArg  arg,
		void**                 temp,
		int                    shared)
{
	GFX_CONT_INIT_UNSAFE;

	GFX_Layout* layout = GFX_PTR_SUB_BYTES(arg, offsetof(GFX_Layout, id));
	_gfx_layout_clear(layout, GFX_CONT_AS_ARG);
}

/******************************************************/
static void _gfx_layout_obj_transfer(

		GFX_RenderObjectIDArg  arg,
		void**                 temp,
		int                    shared)
{
	GFX_CONT_INIT_UNSAFE;

	GFX_Layout* layout = GFX_PTR_SUB_BYTES(arg, offsetof(GFX_Layout, id));
	_gfx_layout_init(layout, GFX_CONT_AS_ARG);
}

/******************************************************/
/** vtable for render object part of the layout */
static const GFX_RenderObjectFuncs _gfx_layout_obj_funcs =
{
	.destruct = _gfx_layout_obj_destruct,
	.prepare  = _gfx_layout_obj_prepare,
	.transfer = _gfx_layout_obj_transfer
};

/******************************************************/
GFX_LayoutHandle _gfx_vertex_layout_get_handle(

		const GFXVertexLayout* layout)
{
	GFX_Layout* internal = (GFX_Layout*)layout;

	/* Initialize the layout if it wasn't already and we can */
	if(!internal->handle)
	{
		GFX_CONT_INIT_UNSAFE;

		if(!GFX_CONT_EQ(NULL) && _gfx_layout_ref(internal, GFX_CONT_AS_ARG))
			_gfx_layout_init(internal, GFX_CONT_AS_ARG);
	}

	return internal->handle;
}

/******************************************************/
GFXVertexLayout* gfx_vertex_layout_create(

		unsigned char  buffers,
		unsigned char  attributes,
		unsigned char  sources)
{
	/* Yeah, okay... no. */
	if(!buffers || !attributes || !sources) return NULL;

	/* Create new layout, append buffers, attributes and sources at the end of the struct */
	size_t alloc =
		sizeof(GFX_Layout) +
		sizeof(GFX_Buffer) * buffers +
		sizeof(GFX_Attribute) * attributes +
		sizeof(GFXVertexSource) * sources;

	GFX_Layout* layout = calloc(1, alloc);
	if(!layout)
	{
		/* Out of memory error */
		gfx_errors_output(
			"[GFX Out of Memory]: Vertex Layout could not be allocated."
		);
		return NULL;
	}

	/* Initialize the layout */
	layout->layout.buffers    = buffers;
	layout->layout.attributes = attributes;
	layout->layout.sources    = sources;

	/* Initialize as object */
	if(!_gfx_render_object_id_init(
		&layout->id,
		GFX_VERTEX_LAYOUT_OBJECT_ORDER,
		GFX_VERTEX_LAYOUT_OBJECT_FLAGS,
		&_gfx_layout_obj_funcs,
		NULL))
	{
		free(layout);
		return NULL;
	}

	return (GFXVertexLayout*)layout;
}

/******************************************************/
void gfx_vertex_layout_free(

		GFXVertexLayout* layout)
{
	if(layout)
	{
		/* Check context */
		GFX_CONT_INIT_UNSAFE;

		if(!_gfx_layout_check((GFX_Layout*)layout, GFX_CONT_AS_ARG))
			return;

		/* Clear as object */
		/* Object clearing will call the destruct callback */
		_gfx_render_object_id_clear(&((GFX_Layout*)layout)->id);
		free(layout);
	}
}

/******************************************************/
int gfx_vertex_layout_set_index_buffer(

		GFXVertexLayout*  layout,
		const GFXBuffer*  buffer,
		size_t            offset)
{
	/* Check context */
	GFX_CONT_INIT_UNSAFE;

	GFX_Layout* internal = (GFX_Layout*)layout;
	if(!_gfx_layout_check(internal, GFX_CONT_AS_ARG)) return 0;

	/* Set buffer */
	if(!buffer)
		internal->indexBuffer = NULL;
	else
	{
		internal->indexBuffer = buffer;
		internal->index       = _gfx_buffer_get_current(buffer);
		internal->indexOffset = offset;
	}

	/* Pass along to renderer */
	if(internal->handle)
		_gfx_layout_set_index_buffer(internal, GFX_CONT_AS_ARG);

	return 1;
}

/******************************************************/
int gfx_vertex_layout_get_index_buffer(

		const GFXVertexLayout*  layout,
		GFXBuffer**             buffer,
		size_t*                 offset)
{
	/* Nothing to return */
	const GFX_Layout* internal = (const GFX_Layout*)layout;
	if(!internal->indexBuffer) return 0;

	/* Check context */
	GFX_CONT_INIT_UNSAFE;
	if(!_gfx_layout_check(internal, GFX_CONT_AS_ARG)) return 0;

	*buffer = (GFXBuffer*)internal->indexBuffer;
	*offset = internal->indexOffset;

	return 1;
}

/******************************************************/
int gfx_vertex_layout_set_vertex_buffer(

		GFXVertexLayout*  layout,
		unsigned char     index,
		const GFXBuffer*  buffer,
		size_t            offset,
		size_t            stride,
		unsigned int      divisor)
{
	if(index >= layout->buffers) return 0;

	/* Check context */
	GFX_CONT_INIT_UNSAFE;

	GFX_Layout* internal = (GFX_Layout*)layout;
	if(!_gfx_layout_check(internal, GFX_CONT_AS_ARG)) return 0;

	/* Set buffer */
	GFX_Buffer* buff = _gfx_layout_get_buffer(internal, index);

	if(!buffer)
		buff->buffer = NULL;
	else
	{
		buff->buffer  = buffer;
		buff->index   = _gfx_buffer_get_current(buffer);
		buff->offset  = offset;
		buff->stride  = stride;
		buff->divisor = divisor;
	}

	/* Pass along to renderer */
	if(internal->handle)
		_gfx_layout_set_vertex_buffer(internal, index, GFX_CONT_AS_ARG);

	return 1;
}

/******************************************************/
int gfx_vertex_layout_get_vertex_buffer(

		const GFXVertexLayout*  layout,
		unsigned char           index,
		GFXBuffer**             buffer,
		size_t*                 offset,
		size_t*                 stride,
		unsigned int*           divisor)
{
	/* Get buffer and check if there's something */
	const GFX_Layout* internal = (const GFX_Layout*)layout;
	GFX_Buffer* buff = _gfx_layout_get_buffer(internal, index);

	if(index >= layout->buffers || !buff->buffer) return 0;

	/* Check context */
	GFX_CONT_INIT_UNSAFE;
	if(!_gfx_layout_check(internal, GFX_CONT_AS_ARG)) return 0;

	*buffer = (GFXBuffer*)buff->buffer;
	*offset = buff->offset;
	*stride = buff->stride;
	*divisor = buff->divisor;

	return 1;
}

/******************************************************/
int gfx_vertex_layout_set_attribute(

		GFXVertexLayout*           layout,
		unsigned char              index,
		const GFXVertexAttribute*  attrib)
{
	if(index >= layout->attributes || !gfx_format_is_valid(attrib->format))
		return 0;

	/* Check context */
	GFX_CONT_INIT_UNSAFE;

	GFX_Layout* internal = (GFX_Layout*)layout;
	if(!_gfx_layout_check(internal, GFX_CONT_AS_ARG)) return 0;

	/* Set attribute */
	GFX_Attribute* attr = _gfx_layout_get_attribute(internal, index);

	if(!attrib)
		gfx_format_invalidate(&attr->attrib.format);
	else
		attr->attrib = *attrib;

	/* Pass along to renderer */
	if(internal->handle)
		_gfx_layout_set_attribute(internal, index, GFX_CONT_AS_ARG);

	return 1;
}

/******************************************************/
int gfx_vertex_layout_get_attribute(

		const GFXVertexLayout*  layout,
		unsigned char           index,
		GFXVertexAttribute*     attrib)
{
	/* Get attribute and check if there's something */
	const GFX_Layout* internal = (const GFX_Layout*)layout;
	GFX_Attribute* attr = _gfx_layout_get_attribute(internal, index);

	if(index >= layout->attributes || !gfx_format_is_valid(attr->attrib.format))
		return 0;

	/* Check context */
	GFX_CONT_INIT_UNSAFE;
	if(!_gfx_layout_check(internal, GFX_CONT_AS_ARG)) return 0;

	*attrib = attr->attrib;

	return 1;
}

/******************************************************/
int gfx_vertex_layout_set_attribute_buffer(

		GFXVertexLayout*  layout,
		unsigned char     attribute,
		unsigned char     buffer)
{
	if(attribute >= layout->attributes || buffer >= layout->buffers)
		return 0;

	/* Check context */
	GFX_CONT_INIT_UNSAFE;

	GFX_Layout* internal = (GFX_Layout*)layout;
	if(!_gfx_layout_check(internal, GFX_CONT_AS_ARG)) return 0;

	/* Set attribute buffer binding */
	/* Add 1 to the buffer index as 0 means it is not set */
	GFX_Attribute* attr = _gfx_layout_get_attribute(internal, attribute);
	attr->buffer = buffer + 1;

	/* Pass along to renderer */
	if(internal->handle)
		_gfx_layout_set_attribute_buffer(internal, attribute, GFX_CONT_AS_ARG);

	return 1;
}

/******************************************************/
int gfx_vertex_layout_get_attribute_buffer(

		const GFXVertexLayout*  layout,
		unsigned char           attribute,
		unsigned char*          buffer)
{
	/* Get attribute and check if there's something */
	const GFX_Layout* internal = (const GFX_Layout*)layout;
	GFX_Attribute* attr = _gfx_layout_get_attribute(internal, attribute);

	if(attribute >= layout->attributes || !attr->buffer)
		return 0;

	/* Check context */
	GFX_CONT_INIT_UNSAFE;
	if(!_gfx_layout_check(internal, GFX_CONT_AS_ARG)) return 0;

	/* Subtract 1 from the buffer index as 0 means it is not set */
	*buffer = attr->buffer - 1;

	return 1;
}

/******************************************************/
int gfx_vertex_layout_set_source(

		GFXVertexLayout*        layout,
		unsigned char           index,
		const GFXVertexSource*  src)
{
	if(index >= layout->sources || !src->count) return 0;

	/* Check context */
	GFX_CONT_INIT_UNSAFE;

	GFX_Layout* internal = (GFX_Layout*)layout;
	if(!_gfx_layout_check(internal, GFX_CONT_AS_ARG)) return 0;

	/* Set attribute */
	GFXVertexSource* source = _gfx_layout_get_source(internal, index);

	if(!src)
		source->count = 0;
	else
		*source = *src;

	return 1;
}

/******************************************************/
int gfx_vertex_layout_get_source(

		const GFXVertexLayout*  layout,
		unsigned char           index,
		GFXVertexSource*        src)
{
	/* Get source and check if there's something */
	const GFX_Layout* internal = (const GFX_Layout*)layout;
	GFXVertexSource* source = _gfx_layout_get_source(internal, index);

	if(index >= layout->sources || !source->count)
		return 0;

	/* Check context */
	GFX_CONT_INIT_UNSAFE;
	if(!_gfx_layout_check(internal, GFX_CONT_AS_ARG)) return 0;

	*src = *source;

	return 1;
}
