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

#include "groufix/core/memory/internal.h"
#include "groufix/core/pipeline/internal.h"
#include "groufix/containers/vector.h"

#include <stdlib.h>

/******************************************************/
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

/* Internal draw call */
struct GFX_DrawCall
{
	GFXDrawCall  call;   /* Super class */
	GLuint       buffer; /* Index buffer */
};

/* Internal Vertex layout */
struct GFX_Layout
{
	/* Super class */
	GFXVertexLayout layout;

	/* Hidden data */
	GLuint                vao;        /* OpenGL handle */
	GFXVector             attributes; /* Stores GFX_Attribute */
	size_t                numBuffers;
	struct GFX_TFBuffer*  buffers;    /* Transform Feedback buffers */
	GFXPrimitive          primitive;  /* Feedback output primitive */

	/* Not a shared resource */
	GFX_Extensions* ext;
};

/******************************************************/
void _gfx_layout_bind(GLuint vao, GFX_Extensions* ext)
{
	/* Prevent binding it twice */
	if(ext->layout != vao)
	{
		ext->layout = vao;
		ext->BindVertexArray(vao);
	}
}

/******************************************************/
static void _gfx_layout_init_attrib(GLuint vao, unsigned int index, const struct GFX_Attribute* attr, GFX_Extensions* ext)
{
	_gfx_layout_bind(vao, ext);

	/* Check if enabled */
	if(attr->size && attr->buffer)
	{
		/* Set the attribute */
		ext->EnableVertexAttribArray(index);
		ext->BindBuffer(GL_ARRAY_BUFFER, attr->buffer);

		/* Check integer value */
		if(attr->interpret & GFX_INTERPRET_INTEGER) ext->VertexAttribIPointer(
			index,
			attr->size,
			attr->type,
			attr->stride,
			(GLvoid*)attr->offset
		);
		else ext->VertexAttribPointer(
			index,
			attr->size,
			attr->type,
			attr->interpret & GFX_INTERPRET_NORMALIZED ? GL_TRUE : GL_FALSE,
			attr->stride,
			(GLvoid*)attr->offset
		);

		/* Check if non-zero to avoid extension error */
		if(attr->divisor) ext->VertexAttribDivisor(index, attr->divisor);
	}

	/* Disable it */
	else ext->DisableVertexAttribArray(index);
}

/******************************************************/
static void _gfx_layout_obj_free(void* object, GFX_Extensions* ext)
{
	struct GFX_Layout* layout = (struct GFX_Layout*)object;

	layout->ext = NULL;
	layout->vao = 0;
	layout->layout.id = 0;

	gfx_vector_clear(&layout->attributes);
}

/******************************************************/
static void _gfx_layout_obj_save(void* object, GFX_Extensions* ext)
{
	struct GFX_Layout* layout = (struct GFX_Layout*)object;

	/* Just don't clear the attribute vector */
	ext->DeleteVertexArrays(1, &layout->vao);

	layout->ext = NULL;
	layout->vao = 0;
}

/******************************************************/
static void _gfx_layout_obj_restore(void* object, GFX_Extensions* ext)
{
	struct GFX_Layout* layout = (struct GFX_Layout*)object;

	/* Create VAO */
	layout->ext = ext;
	ext->GenVertexArrays(1, &layout->vao);

	/* Restore attributes */
	unsigned int i = 0;

	GFXVectorIterator it = layout->attributes.begin;
	while(it != layout->attributes.end)
	{
		_gfx_layout_init_attrib(layout->vao, i++, (struct GFX_Attribute*)it, ext);
		it = gfx_vector_next(&layout->attributes, it);
	}
}

/******************************************************/
/* vtable for hardware part of the layout */
static GFX_HardwareFuncs _gfx_layout_obj_funcs =
{
	_gfx_layout_obj_free,
	_gfx_layout_obj_save,
	_gfx_layout_obj_restore
};

/******************************************************/
GLuint _gfx_vertex_layout_get_handle(const GFXVertexLayout* layout)
{
	return ((struct GFX_Layout*)layout)->vao;
}

/******************************************************/
GFXVertexLayout* gfx_vertex_layout_create(unsigned char drawCalls)
{
	/* Get current window and context */
	GFX_Window* window = _gfx_window_get_current();
	if(!window || !drawCalls) return NULL;

	/* Create new layout, append draw calls to end of struct */
	size_t size = sizeof(struct GFX_Layout) + drawCalls * sizeof(struct GFX_DrawCall);

	struct GFX_Layout* layout = calloc(1, size);
	if(!layout) return NULL;

	/* Register as object */
	layout->layout.id = _gfx_hardware_object_register(layout, &_gfx_layout_obj_funcs);
	if(!layout->layout.id)
	{
		free(layout);
		return NULL;
	}

	/* Create OpenGL resources */
	layout->layout.drawCalls = drawCalls;
	layout->ext = &window->extensions;
	layout->ext->GenVertexArrays(1, &layout->vao);

	gfx_vector_init(&layout->attributes, sizeof(struct GFX_Attribute));

	return (GFXVertexLayout*)layout;
}

/******************************************************/
void gfx_vertex_layout_free(GFXVertexLayout* layout)
{
	if(layout)
	{
		struct GFX_Layout* internal = (struct GFX_Layout*)layout;

		/* Unregister as object */
		_gfx_hardware_object_unregister(layout->id);

		/* Delete VAO */
		if(internal->ext)
		{
			if(internal->ext->layout == internal->vao) internal->ext->layout = 0;
			internal->ext->DeleteVertexArrays(1, &internal->vao);
		}

		gfx_vector_clear(&internal->attributes);
		free(internal->buffers);

		free(layout);
	}
}

/******************************************************/
int gfx_vertex_layout_set_attribute(GFXVertexLayout* layout, unsigned int index, const GFXVertexAttribute* attr)
{
	struct GFX_Layout* internal = (struct GFX_Layout*)layout;
	if(!internal->ext) return 0;

	/* Check index */
	if(index >= internal->ext->limits[GFX_LIM_MAX_VERTEX_ATTRIBS]) return 0;
	size_t size = gfx_vector_get_size(&internal->attributes);

	if(index >= size)
	{
		/* Allocate enough memory */
		GFXVectorIterator it = gfx_vector_insert_range(&internal->attributes, index + 1 - size, NULL, internal->attributes.end);
		if(it == internal->attributes.end) return 0;

		while(it != internal->attributes.end)
		{
			/* Initialize buffer to 0 so the attributes will be ignored */
			((struct GFX_Attribute*)it)->buffer = 0;
			it = gfx_vector_next(&internal->attributes, it);
		}
	}

	/* Set attribute */
	struct GFX_Attribute* set = (struct GFX_Attribute*)gfx_vector_at(&internal->attributes, index);
	int packed = _gfx_is_data_type_packed(attr->type);

	set->size      = attr->size;
	set->type      = packed ? attr->type.packed : attr->type.unpacked;
	set->interpret = packed && attr->interpret & GFX_INTERPRET_INTEGER ? GFX_INTERPRET_FLOAT : attr->interpret;
	set->stride    = attr->stride;
	set->divisor   = attr->divisor;

	/* Send attribute to OpenGL */
	_gfx_layout_init_attrib(internal->vao, index, set, internal->ext);

	return 1;
}

/******************************************************/
static int _gfx_vertex_layout_set_attribute_buffer(GFXVertexLayout* layout, unsigned int index, GLuint buffer, size_t offset)
{
	struct GFX_Layout* internal = (struct GFX_Layout*)layout;
	if(!internal->ext) return 0;

	/* Check what index is being removed */
	size_t size = gfx_vector_get_size(&internal->attributes);
	if(index >= size) return 0;

	/* Set attribute */
	struct GFX_Attribute* set = (struct GFX_Attribute*)gfx_vector_at(&internal->attributes, index);
	set->buffer = buffer;
	set->offset = offset;

	/* Send attribute to OpenGL */
	_gfx_layout_init_attrib(internal->vao, index, set, internal->ext);

	return 1;
}

/******************************************************/
int gfx_vertex_layout_set_attribute_buffer(GFXVertexLayout* layout, unsigned int index, const GFXBuffer* buffer, size_t offset)
{
	GLuint buff = 0;
	if(buffer) buff = _gfx_buffer_get_handle(buffer);

	return _gfx_vertex_layout_set_attribute_buffer(layout, index, buff, offset);
}

/******************************************************/
int gfx_vertex_layout_set_attribute_shared_buffer(GFXVertexLayout* layout, unsigned int index, const GFXSharedBuffer* buffer, size_t offset)
{
	GLuint buff = 0;
	if(buffer)
	{
		buff = _gfx_shared_buffer_get_handle(buffer);
		offset += buffer->offset;
	}

	return _gfx_vertex_layout_set_attribute_buffer(layout, index, buff, offset);
}

/******************************************************/
void gfx_vertex_layout_remove_attribute(GFXVertexLayout* layout, unsigned int index)
{
	struct GFX_Layout* internal = (struct GFX_Layout*)layout;
	if(!internal->ext) return;

	/* Check what index is being removed */
	size_t size = gfx_vector_get_size(&internal->attributes);
	if(index >= size) return;

	if(size > 1)
	{
		/* Mark attribute as 'empty' */
		struct GFX_Attribute* rem = (struct GFX_Attribute*)gfx_vector_at(&internal->attributes, index);
		rem->size = 0;
		rem->buffer = 0;

		/* Deallocate all empty attributes at the end */
		unsigned int num;
		GFXVectorIterator beg = internal->attributes.end;

		for(num = 0; num < size; ++num)
		{
			GFXVectorIterator prev = gfx_vector_previous(&internal->attributes, beg);

			rem = (struct GFX_Attribute*)prev;
			if(rem->size || rem->buffer) break;

			beg = prev;
		}
		gfx_vector_erase_range(&internal->attributes, num, beg);
	}

	/* Clear vector */
	else gfx_vector_clear(&internal->attributes);

	/* Send request to OpenGL */
	_gfx_layout_bind(internal->vao, internal->ext);
	internal->ext->DisableVertexAttribArray(index);
}

/******************************************************/
int gfx_vertex_layout_set_feedback(GFXVertexLayout* layout, GFXPrimitive primitive, size_t num, const GFXFeedbackBuffer* buffers)
{
	struct GFX_Layout* internal = (struct GFX_Layout*)layout;
	if(!internal->ext) return 0;

	/* Check number of buffers */
	if(num > internal->ext->limits[GFX_LIM_MAX_FEEDBACK_BUFFERS]) return 0;

	internal->primitive = primitive;

	/* Free all buffers */
	if(!num)
	{
		free(internal->buffers);
		internal->buffers = NULL;
		internal->numBuffers = 0;
	}
	else
	{
		/* Construct feedback buffers */
		struct GFX_TFBuffer* buffs = malloc(sizeof(struct GFX_TFBuffer) * num);
		if(!buffs) return 0;

		free(internal->buffers);
		internal->buffers = buffs;
		internal->numBuffers = num;

		size_t i;
		for(i = 0; i < num; ++i)
		{
			buffs[i].buffer = _gfx_buffer_get_handle(buffers[i].buffer);
			buffs[i].offset = buffers[i].offset;
			buffs[i].size = buffers[i].size;
		}
	}

	return 1;
}

/******************************************************/
int gfx_vertex_layout_set_draw_call(GFXVertexLayout* layout, unsigned char index, const GFXDrawCall* call)
{
	struct GFX_Layout* internal = (struct GFX_Layout*)layout;
	struct GFX_DrawCall* set = ((struct GFX_DrawCall*)(internal + 1)) + index;

	/* Check index */
	if(!internal->ext || index >= layout->drawCalls) return 0;

	/* Make sure to not set tessellation data */
	set->call = *call;
	if(call->primitive != GFX_PATCHES)
	{
		set->call.patchVertices = 0;
		return 1;
	}

	/* Validate number of vertices per patch */
	return (call->patchVertices && call->patchVertices <= internal->ext->limits[GFX_LIM_MAX_PATCH_VERTICES]) ? 1 : 0;
}

/******************************************************/
static int _gfx_vertex_layout_set_draw_call_buffer(GFXVertexLayout* layout, unsigned char index, GLuint buffer, size_t offset)
{
	/* Check index */
	if(index >= layout->drawCalls) return 0;

	struct GFX_Layout* internal = (struct GFX_Layout*)layout;
	struct GFX_DrawCall* call = ((struct GFX_DrawCall*)(internal + 1)) + index;

	call->buffer = buffer;
	call->call.first = offset;

	return 1;
}

/******************************************************/
int gfx_vertex_layout_set_draw_call_buffer(GFXVertexLayout* layout, unsigned char index, const GFXBuffer* buffer, size_t offset)
{
	GLuint buff = 0;
	if(buffer) buff = _gfx_buffer_get_handle(buffer);

	return _gfx_vertex_layout_set_draw_call_buffer(layout, index, buff, offset);
}

/******************************************************/
int gfx_vertex_layout_set_draw_call_shared_buffer(GFXVertexLayout* layout, unsigned char index, const GFXSharedBuffer* buffer, size_t offset)
{
	GLuint buff = 0;
	if(buffer)
	{
		buff = _gfx_shared_buffer_get_handle(buffer);
		offset += buffer->offset;
	}

	return _gfx_vertex_layout_set_draw_call_buffer(layout, index, buff, offset);
}

/******************************************************/
int gfx_vertex_layout_get_draw_call(GFXVertexLayout* layout, unsigned char index, GFXDrawCall* call)
{
	/* Validate index */
	if(index >= layout->drawCalls) return 0;

	/* Retrieve data */
	struct GFX_Layout* internal = (struct GFX_Layout*)layout;
	*call = (((struct GFX_DrawCall*)(internal + 1)) + index)->call;

	return 1;
}

/******************************************************/
void _gfx_vertex_layout_draw_begin(const GFXVertexLayout* layout, unsigned char startFeedback, unsigned char num)
{
	struct GFX_Layout* internal = (struct GFX_Layout*)layout;

	/* Bind VAO */
	_gfx_layout_bind(internal->vao, internal->ext);

	/* Bind feedback buffers */
	if(num)
	{
		size_t i;
		for(i = 0; i < num; ++i)
		{
			size_t j = i + startFeedback;

			internal->ext->BindBufferRange(
				GL_TRANSFORM_FEEDBACK_BUFFER,
				i,
				internal->buffers[j].buffer,
				internal->buffers[j].offset,
				internal->buffers[j].size
			);
		}

		/* Begin transform feedback */
		internal->ext->BeginTransformFeedback(internal->primitive);
	}
}

/******************************************************/
static void _gfx_vertex_layout_invoke(struct GFX_DrawCall* call, GFX_Extensions* ext)
{
	/* Tessellation */
	if(call->call.patchVertices)
		_gfx_states_set_patch_vertices(call->call.patchVertices, ext);

	/* Draw call */
	switch(call->buffer)
	{
		case 0 :
			ext->DrawArrays(
				call->call.primitive,
				call->call.first,
				call->call.count
			);
			break;

		default :
			ext->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, call->buffer);
			ext->DrawElements(
				call->call.primitive,
				call->call.count,
				call->call.indexType,
				(GLvoid*)call->call.first
			);
			break;
	}
}

/******************************************************/
static void _gfx_vertex_layout_invoke_instanced(struct GFX_DrawCall* call, size_t inst, GFX_Extensions* ext)
{
	/* Tessellation */
	if(call->call.patchVertices)
		_gfx_states_set_patch_vertices(call->call.patchVertices, ext);

	/* Draw call */
	switch(call->buffer)
	{
		case 0 :
			ext->DrawArraysInstanced(
				call->call.primitive,
				call->call.first,
				call->call.count,
				inst
			);
			break;

		default :
			ext->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, call->buffer);
			ext->DrawElementsInstanced(
				call->call.primitive,
				call->call.count,
				call->call.indexType,
				(GLvoid*)call->call.first,
				inst
			);
			break;
	}
}

/******************************************************/
void _gfx_vertex_layout_draw(const GFXVertexLayout* layout, unsigned char startIndex, unsigned char num, size_t inst)
{
	struct GFX_Layout* internal = (struct GFX_Layout*)layout;
	struct GFX_DrawCall* call = ((struct GFX_DrawCall*)(internal + 1)) + startIndex;

	/* Invoke all calls */
	switch(inst)
	{
		case 1 :
			for(; num--; ++call) _gfx_vertex_layout_invoke(call, internal->ext);
			break;

		default :
			for(; num--; ++call) _gfx_vertex_layout_invoke_instanced(call, inst, internal->ext);
			break;
	}
}

/******************************************************/
void _gfx_vertex_layout_draw_end(const GFXVertexLayout* layout, unsigned char numFeedback)
{
	/* Just end it */
	if(numFeedback)
	{
		struct GFX_Layout* internal = (struct GFX_Layout*)layout;
		internal->ext->EndTransformFeedback();
	}
}