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

#include <string.h>

/******************************************************/
int _gfx_is_data_type_packed(GFXDataType type)
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
		case GFX_UNSIGNED_INT_9_9_9_5 :
			return 1;
	}
	return 0;
}

/******************************************************/
unsigned char _gfx_sizeof_data_type(GFXDataType type)
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
		case GFX_UNSIGNED_INT_9_9_9_5 :
			return 4;
	}

	return 0;
}

/******************************************************/
GLint _gfx_texture_format_to_internal(GFXTextureFormat format)
{
	/* Check for packed formats */
	switch(format.type.packed)
	{
		case GFX_UNSIGNED_SHORT_5_6_5     : return GL_RGB565;
		case GFX_UNSIGNED_SHORT_4_4_4_4   : return GL_RGBA4;
		case GFX_UNSIGNED_SHORT_5_5_5_1   : return GL_RGB5_A1;
		case GFX_INT_10_10_10_2           : return GL_RGB10_A2;
		case GFX_UNSIGNED_INT_10_10_10_2  : return GL_RGB10_A2UI;
		case GFX_UNSIGNED_INT_11F_11F_10F : return GL_R11F_G11F_B10F;
		case GFX_UNSIGNED_INT_9_9_9_5     : return GL_RGB9_E5;
	}

	/* ??? */
	return -1;
}

/******************************************************/
GFXTextureFormat _gfx_texture_format_from_internal(GLint format)
{
	GFXTextureFormat ret;
	memset(&ret, 0, sizeof(GFXTextureFormat));

	switch(format)
	{
		/* Check for packed formats */
		case GL_RGB565 :
			ret.components  = 3;
			ret.type.packed = GFX_UNSIGNED_SHORT_5_6_5;
			ret.interpret   = GFX_INTERPRET_NORMALIZED;
			break;

		case GL_RGBA4 :
			ret.components  = 4;
			ret.type.packed = GFX_UNSIGNED_SHORT_4_4_4_4;
			ret.interpret   = GFX_INTERPRET_NORMALIZED;
			break;

		case GL_RGB5_A1 :
			ret.components  = 4;
			ret.type.packed = GFX_UNSIGNED_SHORT_5_5_5_1;
			ret.interpret   = GFX_INTERPRET_NORMALIZED;
			break;

		case GL_RGB10_A2 :
			ret.components  = 4;
			ret.type.packed = GFX_INT_10_10_10_2;
			ret.interpret   = GFX_INTERPRET_NORMALIZED;
			break;

		case GL_RGB10_A2UI :
			ret.components  = 4;
			ret.type.packed = GFX_UNSIGNED_INT_10_10_10_2;
			ret.interpret   = GFX_INTERPRET_INTEGER;
			break;

		case GL_R11F_G11F_B10F :
			ret.components  = 3;
			ret.type.packed = GFX_UNSIGNED_INT_11F_11F_10F;
			ret.interpret   = GFX_INTERPRET_FLOAT;
			break;

		case GL_RGB9_E5 :
			ret.components  = 3;
			ret.type.packed = GFX_UNSIGNED_INT_9_9_9_5;
			ret.interpret   = GFX_INTERPRET_FLOAT;
			break;
	}

	return ret;
}
