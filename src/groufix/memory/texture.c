/**
 * Groufix  :  Graphics Engine produced by Ckef Worx
 * www      :  http://www.ejb.ckef-worx.com
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "groufix/internal.h"

#include <stdlib.h>

/******************************************************/
/** Internal Texture */
struct GFX_Internal_Texture
{
	/* Super class */
	GFXTexture texture;

	/* Hidden data */
	GLuint  handle; /* OpenGL handle */
	size_t  id;     /* Unique ID */
};

/******************************************************/
static void _gfx_texture_obj_free(void* object, const GFX_Extensions* ext)
{
	struct GFX_Internal_Texture* texture = (struct GFX_Internal_Texture*)object;

	glDeleteTextures(1, &texture->handle);
	texture->handle = 0;

	texture->id = 0;
}

/******************************************************/
/* vtable for hardware part of the texture */
static GFX_Hardware_Funcs _gfx_texture_obj_funcs =
{
	_gfx_texture_obj_free,
	NULL,
	NULL
};

/******************************************************/
GLuint _gfx_texture_get_handle(const GFXTexture* texture)
{
	return ((struct GFX_Internal_Texture*)texture)->handle;
}

/******************************************************/
GFXTexture* gfx_texture_create(GFXTextureType type)
{
	/* Get current window and context */
	if(!_gfx_window_get_current()) return NULL;

	/* Create new texture */
	struct GFX_Internal_Texture* tex = malloc(sizeof(struct GFX_Internal_Texture));
	if(!tex) return NULL;

	/* Register as object */
	tex->id = _gfx_hardware_object_register(tex, &_gfx_texture_obj_funcs);
	if(!tex->id)
	{
		free(tex);
		return NULL;
	}

	/* Generate OpenGL resources */
	tex->texture.type = type;
	glGenTextures(1, &tex->handle);

	return (GFXTexture*)tex;
}

/******************************************************/
void gfx_texture_free(GFXTexture* texture)
{
	if(texture)
	{
		struct GFX_Internal_Texture* internal = (struct GFX_Internal_Texture*)texture;

		/* Unregister as object */
		_gfx_hardware_object_unregister(internal->id);

		/* Get current window and context */
		if(_gfx_window_get_current()) glDeleteTextures(1, &internal->handle);

		free(texture);
	}
}