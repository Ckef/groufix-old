/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
 *
 * Groufix is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Groufix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Groufix.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "groufix/memory/internal.h"

#include <string.h>


/* Define unknown format */
#define _GFX_UNKNOWN_FORMAT         -0x0001


/* Define unsupported packed formats */
#ifndef GL_RGB565
	#define GL_RGB565              -0x0002
#endif
#ifndef GL_RGBA4
	#define GL_RGBA4               -0x0003
#endif
#ifndef GL_RGB5_A1
	#define GL_RGB5_A1             -0x0004
#endif
#ifndef GL_RGB10_A2
	#define GL_RGB10_A2            -0x0005
#endif
#ifndef GL_RGB10_A2UI
	#define GL_RGB10_A2UI          -0x0006
#endif
#ifndef GL_R11F_G11F_B10F
	#define GL_R11F_G11F_B10F      -0x0007
#endif
#ifndef GL_RGB9_E5
	#define GL_RGB9_E5             -0x0008
#endif
#ifndef GL_DEPTH24_STENCIL8
	#define GL_DEPTH24_STENCIL8    -0x0009
#endif
#ifndef GL_DEPTH32F_STENCIL8
	#define GL_DEPTH32F_STENCIL8   -0x000a
#endif

/* Define unsupported floating point formats */
#ifndef GL_R16F
	#define GL_R16F                -0x000b
#endif
#ifndef GL_R32F
	#define GL_R32F                -0x000c
#endif
#ifndef GL_RG16F
	#define GL_RG16F               -0x000d
#endif
#ifndef GL_RG32F
	#define GL_RG32F               -0x000e
#endif
#ifndef GL_RGB16F
	#define GL_RGB16F              -0x000f
#endif
#ifndef GL_RGB32F
	#define GL_RGB32F              -0x0010
#endif
#ifndef GL_RGBA16F
	#define GL_RGBA16F             -0x0011
#endif
#ifndef GL_RGBA32F
	#define GL_RGBA32F             -0x0012
#endif

/* Define unsupported unpacked normalized formats */
#ifndef GL_R8_SNORM
	#define GL_R8_SNORM            -0x0013
#endif
#ifndef GL_R8
	#define GL_R8                  -0x0014
#endif
#ifndef GL_R16_SNORM
	#define GL_R16_SNORM           -0x0015
#endif
#ifndef GL_R16
	#define GL_R16                 -0x0016
#endif
#ifndef GL_RG8_SNORM
	#define GL_RG8_SNORM           -0x0017
#endif
#ifndef GL_RG8
	#define GL_RG8                 -0x0018
#endif
#ifndef GL_RG16_SNORM
	#define GL_RG16_SNORM          -0x0019
#endif
#ifndef GL_RG16
	#define GL_RG16                -0x001a
#endif
#ifndef GL_RGB8_SNORM
	#define GL_RGB8_SNORM          -0x001b
#endif
#ifndef GL_RGB8
	#define GL_RGB8                -0x001c
#endif
#ifndef GL_RGB16_SNORM
	#define GL_RGB16_SNORM         -0x001d
#endif
#ifndef GL_RGB16
	#define GL_RGB16               -0x001e
#endif
#ifndef GL_RGBA8_SNORM
	#define GL_RGBA8_SNORM         -0x001f
#endif
#ifndef GL_RGBA8
	#define GL_RGBA8               -0x0020
#endif
#ifndef GL_RGBA16_SNORM
	#define GL_RGBA16_SNORM        -0x0021
#endif
#ifndef GL_RGBA16
	#define GL_RGBA16              -0x0022
#endif

/* Define unsupported unpacked integral formats */
#ifndef GL_R8I
	#define GL_R8I                 -0x0023
#endif
#ifndef GL_R8UI
	#define GL_R8UI                -0x0024
#endif
#ifndef GL_R16I
	#define GL_R16I                -0x0025
#endif
#ifndef GL_R16UI
	#define GL_R16UI               -0x0026
#endif
#ifndef GL_R32I
	#define GL_R32I                -0x0027
#endif
#ifndef GL_R32UI
	#define GL_R32UI               -0x0028
#endif
#ifndef GL_RG8I
	#define GL_RG8I                -0x0029
#endif
#ifndef GL_RG8UI
	#define GL_RG8UI               -0x002a
#endif
#ifndef GL_RG16I
	#define GL_RG16I               -0x002b
#endif
#ifndef GL_RG16UI
	#define GL_RG16UI              -0x002c
#endif
#ifndef GL_RG32I
	#define GL_RG32I               -0x002d
#endif
#ifndef GL_RG32UI
	#define GL_RG32UI              -0x002e
#endif
#ifndef GL_RGB8I
	#define GL_RGB8I               -0x002f
#endif
#ifndef GL_RGB8UI
	#define GL_RGB8UI              -0x0030
#endif
#ifndef GL_RGB16I
	#define GL_RGB16I              -0x0031
#endif
#ifndef GL_RGB16UI
	#define GL_RGB16UI             -0x0032
#endif
#ifndef GL_RGB32I
	#define GL_RGB32I              -0x0033
#endif
#ifndef GL_RGB32UI
	#define GL_RGB32UI             -0x0034
#endif
#ifndef GL_RGBA8I
	#define GL_RGBA8I              -0x0035
#endif
#ifndef GL_RGBA8UI
	#define GL_RGBA8UI             -0x0036
#endif
#ifndef GL_RGBA16I
	#define GL_RGBA16I             -0x0037
#endif
#ifndef GL_RGBA16UI
	#define GL_RGBA16UI            -0x0038
#endif
#ifndef GL_RGBA32I
	#define GL_RGBA32I             -0x0039
#endif
#ifndef GL_RGBA32UI
	#define GL_RGBA32UI            -0x003a
#endif

/* Define unsupported depth formats */
#ifndef GL_DEPTH_COMPONENT16
	#define GL_DEPTH_COMPONENT16   -0x003b
#endif
#ifndef GL_DEPTH_COMPONENT24
	#define GL_DEPTH_COMPONENT24   -0x003c
#endif
#ifndef GL_DEPTH_COMPONENT32F
	#define GL_DEPTH_COMPONENT32F  -0x003d
#endif


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
		case GFX_UNSIGNED_INT_9_9_9_5E :
		case GFX_UNSIGNED_INT_24_8 :
		case GFX_FLOAT_UNSIGNED_INT_24_8 :
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
		case GFX_UNSIGNED_INT_9_9_9_5E :
		case GFX_UNSIGNED_INT_24_8 :
			return 4;

		case GFX_FLOAT_UNSIGNED_INT_24_8 :
			return 8;
	}

	return 0;
}

/******************************************************/
GLint _gfx_texture_format_to_pixel_format(GFXTextureFormat format)
{
	/* Check for packed formats */
	switch(format.type.packed)
	{
		case GFX_UNSIGNED_SHORT_5_6_5     : return GL_RGB;
		case GFX_UNSIGNED_SHORT_4_4_4_4   : return GL_RGBA;
		case GFX_UNSIGNED_SHORT_5_5_5_1   : return GL_RGBA;
		case GFX_INT_10_10_10_2           : return GL_RGBA;
		case GFX_UNSIGNED_INT_10_10_10_2  : return GL_RGBA_INTEGER;
		case GFX_UNSIGNED_INT_11F_11F_10F : return GL_RGB;
		case GFX_UNSIGNED_INT_9_9_9_5E    : return GL_RGB;
		case GFX_UNSIGNED_INT_24_8        : return GL_DEPTH_STENCIL;
		case GFX_FLOAT_UNSIGNED_INT_24_8  : return GL_DEPTH_STENCIL;
	}

	/* Decode unpacked types */
	switch(format.interpret)
	{
		/* Integral format */
		case GFX_INTERPRET_INTEGER : switch(format.components)
		{
			case 1 : return GL_RED_INTEGER;
			case 2 : return GL_RG_INTEGER;
			case 3 : return GL_RGB_INTEGER;
			case 4 : return GL_RGBA_INTEGER;
			default : return _GFX_UNKNOWN_FORMAT;
		}

		/* Depth format */
		case GFX_INTERPRET_DEPTH : switch(format.components)
		{
			case 1 : return GL_DEPTH_COMPONENT;
			default : return _GFX_UNKNOWN_FORMAT;
		}

		/* Floating point and normalized formats */
		default : switch(format.components)
		{
			case 1 : return GL_RED;
			case 2 : return GL_RG;
			case 3 : return GL_RGB;
			case 4 : return GL_RGBA;
			default : return _GFX_UNKNOWN_FORMAT;
		}
	}
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
		case GFX_UNSIGNED_INT_9_9_9_5E    : return GL_RGB9_E5;
		case GFX_UNSIGNED_INT_24_8        : return GL_DEPTH24_STENCIL8;
		case GFX_FLOAT_UNSIGNED_INT_24_8  : return GL_DEPTH32F_STENCIL8;
	}

	/* Decode unpacked types */
	switch(format.interpret)
	{
		/* Floating point format */
		case GFX_INTERPRET_FLOAT : switch(format.components)
		{
			case 1 : switch(format.type.unpacked)
			{
				case GFX_HALF_FLOAT : return GL_R16F;
				case GFX_FLOAT      : return GL_R32F;
				default             : return _GFX_UNKNOWN_FORMAT;
			}
			case 2 : switch(format.type.unpacked)
			{
				case GFX_HALF_FLOAT : return GL_RG16F;
				case GFX_FLOAT      : return GL_RG32F;
				default             : return _GFX_UNKNOWN_FORMAT;
			}
			case 3 : switch(format.type.unpacked)
			{
				case GFX_HALF_FLOAT : return GL_RGB16F;
				case GFX_FLOAT      : return GL_RGB32F;
				default             : return _GFX_UNKNOWN_FORMAT;
			}
			case 4 : switch(format.type.unpacked)
			{
				case GFX_HALF_FLOAT : return GL_RGBA16F;
				case GFX_FLOAT      : return GL_RGBA32F;
				default             : return _GFX_UNKNOWN_FORMAT;
			}
			default : return _GFX_UNKNOWN_FORMAT;
		}

		/* Normalized format */
		case GFX_INTERPRET_NORMALIZED : switch(format.components)
		{
			case 1 : switch(format.type.unpacked)
			{
				case GFX_BYTE           : return GL_R8_SNORM;
				case GFX_UNSIGNED_BYTE  : return GL_R8;
				case GFX_SHORT          : return GL_R16_SNORM;
				case GFX_UNSIGNED_SHORT : return GL_R16;
				default                 : return _GFX_UNKNOWN_FORMAT;
			}
			case 2 : switch(format.type.unpacked)
			{
				case GFX_BYTE           : return GL_RG8_SNORM;
				case GFX_UNSIGNED_BYTE  : return GL_RG8;
				case GFX_SHORT          : return GL_RG16_SNORM;
				case GFX_UNSIGNED_SHORT : return GL_RG16;
				default                 : return _GFX_UNKNOWN_FORMAT;
			}
			case 3 : switch(format.type.unpacked)
			{
				case GFX_BYTE           : return GL_RGB8_SNORM;
				case GFX_UNSIGNED_BYTE  : return GL_RGB8;
				case GFX_SHORT          : return GL_RGB16_SNORM;
				case GFX_UNSIGNED_SHORT : return GL_RGB16;
				default                 : return _GFX_UNKNOWN_FORMAT;
			}
			case 4 : switch(format.type.unpacked)
			{
				case GFX_BYTE           : return GL_RGBA8_SNORM;
				case GFX_UNSIGNED_BYTE  : return GL_RGBA8;
				case GFX_SHORT          : return GL_RGBA16_SNORM;
				case GFX_UNSIGNED_SHORT : return GL_RGBA16;
				default                 : return _GFX_UNKNOWN_FORMAT;
			}
			default : return _GFX_UNKNOWN_FORMAT;
		}

		/* Integral format */
		case GFX_INTERPRET_INTEGER : switch(format.components)
		{
			case 1 : switch(format.type.unpacked)
			{
				case GFX_BYTE           : return GL_R8I;
				case GFX_UNSIGNED_BYTE  : return GL_R8UI;
				case GFX_SHORT          : return GL_R16I;
				case GFX_UNSIGNED_SHORT : return GL_R16UI;
				case GFX_INT            : return GL_R32I;
				case GFX_UNSIGNED_INT   : return GL_R32UI;
				default                 : return _GFX_UNKNOWN_FORMAT;
			}
			case 2 : switch(format.type.unpacked)
			{
				case GFX_BYTE           : return GL_RG8I;
				case GFX_UNSIGNED_BYTE  : return GL_RG8UI;
				case GFX_SHORT          : return GL_RG16I;
				case GFX_UNSIGNED_SHORT : return GL_RG16UI;
				case GFX_INT            : return GL_RG32I;
				case GFX_UNSIGNED_INT   : return GL_RG32UI;
				default                 : return _GFX_UNKNOWN_FORMAT;
			}
			case 3 : switch(format.type.unpacked)
			{
				case GFX_BYTE           : return GL_RGB8I;
				case GFX_UNSIGNED_BYTE  : return GL_RGB8UI;
				case GFX_SHORT          : return GL_RGB16I;
				case GFX_UNSIGNED_SHORT : return GL_RGB16UI;
				case GFX_INT            : return GL_RGB32I;
				case GFX_UNSIGNED_INT   : return GL_RGB32UI;
				default                 : return _GFX_UNKNOWN_FORMAT;
			}
			case 4 : switch(format.type.unpacked)
			{
				case GFX_BYTE           : return GL_RGBA8I;
				case GFX_UNSIGNED_BYTE  : return GL_RGBA8UI;
				case GFX_SHORT          : return GL_RGBA16I;
				case GFX_UNSIGNED_SHORT : return GL_RGBA16UI;
				case GFX_INT            : return GL_RGBA32I;
				case GFX_UNSIGNED_INT   : return GL_RGBA32UI;
				default                 : return _GFX_UNKNOWN_FORMAT;
			}
			default : return _GFX_UNKNOWN_FORMAT;
		}

		/* Depth formats */
		case GFX_INTERPRET_DEPTH : switch(format.components)
		{
			case 1 : switch(format.type.unpacked)
			{
				case GFX_UNSIGNED_SHORT : return GL_DEPTH_COMPONENT16;
				case GFX_UNSIGNED_INT   : return GL_DEPTH_COMPONENT24;
				case GFX_FLOAT          : return GL_DEPTH_COMPONENT32F;
				default                 : return _GFX_UNKNOWN_FORMAT;
			}
			default : return _GFX_UNKNOWN_FORMAT;
		}
	}

	/* ??? */
	return -1;
}

/******************************************************/
GFXTextureFormat _gfx_texture_format_from_internal(GLint format)
{
	GFXTextureFormat ret;
	memset(&ret, 0, sizeof(GFXTextureFormat));

	/* Check for a valid format */
	if(format >= 0) switch(format)
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
			ret.type.packed = GFX_UNSIGNED_INT_9_9_9_5E;
			ret.interpret   = GFX_INTERPRET_FLOAT;
			break;

		case GL_DEPTH24_STENCIL8 :
			ret.components  = 2;
			ret.type.packed = GFX_UNSIGNED_INT_24_8;
			ret.interpret   = GFX_INTERPRET_DEPTH;
			break;

		case GL_DEPTH32F_STENCIL8 :
			ret.components  = 2;
			ret.type.packed = GFX_FLOAT_UNSIGNED_INT_24_8;
			ret.interpret   = GFX_INTERPRET_DEPTH;
			break;

		/* Unpacked floating point formats */
		case GL_R16F :
			ret.components    = 1;
			ret.type.unpacked = GFX_HALF_FLOAT;
			ret.interpret     = GFX_INTERPRET_FLOAT;
			break;

		case GL_R32F :
			ret.components    = 1;
			ret.type.unpacked = GFX_FLOAT;
			ret.interpret     = GFX_INTERPRET_FLOAT;
			break;

		case GL_RG16F :
			ret.components    = 2;
			ret.type.unpacked = GFX_HALF_FLOAT;
			ret.interpret     = GFX_INTERPRET_FLOAT;
			break;

		case GL_RG32F :
			ret.components    = 2;
			ret.type.unpacked = GFX_FLOAT;
			ret.interpret     = GFX_INTERPRET_FLOAT;
			break;

		case GL_RGB16F :
			ret.components    = 3;
			ret.type.unpacked = GFX_HALF_FLOAT;
			ret.interpret     = GFX_INTERPRET_FLOAT;
			break;

		case GL_RGB32F :
			ret.components    = 3;
			ret.type.unpacked = GFX_FLOAT;
			ret.interpret     = GFX_INTERPRET_FLOAT;
			break;

		case GL_RGBA16F :
			ret.components    = 4;
			ret.type.unpacked = GFX_HALF_FLOAT;
			ret.interpret     = GFX_INTERPRET_FLOAT;
			break;

		case GL_RGBA32F :
			ret.components    = 4;
			ret.type.unpacked = GFX_FLOAT;
			ret.interpret     = GFX_INTERPRET_FLOAT;
			break;

		/* Unpacked normalized formats */
		case GL_R8_SNORM :
			ret.components    = 1;
			ret.type.unpacked = GFX_BYTE;
			ret.interpret     = GFX_INTERPRET_NORMALIZED;
			break;

		case GL_R8 :
			ret.components    = 1;
			ret.type.unpacked = GFX_UNSIGNED_BYTE;
			ret.interpret     = GFX_INTERPRET_NORMALIZED;
			break;

		case GL_R16_SNORM :
			ret.components    = 1;
			ret.type.unpacked = GFX_SHORT;
			ret.interpret     = GFX_INTERPRET_NORMALIZED;
			break;

		case GL_R16 :
			ret.components    = 1;
			ret.type.unpacked = GFX_UNSIGNED_SHORT;
			ret.interpret     = GFX_INTERPRET_NORMALIZED;
			break;

		case GL_RG8_SNORM :
			ret.components    = 2;
			ret.type.unpacked = GFX_BYTE;
			ret.interpret     = GFX_INTERPRET_NORMALIZED;
			break;

		case GL_RG8 :
			ret.components    = 2;
			ret.type.unpacked = GFX_UNSIGNED_BYTE;
			ret.interpret     = GFX_INTERPRET_NORMALIZED;
			break;

		case GL_RG16_SNORM :
			ret.components    = 2;
			ret.type.unpacked = GFX_SHORT;
			ret.interpret     = GFX_INTERPRET_NORMALIZED;
			break;

		case GL_RG16 :
			ret.components    = 2;
			ret.type.unpacked = GFX_UNSIGNED_SHORT;
			ret.interpret     = GFX_INTERPRET_NORMALIZED;
			break;

		case GL_RGB8_SNORM :
			ret.components    = 3;
			ret.type.unpacked = GFX_BYTE;
			ret.interpret     = GFX_INTERPRET_NORMALIZED;
			break;

		case GL_RGB8 :
			ret.components    = 3;
			ret.type.unpacked = GFX_UNSIGNED_BYTE;
			ret.interpret     = GFX_INTERPRET_NORMALIZED;
			break;

		case GL_RGB16_SNORM :
			ret.components    = 3;
			ret.type.unpacked = GFX_SHORT;
			ret.interpret     = GFX_INTERPRET_NORMALIZED;
			break;

		case GL_RGB16 :
			ret.components    = 3;
			ret.type.unpacked = GFX_UNSIGNED_SHORT;
			ret.interpret     = GFX_INTERPRET_NORMALIZED;
			break;

		case GL_RGBA8_SNORM :
			ret.components    = 4;
			ret.type.unpacked = GFX_BYTE;
			ret.interpret     = GFX_INTERPRET_NORMALIZED;
			break;

		case GL_RGBA8 :
			ret.components    = 4;
			ret.type.unpacked = GFX_UNSIGNED_BYTE;
			ret.interpret     = GFX_INTERPRET_NORMALIZED;
			break;

		case GL_RGBA16_SNORM :
			ret.components    = 4;
			ret.type.unpacked = GFX_SHORT;
			ret.interpret     = GFX_INTERPRET_NORMALIZED;
			break;

		case GL_RGBA16 :
			ret.components    = 4;
			ret.type.unpacked = GFX_UNSIGNED_SHORT;
			ret.interpret     = GFX_INTERPRET_NORMALIZED;
			break;

		/* Unpacked integral formats */
		case GL_R8I :
			ret.components    = 1;
			ret.type.unpacked = GFX_BYTE;
			ret.interpret     = GFX_INTERPRET_INTEGER;
			break;

		case GL_R8UI :
			ret.components    = 1;
			ret.type.unpacked = GFX_UNSIGNED_BYTE;
			ret.interpret     = GFX_INTERPRET_INTEGER;
			break;

		case GL_R16I :
			ret.components    = 1;
			ret.type.unpacked = GFX_SHORT;
			ret.interpret     = GFX_INTERPRET_INTEGER;
			break;

		case GL_R16UI :
			ret.components    = 1;
			ret.type.unpacked = GFX_UNSIGNED_SHORT;
			ret.interpret     = GFX_INTERPRET_INTEGER;
			break;

		case GL_R32I :
			ret.components    = 1;
			ret.type.unpacked = GFX_INT;
			ret.interpret     = GFX_INTERPRET_INTEGER;
			break;

		case GL_R32UI :
			ret.components    = 1;
			ret.type.unpacked = GFX_UNSIGNED_INT;
			ret.interpret     = GFX_INTERPRET_INTEGER;
			break;

		case GL_RG8I :
			ret.components    = 2;
			ret.type.unpacked = GFX_BYTE;
			ret.interpret     = GFX_INTERPRET_INTEGER;
			break;

		case GL_RG8UI :
			ret.components    = 2;
			ret.type.unpacked = GFX_UNSIGNED_BYTE;
			ret.interpret     = GFX_INTERPRET_INTEGER;
			break;

		case GL_RG16I :
			ret.components    = 2;
			ret.type.unpacked = GFX_SHORT;
			ret.interpret     = GFX_INTERPRET_INTEGER;
			break;

		case GL_RG16UI :
			ret.components    = 2;
			ret.type.unpacked = GFX_UNSIGNED_SHORT;
			ret.interpret     = GFX_INTERPRET_INTEGER;
			break;

		case GL_RG32I :
			ret.components    = 2;
			ret.type.unpacked = GFX_INT;
			ret.interpret     = GFX_INTERPRET_INTEGER;
			break;

		case GL_RG32UI :
			ret.components    = 2;
			ret.type.unpacked = GFX_UNSIGNED_INT;
			ret.interpret     = GFX_INTERPRET_INTEGER;
			break;

		case GL_RGB8I :
			ret.components    = 3;
			ret.type.unpacked = GFX_BYTE;
			ret.interpret     = GFX_INTERPRET_INTEGER;
			break;

		case GL_RGB8UI :
			ret.components    = 3;
			ret.type.unpacked = GFX_UNSIGNED_BYTE;
			ret.interpret     = GFX_INTERPRET_INTEGER;
			break;

		case GL_RGB16I :
			ret.components    = 3;
			ret.type.unpacked = GFX_SHORT;
			ret.interpret     = GFX_INTERPRET_INTEGER;
			break;

		case GL_RGB16UI :
			ret.components    = 3;
			ret.type.unpacked = GFX_UNSIGNED_SHORT;
			ret.interpret     = GFX_INTERPRET_INTEGER;
			break;

		case GL_RGB32I :
			ret.components    = 3;
			ret.type.unpacked = GFX_INT;
			ret.interpret     = GFX_INTERPRET_INTEGER;
			break;

		case GL_RGB32UI :
			ret.components    = 3;
			ret.type.unpacked = GFX_UNSIGNED_INT;
			ret.interpret     = GFX_INTERPRET_INTEGER;
			break;

		case GL_RGBA8I :
			ret.components    = 4;
			ret.type.unpacked = GFX_BYTE;
			ret.interpret     = GFX_INTERPRET_INTEGER;
			break;

		case GL_RGBA8UI :
			ret.components    = 4;
			ret.type.unpacked = GFX_UNSIGNED_BYTE;
			ret.interpret     = GFX_INTERPRET_INTEGER;
			break;

		case GL_RGBA16I :
			ret.components    = 4;
			ret.type.unpacked = GFX_SHORT;
			ret.interpret     = GFX_INTERPRET_INTEGER;
			break;

		case GL_RGBA16UI :
			ret.components    = 4;
			ret.type.unpacked = GFX_UNSIGNED_SHORT;
			ret.interpret     = GFX_INTERPRET_INTEGER;
			break;

		case GL_RGBA32I :
			ret.components    = 4;
			ret.type.unpacked = GFX_INT;
			ret.interpret     = GFX_INTERPRET_INTEGER;
			break;

		case GL_RGBA32UI :
			ret.components    = 4;
			ret.type.unpacked = GFX_UNSIGNED_INT;
			ret.interpret     = GFX_INTERPRET_INTEGER;
			break;

		/* Depth formats */
		case GL_DEPTH_COMPONENT16 :
			ret.components    = 1;
			ret.type.unpacked = GFX_UNSIGNED_SHORT;
			ret.interpret     = GFX_INTERPRET_DEPTH;
			break;

		case GL_DEPTH_COMPONENT24 :
			ret.components    = 1;
			ret.type.unpacked = GFX_UNSIGNED_INT;
			ret.interpret     = GFX_INTERPRET_DEPTH;
			break;

		case GL_DEPTH_COMPONENT32F :
			ret.components    = 1;
			ret.type.unpacked = GFX_FLOAT;
			ret.interpret     = GFX_INTERPRET_DEPTH;
			break;
	}

	return ret;
}
