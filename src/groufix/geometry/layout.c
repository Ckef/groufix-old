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
#include "groufix/internal.h"

#include <stdlib.h>

/******************************************************/
/** Internal Vertex layout */
struct GFX_Internal_Layout
{
	/* Layout */
	GLuint     vao;        /* Super class */
	GFXVector  attributes; /* Stores GFX_Internal_Attribute */

	/* Draw calls */
	GFXVector  drawCalls;  /* Stores GFXDrawCall */
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
static void _gfx_layout_obj_free(GFX_Hardware_Object object, const GFX_Extensions* ext)
{
	struct GFX_Internal_Layout* layout = (struct GFX_Internal_Layout*)object;

	/* Destroy layout itself */
	ext->DeleteVertexArrays(1, &layout->vao);
	layout->vao = 0;

	gfx_vector_clear(&layout->attributes);
}

/******************************************************/
static void _gfx_layout_obj_save(GFX_Hardware_Object object, const GFX_Extensions* ext)
{
	struct GFX_Internal_Layout* layout = (struct GFX_Internal_Layout*)object;

	/* Just don't clear the attribute vector */
	ext->DeleteVertexArrays(1, &layout->vao);
	layout->vao = 0;
}

/******************************************************/
static void _gfx_layout_obj_restore(GFX_Hardware_Object object, const GFX_Extensions* ext)
{
	struct GFX_Internal_Layout* layout = (struct GFX_Internal_Layout*)object;

	/* Create VAO */
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
GLuint _gfx_vertex_layout_get_handle(const GFXVertexLayout layout)
{
	GLuint* handle = (GLuint*)layout;

	return *handle;
}

/******************************************************/
GFXVertexLayout gfx_vertex_layout_create(void)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return NULL;

	/* Create new layout */
	struct GFX_Internal_Layout* layout = malloc(sizeof(struct GFX_Internal_Layout));
	if(!layout) return NULL;

	window->extensions.GenVertexArrays(1, &layout->vao);

	gfx_vector_init(&layout->attributes, sizeof(struct GFX_Internal_Attribute));
	gfx_vector_init(&layout->drawCalls, sizeof(GFXDrawCall));

	/* Register as object */
	_gfx_hardware_object_register(layout, &_gfx_layout_obj_funcs);

	return layout;
}

/******************************************************/
void gfx_vertex_layout_free(GFXVertexLayout layout)
{
	if(layout)
	{
		struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;

		/* Get current window and context */
		GFX_Internal_Window* window = _gfx_window_get_current();
		if(window) window->extensions.DeleteVertexArrays(1, &internal->vao);

		gfx_vector_clear(&internal->attributes);
		gfx_vector_clear(&internal->drawCalls);
		free(layout);

		/* Unregister as object */
		_gfx_hardware_object_unregister(internal);
	}
}

/******************************************************/
int gfx_vertex_layout_set_attribute(GFXVertexLayout layout, unsigned int index, const GFXVertexAttribute* attr, const GFXBuffer* buffer)
{
	/* Get current window and internal layout */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return 0;

	if(index >= window->extensions.MAX_VERTEX_ATTRIBS) return 0;

	struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;

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
	_gfx_layout_init_attrib(internal->vao, (struct GFX_Internal_Attribute*)it, &window->extensions);

	return 1;
}

/******************************************************/
void gfx_vertex_layout_remove_attribute(GFXVertexLayout layout, unsigned int index)
{
	/* Get current window and internal layout */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return;

	struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;

	/* Find the attribute and remove it */
	GFXVectorIterator it;
	for(it = internal->attributes.begin; it != internal->attributes.end; it = gfx_vector_next(&internal->attributes, it))
	{
		if(((struct GFX_Internal_Attribute*)it)->index == index)
		{
			gfx_vector_erase(&internal->attributes, it);

			/* Send request to OpenGL */
			window->extensions.BindVertexArray(internal->vao);
			window->extensions.DisableVertexAttribArray(index);

			break;
		}
	}
}
