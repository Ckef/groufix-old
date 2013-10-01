/**
 * Groufix  :  Graphics Engine produced by Ckef Worx
 * www      :  http://www.ejb.ckef-worx.com
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "groufix/containers/deque.h"
#include "groufix/containers/vector.h"
#include "groufix/internal.h"

#include <stdlib.h>

/******************************************************/
/** Internal Vertex layout */
struct GFX_Internal_Layout
{
	/* Super class */
	GFXVertexLayout layout;

	/* Hidden data */
	GLuint     vao;        /* OpenGL handle */
	GFXVector  attributes; /* Stores GFX_Internal_Attribute */
	GFXDeque   drawCalls;  /* Stores GFX_Internal_Draw */

	/* Not a shared resource */
	const GFX_Extensions* ext;
};

/** Internal vertex attribute */
struct GFX_Internal_Attribute
{
	/* Super class */
	GFXVertexAttribute attr;

	/* Hidden data */
	GLuint        buffer;
	unsigned int  index;
};

/** Internal draw call */
struct GFX_Internal_Draw
{
	GFXDrawCall  call;   /* Super class */
	GLuint       buffer; /* Index buffer */
};

/******************************************************/
static void _gfx_layout_init_attrib(GLuint vao, const struct GFX_Internal_Attribute* attr, const GFX_Extensions* ext)
{
	/* Set the attribute */
	ext->BindVertexArray(vao);
	ext->EnableVertexAttribArray(attr->index);

	ext->BindBuffer(GL_ARRAY_BUFFER, attr->buffer);

	if(attr->attr.interpret & GFX_INTERPRET_INTEGER) ext->VertexAttribIPointer(
		attr->index,
		attr->attr.size,
		attr->attr.type,
		attr->attr.stride,
		(GLvoid*)attr->attr.offset
	);
	else ext->VertexAttribPointer(
		attr->index,
		attr->attr.size,
		attr->attr.type,
		attr->attr.interpret & GFX_INTERPRET_NORMALIZED,
		attr->attr.stride,
		(GLvoid*)attr->attr.offset
	);

	/* Check if non-zero to avoid extension error */
	if(attr->attr.divisor) ext->VertexAttribDivisor(attr->index, attr->attr.divisor);
}

/******************************************************/
static void _gfx_layout_obj_free(void* object, const GFX_Extensions* ext)
{
	struct GFX_Internal_Layout* layout = (struct GFX_Internal_Layout*)object;

	/* Destroy layout itself */
	ext->DeleteVertexArrays(1, &layout->vao);

	layout->ext = NULL;
	layout->vao = 0;
	layout->layout.id = 0;

	gfx_vector_clear(&layout->attributes);
}

/******************************************************/
static void _gfx_layout_obj_save(void* object, const GFX_Extensions* ext)
{
	struct GFX_Internal_Layout* layout = (struct GFX_Internal_Layout*)object;

	/* Just don't clear the attribute vector */
	ext->DeleteVertexArrays(1, &layout->vao);

	layout->ext = NULL;
	layout->vao = 0;
}

/******************************************************/
static void _gfx_layout_obj_restore(void* object, const GFX_Extensions* ext)
{
	struct GFX_Internal_Layout* layout = (struct GFX_Internal_Layout*)object;

	/* Create VAO */
	layout->ext = ext;
	ext->GenVertexArrays(1, &layout->vao);

	/* Restore attributes */
	GFXVectorIterator it;
	for(it = layout->attributes.begin; it != layout->attributes.end; it = gfx_vector_next(&layout->attributes, it))
		_gfx_layout_init_attrib(layout->vao, (struct GFX_Internal_Attribute*)it, ext);
}

/******************************************************/
/* vtable for hardware part of the layout */
static GFX_Hardware_Funcs _gfx_layout_obj_funcs =
{
	_gfx_layout_obj_free,
	_gfx_layout_obj_save,
	_gfx_layout_obj_restore
};

/******************************************************/
GLuint _gfx_vertex_layout_get_handle(const GFXVertexLayout* layout)
{
	return ((struct GFX_Internal_Layout*)layout)->vao;
}

/******************************************************/
GFXVertexLayout* gfx_vertex_layout_create(void)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return NULL;

	/* Create new layout */
	struct GFX_Internal_Layout* layout = calloc(1, sizeof(struct GFX_Internal_Layout));
	if(!layout) return NULL;

	/* Register as object */
	layout->layout.id = _gfx_hardware_object_register(layout, &_gfx_layout_obj_funcs);
	if(!layout->layout.id)
	{
		free(layout);
		return NULL;
	}

	/* Create OpenGL resources */
	layout->ext = &window->extensions;
	layout->ext->GenVertexArrays(1, &layout->vao);

	gfx_vector_init(&layout->attributes, sizeof(struct GFX_Internal_Attribute));
	gfx_deque_init(&layout->drawCalls, sizeof(struct GFX_Internal_Draw));

	return (GFXVertexLayout*)layout;
}

/******************************************************/
void gfx_vertex_layout_free(GFXVertexLayout* layout)
{
	if(layout)
	{
		/* Unregister as object */
		_gfx_hardware_object_unregister(layout->id);

		struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;

		/* Delete VAO */
		if(internal->ext) internal->ext->DeleteVertexArrays(1, &internal->vao);

		gfx_vector_clear(&internal->attributes);
		gfx_deque_clear(&internal->drawCalls);
		free(layout);
	}
}

/******************************************************/
int gfx_vertex_layout_set_attribute(GFXVertexLayout* layout, unsigned int index, const GFXVertexAttribute* attr, const GFXBuffer* buffer)
{
	struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;

	if(!internal->ext || index >= internal->ext->MAX_VERTEX_ATTRIBS) return 0;

	/* Find the attribute */
	GFXVectorIterator it;
	for(it = internal->attributes.begin; it != internal->attributes.end; it = gfx_vector_next(&internal->attributes, it))
	{
		/* Replace data */
		struct GFX_Internal_Attribute* set = (struct GFX_Internal_Attribute*)it;
		if(set->index == index)
		{
			set->attr = *attr;
			set->buffer = _gfx_buffer_get_handle(buffer);

			break;
		}
	}

	/* Insert new attribute */
	if(it == internal->attributes.end)
	{
		struct GFX_Internal_Attribute insert;
		insert.attr   = *attr;
		insert.buffer = _gfx_buffer_get_handle(buffer);
		insert.index  = index;

		it = gfx_vector_insert(&internal->attributes, &insert, it);
		if(it == internal->attributes.end) return 0;
	}

	/* Send attribute to OpenGL */
	_gfx_layout_init_attrib(internal->vao, (struct GFX_Internal_Attribute*)it, internal->ext);

	return 1;
}

/******************************************************/
void gfx_vertex_layout_remove_attribute(GFXVertexLayout* layout, unsigned int index)
{
	struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;

	if(!internal->ext) return;

	/* Find the attribute and remove it */
	GFXVectorIterator it;
	for(it = internal->attributes.begin; it != internal->attributes.end; it = gfx_vector_next(&internal->attributes, it))
	{
		if(((struct GFX_Internal_Attribute*)it)->index == index)
		{
			gfx_vector_erase(&internal->attributes, it);

			/* Send request to OpenGL */
			internal->ext->BindVertexArray(internal->vao);
			internal->ext->DisableVertexAttribArray(index);

			break;
		}
	}
}

/******************************************************/
unsigned short gfx_vertex_layout_push(GFXVertexLayout* layout, const GFXDrawCall* call, const GFXBuffer* buffer)
{
	struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;

	/* Check for overflow (jebus, how many draw calls is that?) */
	unsigned short index = gfx_deque_get_size(&internal->drawCalls) + 1;
	if(!index) return 0;

	/* Try to push a new element */
	struct GFX_Internal_Draw draw;
	draw.call   = *call;
	draw.buffer = 0;

	if(buffer) draw.buffer = _gfx_buffer_get_handle(buffer);

	/* Return actual index + 1 */
	return gfx_deque_push_back(&internal->drawCalls, &draw) == internal->drawCalls.end ? 0 : index;
}

/******************************************************/
int gfx_vertex_layout_set(GFXVertexLayout* layout, unsigned short index, const GFXDrawCall* call, const GFXBuffer* buffer)
{
	struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;

	/* Check index */
	unsigned short size = gfx_deque_get_size(&internal->drawCalls);
	if(index > size) return 0;

	/* Replace data */
	struct GFX_Internal_Draw* draw = (struct GFX_Internal_Draw*)gfx_deque_at(&internal->drawCalls, index - 1);
	draw->call   = *call;
	draw->buffer = 0;

	if(buffer) draw->buffer = _gfx_buffer_get_handle(buffer);

	return 1;
}

/******************************************************/
unsigned short gfx_vertex_layout_pop(GFXVertexLayout* layout)
{
	struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;

	/* Get index of last call */
	unsigned short index = gfx_deque_get_size(&internal->drawCalls);

	/* Try to pop the last element */
	gfx_deque_pop_back(&internal->drawCalls);

	return index;
}

/******************************************************/
void gfx_vertex_layout_draw(GFXVertexLayout* layout, unsigned short num, unsigned short startIndex)
{
	struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;

	/* Bind VAO */
	internal->ext->BindVertexArray(internal->vao);

	/* Render all calls */
	GFXDequeIterator it = gfx_deque_at(&internal->drawCalls, startIndex - 1);
	while(num--)
	{
		struct GFX_Internal_Draw* call = (struct GFX_Internal_Draw*)it;
		glDrawArrays(call->call.primitive, call->call.first, call->call.count);

		it = gfx_deque_next(&internal->drawCalls, it);
	}
}

/******************************************************/
void gfx_vertex_layout_draw_indexed(GFXVertexLayout* layout, unsigned short num, unsigned short startIndex)
{
	struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;

	/* Bind VAO */
	internal->ext->BindVertexArray(internal->vao);

	/* Render all calls */
	GFXDequeIterator it = gfx_deque_at(&internal->drawCalls, startIndex - 1);
	while(num--)
	{
		struct GFX_Internal_Draw* call = (struct GFX_Internal_Draw*)it;

		internal->ext->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, call->buffer);
		glDrawElements(call->call.primitive, call->call.count, call->call.indexType, (GLvoid*)call->call.first);

		it = gfx_deque_next(&internal->drawCalls, it);
	}
}

/******************************************************/
void gfx_vertex_layout_draw_instanced(GFXVertexLayout* layout, unsigned short num, unsigned short startIndex, size_t inst)
{
	struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;

	/* Bind VAO */
	internal->ext->BindVertexArray(internal->vao);

	/* Render all calls */
	GFXDequeIterator it = gfx_deque_at(&internal->drawCalls, startIndex - 1);
	while(num--)
	{
		struct GFX_Internal_Draw* call = (struct GFX_Internal_Draw*)it;
		internal->ext->DrawArraysInstanced(call->call.primitive, call->call.first, call->call.count, inst);

		it = gfx_deque_next(&internal->drawCalls, it);
	}
}

/******************************************************/
void gfx_vertex_layout_draw_indexed_instanced(GFXVertexLayout* layout, unsigned short num, unsigned short startIndex, size_t inst)
{
	struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;

	/* Bind VAO */
	internal->ext->BindVertexArray(internal->vao);

	/* Render all calls */
	GFXDequeIterator it = gfx_deque_at(&internal->drawCalls, startIndex - 1);
	while(num--)
	{
		struct GFX_Internal_Draw* call = (struct GFX_Internal_Draw*)it;

		internal->ext->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, call->buffer);
		internal->ext->DrawElementsInstanced(call->call.primitive, call->call.count, call->call.indexType, (GLvoid*)call->call.first, inst);

		it = gfx_deque_next(&internal->drawCalls, it);
	}
}
