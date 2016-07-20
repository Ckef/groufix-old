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

#include "groufix/core/utils.h"

/******************************************************/
static inline GLenum _gfx_gl_from_type(

		GFXDataType type)
{
	switch(type)
	{
	case GFX_BYTE :
		return GL_BYTE;
	case GFX_UNSIGNED_BYTE :
		return GL_UNSIGNED_BYTE;
	case GFX_SHORT :
		return GL_SHORT;
	case GFX_UNSIGNED_SHORT :
		return GL_UNSIGNED_SHORT;
	case GFX_INT :
		return GL_INT;
	case GFX_UNSIGNED_INT :
		return GL_UNSIGNED_INT;
	case GFX_HALF_FLOAT :
		return GL_HALF_FLOAT;
	case GFX_FLOAT :
		return GL_FLOAT;
	case GFX_DOUBLE :
		return GL_DOUBLE;
	}

	return GL_BYTE;
}

/******************************************************/
static inline int _gfx_gl_from_type_to_general(

		GFXDataType type)
{
	/* Return 0 on integer, 1 on float and 2 on double */
	switch(type)
	{
	case GFX_BYTE :
	case GFX_UNSIGNED_BYTE :
	case GFX_SHORT :
	case GFX_UNSIGNED_SHORT :
	case GFX_INT :
	case GFX_UNSIGNED_INT :
		return 0;

	case GFX_HALF_FLOAT :
	case GFX_FLOAT :
		return 1;

	case GFX_DOUBLE :
		return 2;
	}

	return 0;
}

/******************************************************/
int _gfx_gl_format_to_vertex(

		const GFXVertexAttribute*  attribute,
		int*                       shaderType,
		GLint*                     size,
		GLenum*                    type,
		GLboolean*                 normalized,
		GFX_CONT_ARG)
{
	/* Make format unambiguous */
	GFXFormat format = gfx_format(
		attribute->format.type,
		attribute->format.depth,
		attribute->format.flags
	);

	if(!gfx_format_is_valid(format))
	{
		gfx_errors_push(
			GFX_ERROR_INCOMPATIBLE_CONTEXT,
			"A requested vertex attribute format is not a valid format."
		);
		return 0;
	}

	/* Validate flags */
	if(
		(format.flags & GFX_FORMAT_EXPONENT) ||
		(format.flags & GFX_FORMAT_DEPTH) ||
		(format.flags & GFX_FORMAT_STENCIL))
	{
		gfx_errors_push(
			GFX_ERROR_INCOMPATIBLE_CONTEXT,
			"A requested vertex attribute format contains unsupported flags."
		);
		return 0;
	}

	/* Get the general shader type type, client type and whether it's normalized or not */
	*shaderType = _gfx_gl_from_type_to_general(attribute->type);
	*type       = _gfx_gl_from_type(format.type);
	*normalized = format.flags & GFX_FORMAT_NORMALIZED ? GL_TRUE : GL_FALSE;

	/* Count size */
	unsigned char c;
	*size = 0;

	for(c = 0; c < 4; ++c)
		*size += format.depth.data[c] ? 1 : 0;

	/* Validate the type of the stored attribute */
	/* But first check if all components are of equal size */
	/* Also get the default bit size of the type and format flags to deny */
	int sameSize = 1;
	for(c = 1; c < *size; ++c) sameSize =
		sameSize && (format.depth.data[c] == format.depth.data[0]);

	unsigned char typeSize =
		_gfx_sizeof_data_type(format.type) << 3;

	/* The opposite of the host order is what we'll be denying no matter what */
	GFXFormatFlags denyFlags =
		GFX_HOST_ENDIANNESS.order == GFX_LITTLE_ENDIAN ?
		GFX_FORMAT_BIG_ENDIAN : GFX_FORMAT_LITTLE_ENDIAN;

	/* Now switch based on the type in shaders */
	int success = 0;

	switch(*shaderType)
	{
		/* Integral type */
	case 0 :
		if(
			format.type != GFX_HALF_FLOAT &&
			format.type != GFX_FLOAT &&
			format.type != GFX_DOUBLE)
		{
			success = sameSize && format.depth.data[0] == typeSize;
			denyFlags |= GFX_FORMAT_NORMALIZED | GFX_FORMAT_REVERSE;
		}

		break;

		/* Floating point type */
	case 1 :
		if(sameSize)
		{
			success = format.depth.data[0] == typeSize;

			/* Reverse order (must normalize if we do so) */
			if(format.flags & GFX_FORMAT_REVERSE)
			{
				success =
					success &&
					(*size == 4) &&
					(*type == GL_UNSIGNED_BYTE) &&
					(*normalized == GL_TRUE);
				*size =
					GL_BGRA;
			}
		}

		/* Forcefully disable normalization when float to float */
		/* We ignore the original value as the flag is explicitly ignored if not integral */
		if(
			format.type == GFX_HALF_FLOAT ||
			format.type == GFX_FLOAT ||
			format.type == GFX_DOUBLE)
		{
			*normalized = GL_FALSE;
		}

		/* Apparently it's packed, check the size of it */
		else if(
			!sameSize &&
			(format.depth.data[0] + format.depth.data[1] +
			format.depth.data[2] + format.depth.data[3]) == typeSize)
		{
			/* 10 RGB, 2 A, integers */
			if(
				format.depth.data[0] == 10 && format.depth.data[1] == 10 &&
				format.depth.data[2] == 10 && format.depth.data[3] == 2)
			{
				*type =
					(format.type == GFX_BYTE ||
					format.type == GFX_SHORT ||
					format.type == GFX_INT) ?
					GL_INT_2_10_10_10_REV : GL_UNSIGNED_INT_2_10_10_10_REV;

				/* Reverse order (must normalize if we do so) */
				if(!(format.flags & GFX_FORMAT_REVERSE))
					success = 1;
				else
				{
					success = (*normalized == GL_TRUE);
					*size = GL_BGRA;
				}
			}

			/* 11 RG, 10 B, floats */
			else if(
				format.depth.data[0] == 11 && format.depth.data[1] == 11 &&
				format.depth.data[2] == 10 && format.depth.data[3] == 0)
			{
				*type =
					GL_UNSIGNED_INT_10F_11F_11F_REV;

				success =
					GFX_CONT_GET.ext[GFX_EXT_VERTEX_PACKED_FLOATS] &&
					(format.type == GFX_UNSIGNED_BYTE ||
					format.type == GFX_UNSIGNED_SHORT ||
					format.type == GFX_UNSIGNED_INT);

				denyFlags |=
					GFX_FORMAT_REVERSE;
			}
		}

		break;

		/* Double precision floating point */
	case 2 :
		if(format.type == GFX_DOUBLE)
		{
			success =
				GFX_CONT_GET.ext[GFX_EXT_VERTEX_DOUBLE_PRECISION] &&
				sameSize && format.depth.data[0] == typeSize;
			denyFlags |=
				GFX_FORMAT_REVERSE;
		}

		break;
	}

	/* Throw error if validation failed */
	if(!success || (format.flags & denyFlags))
	{
		gfx_errors_push(
			GFX_ERROR_INCOMPATIBLE_CONTEXT,
			"A requested vertex attribute format is unsupported."
		);
		return 0;
	}

	return 1;
}
