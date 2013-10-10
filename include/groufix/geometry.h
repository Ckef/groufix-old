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

#include "groufix/memory.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Vertex Layout metadata
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


/** Draw call */
typedef struct GFXDrawCall
{
	GFXPrimitive  primitive;
	GFXDataType   indexType; /* Can only be an unsigned type */

	uintptr_t     first;     /* First index (regular) or byte offset (indexed) */
	size_t        count;     /* Number of vertices to draw */

} GFXDrawCall;


/** Vertex Attribute */
typedef struct GFXVertexAttribute
{
	unsigned char     size;      /* Number of elements */
	GFXDataType       type;      /* Data type of each element */
	GFXInterpretType  interpret; /* How to interpret each element */

	size_t            stride;    /* Byte offset between consecutive attributes */
	size_t            offset;    /* Byte offset of the first occurrence of the attribute */
	unsigned int      divisor;   /* Rate at which to advance, 0 for no instancing, requires GFX_EXT_INSTANCED_ATTRIBUTES */

} GFXVertexAttribute;


/********************************************************
 * Vertex Layout (vertex specifications + draw calls)
 *******************************************************/

/** Vertex Layout */
typedef struct GFXVertexLayout
{
	size_t id; /* Unique ID */

} GFXVertexLayout;


/**
 * Creates a new vertex layout.
 *
 * @return NULL on failure.
 *
 */
GFXVertexLayout* gfx_vertex_layout_create(void);

/**
 * Makes sure the vertex layout is freed properly.
 *
 */
void gfx_vertex_layout_free(GFXVertexLayout* layout);

/**
 * Sets an attribute of a vertex layout.
 *
 * @param index  Index of the attribute to set (must be < GFX_LIM_MAX_VERTEX_ATTRIBS).
 * @param buffer Buffer to read this attribute from (cannot be NULL).
 * @return Zero on failure.
 *
 */
int gfx_vertex_layout_set_attribute(GFXVertexLayout* layout, unsigned int index, const GFXVertexAttribute* attr, const GFXBuffer* buffer);

/**
 * Retrieves an attribute from a vertex layout.
 *
 * @param index Index of the attribute to retrieve.
 * @return Zero on failure (nothing is written to the output parameters).
 *
 */
int gfx_vertex_layout_get_attribute(GFXVertexLayout* layout, unsigned int index, GFXVertexAttribute* attr);

/**
 * Removes an attribute from a vertex layout.
 *
 * @param index  Index of the attribute to remove.
 *
 */
void gfx_vertex_layout_remove_attribute(GFXVertexLayout* layout, unsigned int index);

/**
 * Adds a draw call to the vertex layout.
 *
 * @param buffer Index buffer associated with indexed drawing, can be NULL.
 * @return index of the draw call (0 on failure).
 *
 */
unsigned short gfx_vertex_layout_push(GFXVertexLayout* layout, const GFXDrawCall* call, const GFXBuffer* buffer);

/**
 * Changes a draw call of the vertex layout.
 *
 * @param index  Index of the draw call.
 * @param buffer Index buffer associated with indexed drawing, can be NULL.
 * @return Non-zero if the draw call could be changed.
 *
 */
int gfx_vertex_layout_set(GFXVertexLayout* layout, unsigned short index, const GFXDrawCall* call, const GFXBuffer* buffer);

/**
 * Retrieves a draw call from the vertex layout.
 *
 * @param index Index of the draw call to retrieve.
 * @return Zero on failure (nothing is written to the output parameters).
 *
 */
int gfx_vertex_layout_get(GFXVertexLayout* layout, unsigned short index, GFXDrawCall* call);

/**
 * Removes the last added draw call from the vertex layout.
 *
 * @return index of the removed draw call (0 if no calls were present).
 *
 */
unsigned short gfx_vertex_layout_pop(GFXVertexLayout* layout);


/********************************************************
 * Issue draw calls associated with a vertex layout
 *******************************************************/

/**
 * Regular drawing using a given vertex layout.
 *
 */
void gfx_vertex_layout_draw(GFXVertexLayout* layout, unsigned short num, unsigned short startIndex);

/**
 * Indexed drawing using a given vertex layout.
 *
 */
void gfx_vertex_layout_draw_indexed(GFXVertexLayout* layout, unsigned short num, unsigned short startIndex);

/**
 * Instanced drawing using a given vertex layout.
 *
 */
void gfx_vertex_layout_draw_instanced(GFXVertexLayout* layout, unsigned short num, unsigned short startIndex, size_t inst);

/**
 * Indexed AND instanced drawing using a given vertex layout.
 *
 */
void gfx_vertex_layout_draw_indexed_instanced(GFXVertexLayout* layout, unsigned short num, unsigned short startIndex, size_t inst);


#ifdef __cplusplus
}
#endif

#endif // GFX_GEOMETRY_H
