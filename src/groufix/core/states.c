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

#include <limits.h>

/******************************************************/
/** Default state blueprint */
const GFXPipeState _gfx_state_default =
{
	.render =
	{
		.state = GFX_STATE_DEFAULT
	},

	.depth =
	{	.test = GFX_FRAG_LESS
	},

	.blend =
	{
		.stateRGB  = GFX_BLEND_ADD,
		.stateA    = GFX_BLEND_ADD,
		.sourceRGB = GFX_BLEND_ONE,
		.sourceA   = GFX_BLEND_ONE,
		.bufferRGB = GFX_BLEND_ZERO,
		.bufferA   = GFX_BLEND_ZERO
	},

	.stencil =
	{
		.testFront  = GFX_FRAG_ALWAYS,
		.testBack   = GFX_FRAG_ALWAYS,

		.frontFail  = GFX_STENCIL_KEEP,
		.frontDepth = GFX_STENCIL_KEEP,
		.frontPass  = GFX_STENCIL_KEEP,
		.frontRef   = 0,
		.frontMask  = UINT_MAX,

		.backFail   = GFX_STENCIL_KEEP,
		.backDepth  = GFX_STENCIL_KEEP,
		.backPass   = GFX_STENCIL_KEEP,
		.backRef    = 0,
		.backMask   = UINT_MAX
	}
};
