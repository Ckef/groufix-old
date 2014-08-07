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

#include "groufix/core/errors.h"
#include "groufix/core/memory/internal.h"
#include "groufix/core/pipeline/internal.h"
#include "groufix/core/shading/internal.h"

#include <stdlib.h>

/******************************************************/
/* Internal Texture */
struct GFX_Texture
{
	/* Super class */
	GFXTexture texture;

	/* Hidden data */
	GLuint  buffer;
	GLuint  handle; /* OpenGL handle */
	GLenum  target;
	GLint   format; /* Internal format */
};

/******************************************************/
static inline unsigned char _gfx_texture_get_num_mipmaps(

		GFXTextureType  type,
		size_t          w,
		size_t          h,
		size_t          d)
{
	/* Get correct dimensions */
	switch(type)
	{
		case GFX_TEXTURE_1D :
			h = d = 1;
			break;

		case GFX_TEXTURE_2D :
		case GFX_CUBEMAP :
			d = 1;
			break;

		default : break;
	}

	/* Calculate number of mipmaps */
	size_t max = w > h ? (w > d ? w : d) : (h > d ? h : d);

	unsigned char num = 0;
	while(max >>= 1) ++num;

	return num;
}

/******************************************************/
static inline void _gfx_texture_get_mipmap_size(

		unsigned char  mipmap,
		size_t*        w,
		size_t*        h,
		size_t*        d)
{
	size_t k = 1 << mipmap;
	*w /= k;
	*h /= k;
	*d /= k;
	*w = *w ? *w : 1;
	*h = *h ? *h : 1;
	*d = *d ? *d : 1;
}

/******************************************************/
static int _gfx_texture_eval_target(

		GLenum                 target,
		const GFX_Extensions*  ext)
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

		/* GFX_EXT_LAYERED_CUBEMAP */
		case GL_TEXTURE_CUBE_MAP_ARRAY :

			if(!ext->flags[GFX_EXT_LAYERED_CUBEMAP])
			{
				gfx_errors_push(
					GFX_ERROR_INCOMPATIBLE_CONTEXT,
					"GFX_EXT_LAYERED_CUBEMAP is incompatible with this context."
				);
				return 0;
			}
			return 1;

		/* Everything else */
		default : return 1;
	}
}

/******************************************************/
static inline GLint _gfx_texture_eval_pixel_format(

		GFXTextureFormat format)
{
	GLint form = _gfx_texture_format_to_pixel_format(format);
	if(form < 0) gfx_errors_push(
		GFX_ERROR_INVALID_VALUE,
		"A requested pixel transfer format is not supported."
	);
	return form;
}

/******************************************************/
static inline GLint _gfx_texture_eval_internal_format(

		GFXTextureFormat format)
{
	GLint form = _gfx_texture_format_to_internal(format);
	if(form < 0) gfx_errors_push(
		GFX_ERROR_INVALID_VALUE,
		"A requested texture format is not supported."
	);
	return form;
}

/******************************************************/
static void _gfx_texture_obj_free(

		void*            object,
		GFX_Extensions*  ext)
{
	struct GFX_Texture* texture = (struct GFX_Texture*)object;

	texture->handle = 0;
	texture->buffer = 0;

	texture->texture.id = 0;
}

/******************************************************/
/* vtable for hardware part of the texture */
static GFX_HardwareFuncs _gfx_texture_obj_funcs =
{
	_gfx_texture_obj_free,
	NULL,
	NULL
};

/******************************************************/
static struct GFX_Texture* _gfx_texture_alloc(

		GLenum            target,
		GFXTextureFormat  format,
		GFX_Extensions*   ext)
{
	/* Validate type & format */
	GLint form = _gfx_texture_eval_internal_format(format);
	if(form < 0 || !_gfx_texture_eval_target(target, ext)) return NULL;

	/* Create new texture */
	struct GFX_Texture* tex = calloc(1, sizeof(struct GFX_Texture));
	if(!tex)
	{
		/* Out of memory error */
		gfx_errors_push(
			GFX_ERROR_OUT_OF_MEMORY,
			"Texture could not be allocated."
		);
		return NULL;
	}

	/* Register as object */
	tex->texture.id = _gfx_hardware_object_register(
		tex,
		&_gfx_texture_obj_funcs
	);

	if(!tex->texture.id)
	{
		free(tex);
		return NULL;
	}

	/* Allocate OGL resources */
	ext->GenTextures(1, &tex->handle);

	tex->target = target;
	tex->format = form;
	tex->texture.samples = 1;

	return tex;
}

/******************************************************/
static void _gfx_texture_set_size(

		struct GFX_Texture*  tex,
		size_t               width,
		size_t               height,
		size_t               depth,
		GFX_Extensions*      ext)
{
	/* Get texture dimensions */
	size_t w = width;
	size_t h = height;
	size_t d = depth;

	switch(tex->target)
	{
		case GL_TEXTURE_1D :
			h = d = height = depth = 1;
			break;

		case GL_TEXTURE_1D_ARRAY :
			h = d = depth = 1;
			break;

		case GL_TEXTURE_2D :
		case GL_TEXTURE_2D_MULTISAMPLE :
		case GL_TEXTURE_CUBE_MAP :
			d = depth = 1;
			break;

		case GL_TEXTURE_2D_ARRAY :
		case GL_TEXTURE_2D_MULTISAMPLE_ARRAY :
		case GL_TEXTURE_CUBE_MAP_ARRAY :
			d = 1;
			break;

		/* Dafuq are you doing ??? */
		case GL_TEXTURE_BUFFER : return;
	}

	/* Remember dimensions + layers */
	tex->texture.width  = width;
	tex->texture.height = height;
	tex->texture.depth  = depth;

	/* Get pixel format and type */
	GFXTextureFormat format =
		_gfx_texture_format_from_internal(tex->format);
	GLint pixForm =
		_gfx_texture_format_to_pixel_format(format);
	GLenum pixType =
		_gfx_is_data_type_packed(format.type) ?
		format.type.packed :
		format.type.unpacked;

	int old;
	_gfx_binder_bind_texture(tex->handle, tex->target, 0, &old, ext);

	/* Allocate all mipmaps */
	unsigned char m;
	for(m = 0; m <= tex->texture.mipmaps; ++m)
	{
		/* Get mipmap dimensions */
		size_t mw = w;
		size_t mh = h;
		size_t md = d;
		_gfx_texture_get_mipmap_size(m, &mw, &mh, &md);

		switch(tex->target)
		{
			case GL_TEXTURE_1D :
				ext->TexImage1D(
					tex->target, m,
					tex->format, mw, 0,
					pixForm,
					pixType,
					NULL
				);
				break;

			case GL_TEXTURE_1D_ARRAY :
				ext->TexImage2D(
					tex->target, m,
					tex->format, mw,
					height, 0,
					pixForm,
					pixType,
					NULL
				);
				break;

			case GL_TEXTURE_2D :
				ext->TexImage2D(
					tex->target, m,
					tex->format, mw, mh, 0,
					pixForm,
					pixType,
					NULL
				);
				break;

			case GL_TEXTURE_2D_ARRAY :
				ext->TexImage3D(
					tex->target, m,
					tex->format, mw, mh,
					depth, 0,
					pixForm,
					pixType,
					NULL
				);
				break;

			case GL_TEXTURE_2D_MULTISAMPLE :
				ext->TexImage2DMultisample(
					tex->target,
					tex->texture.samples,
					tex->format, mw, mh,
					GL_FALSE
				);
				break;

			case GL_TEXTURE_2D_MULTISAMPLE_ARRAY :
				ext->TexImage3DMultisample(
					tex->target,
					tex->texture.samples,
					tex->format, mw, mh,
					depth,
					GL_FALSE
				);
				break;

			case GL_TEXTURE_3D :
				ext->TexImage3D(
					tex->target, m,
					tex->format, mw, mh, md, 0,
					pixForm,
					pixType,
					NULL
				);
				break;

			case GL_TEXTURE_CUBE_MAP :
				ext->TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, m, tex->format, mw, mh, 0, pixForm, pixType, NULL);
				ext->TexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, m, tex->format, mw, mh, 0, pixForm, pixType, NULL);
				ext->TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, m, tex->format, mw, mh, 0, pixForm, pixType, NULL);
				ext->TexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, m, tex->format, mw, mh, 0, pixForm, pixType, NULL);
				ext->TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, m, tex->format, mw, mh, 0, pixForm, pixType, NULL);
				ext->TexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, m, tex->format, mw, mh, 0, pixForm, pixType, NULL);
				break;

			case GL_TEXTURE_CUBE_MAP_ARRAY :
				ext->TexImage3D(
					tex->target, m,
					tex->format, mw, mh,
					depth * 6, 0,
					pixForm,
					pixType,
					NULL
				);
				break;
		}
	}
}

/******************************************************/
GLuint _gfx_texture_get_handle(

		const GFXTexture* texture)
{
	return ((struct GFX_Texture*)texture)->handle;
}

/******************************************************/
GLenum _gfx_texture_get_internal_target(

		const GFXTexture* texture)
{
	return ((struct GFX_Texture*)texture)->target;
}

/******************************************************/
GFXTexture* gfx_texture_create(

		GFXTextureType    type,
		GFXTextureFormat  format,
		int               mipmaps,
		size_t            width,
		size_t            height,
		size_t            depth)
{
	/* Get current window and context */
	GFX_Window* window = _gfx_window_get_current();
	if(!window) return NULL;

	/* Get target */
	GLenum target;
	switch(type)
	{
		case GFX_TEXTURE_1D :
			target = (height > 1) ? GL_TEXTURE_1D_ARRAY : GL_TEXTURE_1D;
			break;

		case GFX_TEXTURE_2D :
			target = (depth > 1) ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
			break;

		case GFX_TEXTURE_3D :
			target = GL_TEXTURE_3D;
			break;

		case GFX_CUBEMAP :
			target = (depth > 1) ? GL_TEXTURE_CUBE_MAP_ARRAY : GL_TEXTURE_CUBE_MAP;
			break;

		/* ??? */
		default : return NULL;
	}

	/* Allocate texture */
	struct GFX_Texture* tex = _gfx_texture_alloc(
		target,
		format,
		&window->extensions
	);

	if(!tex) return NULL;

	/* Limit mipmaps */
	unsigned char maxMips =
		_gfx_texture_get_num_mipmaps(type, width, height, depth);
	mipmaps =
		(mipmaps < 0 || mipmaps > maxMips) ? maxMips : mipmaps;

	tex->texture.type = type;
	tex->texture.mipmaps = mipmaps;

	/* Set parameters and allocate data */
	int old;
	_gfx_binder_bind_texture(
		tex->handle,
		target,
		0,
		&old,
		&window->extensions);

	window->extensions.TexParameteri(
		tex->target,
		GL_TEXTURE_BASE_LEVEL,
		0);

	window->extensions.TexParameteri(
		tex->target,
		GL_TEXTURE_MAX_LEVEL,
		mipmaps);

	_gfx_texture_set_size(
		tex,
		width,
		height,
		depth,
		&window->extensions);

	return (GFXTexture*)tex;
}

/******************************************************/
GFXTexture* gfx_texture_create_multisample(

		GFXTextureFormat  format,
		unsigned char     samples,
		size_t            width,
		size_t            height,
		size_t            depth)
{
	/* Get current window and context */
	GFX_Window* window = _gfx_window_get_current();
	if(!window) return NULL;

	GLenum target = (depth > 1) ?
		GL_TEXTURE_2D_MULTISAMPLE_ARRAY : GL_TEXTURE_2D_MULTISAMPLE;

	/* Allocate texture */
	struct GFX_Texture* tex = _gfx_texture_alloc(
		target,
		format,
		&window->extensions
	);

	if(!tex) return NULL;

	/* Limit samples */
	int maxSamples = window->extensions.limits[GFX_LIM_MAX_SAMPLES];

	tex->texture.type =
		GFX_TEXTURE_2D;
	tex->texture.samples =
		(samples > maxSamples) ?
		maxSamples : (samples < 2 ? 2 : samples);

	_gfx_texture_set_size(
		tex,
		width,
		height,
		depth,
		&window->extensions
	);

	return (GFXTexture*)tex;
}

/******************************************************/
GFXTexture* gfx_texture_create_buffer_link(

		GFXTextureFormat  format,
		const GFXBuffer*  buffer)
{
	/* Get current window and context & validate format */
	GFX_Window* window = _gfx_window_get_current();
	if(
		!window ||
		_gfx_is_data_type_packed(format.type) ||
		format.interpret == GFX_INTERPRET_DEPTH)
	{
		return NULL;
	}

	/* Allocate texture */
	struct GFX_Texture* tex = _gfx_texture_alloc(
		GL_TEXTURE_BUFFER,
		format,
		&window->extensions
	);

	if(!tex) return NULL;

	tex->buffer = _gfx_buffer_get_handle(buffer);
	tex->texture.type = GFX_TEXTURE_1D;

	/* Compute dimensions */
	tex->texture.width =
		format.components * _gfx_sizeof_data_type(format.type);
	if(tex->texture.width)
		tex->texture.width = buffer->size / tex->texture.width;

	int max = window->extensions.limits[GFX_LIM_MAX_BUFFER_TEXTURE_SIZE];
	tex->texture.width = tex->texture.width > max ? max : tex->texture.width;

	tex->texture.height = 1;
	tex->texture.depth  = 1;

	/* Link buffer */
	int old;
	_gfx_binder_bind_texture(
		tex->handle,
		tex->target,
		0,
		&old,
		&window->extensions);

	window->extensions.TexBuffer(
		tex->target,
		tex->format,
		tex->buffer);

	return (GFXTexture*)tex;
}

/******************************************************/
void gfx_texture_free(

		GFXTexture* texture)
{
	if(texture)
	{
		struct GFX_Texture* internal = (struct GFX_Texture*)texture;

		/* Unregister as object */
		_gfx_hardware_object_unregister(texture->id);

		/* Get current window and context */
		GFX_Window* window = _gfx_window_get_current();
		if(window)
		{
			_gfx_binder_unbind_texture(
				internal->handle,
				&window->extensions);

			window->extensions.DeleteTextures(
				1,
				&internal->handle);
		}

		free(texture);
	}
}

/******************************************************/
GFXTextureFormat gfx_texture_get_format(

		GFXTexture* texture)
{
	struct GFX_Texture* internal = (struct GFX_Texture*)texture;
	return _gfx_texture_format_from_internal(internal->format);
}

/******************************************************/
void gfx_texture_write(

		GFXTextureImage          image,
		const GFXPixelTransfer*  transfer,
		const void*              data)
{
	/* Get current window and context */
	GFX_Window* window = _gfx_window_get_current();
	if(!window) return;

	struct GFX_Texture* internal = (struct GFX_Texture*)image.texture;

	if(internal->buffer)
	{
		/* Write indirectly to the linked buffer */
		size_t size = transfer->format.components *
			_gfx_sizeof_data_type(transfer->format.type);

		window->extensions.BindBuffer(
			GL_ARRAY_BUFFER,
			internal->buffer);

		window->extensions.BufferSubData(
			GL_ARRAY_BUFFER,
			transfer->xOffset * size,
			transfer->width * size,
			data);
	}
	else
	{
		/* Upload texture data */
		int old;
		_gfx_binder_bind_texture(
			internal->handle,
			internal->target,
			0,
			&old,
			&window->extensions);

		_gfx_states_set_pixel_unpack_alignment(
			transfer->alignment,
			&window->extensions);

		GLint pixForm =
			_gfx_texture_eval_pixel_format(transfer->format);
		GLenum pixType =
			_gfx_is_data_type_packed(transfer->format.type) ?
			transfer->format.type.packed :
			transfer->format.type.unpacked;

		if(pixForm >= 0) switch(internal->target)
		{
			case GL_TEXTURE_1D :
				window->extensions.TexSubImage1D(
					internal->target,
					image.mipmap,
					transfer->xOffset,
					transfer->width,
					pixForm,
					pixType,
					data
				);
				break;

			case GL_TEXTURE_1D_ARRAY :
				window->extensions.TexSubImage2D(
					internal->target,
					image.mipmap,
					transfer->xOffset,
					transfer->yOffset + image.layer,
					transfer->width,
					transfer->height,
					pixForm,
					pixType,
					data
				);
				break;

			case GL_TEXTURE_2D :
				window->extensions.TexSubImage2D(
					internal->target,
					image.mipmap,
					transfer->xOffset,
					transfer->yOffset,
					transfer->width,
					transfer->height,
					pixForm,
					pixType,
					data
				);
				break;

			case GL_TEXTURE_2D_ARRAY :
				window->extensions.TexSubImage3D(
					internal->target,
					image.mipmap,
					transfer->xOffset,
					transfer->yOffset,
					transfer->zOffset + image.layer,
					transfer->width,
					transfer->height,
					transfer->depth,
					pixForm,
					pixType,
					data
				);
				break;

			case GL_TEXTURE_3D :
				window->extensions.TexSubImage3D(
					internal->target,
					image.mipmap,
					transfer->xOffset,
					transfer->yOffset,
					transfer->zOffset,
					transfer->width,
					transfer->height,
					transfer->depth,
					pixForm,
					pixType,
					data
				);
				break;

			case GL_TEXTURE_CUBE_MAP :
				window->extensions.TexSubImage2D(
					image.face + GL_TEXTURE_CUBE_MAP_POSITIVE_X,
					image.mipmap,
					transfer->xOffset,
					transfer->yOffset,
					transfer->width,
					transfer->height,
					pixForm,
					pixType,
					data
				);
				break;

			case GL_TEXTURE_CUBE_MAP_ARRAY :
				window->extensions.TexSubImage3D(
					internal->target,
					image.mipmap,
					transfer->xOffset,
					transfer->yOffset,
					(transfer->zOffset + image.layer) * 6 + image.face,
					transfer->width,
					transfer->height,
					transfer->depth,
					pixForm,
					pixType,
					data
				);
				break;
		}
	}
}

/******************************************************/
void gfx_texture_write_from_buffer(

		GFXTextureImage          image,
		const GFXPixelTransfer*  transfer,
		const GFXBuffer*         buffer,
		size_t                   offset)
{
	/* Get current window and context */
	GFX_Window* window = _gfx_window_get_current();
	if(!window) return;

	struct GFX_Texture* internal = (struct GFX_Texture*)image.texture;

	if(internal->buffer)
	{
		/* Copy the buffer data to the linked buffer */
		window->extensions.BindBuffer(
			GL_COPY_READ_BUFFER,
			_gfx_buffer_get_handle(buffer));

		window->extensions.BindBuffer(
			GL_COPY_WRITE_BUFFER,
			internal->buffer);

		size_t size = transfer->format.components *
			_gfx_sizeof_data_type(transfer->format.type);

		window->extensions.CopyBufferSubData(
			GL_COPY_READ_BUFFER,
			GL_COPY_WRITE_BUFFER,
			offset,
			transfer->xOffset * size,
			transfer->width * size);
	}
	else
	{
		/* Bind buffer as unpack pixel buffer before performing the copy */
		window->extensions.BindBuffer(
			GL_PIXEL_UNPACK_BUFFER,
			_gfx_buffer_get_handle(buffer));

		gfx_texture_write(
			image,
			transfer,
			GFX_UINT_TO_VOID(offset));

		/* Also unbind for future transfers */
		window->extensions.BindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}
}

/******************************************************/
void gfx_texture_resize(

		GFXTexture*  texture,
		size_t       width,
		size_t       height,
		size_t       depth)
{
	/* Get current window and context */
	GFX_Window* window = _gfx_window_get_current();
	if(!window) return;

	_gfx_texture_set_size(
		(struct GFX_Texture*)texture,
		width,
		height,
		depth,
		&window->extensions
	);
}

/******************************************************/
void gfx_texture_generate_mipmaps(

		GFXTexture* texture)
{
	/* Get current window and context */
	GFX_Window* window = _gfx_window_get_current();
	if(!window) return;

	struct GFX_Texture* internal = (struct GFX_Texture*)texture;
	int old;

	_gfx_binder_bind_texture(
		internal->handle,
		internal->target,
		0,
		&old,
		&window->extensions
	);

	window->extensions.GenerateMipmap(internal->target);
}
