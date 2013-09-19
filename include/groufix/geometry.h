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

#ifndef GFX_GEOMETRY_H
#define GFX_GEOMETRY_H

#include "groufix/buffer.h"
#include "groufix/utils.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Vertex Layout (vertex specifications + draw calls)
 *******************************************************/

/** Primitive types */
typedef enum GFXPrimitive
{
	GFX_POINTS          = 0x0000,
	GFX_LINES           = 0x0001,
	GFX_LINE_LOOP       = 0x0002,
	GFX_LINE_STRIP      = 0x0003,
	GFX_TRIANGLES       = 0x0004,
	GFX_TRIANGLE_STRIP  = 0x0005,
	GFX_TRIANGLE_FAN    = 0x0006

} GFXPrimitive;


/** Vertex Attribute */
typedef struct GFXVertexAttribute
{
	unsigned char     size;      /* Number of elements */
	GFXDataType       type;      /* Data type of each element */
	GFXInterpretType  interpret; /* How to interpret each element */

	size_t            stride;    /* Byte offset between consecutive attributes */
	size_t            offset;    /* Byte offset of the first occurence of the attribute */
	unsigned int      divisor;   /* Rate at which attributes advance, 0 for no instancing */

} GFXVertexAttribute;


/** Draw call */
typedef struct GFXDrawCall
{
	GFXPrimitive  primitive;
	GFXDataType   indexType; /* Can only be an unsigned type */

	uintptr_t     first;     /* First index (direct) or byte offset (indexed) */
	size_t        count;     /* Number of indices to draw */

} GFXDrawCall;


/** Vertex Layout */
typedef void* GFXVertexLayout;


/**
 * Creates a new vertex layout.
 *
 * @return Non-zero on success.
 *
 */
GFXVertexLayout gfx_vertex_layout_create(void);

/**
 * Makes sure the vertex layout is freed properly.
 *
 */
void gfx_vertex_layout_free(GFXVertexLayout layout);

/**
 * Sets an attribute of a vertex layout.
 *
 * @param index  Index of the attribute to set.
 * @param buffer Buffer to read this attribute from.
 * @return Zero on failure.
 *
 */
int gfx_vertex_layout_set_attribute(GFXVertexLayout layout, unsigned int index, const GFXVertexAttribute* attr, const GFXBuffer* buffer);

/**
 * Removes an attribute from a vertex layout.
 *
 */
void gfx_vertex_layout_remove_attribute(GFXVertexLayout, unsigned int index);


#ifdef __cplusplus
}
#endif

#endif // GFX_GEOMETRY_H
