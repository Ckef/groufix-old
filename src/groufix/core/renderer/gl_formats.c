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

		GFXFormat   format,
		GLint*      size,
		GLenum*     type,
		GLboolean*  normalized,
		GFX_CONT_ARG)
{
	/* Make format unambiguous */
	format = gfx_format(format.type, format.depth, format.flags);

	return 1;
}
