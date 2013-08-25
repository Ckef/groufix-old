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

#include "groufix/hardware.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Data types associated with the GPU
 *******************************************************/

/** Storage data type */
typedef enum GFXDataType
{
	GFX_BYTE            = 0x1400,
	GFX_UNSIGNED_BYTE   = 0x1401,
	GFX_SHORT           = 0x1402,
	GFX_UNSIGNED_SHORT  = 0x1403,
	GFX_INT             = 0x1404,
	GFX_UNSIGNED_INT    = 0x1405,
	GFX_FLOAT           = 0x1406,
	GFX_HALF_FLOAT      = 0x140b

} GFXDataType;


/** Interpreted type */
typedef enum GFXInterpretType
{
	GFX_INTERPRET_FLOAT       = 0x00,
	GFX_INTERPRET_NORMALIZED  = 0x01,
	GFX_INTERPRET_INTEGER     = 0x02

} GFXInterpretType;


/********************************************************
 * Vertex specifications
 *******************************************************/

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


#ifdef __cplusplus
}
#endif

#endif // GFX_GEOMETRY_H
