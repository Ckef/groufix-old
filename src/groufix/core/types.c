/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#include "groufix/core/memory.h"

/******************************************************/
/** Host endianness */
const GFXHostEndianness GFX_HOST_ENDIANNESS = {{ 0x00, 0x01 }};


/******************************************************/
void _gfx_split_depth(

		unsigned short  depth,
		unsigned char*  red,
		unsigned char*  green,
		unsigned char*  blue)
{
	unsigned short delta = depth / 3;

	*red   = delta;
	*green = delta;
	*blue  = delta;
	delta  = depth - (delta * 3);

	if(delta > 0) ++(*green);
	if(delta > 1) ++(*red);
}

/******************************************************/
unsigned char _gfx_sizeof_data_type(

		GFXDataType type)
{
	switch(type)
	{
		case GFX_BIT :
			return 1;

		case GFX_NIBBLE :
			return 4;

		case GFX_BYTE :
		case GFX_UNSIGNED_BYTE :
			return 8;

		case GFX_SHORT :
		case GFX_UNSIGNED_SHORT :
		case GFX_HALF_FLOAT :
			return 16;

		case GFX_INT :
		case GFX_UNSIGNED_INT:
		case GFX_FLOAT :
			return 32;

		case GFX_DOUBLE :
			return 64;
	}

	return 0;
}

/******************************************************/
GFX_API GFXFormat gfx_format(

		GFXDataType     type,
		GFXBitDepth     depth,
		GFXFormatFlags  flags)
{
	/* Pack all the bit depth values so there are only trailing 0s */
	unsigned char c;
	unsigned char s;

	for(c = 0, s = 0; c < 4; ++c, ++s)
		if(!depth.data[c])
		{
			++s;

			/* Move s to a non-zero channel */
			while(s < 4 && !depth.data[s]) ++s;
			if(s >= 4) break;

			depth.data[c] = depth.data[s];
			depth.data[s] = 0;
		}

	/* No components, both little and big endian, no channel for exponent or stencil */
	if(
		(!depth.data[0]) ||
		((flags & GFX_FORMAT_LITTLE_ENDIAN) && (flags & GFX_FORMAT_BIG_ENDIAN)) ||
		(!depth.data[1] && (flags & GFX_FORMAT_EXPONENT)) ||
		(!depth.data[1] && (flags & GFX_FORMAT_DEPTH) && (flags & GFX_FORMAT_STENCIL)))
	{
		GFXFormat format =
		{
			.type  = type,
			.depth = {{ 0, 0, 0, 0 }},
			.flags = 0
		};

		return format;
	}

	/* Ok, return format */
	GFXFormat format =
	{
		.type  = type,
		.depth = depth,
		.flags = flags
	};

	return format;
}

/******************************************************/
GFXFormat gfx_format_from_type(

		GFXDataType     type,
		unsigned char   components,
		GFXFormatFlags  flags)
{
	/* Fill in components with the size of type */
	unsigned char size =
		_gfx_sizeof_data_type(type);

	GFXBitDepth depth =
	{{
		components >= 1 ? size : 0,
		components >= 2 ? size : 0,
		components >= 3 ? size : 0,
		components >= 4 ? size : 0
	}};

	return gfx_format(type, depth, flags);
}
