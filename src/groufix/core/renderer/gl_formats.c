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

#include "groufix/core/renderer.h"

/******************************************************/
int _gfx_gl_format_to_vertex(

		const GFXVertexAttribute*  attribute,
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
		(format.flags & GFX_FORMAT_STENCIL) ||
		((format.flags & GFX_FORMAT_LITTLE_ENDIAN) && (format.flags & GFX_FORMAT_BIG_ENDIAN)))
	{
		gfx_errors_push(
			GFX_ERROR_INCOMPATIBLE_CONTEXT,
			"A requested vertex attribute format contains unsupported flags."
		);
		return 0;
	}

	/* Count size */
	unsigned char c;
	*size = 0;

	for(c = 0; c < 4; ++c) *size += format.depth.data[c] ? 1 : 0;

	/* Get whether it's normalized or not */
	*normalized = format.flags & GFX_FORMAT_NORMALIZED ? GL_TRUE : GL_FALSE;

	return 1;
}
