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

#ifndef GFX_MEMORY_DATATYPES_H
#define GFX_MEMORY_DATATYPES_H

#include "groufix/internal.h"

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Datatype helpers
 *******************************************************/

/**
 * Returns 1 if packed data, 0 if unpacked.
 *
 */
inline int _gfx_is_data_type_packed(GFXDataType type)
{
	switch(type.packed)
	{
		/* Check for any packed type */
		case GFX_UNSIGNED_SHORT_5_6_5 :
		case GFX_UNSIGNED_SHORT_4_4_4_4 :
		case GFX_UNSIGNED_SHORT_5_5_5_1 :
		case GFX_INT_10_10_10_2 :
		case GFX_UNSIGNED_INT_10_10_10_2 :
		case GFX_UNSIGNED_INT_11F_11F_10F :
		case GFX_UNSIGNED_INT_9_9_9_5E :
		case GFX_UNSIGNED_INT_24_8 :
		case GFX_FLOAT_UNSIGNED_INT_24_8 :
			return 1;
	}
	return 0;
}

/**
 * Returns the size of a data type in bytes.
 *
 */
inline unsigned char _gfx_sizeof_data_type(GFXDataType type)
{
	/* Check for unpacked datatypes */
	switch(type.unpacked)
	{
		case GFX_BYTE :
		case GFX_UNSIGNED_BYTE :
			return 1;

		case GFX_SHORT :
		case GFX_UNSIGNED_SHORT :
		case GFX_HALF_FLOAT :
			return 2;

		case GFX_INT :
		case GFX_UNSIGNED_INT:
		case GFX_FLOAT :
			return 4;
	}

	/* Check for packed datatypes */
	switch(type.packed)
	{
		case GFX_UNSIGNED_SHORT_5_6_5 :
		case GFX_UNSIGNED_SHORT_4_4_4_4 :
		case GFX_UNSIGNED_SHORT_5_5_5_1 :
			return 2;

		case GFX_INT_10_10_10_2 :
		case GFX_UNSIGNED_INT_10_10_10_2 :
		case GFX_UNSIGNED_INT_11F_11F_10F :
		case GFX_UNSIGNED_INT_9_9_9_5E :
		case GFX_UNSIGNED_INT_24_8 :
			return 4;

		case GFX_FLOAT_UNSIGNED_INT_24_8 :
			return 8;
	}

	return 0;
}


/********************************************************
 * Format converters
 *******************************************************/

/**
 * Converts a texture format to a client pixel format.
 *
 * @return -1 on failure.
 *
 */
GLint _gfx_texture_format_to_pixel_format(GFXTextureFormat format);

/**
 * Converts a texture format to an internal format.
 *
 * @return -1 on failure.
 *
 */
GLint _gfx_texture_format_to_internal(GFXTextureFormat format);

/**
 * Converts an internal format to a texture format.
 *
 */
GFXTextureFormat _gfx_texture_format_from_internal(GLint format);


#ifdef __cplusplus
}
#endif

#endif // GFX_MEMORY_DATATYPES_H