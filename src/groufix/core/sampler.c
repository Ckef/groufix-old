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

#include "groufix/core/renderer.h"

/******************************************************/
/* Internal sampler */
typedef struct GFX_Sampler
{
	/* Super class */
	GFXSampler sampler;

	/* Hidden data */
	GFX_RenderObjectID  id;
	unsigned int        references; /* Reference counter */
	GLuint              handle;     /* OpenGL handle */

} GFX_Sampler;


/******************************************************/
GLuint _gfx_sampler_get_handle(

		const GFXSampler* sampler)
{
	return ((const GFX_Sampler*)sampler)->handle;
}

/******************************************************/
GFXSampler* _gfx_sampler_create(

		GFXSampler* values)
{
	return NULL;
}

/******************************************************/
int _gfx_sampler_reference(

		GFXSampler* sampler)
{
	return 0;
}

/******************************************************/
void _gfx_sampler_free(

		GFXSampler* sampler)
{
}
