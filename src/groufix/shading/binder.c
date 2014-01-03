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

#include "groufix/containers/vector.h"
#include "groufix/internal.h"

/******************************************************/
/* Reference count */
static size_t _gfx_binder_ref_count = 0;

/* Texture Unit & Buffer binder */
static struct GFX_Internal_Binder
{
	GFXVector uniformBuffers; /* Stores unsigned char + GFX_Internal_UniformBuffer */
	GFXVector textureUnits;   /* Stores unsigned char + GFX_Internal_TextureUnit */

} _gfx_binder;

/* Uniform buffer key */
struct GFX_Internal_UniformBuffer
{
	GLuint      buffer;
	GLintptr    offset;
	GLsizeiptr  size;
};

/* Texture unit key */
struct GFX_Internal_TextureUnit
{
	GLuint texture;
};

/******************************************************/
void _gfx_binder_reference(int ref)
{
	/* Clear vector */
	if(_gfx_binder_ref_count <= -ref)
	{
		/* Make sure they were initialized */
		if(!_gfx_binder_ref_count) return;

		gfx_vector_clear(&_gfx_binder.uniformBuffers);
		gfx_vector_clear(&_gfx_binder.textureUnits);

		ref = -_gfx_binder_ref_count;
	}

	/* Initialize vectors */
	else if(!_gfx_binder_ref_count && ref)
	{
		gfx_vector_init(&_gfx_binder.uniformBuffers,
			sizeof(unsigned char) + sizeof(struct GFX_Internal_UniformBuffer));
		gfx_vector_init(&_gfx_binder.textureUnits,
			sizeof(unsigned char) + sizeof(struct GFX_Internal_TextureUnit));
	}

	_gfx_binder_ref_count += ref;
}
