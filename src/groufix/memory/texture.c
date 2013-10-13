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

#ifdef GFX_GLES

	/* Some defines to be compatible */
	#define GL_TEXTURE_1D                    0x0de0
	#define GL_TEXTURE_1D_ARRAY              0x8c18
	#define GL_TEXTURE_2D_MULTISAMPLE        0x9100
	#define GL_TEXTURE_2D_MULTISAMPLE_ARRAY  0x9102
	#define GL_TEXTURE_BUFFER                0x8c2a

#endif

/******************************************************/
/** Internal Texture */
struct GFX_Internal_Texture
{
	/* Super class */
	GFXTexture texture;

	/* Hidden data */
	GLuint  buffer;
	GLuint  handle; /* OpenGL handle */
	size_t  id;     /* Unique ID */

	/* Texture data */
	GLenum  target;
	GLint   format; /* Internal format */
};

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
static struct GFX_Internal_Texture* _gfx_texture_alloc(GLenum target, GFXTextureFormat format, const GFX_Extensions* ext)
{
	/* Validate type & format */
	GLint form = _gfx_texture_format_to_internal(format);
	if(form < 0)
	{
		gfx_errors_push(
			GFX_ERROR_INVALID_ENUM,
			"A requested texture format is not supported."
		);
		return NULL;
	}
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
	tex->target = target;
	tex->format = form;

	return tex;
}

/******************************************************/
GLuint _gfx_texture_get_handle(const GFXTexture* texture)
{
	return ((struct GFX_Internal_Texture*)texture)->handle;
}

/******************************************************/
GFXTexture* gfx_texture_create(GFXTextureType type, GFXTextureFormat format, unsigned char layers)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return NULL;

	/* Get target */
	GLenum target;
	switch(type)
	{
		case GFX_TEXTURE_1D :
			target = layers ? GL_TEXTURE_1D_ARRAY : GL_TEXTURE_1D;
			break;

		case GFX_TEXTURE_2D :
			target = layers ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
			break;

		case GFX_TEXTURE_3D :
			target = GL_TEXTURE_3D;
			break;

		case GFX_CUBEMAP :
			target = GL_TEXTURE_CUBE_MAP;
			break;

		/* ??? */
		default : target = 0;
	}

	/* Allocate texture */
	struct GFX_Internal_Texture* tex = _gfx_texture_alloc(target, format, &window->extensions);
	if(!tex) return NULL;

	/* Validate layers */
	if(type != GFX_TEXTURE_1D && type != GFX_TEXTURE_2D) layers = 0;

	tex->texture.type   = type;
	tex->texture.layers = layers;

	return (GFXTexture*)tex;
}

/******************************************************/
GFXTexture* gfx_texture_create_multisample(GFXTextureFormat format, unsigned char layers)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return NULL;

	/* Get target */
	GLenum target = layers ? GL_TEXTURE_2D_MULTISAMPLE_ARRAY : GL_TEXTURE_2D_MULTISAMPLE;

	/* Allocate texture */
	struct GFX_Internal_Texture* tex = _gfx_texture_alloc(target, format, &window->extensions);
	if(!tex) return NULL;

	tex->texture.type   = GFX_TEXTURE_2D;
	tex->texture.layers = layers;

	return (GFXTexture*)tex;
}

/******************************************************/
GFXTexture* gfx_texture_create_buffer_link(GFXTextureFormat format, const GFXBuffer* buffer)
{
	/* Get current window and context & format type */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window || _gfx_is_data_type_packed(format.type)) return NULL;

	/* Allocate texture */
	struct GFX_Internal_Texture* tex = _gfx_texture_alloc(GL_TEXTURE_BUFFER, format, &window->extensions);
	if(!tex) return NULL;

	tex->buffer = _gfx_buffer_get_handle(buffer);
	tex->texture.type = GFX_TEXTURE_1D;

	/* Compute dimensions */
	tex->texture.height = 1;
	tex->texture.width = format.components * _gfx_sizeof_data_type(format.type);
	if(tex->texture.width) tex->texture.width = buffer->size / tex->texture.width;

	int max = window->extensions.limits[GFX_LIM_MAX_BUFFER_TEXTURE_SIZE];
	tex->texture.width = tex->texture.width > max ? max : tex->texture.width;

	/* Link buffer */
	window->extensions.BindTexture(tex->target, tex->handle);
	window->extensions.TexBuffer(tex->target, tex->format, tex->buffer);

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

/******************************************************/
GFXTextureFormat gfx_texture_get_format(GFXTexture* texture)
{
	struct GFX_Internal_Texture* internal = (struct GFX_Internal_Texture*)texture;

	return _gfx_texture_format_from_internal(internal->format);
}
