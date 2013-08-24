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

#include "groufix/internal.h"
#include "groufix/containers/vector.h"

#include <stdlib.h>

/******************************************************/
/** \brief Internal hardware layout */
struct GFX_Internal_Layout
{
	/* Super class */
	GFX_Hardware_Layout layout;

	GFXVector attributes; /* Stores the below */
};

/** \brief Layout Attribute */
struct GFX_Internal_Attribute
{
	/* Super class */
	GFXVertexAttribute attribute;

	/* Hidden data */
	GFX_Hardware_Buffer  buffer;
	unsigned int         index;
};

/******************************************************/
static void _gfx_hardware_layout_init_attrib(GFX_Hardware_Layout layout, struct GFX_Internal_Attribute* attr, const GFX_Extensions* ext)
{
	/* Set the attribute */
	ext->BindVertexArray(layout);
	ext->EnableVertexAttribArray(attr->index);

	ext->BindBuffer(GL_ARRAY_BUFFER, attr->buffer);

	if(attr->attribute.interpret & GFX_INTERPRET_INTEGER) ext->VertexAttribIPointer(
		attr->index,
		attr->attribute.size,
		attr->attribute.type,
		attr->attribute.stride,
		(GLvoid*)attr->attribute.offset
	);
	else ext->VertexAttribPointer(
		attr->index,
		attr->attribute.size,
		attr->attribute.type,
		attr->attribute.interpret & GFX_INTERPRET_NORMALIZED,
		attr->attribute.stride,
		(GLvoid*)attr->attribute.offset
	);

	/* Check if non-zero to avoid extension error */
	if(attr->attribute.divisor) ext->VertexAttribDivisor(attr->index, attr->attribute.divisor);
}

/******************************************************/
static void _gfx_hardware_layout_obj_free(GFX_Hardware_Object object, const GFX_Extensions* ext)
{
	struct GFX_Internal_Layout* layout = (struct GFX_Internal_Layout*)object;

	/* Delete everything */
	ext->DeleteVertexArrays(1, &layout->layout);
	layout->layout = 0;

	gfx_vector_clear(&layout->attributes);
}

/******************************************************/
static void _gfx_hardware_layout_obj_save(GFX_Hardware_Object object, const GFX_Extensions* ext)
{
	struct GFX_Internal_Layout* layout = (struct GFX_Internal_Layout*)object;

	/* Just don't clear the attribute vector */
	ext->DeleteVertexArrays(1, &layout->layout);
	layout->layout = 0;
}

/******************************************************/
static void _gfx_hardware_layout_obj_restore(GFX_Hardware_Object object, const GFX_Extensions* ext)
{
	struct GFX_Internal_Layout* layout = (struct GFX_Internal_Layout*)object;

	/* Create VAO */
	ext->GenVertexArrays(1, &layout->layout);

	/* Restore attributes */
	GFXVectorIterator it;
	for(it = layout->attributes.begin; it != layout->attributes.end; it = gfx_vector_next(&layout->attributes, it))
		_gfx_hardware_layout_init_attrib(layout->layout, (struct GFX_Internal_Attribute*)it, ext);
}

/******************************************************/
/* vtable for hardware buffer object */
static GFX_Hardware_Funcs _gfx_hardware_layout_obj_funcs =
{
	_gfx_hardware_layout_obj_free,
	_gfx_hardware_layout_obj_save,
	_gfx_hardware_layout_obj_restore
};

/******************************************************/
GFX_Hardware_Layout* _gfx_hardware_layout_create(const GFX_Extensions* ext)
{
	struct GFX_Internal_Layout* layout = malloc(sizeof(struct GFX_Internal_Layout));
	if(!layout) return NULL;

	/* Create VAO and attribute vector */
	ext->GenVertexArrays(1, &layout->layout);
	gfx_vector_init(&layout->attributes, sizeof(struct GFX_Internal_Attribute));

	/* Register as object */
	_gfx_hardware_object_register(layout, &_gfx_hardware_layout_obj_funcs);

	return (GFX_Hardware_Layout*)layout;
}

/******************************************************/
void _gfx_hardware_layout_free(GFX_Hardware_Layout* layout, const GFX_Extensions* ext)
{
	if(layout)
	{
		_gfx_hardware_layout_obj_free(layout, ext);
		free(layout);

		/* Unregister as object */
		_gfx_hardware_object_unregister(layout);
	}
}

/******************************************************/
int _gfx_hardware_layout_set_attrib(GFX_Hardware_Layout* layout, unsigned int index, const GFXVertexAttribute* attr, GFX_Hardware_Buffer src, const GFX_Extensions* ext)
{
	/* Derp */
	if(index >= ext->MAX_VERTEX_ATTRIBS) return 0;

	/* Create attribute */
	struct GFX_Internal_Attribute new;
	new.attribute = *attr;
	new.buffer = src;
	new.index = index;

	struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;

	/* Find the attribute */
	GFXVectorIterator it;
	for(it = internal->attributes.begin; it != internal->attributes.end; it = gfx_vector_next(&internal->attributes, it))
	{
		struct GFX_Internal_Attribute* set = (struct GFX_Internal_Attribute*)it;

		/* Replace data */
		if(set->index == index)
		{
			*set = new;
			break;
		}
	}

	/* Insert new attribute */
	if(it == internal->attributes.end)
		if(gfx_vector_insert(&internal->attributes, &new, it) == internal->attributes.end) return 0;

	/* Send attribute to OpenGL */
	_gfx_hardware_layout_init_attrib(*layout, &new, ext);

	return 1;
}

/******************************************************/
int _gfx_hardware_layout_get_attrib(GFX_Hardware_Layout* layout, unsigned int index, GFXVertexAttribute* attr, GFX_Hardware_Buffer* src, const GFX_Extensions* ext)
{
	/* Herp */
	if(index >= ext->MAX_VERTEX_ATTRIBS) return 0;

	struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;

	/* Find the attribute */
	GFXVectorIterator it;
	for(it = internal->attributes.begin; it != internal->attributes.end; it = gfx_vector_next(&internal->attributes, it))
	{
		struct GFX_Internal_Attribute* get = (struct GFX_Internal_Attribute*)it;

		/* Return data */
		if(get->index == index)
		{
			*attr = get->attribute;
			*src = get->buffer;

			return 1;
		}
	}
	return 0;
}

/******************************************************/
void _gfx_hardware_layout_remove_attrib(GFX_Hardware_Layout* layout, unsigned int index, const GFX_Extensions* ext)
{
	/* Merp */
	if(index < ext->MAX_VERTEX_ATTRIBS)
	{
		struct GFX_Internal_Layout* internal = (struct GFX_Internal_Layout*)layout;

		/* Find the attribute and remove */
		GFXVectorIterator it;
		for(it = internal->attributes.begin; it != internal->attributes.end; it = gfx_vector_next(&internal->attributes, it))
		{
			if(((struct GFX_Internal_Attribute*)it)->index == index)
			{
				gfx_vector_erase(&internal->attributes, it);

				/* Send request to OpenGL */
				ext->BindVertexArray(*layout);
				ext->DisableVertexAttribArray(index);

				break;
			}
		}
	}
}
