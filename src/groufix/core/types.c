/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
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
int _gfx_is_data_type_packed(

		GFXDataType type)
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
unsigned char _gfx_sizeof_data_type(

		GFXDataType type)
{
	/* Check for unpacked datatypes */
	switch(type.unpacked)
	{
		case GFX_BIT :
		case GFX_NIBBLE :
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
