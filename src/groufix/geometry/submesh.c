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
/** \brief Internal Submesh */
struct GFX_Internal_Submesh
{
	/* Draw calls */
	GFXVector  drawCalls;  /* Stores GFXDrawCall */

	/* Layout */
	GLuint     vao;
	GFXVector  attributes; /* Stores GFX_Internal_Attribute */
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
static void _gfx_submesh_init_attrib(GLuint vao, const struct GFX_Internal_Attribute* attr, const GFX_Extensions* ext)
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
static void _gfx_submesh_obj_free(GFX_Hardware_Object object, const GFX_Extensions* ext)
{
	struct GFX_Internal_Submesh* sub = (struct GFX_Internal_Submesh*)object;

	/* Delete everything */
	ext->DeleteVertexArrays(1, &sub->vao);
	sub->vao = 0;

	gfx_vector_clear(&sub->attributes);
}

/******************************************************/
static void _gfx_submesh_obj_save(GFX_Hardware_Object object, const GFX_Extensions* ext)
{
	struct GFX_Internal_Submesh* sub = (struct GFX_Internal_Submesh*)object;

	/* Just don't clear the attribute vector */
	ext->DeleteVertexArrays(1, &sub->vao);
	sub->vao = 0;
}

/******************************************************/
static void _gfx_submesh_obj_restore(GFX_Hardware_Object object, const GFX_Extensions* ext)
{
	struct GFX_Internal_Submesh* sub = (struct GFX_Internal_Submesh*)object;

	/* Create VAO */
	ext->GenVertexArrays(1, &sub->vao);

	/* Restore attributes */
	GFXVectorIterator it;
	for(it = sub->attributes.begin; it != sub->attributes.end; it = gfx_vector_next(&sub->attributes, it))
		_gfx_submesh_init_attrib(sub->vao, (struct GFX_Internal_Attribute*)it, ext);
}

/******************************************************/
/* vtable for hardware part of submesh */
static GFX_Hardware_Funcs _gfx_submesh_obj_funcs =
{
	_gfx_submesh_obj_free,
	_gfx_submesh_obj_save,
	_gfx_submesh_obj_restore
};

/******************************************************/
static void _gfx_submesh_init(struct GFX_Internal_Submesh* submesh, const GFX_Extensions* ext)
{
	/* Create VAO and vectors */
	ext->GenVertexArrays(1, &submesh->vao);
	gfx_vector_init(&submesh->drawCalls, sizeof(GFXDrawCall));
	gfx_vector_init(&submesh->attributes, sizeof(struct GFX_Internal_Attribute));

	/* Register as object */
	_gfx_hardware_object_register(submesh, &_gfx_submesh_obj_funcs);
}

/******************************************************/
static void _gfx_submesh_free(struct GFX_Internal_Submesh* submesh, const GFX_Extensions* ext)
{
	if(submesh)
	{
		gfx_vector_clear(&submesh->drawCalls);
		_gfx_submesh_obj_free(submesh, ext);

		/* Unregister as object */
		_gfx_hardware_object_unregister(submesh);
	}
}

/******************************************************/
static int _gfx_submesh_set_attrib(struct GFX_Internal_Submesh* submesh, const struct GFX_Internal_Attribute* attr, const GFX_Extensions* ext)
{
	/* Derp */
	if(attr->index >= ext->MAX_VERTEX_ATTRIBS) return 0;

	/* Find the attribute */
	GFXVectorIterator it;
	for(it = submesh->attributes.begin; it != submesh->attributes.end; it = gfx_vector_next(&submesh->attributes, it))
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
	if(it == submesh->attributes.end)
		if(gfx_vector_insert(&submesh->attributes, attr, it) == submesh->attributes.end) return 0;

	/* Send attribute to OpenGL */
	_gfx_submesh_init_attrib(submesh->vao, attr, ext);

	return 1;
}

/******************************************************/
static void _gfx_submesh_remove_attrib(struct GFX_Internal_Submesh* submesh, unsigned int index, const GFX_Extensions* ext)
{
	/* Herp */
	if(index < ext->MAX_VERTEX_ATTRIBS)
	{
		/* Find the attribute and remove */
		GFXVectorIterator it;
		for(it = submesh->attributes.begin; it != submesh->attributes.end; it = gfx_vector_next(&submesh->attributes, it))
		{
			if(((struct GFX_Internal_Attribute*)it)->index == index)
			{
				gfx_vector_erase(&submesh->attributes, it);

				/* Send request to OpenGL */
				ext->BindVertexArray(submesh->vao);
				ext->DisableVertexAttribArray(index);

				break;
			}
		}
	}
}
