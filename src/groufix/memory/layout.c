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

#include "groufix/containers/vector.h"
#include "groufix/memory/internal.h"

#include <stdlib.h>

/******************************************************/
/** Currently active VAO */
static GLuint _gfx_current_vao = 0;

/** Internal Vertex layout */
struct GFX_Internal_Layout
{
	/* Super class */
	GFXVertexLayout layout;

	/* Hidden data */
	GLuint     vao;        /* OpenGL handle */
	GFXVector  attributes; /* Stores GFX_Internal_Attribute */

	/* Not a shared resource */
	const GFX_Extensions* ext;
};

/** Internal vertex attribute */
struct GFX_Internal_Attribute
{
	GFXVertexAttribute  attr;   /* Super class */
	GLuint              buffer; /* Vertex buffer */
};

/******************************************************/
static inline void _gfx_layout_bind(GLuint vao, const GFX_Extensions* ext)
{
	/* Prevent binding it twice */
	if(_gfx_current_vao != vao)
	{
		_gfx_current_vao = vao;
		ext->BindVertexArray(vao);
	}
}

/******************************************************/
void _gfx_layout_force_rebind(void)
{
	_gfx_current_vao = 0;
}

/******************************************************/
static void _gfx_layout_init_attrib(GLuint vao, unsigned int index, const struct GFX_Internal_Attribute* attr, const GFX_Extensions* ext)
{
	/* Validate attribute */
	if(!attr->attr.size) return;

	/* Set the attribute */
	_gfx_layout_bind(vao, ext);
	ext->EnableVertexAttribArray(index);

	ext->BindBuffer(GL_ARRAY_BUFFER, attr->buffer);

	/* Override if packed type */
	int packed = _gfx_is_data_type_packed(attr->attr.type);

	if(!packed && attr->attr.interpret & GFX_INTERPRET_INTEGER) ext->VertexAttribIPointer(
		index,
		attr->attr.size,
		attr->attr.type.unpacked,
		attr->attr.stride,
		(GLvoid*)attr->attr.offset
	);
	else ext->VertexAttribPointer(
		index,
		attr->attr.size,
		packed ? attr->attr.type.packed : attr->attr.type.unpacked,
		attr->attr.interpret & GFX_INTERPRET_NORMALIZED,
		attr->attr.stride,
		(GLvoid*)attr->attr.offset
	);

	/* Check if non-zero to avoid extension error */
	if(attr->attr.divisor) ext->VertexAttribDivisor(index, attr->attr.divisor);
}

/******************************************************/
static void _gfx_layout_obj_free(void* object, GFX_Extensions* ext)
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
static void _gfx_layout_obj_save(void* object, GFX_Extensions* ext)
{
	struct GFX_Internal_Layout* layout = (struct GFX_Internal_Layout*)object;

	/* First unbind itself */
	if(_gfx_current_vao == layout->vao) _gfx_layout_force_rebind();

	/* Just don't clear the attribute vector */
	ext->DeleteVertexArrays(1, &layout->vao);

	layout->ext = NULL;
	layout->vao = 0;
}

/******************************************************/
static void _gfx_layout_obj_restore(void* object, GFX_Extensions* ext)
{
	struct GFX_Internal_Layout* layout = (struct GFX_Internal_Layout*)object;

	/* Create VAO */
	layout->ext = ext;
	ext->GenVertexArrays(1, &layout->vao);

	/* Restore attributes */
	unsigned int i = 0;
	GFXVectorIterator it = layout->attributes.begin;
	while(it != layout->attributes.end)
	{
		_gfx_layout_init_attrib(layout->vao, i++, (struct GFX_Internal_Attribute*)it, ext);
		it = gfx_vector_next(&layout->attributes, it);
	}
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
GFXVertexLayout* gfx_vertex_layout_create(unsigned char drawCalls)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window || !drawCalls) return NULL;

	/* Create new layout, append draw calls to end of struct */
	size_t size = sizeof(struct GFX_Internal_Layout) + drawCalls * sizeof(GFXDrawCall);

	struct GFX_Internal_Layout* layout = calloc(1, size);
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

	gfx_vector_init(&layout->attributes, sizeof(struct GFX_Internal_Attribute));

	return (GFXVertexLayout*)layout;
}

/******************************************************/
void gfx_vertex_layout_free(GFXVertexLayout* layout)
{
	if(layout)
	{
		struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;

		/* Unregister as object */
		_gfx_hardware_object_unregister(layout->id);

		/* Delete VAO */
		if(internal->ext) internal->ext->DeleteVertexArrays(1, &internal->vao);

		gfx_vector_clear(&internal->attributes);
		free(layout);
	}
}

/******************************************************/
int gfx_vertex_layout_set_attribute(GFXVertexLayout* layout, unsigned int index, const GFXVertexAttribute* attr, const GFXBuffer* buffer)
{
	struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;
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
			/* Initialize size to 0 so the attributes will be ignored */
			((struct GFX_Internal_Attribute*)it)->attr.size = 0;
			it = gfx_vector_next(&internal->attributes, it);
		}
	}

	/* Set attribute */
	struct GFX_Internal_Attribute* set = (struct GFX_Internal_Attribute*)gfx_vector_at(&internal->attributes, index);
	set->attr = *attr;
	set->buffer = _gfx_buffer_get_handle(buffer);

	/* Send attribute to OpenGL */
	_gfx_layout_init_attrib(internal->vao, index, set, internal->ext);

	return 1;
}

/******************************************************/
int gfx_vertex_layout_get_attribute(GFXVertexLayout* layout, unsigned int index, GFXVertexAttribute* attr)
{
	struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;

	/* Validate index */
	size_t size = gfx_vector_get_size(&internal->attributes);
	if(index >= size) return 0;

	/* Retrieve data */
	struct GFX_Internal_Attribute* get = (struct GFX_Internal_Attribute*)gfx_vector_at(&internal->attributes, index);
	*attr = get->attr;

	/* Retrieve size for validity */
	return get->attr.size;
}

/******************************************************/
void gfx_vertex_layout_remove_attribute(GFXVertexLayout* layout, unsigned int index)
{
	struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;
	if(!internal->ext) return;

	/* Check what index is being removed */
	size_t size = gfx_vector_get_size(&internal->attributes);
	if(index >= size) return;

	if(size > 1)
	{
		/* Mark attribute as 'empty' */
		((struct GFX_Internal_Attribute*)gfx_vector_at(&internal->attributes, index))->attr.size = 0;

		/* Deallocate all empty attributes at the end */
		unsigned int num;
		GFXVectorIterator beg = internal->attributes.end;

		for(num = 0; num < size; ++num)
		{
			GFXVectorIterator prev = gfx_vector_previous(&internal->attributes, beg);
			if(((struct GFX_Internal_Attribute*)prev)->attr.size) break;

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
int gfx_vertex_layout_set(GFXVertexLayout* layout, unsigned char index, const GFXDrawCall* call)
{
	/* Check index */
	if(index >= layout->drawCalls) return 0;

	/* Replace data */
	struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;
	*(((GFXDrawCall*)(internal + 1)) + index) = *call;

	return 1;
}

/******************************************************/
int gfx_vertex_layout_get(GFXVertexLayout* layout, unsigned char index, GFXDrawCall* call)
{
	/* Validate index */
	if(index >= layout->drawCalls) return 0;

	/* Retrieve data */
	struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;
	*call = *(((GFXDrawCall*)(internal + 1)) + index);

	return 1;
}

/******************************************************/
void _gfx_vertex_layout_draw(const GFXVertexLayout* layout, unsigned char startIndex, unsigned char num)
{
	struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;

	/* Bind VAO */
	_gfx_layout_bind(internal->vao, internal->ext);

	/* Render all calls */
	GFXDrawCall* call;
	for(call = ((GFXDrawCall*)(internal + 1)) + startIndex; num--; ++call)
	{
		internal->ext->DrawArrays(call->primitive, call->first, call->count);
	}
}

/******************************************************/
void _gfx_vertex_layout_draw_indexed(const GFXVertexLayout* layout, unsigned char startIndex, unsigned char num)
{
	struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;

	/* Bind VAO */
	_gfx_layout_bind(internal->vao, internal->ext);

	/* Render all calls */
	GFXDrawCall* call;
	for(call = ((GFXDrawCall*)(internal + 1)) + startIndex; num--; ++call)
	{
		internal->ext->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, _gfx_buffer_get_handle(call->buffer));
		internal->ext->DrawElements(call->primitive, call->count, call->indexType, (GLvoid*)call->first);
	}
}

/******************************************************/
void _gfx_vertex_layout_draw_instanced(const GFXVertexLayout* layout, unsigned char startIndex, unsigned char num, size_t inst)
{
	struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;

	/* Bind VAO */
	_gfx_layout_bind(internal->vao, internal->ext);

	/* Render all calls */
	GFXDrawCall* call;
	for(call = ((GFXDrawCall*)(internal + 1)) + startIndex; num--; ++call)
	{
		internal->ext->DrawArraysInstanced(call->primitive, call->first, call->count, inst);
	}
}

/******************************************************/
void _gfx_vertex_layout_draw_indexed_instanced(const GFXVertexLayout* layout, unsigned char startIndex, unsigned char num, size_t inst)
{
	struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;

	/* Bind VAO */
	_gfx_layout_bind(internal->vao, internal->ext);

	/* Render all calls */
	GFXDrawCall* call;
	for(call = ((GFXDrawCall*)(internal + 1)) + startIndex; num--; ++call)
	{
		internal->ext->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, _gfx_buffer_get_handle(call->buffer));
		internal->ext->DrawElementsInstanced(call->primitive, call->count, call->indexType, (GLvoid*)call->first, inst);
	}
}
