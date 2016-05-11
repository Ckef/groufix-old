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
void _gfx_split_depth(

		unsigned short   depth,
		unsigned short*  red,
		unsigned short*  green,
		unsigned short*  blue)
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
unsigned char _gfx_sizeof_unpacked_data_type(

		GFXUnpackedType type)
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
unsigned char _gfx_sizeof_packed_data_type(

		GFXPackedType type)
{
	switch(type)
	{
		case GFX_UNSIGNED_SHORT_5_6_5 :
		case GFX_UNSIGNED_SHORT_4_4_4_4 :
		case GFX_UNSIGNED_SHORT_5_5_5_1 :
			return 16;

		case GFX_INT_10_10_10_2 :
		case GFX_UNSIGNED_INT_10_10_10_2 :
		case GFX_UNSIGNED_INT_11F_11F_10F :
		case GFX_UNSIGNED_INT_9_9_9_5E :
		case GFX_UNSIGNED_INT_24_8 :
			return 32;

		case GFX_FLOAT_UNSIGNED_INT_24_8 :
			return 64;
	}

	return 0;
}
