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
#include "groufix/errors.h"

#include <stdlib.h>

/******************************************************/
/** Internal Texture */
struct GFX_Internal_Texture
{
	/* Super class */
	GFXTexture texture;

	/* Hidden data */
	GLuint  buffer; /* Associated buffer */
	GLuint  handle; /* OpenGL handle */
	size_t  id;     /* Unique ID */
};

/******************************************************/
static GLenum _gfx_texture_get_target(GFXTextureType type, unsigned char array)
{
	switch(type)
	{
		case GFX_TEXTURE_1D : return array ? GL_TEXTURE_1D_ARRAY : GL_TEXTURE_1D;
		case GFX_TEXTURE_2D : return array ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
		case GFX_TEXTURE_3D : return GL_TEXTURE_3D;
		case GFX_CUBEMAP    : return GL_TEXTURE_CUBE_MAP;

		/* ??? */
		default : return 0;
	}
}

/******************************************************/
static int _gfx_texture_eval_target(GLenum target, const GFX_Extensions* ext)
{
	switch(target)
	{
		/* GFX_EXT_TEXTURE_1D */
		case GL_TEXTURE_1D :
		case GL_TEXTURE_1D_ARRAY :

			if(!ext->flags[GFX_EXT_TEXTURE_1D])
			{
				gfx_errors_push(
					GFX_ERROR_INCOMPATIBLE_CONTEXT,
					"GFX_EXT_TEXTURE_1D is incompatible with this context."
				);
				return 0;
			}
			return 1;

		/* GFX_EXT_BUFFER_TEXTURE */
		case GL_TEXTURE_BUFFER :

			if(!ext->flags[GFX_EXT_BUFFER_TEXTURE])
			{
				gfx_errors_push(
					GFX_ERROR_INCOMPATIBLE_CONTEXT,
					"GFX_EXT_BUFFER_TEXTURE is incompatible with this context."
				);
				return 0;
			}
			return 1;

		/* GFX_EXT_MULTISAMPLE_TEXTURE */
		case GL_TEXTURE_2D_MULTISAMPLE :
		case GL_TEXTURE_2D_MULTISAMPLE_ARRAY :

			if(!ext->flags[GFX_EXT_MULTISAMPLE_TEXTURE])
			{
				gfx_errors_push(
					GFX_ERROR_INCOMPATIBLE_CONTEXT,
					"GFX_EXT_MULTISAMPLE_TEXTURE is incompatible with this context."
				);
				return 0;
			}
			return 1;

		/* Everything else */
		default : return 1;
	}
}

/******************************************************/
static void _gfx_texture_obj_free(void* object, const GFX_Extensions* ext)
{
	struct GFX_Internal_Texture* texture = (struct GFX_Internal_Texture*)object;

	ext->DeleteTextures(1, &texture->handle);
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
static struct GFX_Internal_Texture* _gfx_texture_alloc(GLenum target, const GFX_Extensions* ext)
{
	/* Validate type */
	if(!_gfx_texture_eval_target(target, ext)) return NULL;

	/* Create new texture */
	struct GFX_Internal_Texture* tex = calloc(1, sizeof(struct GFX_Internal_Texture));
	if(!tex) return NULL;

	/* Register as object */
	tex->id = _gfx_hardware_object_register(tex, &_gfx_texture_obj_funcs);
	if(!tex->id)
	{
		free(tex);
		return NULL;
	}
	ext->GenTextures(1, &tex->handle);

	return tex;
}

/******************************************************/
GLuint _gfx_texture_get_handle(const GFXTexture* texture)
{
	return ((struct GFX_Internal_Texture*)texture)->handle;
}

/******************************************************/
GFXTexture* gfx_texture_create(GFXTextureType type, unsigned char layers)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return NULL;

	/* Get target */
	GLenum target = _gfx_texture_get_target(type, layers);

	/* Allocate texture */
	struct GFX_Internal_Texture* tex = _gfx_texture_alloc(target, &window->extensions);
	if(!tex) return NULL;

	tex->texture.type   = type;
	tex->texture.layers = layers;

	return (GFXTexture*)tex;
}

/******************************************************/
GFXTexture* gfx_texture_create_multisample(unsigned char layers)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return NULL;

	/* Get target */
	GLenum target = layers ? GL_TEXTURE_2D_MULTISAMPLE_ARRAY : GL_TEXTURE_2D_MULTISAMPLE;

	/* Allocate texture */
	struct GFX_Internal_Texture* tex = _gfx_texture_alloc(target, &window->extensions);
	if(!tex) return NULL;

	tex->texture.type   = GFX_TEXTURE_2D;
	tex->texture.layers = layers;

	return (GFXTexture*)tex;
}

/******************************************************/
GFXTexture* gfx_texture_create_buffer_link(const GFXBuffer* buffer)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return NULL;

	/* Allocate texture */
	struct GFX_Internal_Texture* tex = _gfx_texture_alloc(GL_TEXTURE_BUFFER, &window->extensions);
	if(!tex) return NULL;

	tex->buffer = _gfx_buffer_get_handle(buffer);
	tex->texture.type = GFX_TEXTURE_1D;

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
		GFX_Internal_Window* window = _gfx_window_get_current();
		if(window) window->extensions.DeleteTextures(1, &internal->handle);

		free(texture);
	}
}
