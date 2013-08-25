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

#include "groufix/geometry.h"
#include "groufix/containers/vector.h"
#include "groufix/internal.h"

/******************************************************/
/** \brief Internal hardware layout */
struct GFX_Internal_Layout
{
	GLuint     vao;
	GFXVector  attributes; /* Stores the below */
};

/** \brief Internal vertex attribute */
struct GFX_Internal_Attribute
{
	/* Super class */
	GFXVertexAttribute attr;

	/* Hidden data */
	GLuint        buffer;
	unsigned int  index;
};

/******************************************************/
static void _gfx_hardware_layout_init_attrib(GLuint vao, const struct GFX_Internal_Attribute* attr, const GFX_Extensions* ext)
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
static void _gfx_hardware_layout_obj_free(GFX_Hardware_Object object, const GFX_Extensions* ext)
{
	struct GFX_Internal_Layout* layout = (struct GFX_Internal_Layout*)object;

	/* Delete everything */
	ext->DeleteVertexArrays(1, &layout->vao);
	layout->vao = 0;

	gfx_vector_clear(&layout->attributes);
}

/******************************************************/
static void _gfx_hardware_layout_obj_save(GFX_Hardware_Object object, const GFX_Extensions* ext)
{
	struct GFX_Internal_Layout* layout = (struct GFX_Internal_Layout*)object;

	/* Just don't clear the attribute vector */
	ext->DeleteVertexArrays(1, &layout->vao);
	layout->vao = 0;
}

/******************************************************/
static void _gfx_hardware_layout_obj_restore(GFX_Hardware_Object object, const GFX_Extensions* ext)
{
	struct GFX_Internal_Layout* layout = (struct GFX_Internal_Layout*)object;

	/* Create VAO */
	ext->GenVertexArrays(1, &layout->vao);

	/* Restore attributes */
	GFXVectorIterator it;
	for(it = layout->attributes.begin; it != layout->attributes.end; it = gfx_vector_next(&layout->attributes, it))
		_gfx_hardware_layout_init_attrib(layout->vao, (struct GFX_Internal_Attribute*)it, ext);
}

/******************************************************/
/* vtable for hardware layout object */
static GFX_Hardware_Funcs _gfx_hardware_layout_obj_funcs =
{
	_gfx_hardware_layout_obj_free,
	_gfx_hardware_layout_obj_save,
	_gfx_hardware_layout_obj_restore
};

/******************************************************/
static void _gfx_hardware_layout_init(struct GFX_Internal_Layout* layout, const GFX_Extensions* ext)
{
	/* Create VAO and attribute vector */
	ext->GenVertexArrays(1, &layout->vao);
	gfx_vector_init(&layout->attributes, sizeof(struct GFX_Internal_Attribute));

	/* Register as object */
	_gfx_hardware_object_register(layout, &_gfx_hardware_layout_obj_funcs);
}

/******************************************************/
static void _gfx_hardware_layout_free(struct GFX_Internal_Layout* layout, const GFX_Extensions* ext)
{
	if(layout)
	{
		_gfx_hardware_layout_obj_free(layout, ext);

		/* Unregister as object */
		_gfx_hardware_object_unregister(layout);
	}
}

/******************************************************/
static int _gfx_hardware_layout_set_attrib(struct GFX_Internal_Layout* layout, const struct GFX_Internal_Attribute* attr, const GFX_Extensions* ext)
{
	/* Derp */
	if(attr->index >= ext->MAX_VERTEX_ATTRIBS) return 0;

	/* Find the attribute */
	GFXVectorIterator it;
	for(it = layout->attributes.begin; it != layout->attributes.end; it = gfx_vector_next(&layout->attributes, it))
	{
		struct GFX_Internal_Attribute* set = (struct GFX_Internal_Attribute*)it;

		/* Replace data */
		if(set->index == attr->index)
		{
			*set = *attr;
			break;
		}
	}

	/* Insert new attribute */
	if(it == layout->attributes.end)
		if(gfx_vector_insert(&layout->attributes, attr, it) == layout->attributes.end) return 0;

	/* Send attribute to OpenGL */
	_gfx_hardware_layout_init_attrib(layout->vao, attr, ext);

	return 1;
}

/******************************************************/
static void _gfx_hardware_layout_remove_attrib(struct GFX_Internal_Layout* layout, unsigned int index, const GFX_Extensions* ext)
{
	/* Herp */
	if(index < ext->MAX_VERTEX_ATTRIBS)
	{
		/* Find the attribute and remove */
		GFXVectorIterator it;
		for(it = layout->attributes.begin; it != layout->attributes.end; it = gfx_vector_next(&layout->attributes, it))
		{
			if(((struct GFX_Internal_Attribute*)it)->index == index)
			{
				gfx_vector_erase(&layout->attributes, it);

				/* Send request to OpenGL */
				ext->BindVertexArray(layout->vao);
				ext->DisableVertexAttribArray(index);

				break;
			}
		}
	}
}
