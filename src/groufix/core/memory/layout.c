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

#include "groufix/core/memory/internal.h"
#include "groufix/core/pipeline/internal.h"
#include "groufix/core/errors.h"

#include <stdlib.h>

/******************************************************/
/* Internal draw function */
typedef void (*GFX_DrawFunc)(

		GFXDrawCall*,
		size_t,
		size_t,
		unsigned int,
		GFX_WIND_ARG);


/* Internal vertex attribute */
struct GFX_Attribute
{
	unsigned char     size;
	GLenum            type;
	GFXInterpretType  interpret;
	GLsizei           stride;
	GLuint            divisor;

	GLuint            buffer; /* Vertex buffer */
	size_t            offset; /* Offset within the buffer */
};

/* Internal transform feedback buffer */
struct GFX_TFBuffer
{
	GLuint      buffer;
	GLintptr    offset;
	GLsizeiptr  size;
};

/* Internal Vertex layout */
struct GFX_Layout
{
	/* Super class */
	GFXVertexLayout layout;

	/* Hidden data */
	GLuint                vao;           /* OpenGL handle */
	GFXVector             attributes;    /* Stores GFX_Attribute */
	unsigned int          instanced;     /* Non zero if any attribute has a divisor */

	GFXPrimitive          TFPrimitive;   /* Feedback output primitive */
	size_t                TFNumBuffers;
	struct GFX_TFBuffer*  TFBuffers;     /* Transform Feedback buffers */

	GLint                 patchVertices; /* Number of vertices per patch */
	GLuint                indexBuffer;
	size_t                indexOffset;   /* Byte offset into index buffer */
};

/******************************************************/
void _gfx_vertex_layout_bind(

		GLuint vao,
		GFX_WIND_ARG)
{
	/* Prevent binding it twice */
	if(GFX_REND_GET.vao != vao)
	{
		GFX_REND_GET.vao = vao;
		GFX_REND_GET.BindVertexArray(vao);
	}
}

/******************************************************/
static void _gfx_layout_draw(

		GFXDrawCall*  call,
		size_t        offset,
		size_t        inst,
		unsigned int  base,
		GFX_WIND_ARG)
{
	GFX_REND_GET.DrawArrays(
		call->primitive,
		call->first,
		call->count
	);
}

/******************************************************/
static void _gfx_layout_draw_indexed(

		GFXDrawCall*  call,
		size_t        offset,
		size_t        inst,
		unsigned int  base,
		GFX_WIND_ARG)
{
	GFX_REND_GET.DrawElements(
		call->primitive,
		call->count,
		call->indexType,
		(GLvoid*)(offset + call->first)
	);
}

/******************************************************/
static void _gfx_layout_draw_instanced(

		GFXDrawCall*  call,
		size_t        offset,
		size_t        inst,
		unsigned int  base,
		GFX_WIND_ARG)
{
	GFX_REND_GET.DrawArraysInstanced(
		call->primitive,
		call->first,
		call->count,
		inst
	);
}

/******************************************************/
static void _gfx_layout_draw_indexed_instanced(

		GFXDrawCall*  call,
		size_t        offset,
		size_t        inst,
		unsigned int  base,
		GFX_WIND_ARG)
{
	GFX_REND_GET.DrawElementsInstanced(
		call->primitive,
		call->count,
		call->indexType,
		(GLvoid*)(offset + call->first),
		inst
	);
}

/******************************************************/
static void _gfx_layout_draw_instanced_base(

		GFXDrawCall*  call,
		size_t        offset,
		size_t        inst,
		unsigned int  base,
		GFX_WIND_ARG)
{
	GFX_REND_GET.DrawArraysInstancedBaseInstance(
		call->primitive,
		call->first,
		call->count,
		inst,
		base
	);
}

/******************************************************/
static void _gfx_layout_draw_indexed_instanced_base(

		GFXDrawCall*  call,
		size_t        offset,
		size_t        inst,
		unsigned int  base,
		GFX_WIND_ARG)
{
	GFX_REND_GET.DrawElementsInstancedBaseInstance(
		call->primitive,
		call->count,
		call->indexType,
		(GLvoid*)(offset + call->first),
		inst,
		base
	);
}

/******************************************************/
static void _gfx_layout_invoke_draw(

		struct GFX_Layout*  layout,
		unsigned char       startIndex,
		unsigned char       num,
		size_t              inst,
		unsigned int        base,
		GFX_DrawType        type,
		GFX_WIND_ARG)
{
	/* Jump table */
	static const GFX_DrawFunc jump[] =
	{
		_gfx_layout_draw,
		_gfx_layout_draw_indexed,
		_gfx_layout_draw_instanced,
		_gfx_layout_draw_indexed_instanced,
		_gfx_layout_draw_instanced_base,
		_gfx_layout_draw_indexed_instanced_base
	};

	/* Get table index */
	type = (type << 1) + (layout->indexBuffer ? 1 : 0);

	/* Iterate over all draw calls & invoke */
	GFXDrawCall* call = (GFXDrawCall*)(layout + 1) + startIndex;
	while(num--) jump[type](
		call++,
		layout->indexOffset,
		inst,
		base,
		GFX_WIND_AS_ARG
	);
}

/******************************************************/
static void _gfx_layout_init_attrib(

		GLuint                       vao,
		unsigned int                 index,
		const struct GFX_Attribute*  attr,
		GFX_WIND_ARG)
{
	/* Check if enabled */
	if(attr->size && attr->buffer)
	{
		/* Set the attribute */
		GFX_REND_GET.EnableVertexArrayAttrib(vao, index);
		GFX_REND_GET.BindBuffer(GL_ARRAY_BUFFER, attr->buffer);
		_gfx_vertex_layout_bind(vao, GFX_WIND_AS_ARG);

		/* Check integer value */
		if(attr->interpret & GFX_INTERPRET_INTEGER)
		{
			GFX_REND_GET.VertexAttribIPointer(
				index,
				attr->size,
				attr->type,
				attr->stride,
				(GLvoid*)attr->offset);
		}
		else
		{
			GFX_REND_GET.VertexAttribPointer(
				index,
				attr->size,
				attr->type,
				attr->interpret & GFX_INTERPRET_NORMALIZED ? GL_TRUE : GL_FALSE,
				attr->stride,
				(GLvoid*)attr->offset);
		}

		/* Check if non-zero to avoid extension error */
		if(attr->divisor)
			GFX_REND_GET.VertexAttribDivisor(index, attr->divisor);
	}

	/* Disable it */
	else GFX_REND_GET.DisableVertexArrayAttrib(vao, index);
}

/******************************************************/
static void _gfx_layout_obj_free(

		void*         object,
		unsigned int  id)
{
	struct GFX_Layout* layout = (struct GFX_Layout*)object;

	layout->layout.id = id;
	layout->vao = 0;

	gfx_vector_clear(&layout->attributes);
}

/******************************************************/
static void _gfx_layout_obj_save(

		void*         object,
		unsigned int  id)
{
	GFX_WIND_INIT_UNSAFE;

	struct GFX_Layout* layout = (struct GFX_Layout*)object;

	/* Just don't clear the attribute vector */
	layout->layout.id = id;
	GFX_REND_GET.DeleteVertexArrays(1, &layout->vao);
	layout->vao = 0;
}

/******************************************************/
static void _gfx_layout_obj_restore(

		void*         object,
		unsigned int  id)
{
	GFX_WIND_INIT_UNSAFE;

	struct GFX_Layout* layout = (struct GFX_Layout*)object;

	/* Create VAO */
	layout->layout.id = id;
	GFX_REND_GET.CreateVertexArrays(1, &layout->vao);

	/* Restore attributes */
	unsigned int i = 0;

	GFXVectorIterator it = layout->attributes.begin;
	while(it != layout->attributes.end)
	{
		_gfx_layout_init_attrib(
			layout->vao,
			i++,
			(struct GFX_Attribute*)it,
			GFX_WIND_AS_ARG
		);

		it = gfx_vector_next(&layout->attributes, it);
	}
}

/******************************************************/
/* vtable for render object part of the layout */
static GFX_RenderObjectFuncs _gfx_layout_obj_funcs =
{
	_gfx_layout_obj_free,
	_gfx_layout_obj_save,
	_gfx_layout_obj_restore
};

/******************************************************/
GLuint _gfx_vertex_layout_get_handle(

		const GFXVertexLayout* layout)
{
	return ((struct GFX_Layout*)layout)->vao;
}

/******************************************************/
GFXVertexLayout* gfx_vertex_layout_create(

		unsigned char drawCalls)
{
	GFX_WIND_INIT(NULL);

	if(!drawCalls) return NULL;

	/* Create new layout, append draw calls to end of struct */
	size_t size = sizeof(struct GFX_Layout) + drawCalls * sizeof(GFXDrawCall);

	struct GFX_Layout* layout = calloc(1, size);
	if(!layout)
	{
		/* Out of memory error */
		gfx_errors_push(
			GFX_ERROR_OUT_OF_MEMORY,
			"Vertex Layout could not be allocated."
		);
		return NULL;
	}

	/* Register as object */
	layout->layout.id = _gfx_render_object_register(
		&GFX_WIND_GET.objects,
		layout,
		&_gfx_layout_obj_funcs
	);

	if(!layout->layout.id)
	{
		free(layout);
		return NULL;
	}

	/* Create OpenGL resources */
	layout->layout.drawCalls = drawCalls;
	GFX_REND_GET.CreateVertexArrays(1, &layout->vao);

	gfx_vector_init(&layout->attributes, sizeof(struct GFX_Attribute));

	return (GFXVertexLayout*)layout;
}

/******************************************************/
void gfx_vertex_layout_free(

		GFXVertexLayout* layout)
{
	if(layout)
	{
		GFX_WIND_INIT_UNSAFE;

		struct GFX_Layout* internal = (struct GFX_Layout*)layout;

		if(!GFX_WIND_EQ(NULL))
		{
			/* Delete VAO */
			if(GFX_REND_GET.vao == internal->vao)
				GFX_REND_GET.vao = 0;

			GFX_REND_GET.DeleteVertexArrays(1, &internal->vao);

			/* Unregister as object */
			_gfx_render_object_unregister(
				&GFX_WIND_GET.objects,
				layout->id
			);
		}

		gfx_vector_clear(&internal->attributes);
		free(internal->TFBuffers);

		free(layout);
	}
}

/******************************************************/
int gfx_vertex_layout_set_feedback(

		GFXVertexLayout*          layout,
		GFXPrimitive              primitive,
		size_t                    num,
		const GFXFeedbackBuffer*  buffers)
{
	GFX_WIND_INIT(0);

	struct GFX_Layout* internal = (struct GFX_Layout*)layout;

	/* Check number of buffers */
	if(num > GFX_WIND_GET.lim[GFX_LIM_MAX_FEEDBACK_BUFFERS])
		return 0;

	free(internal->TFBuffers);
	internal->TFPrimitive  = primitive;
	internal->TFNumBuffers = num;
	internal->TFBuffers    = NULL;

	/* Free all buffers */
	if(num)
	{
		/* Construct feedback buffers */
		internal->TFBuffers = malloc(sizeof(struct GFX_TFBuffer) * num);
		if(!internal->TFBuffers)
		{
			/* Out of memory error */
			gfx_errors_push(
				GFX_ERROR_OUT_OF_MEMORY,
				"Vertex Layout ran out of memory during feedback allocation."
			);
			return 0;
		}

		size_t i;
		for(i = 0; i < num; ++i)
		{
			internal->TFBuffers[i].buffer =
				_gfx_buffer_get_handle(buffers[i].buffer);
			internal->TFBuffers[i].offset =
				buffers[i].offset;
			internal->TFBuffers[i].size =
				buffers[i].size;
		}
	}

	return 1;
}

/******************************************************/
int gfx_vertex_layout_set_patch_vertices(

		GFXVertexLayout*  layout,
		unsigned int      vertices)
{
	GFX_WIND_INIT(0);

	struct GFX_Layout* internal = (struct GFX_Layout*)layout;

	/* Bound check */
	if(vertices > GFX_WIND_GET.lim[GFX_LIM_MAX_PATCH_VERTICES])
		return 0;

	internal->patchVertices = vertices;

	return 1;
}

/******************************************************/
static int _gfx_layout_set_attribute(

		struct GFX_Layout*  layout,
		unsigned int        index,
		GFX_WIND_ARG)
{
	/* Check index */
	if(index >= GFX_WIND_GET.lim[GFX_LIM_MAX_VERTEX_ATTRIBS]) return 0;
	size_t size = gfx_vector_get_size(&layout->attributes);

	if(index >= size)
	{
		/* Allocate enough memory */
		GFXVectorIterator it = gfx_vector_insert_range(
			&layout->attributes,
			index + 1 - size,
			NULL,
			layout->attributes.end
		);

		if(it == layout->attributes.end) return 0;

		while(it != layout->attributes.end)
		{
			/* Initialize size and buffer to 0 so the attributes will be ignored */
			struct GFX_Attribute* attr = (struct GFX_Attribute*)it;
			attr->size = 0;
			attr->buffer = 0;

			it = gfx_vector_next(&layout->attributes, it);
		}
	}

	return 1;
}

/******************************************************/
int gfx_vertex_layout_set_attribute(

		GFXVertexLayout*           layout,
		unsigned int               index,
		const GFXVertexAttribute*  attr)
{
	GFX_WIND_INIT(0);

	struct GFX_Layout* internal = (struct GFX_Layout*)layout;

	if(
		!attr->size ||
		attr->type.unpacked == GFX_BIT ||
		attr->type.unpacked == GFX_NIBBLE)
	{
		return 0;
	}

	if(!_gfx_layout_set_attribute(internal, index, GFX_WIND_AS_ARG))
		return 0;

	/* Check whether it is instanced */
	struct GFX_Attribute* set =
		(struct GFX_Attribute*)gfx_vector_at(&internal->attributes, index);

	if(!set->size && attr->divisor)
		++internal->instanced;
	else if(set->size && !set->divisor && attr->divisor)
		++internal->instanced;
	else if(set->size && set->divisor && !attr->divisor)
		--internal->instanced;

	/* Set attribute */
	int packed = _gfx_is_data_type_packed(attr->type);

	set->size      = attr->size;
	set->type      = packed ? attr->type.packed : attr->type.unpacked;
	set->stride    = attr->stride;
	set->divisor   = attr->divisor;
	set->interpret = attr->interpret;

	/* Resolve how to interpret */
	set->interpret = (set->interpret & GFX_INTERPRET_DEPTH) ?
		GFX_INTERPRET_FLOAT : set->interpret;
	set->interpret = (set->interpret & GFX_INTERPRET_STENCIL) ?
		GFX_INTERPRET_INTEGER : set->interpret;
	set->interpret = packed && (set->interpret & GFX_INTERPRET_INTEGER) ?
		GFX_INTERPRET_FLOAT : set->interpret;

	/* Send attribute to OpenGL */
	_gfx_layout_init_attrib(internal->vao, index, set, GFX_WIND_AS_ARG);

	return 1;
}

/******************************************************/
static int _gfx_layout_set_attribute_buffer(

		GFXVertexLayout*  layout,
		unsigned int      index,
		GLuint            buffer,
		size_t            offset,
		GFX_WIND_ARG)
{
	struct GFX_Layout* internal = (struct GFX_Layout*)layout;
	if(!_gfx_layout_set_attribute(internal, index, GFX_WIND_AS_ARG))
		return 0;

	/* Set attribute */
	struct GFX_Attribute* set =
		(struct GFX_Attribute*)gfx_vector_at(&internal->attributes, index);

	set->buffer = buffer;
	set->offset = offset;

	/* Send attribute to OpenGL */
	_gfx_layout_init_attrib(internal->vao, index, set, GFX_WIND_AS_ARG);

	return 1;
}

/******************************************************/
int gfx_vertex_layout_set_attribute_buffer(

		GFXVertexLayout*  layout,
		unsigned int      index,
		const GFXBuffer*  buffer,
		size_t            offset)
{
	GFX_WIND_INIT(0);

	GLuint buff = 0;
	if(buffer) buff = _gfx_buffer_get_handle(buffer);

	return _gfx_layout_set_attribute_buffer(
		layout,
		index,
		buff,
		offset,
		GFX_WIND_AS_ARG
	);
}

/******************************************************/
int gfx_vertex_layout_set_attribute_shared_buffer(

		GFXVertexLayout*        layout,
		unsigned int            index,
		const GFXSharedBuffer*  buffer,
		size_t                  offset)
{
	GFX_WIND_INIT(0);

	GLuint buff = 0;
	if(buffer)
	{
		buff = _gfx_shared_buffer_get_handle(buffer);
		offset += buffer->offset;
	}

	return _gfx_layout_set_attribute_buffer(
		layout,
		index,
		buff,
		offset,
		GFX_WIND_AS_ARG
	);
}

/******************************************************/
unsigned int gfx_vertex_layout_count_instanced(

		GFXVertexLayout* layout)
{
	return ((struct GFX_Layout*)layout)->instanced;
}

/******************************************************/
void gfx_vertex_layout_remove_attribute(

		GFXVertexLayout*  layout,
		unsigned int      index)
{
	GFX_WIND_INIT();

	struct GFX_Layout* internal = (struct GFX_Layout*)layout;

	/* Check what index is being removed */
	size_t size = gfx_vector_get_size(&internal->attributes);
	if(index < size)
	{
		/* Check if it had a divisor */
		struct GFX_Attribute* rem = (struct GFX_Attribute*)gfx_vector_at(
			&internal->attributes,
			index
		);

		if(rem->size && rem->divisor) --internal->instanced;

		/* Mark attribute as 'empty' */
		rem->size = 0;
		rem->buffer = 0;

		/* Deallocate all trailing empty attributes */
		unsigned int num;
		GFXVectorIterator beg = internal->attributes.end;

		for(num = 0; num < size; ++num)
		{
			GFXVectorIterator prev = gfx_vector_previous(
				&internal->attributes,
				beg
			);

			rem = (struct GFX_Attribute*)prev;
			if(rem->size || rem->buffer) break;

			beg = prev;
		}
		gfx_vector_erase_range(&internal->attributes, num, beg);
	}

	/* Send request to OpenGL */
	GFX_REND_GET.DisableVertexArrayAttrib(internal->vao, index);
}

/******************************************************/
int gfx_vertex_layout_set_draw_call(

		GFXVertexLayout*    layout,
		unsigned char       index,
		const GFXDrawCall*  call)
{
	GFX_WIND_INIT(0);

	/* Check index */
	if(index >= layout->drawCalls) return 0;
	struct GFX_Layout* internal = (struct GFX_Layout*)layout;

	/* Check extensions */
	if(
		call->primitive == GFX_PATCHES &&
		!GFX_WIND_GET.ext[GFX_EXT_TESSELLATION_SHADER])
	{
		return 0;
	}

	((GFXDrawCall*)(internal + 1))[index] = *call;

	return 1;
}

/******************************************************/
int gfx_vertex_layout_get_draw_call(

		GFXVertexLayout*  layout,
		unsigned char     index,
		GFXDrawCall*      call)
{
	/* Validate index */
	if(index >= layout->drawCalls) return 0;

	/* Retrieve data */
	struct GFX_Layout* internal = (struct GFX_Layout*)layout;
	*call = ((GFXDrawCall*)(internal + 1))[index];

	return 1;
}

/******************************************************/
void gfx_vertex_layout_set_index_buffer(

		GFXVertexLayout*  layout,
		const GFXBuffer*  buffer,
		size_t            offset)
{
	struct GFX_Layout* internal = (struct GFX_Layout*)layout;

	internal->indexBuffer = 0;
	internal->indexOffset = 0;

	if(buffer)
	{
		internal->indexBuffer = _gfx_buffer_get_handle(buffer);
		internal->indexOffset = offset;
	}
}

/******************************************************/
void gfx_vertex_layout_set_index_shared_buffer(

		GFXVertexLayout*        layout,
		const GFXSharedBuffer*  buffer,
		size_t                  offset)
{
	struct GFX_Layout* internal = (struct GFX_Layout*)layout;

	internal->indexBuffer = 0;
	internal->indexOffset = 0;

	if(buffer)
	{
		internal->indexBuffer = _gfx_shared_buffer_get_handle(buffer);
		internal->indexOffset = offset + buffer->offset;
	}
}

/******************************************************/
void _gfx_vertex_layout_draw(

		const GFXVertexLayout*  layout,
		GFXVertexSource         source,
		size_t                  inst,
		unsigned int            base,
		GFX_DrawType            type,
		GFX_WIND_ARG)
{
	/* Bind VAO & Index buffer & Tessellation vertices */
	struct GFX_Layout* internal = (struct GFX_Layout*)layout;

	_gfx_vertex_layout_bind(
		internal->vao,
		GFX_WIND_AS_ARG);

	GFX_REND_GET.BindBuffer(
		GL_ELEMENT_ARRAY_BUFFER,
		internal->indexBuffer);

	_gfx_states_set_patch_vertices(
		internal->patchVertices,
		GFX_WIND_AS_ARG);

	/* Draw using a feedback buffer */
	if(source.numFeedback)
	{
		while(source.numFeedback--)
		{
			size_t i = source.startFeedback + source.numFeedback;

			GFX_REND_GET.BindBufferRange(
				GL_TRANSFORM_FEEDBACK_BUFFER,
				source.numFeedback,
				internal->TFBuffers[i].buffer,
				internal->TFBuffers[i].offset,
				internal->TFBuffers[i].size
			);
		}

		/* Begin feedback, draw, end feedback */
		GFX_REND_GET.BeginTransformFeedback(
			internal->TFPrimitive);

		_gfx_layout_invoke_draw(
			internal,
			source.startDraw,
			source.numDraw,
			inst,
			base,
			type,
			GFX_WIND_AS_ARG);

		GFX_REND_GET.EndTransformFeedback();
	}

	/* Draw without feedback buffer */
	else _gfx_layout_invoke_draw(
		internal,
		source.startDraw,
		source.numDraw,
		inst,
		base,
		type,
		GFX_WIND_AS_ARG
	);
}
