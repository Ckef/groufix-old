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

#include <stdlib.h>

/******************************************************/
GFXHardwareLayout* gfx_hardware_layout_create(const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	/* Allocate */
	GFXHardwareLayout* layout = (GFXHardwareLayout*)malloc(sizeof(GFXHardwareLayout));
	if(!layout) return NULL;

	GLuint vao;
	ext->GenVertexArrays(1, &vao);
	ext->BindVertexArray(vao);

	layout->handle = vao;

	return layout;
}

/******************************************************/
void gfx_hardware_layout_free(GFXHardwareLayout* layout, const GFXHardwareContext cnt)
{
	if(layout)
	{
		const GFX_Extensions* ext = VOID_TO_EXT(cnt);

		GLuint vao = layout->handle;
		ext->DeleteVertexArrays(1, &vao);

		free(layout);
	}
}

/******************************************************/
void gfx_hardware_layout_bind(GFXHardwareLayout* layout, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	ext->BindVertexArray(layout->handle);
}

/******************************************************/
void gfx_hardware_layout_unbind(const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	ext->BindVertexArray(0);
}

/******************************************************/
unsigned int gfx_hardware_layout_get_max_attributes(const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	GLint max;
	ext->GetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max);

	return max;
}

/******************************************************/
int gfx_hardware_layout_enable_attribute(unsigned int index, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	/* Validate index */
	if(index >= gfx_hardware_layout_get_max_attributes(cnt)) return 0;

	ext->EnableVertexAttribArray(index);

	return 1;
}

/******************************************************/
int gfx_hardware_layout_disable_attribute(unsigned int index, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	/* Validate index */
	if(index >= gfx_hardware_layout_get_max_attributes(cnt)) return 0;

	ext->DisableVertexAttribArray(index);

	return 1;
}

/******************************************************/
int gfx_hardware_layout_set_attribute(unsigned int index, const GFXHardwareAttribute* attr, GFXHardwareBuffer* src, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	/* Validate all of it */
	if(index >= gfx_hardware_layout_get_max_attributes(cnt) || attr->size < 1 || attr->size > 4) return 0;

	ext->BindBuffer(GL_ARRAY_BUFFER, src->handle);

	if(attr->interpret & GFX_LAYOUT_INTEGER) ext->VertexAttribIPointer(
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
		attr->interpret & GFX_LAYOUT_NORMALIZED,
		attr->stride,
		(GLvoid*)attr->offset
	);

	return 1;
}

/******************************************************/
int gfx_hardware_layout_set_attribute_divisor(unsigned int index, unsigned int instances, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	if(index >= gfx_hardware_layout_get_max_attributes(cnt)) return 0;

	ext->VertexAttribDivisor(index, instances);

	return 1;
}

/******************************************************/
int gfx_hardware_layout_get_attribute(unsigned int index, GFXHardwareAttribute* attr, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	if(index >= gfx_hardware_layout_get_max_attributes(cnt)) return 0;

	GLint size;
	GLuint type;
	GLint norm;
	GLint integ;
	GLint stride;
	GLvoid* offset;
	ext->GetVertexAttribIiv(index, GL_VERTEX_ATTRIB_ARRAY_SIZE, &size);
	ext->GetVertexAttribIuiv(index, GL_VERTEX_ATTRIB_ARRAY_TYPE, &type);
	ext->GetVertexAttribIiv(index, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &norm);
	ext->GetVertexAttribIiv(index, GL_VERTEX_ATTRIB_ARRAY_INTEGER, &integ);
	ext->GetVertexAttribIiv(index, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride);
	ext->GetVertexAttribPointerv(index, GL_VERTEX_ATTRIB_ARRAY_POINTER, &offset);

	attr->size = size;
	attr->type = type;
	attr->interpret = integ ? GFX_LAYOUT_INTEGER : (norm ? GFX_LAYOUT_NORMALIZED : GFX_LAYOUT_FLOAT);
	attr->stride = stride;
	attr->offset = VOID_TO_UINT(offset);

	return 1;
}

/******************************************************/
void gfx_hardware_layout_set_index_buffer(GFXHardwareBuffer* buffer, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	ext->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->handle);
}
