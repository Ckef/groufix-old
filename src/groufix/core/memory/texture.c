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

#include "groufix/core/memory/internal.h"
#include "groufix/core/pipeline/internal.h"
#include "groufix/core/shading/internal.h"
#include "groufix/core/errors.h"

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
			h = 1;

		case GFX_TEXTURE_2D :
		case GFX_CUBEMAP :
			d = 1;

		default : break;
	}

	/* Calculate number of mipmaps */
	size_t max = w > h ? (w > d ? w : d) : (h > d ? h : d);

	unsigned char num = 0;
	while(max >>= 1) ++num;

	return num;
}

/******************************************************/
static int _gfx_texture_eval_target(

		GLenum       target,
		GFX_Window*  window)
{
	switch(target)
	{
		/* GFX_EXT_TEXTURE_1D */
		case GL_TEXTURE_1D :
		case GL_TEXTURE_1D_ARRAY :

			if(!window->ext[GFX_EXT_TEXTURE_1D])
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

			if(!window->ext[GFX_EXT_BUFFER_TEXTURE])
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

			if(!window->ext[GFX_EXT_MULTISAMPLE_TEXTURE])
			{
				gfx_errors_push(
					GFX_ERROR_INCOMPATIBLE_CONTEXT,
					"GFX_EXT_MULTISAMPLE_TEXTURE is incompatible with this context."
				);
				return 0;
			}
			return 1;

		/* GFX_EXT_LAYERED_MULTISAMPLE_TEXTURE */
		case GL_TEXTURE_2D_MULTISAMPLE_ARRAY :

			if(!window->ext[GFX_EXT_LAYERED_MULTISAMPLE_TEXTURE])
			{
				gfx_errors_push(
					GFX_ERROR_INCOMPATIBLE_CONTEXT,
					"GFX_EXT_LAYERED_MULTISAMPLE_TEXTURE is incompatible with this context."
				);
				return 0;
			}
			return 1;

		/* GFX_EXT_LAYERED_CUBEMAP */
		case GL_TEXTURE_CUBE_MAP_ARRAY :

			if(!window->ext[GFX_EXT_LAYERED_CUBEMAP])
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

		void* object)
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
		GFX_Window*       window)
{
	/* Validate type & format */
	GLint form = _gfx_texture_eval_internal_format(format);
	if(form < 0 || !_gfx_texture_eval_target(target, window)) return NULL;

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
	window->renderer.CreateTextures(target, 1, &tex->handle);

	tex->target = target;
	tex->format = form;
	tex->texture.samples = 1;

	return tex;
}

/******************************************************/
static void _gfx_texture_set_storage(

		struct GFX_Texture*  tex,
		size_t               width,
		size_t               height,
		size_t               depth,
		GFX_Window*          window)
{
	tex->texture.width  = width;
	tex->texture.height = height;
	tex->texture.depth  = depth;

	/* Allocate storage */
	if(window->ext[GFX_EXT_DIRECT_STATE_ACCESS])
	{
		switch(tex->target)
		{
			case GL_TEXTURE_1D :
				window->renderer.TextureStorage1D(
					tex->handle,
					tex->texture.mipmaps + 1,
					tex->format,
					width
				);
				break;

			case GL_TEXTURE_1D_ARRAY :
			case GL_TEXTURE_2D :
			case GL_TEXTURE_CUBE_MAP :
				window->renderer.TextureStorage2D(
					tex->handle,
					tex->texture.mipmaps + 1,
					tex->format,
					width,
					height
				);
				break;

			case GL_TEXTURE_2D_ARRAY :
			case GL_TEXTURE_3D :
				window->renderer.TextureStorage3D(
					tex->handle,
					tex->texture.mipmaps + 1,
					tex->format,
					width,
					height,
					depth
				);
				break;

			case GL_TEXTURE_2D_MULTISAMPLE :
				window->renderer.TextureStorage2DMultisample(
					tex->handle,
					tex->texture.samples,
					tex->format,
					width,
					height,
					GL_FALSE
				);
				break;

			case GL_TEXTURE_2D_MULTISAMPLE_ARRAY :
				window->renderer.TextureStorage3DMultisample(
					tex->handle,
					tex->texture.samples,
					tex->format,
					width,
					height,
					depth,
					GL_FALSE
				);
				break;

			case GL_TEXTURE_CUBE_MAP_ARRAY :
				window->renderer.TextureStorage3D(
					tex->handle,
					tex->texture.mipmaps + 1,
					tex->format,
					width,
					height,
					depth * 6
				);
				break;
		}
	}
	else
	{
		/* Bind it to a unit if no direct state access */
		int old;
		_gfx_binder_bind_texture(
			tex->handle,
			tex->target,
			0,
			&old,
			window
		);

		switch(tex->target)
		{
			case GL_TEXTURE_1D :
				window->renderer.TexStorage1D(
					tex->target,
					tex->texture.mipmaps + 1,
					tex->format,
					width
				);
				break;

			case GL_TEXTURE_1D_ARRAY :
			case GL_TEXTURE_2D :
			case GL_TEXTURE_CUBE_MAP :
				window->renderer.TexStorage2D(
					tex->target,
					tex->texture.mipmaps + 1,
					tex->format,
					width,
					height
				);
				break;

			case GL_TEXTURE_2D_ARRAY :
			case GL_TEXTURE_3D :
				window->renderer.TexStorage3D(
					tex->target,
					tex->texture.mipmaps + 1,
					tex->format,
					width,
					height,
					depth
				);
				break;

			case GL_TEXTURE_2D_MULTISAMPLE :
				window->renderer.TexStorage2DMultisample(
					tex->target,
					tex->texture.samples,
					tex->format,
					width,
					height,
					GL_FALSE
				);
				break;

			case GL_TEXTURE_2D_MULTISAMPLE_ARRAY :
				window->renderer.TexStorage3DMultisample(
					tex->target,
					tex->texture.samples,
					tex->format,
					width,
					height,
					depth,
					GL_FALSE
				);
				break;

			case GL_TEXTURE_CUBE_MAP_ARRAY :
				window->renderer.TexStorage3D(
					tex->target,
					tex->texture.mipmaps + 1,
					tex->format,
					width,
					height,
					depth * 6
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
	GFX_Window* window = _gfx_window_get_current();
	if(!window) return NULL;

	/* Get target */
	GLenum target;
	switch(type)
	{
		case GFX_TEXTURE_1D :
			target = (height > 1) ?
				GL_TEXTURE_1D_ARRAY : GL_TEXTURE_1D;
			break;

		case GFX_TEXTURE_2D :
			target = (depth > 1) ?
				GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
			break;

		case GFX_TEXTURE_3D :
			target = GL_TEXTURE_3D;
			break;

		case GFX_CUBEMAP :
			target = (depth > 1) ?
				GL_TEXTURE_CUBE_MAP_ARRAY : GL_TEXTURE_CUBE_MAP;
			break;

		/* ??? */
		default : return NULL;
	}

	/* Allocate texture */
	struct GFX_Texture* tex = _gfx_texture_alloc(target, format, window);
	if(!tex) return NULL;

	/* Limit mipmaps */
	unsigned char maxMips =
		_gfx_texture_get_num_mipmaps(type, width, height, depth);
	mipmaps =
		(mipmaps < 0 || mipmaps > maxMips) ? maxMips : mipmaps;

	tex->texture.type = type;
	tex->texture.mipmaps = mipmaps;

	/* Set parameters */
	if(window->ext[GFX_EXT_DIRECT_STATE_ACCESS])
	{
		window->renderer.TextureParameteri(
			tex->handle, GL_TEXTURE_BASE_LEVEL, 0);
		window->renderer.TextureParameteri(
			tex->handle, GL_TEXTURE_MAX_LEVEL, mipmaps);
	}
	else
	{
		int old;
		_gfx_binder_bind_texture(
			tex->handle,
			target,
			0,
			&old,
			window
		);

		window->renderer.TexParameteri(
			tex->target, GL_TEXTURE_BASE_LEVEL, 0);
		window->renderer.TexParameteri(
			tex->target, GL_TEXTURE_MAX_LEVEL, mipmaps);
	}

	/* Allocate data */
	_gfx_texture_set_storage(tex, width, height, depth, window);

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
	GFX_Window* window = _gfx_window_get_current();
	if(!window) return NULL;

	GLenum target = (depth > 1) ?
		GL_TEXTURE_2D_MULTISAMPLE_ARRAY : GL_TEXTURE_2D_MULTISAMPLE;

	/* Allocate texture */
	struct GFX_Texture* tex = _gfx_texture_alloc(target, format, window);
	if(!tex) return NULL;

	/* Limit samples */
	int maxSamples = window->lim[GFX_LIM_MAX_SAMPLES];

	tex->texture.type =
		GFX_TEXTURE_2D;
	tex->texture.samples =
		(samples > maxSamples) ?
		maxSamples : (samples < 2 ? 2 : samples);

	_gfx_texture_set_storage(tex, width, height, depth, window);

	return (GFXTexture*)tex;
}

/******************************************************/
GFXTexture* gfx_texture_create_buffer_link(

		GFXTextureFormat  format,
		const GFXBuffer*  buffer)
{
	GFX_Window* window = _gfx_window_get_current();

	if(
		!window ||
		_gfx_is_data_type_packed(format.type) ||
		format.interpret == GFX_INTERPRET_DEPTH)
	{
		return NULL;
	}

	/* Allocate texture */
	struct GFX_Texture* tex = _gfx_texture_alloc(GL_TEXTURE_BUFFER, format, window);
	if(!tex) return NULL;

	tex->buffer = _gfx_buffer_get_handle(buffer);
	tex->texture.type = GFX_TEXTURE_1D;

	/* Compute dimensions */
	tex->texture.width =
		format.components * _gfx_sizeof_data_type(format.type);
	if(tex->texture.width)
		tex->texture.width = buffer->size / tex->texture.width;

	int max = window->lim[GFX_LIM_MAX_BUFFER_TEXTURE_SIZE];
	tex->texture.width = tex->texture.width > max ? max : tex->texture.width;

	tex->texture.height = 1;
	tex->texture.depth  = 1;

	/* Link buffer */
	if(window->ext[GFX_EXT_DIRECT_STATE_ACCESS])
	{
		window->renderer.TextureBuffer(
			tex->handle, tex->format, tex->buffer);
	}
	else
	{
		int old;
		_gfx_binder_bind_texture(
			tex->handle,
			tex->target,
			0,
			&old,
			window
		);

		window->renderer.TexBuffer(
			tex->target, tex->format, tex->buffer);
	}

	return (GFXTexture*)tex;
}

/******************************************************/
void gfx_texture_free(

		GFXTexture* texture)
{
	if(texture)
	{
		GFX_Window* window = _gfx_window_get_current();
		struct GFX_Texture* internal = (struct GFX_Texture*)texture;

		/* Unregister as object */
		_gfx_hardware_object_unregister(texture->id);

		if(window)
		{
			_gfx_binder_unbind_texture(internal->handle, window);
			window->renderer.DeleteTextures(1, &internal->handle);
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
	GFX_Window* window = _gfx_window_get_current();
	struct GFX_Texture* internal = (struct GFX_Texture*)image.texture;

	if(!window) return;

	if(internal->buffer)
	{
		/* Write indirectly to the linked buffer */
		size_t size = transfer->format.components *
			_gfx_sizeof_data_type(transfer->format.type);

		window->renderer.NamedBufferSubData(
			internal->buffer,
			transfer->xOffset * size,
			transfer->width * size,
			data);
	}
	else
	{
		/* Upload texture data */
		_gfx_states_set_pixel_unpack_alignment(
			transfer->alignment,
			&window->renderer);

		GLint pixForm =
			_gfx_texture_eval_pixel_format(transfer->format);
		GLenum pixType =
			_gfx_is_data_type_packed(transfer->format.type) ?
			transfer->format.type.packed :
			transfer->format.type.unpacked;

		/* Say wut? */
		if(pixForm < 0) return;

		if(window->ext[GFX_EXT_DIRECT_STATE_ACCESS])
		{
			switch(internal->target)
			{
				case GL_TEXTURE_1D :
					window->renderer.TextureSubImage1D(
						internal->handle,
						image.mipmap,
						transfer->xOffset,
						transfer->width,
						pixForm,
						pixType,
						data
					);
					break;

				case GL_TEXTURE_1D_ARRAY :
					window->renderer.TextureSubImage2D(
						internal->handle,
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
					window->renderer.TextureSubImage2D(
						internal->handle,
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
					window->renderer.TextureSubImage3D(
						internal->handle,
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
					window->renderer.TextureSubImage3D(
						internal->handle,
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
					window->renderer.TextureSubImage3D(
						internal->handle,
						image.mipmap,
						transfer->xOffset,
						transfer->yOffset,
						image.face,
						transfer->width,
						transfer->height,
						transfer->depth,
						pixForm,
						pixType,
						data
					);
					break;

				case GL_TEXTURE_CUBE_MAP_ARRAY :
					window->renderer.TextureSubImage3D(
						internal->handle,
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
		else
		{
			/* Bind it to a unit if no direct state access */
			int old;
			_gfx_binder_bind_texture(
				internal->handle,
				internal->target,
				0,
				&old,
				window
			);

			switch(internal->target)
			{
				case GL_TEXTURE_1D :
					window->renderer.TexSubImage1D(
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
					window->renderer.TexSubImage2D(
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
					window->renderer.TexSubImage2D(
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
					window->renderer.TexSubImage3D(
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
					window->renderer.TexSubImage3D(
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
					window->renderer.TexSubImage3D(
						internal->target,
						image.mipmap,
						transfer->xOffset,
						transfer->yOffset,
						image.face,
						transfer->width,
						transfer->height,
						transfer->depth,
						pixForm,
						pixType,
						data
					);
					break;

				case GL_TEXTURE_CUBE_MAP_ARRAY :
					window->renderer.TexSubImage3D(
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
}

/******************************************************/
void gfx_texture_write_from_buffer(

		GFXTextureImage          image,
		const GFXPixelTransfer*  transfer,
		const GFXBuffer*         buffer,
		size_t                   offset)
{
	GFX_Window* window = _gfx_window_get_current();
	struct GFX_Texture* internal = (struct GFX_Texture*)image.texture;

	if(!window) return;

	if(internal->buffer)
	{
		/* Copy the buffer data to the linked buffer */
		size_t size = transfer->format.components *
			_gfx_sizeof_data_type(transfer->format.type);

		window->renderer.CopyNamedBufferSubData(
			_gfx_buffer_get_handle(buffer),
			internal->buffer,
			offset,
			transfer->xOffset * size,
			transfer->width * size);
	}
	else
	{
		/* Bind buffer as unpack pixel buffer before performing the copy */
		window->renderer.BindBuffer(
			GL_PIXEL_UNPACK_BUFFER,
			_gfx_buffer_get_handle(buffer));

		gfx_texture_write(
			image,
			transfer,
			GFX_UINT_TO_VOID(offset));

		/* Also unbind for future transfers */
		window->renderer.BindBuffer(
			GL_PIXEL_UNPACK_BUFFER,
			0);
	}
}

/******************************************************/
void gfx_texture_generate_mipmaps(

		GFXTexture* texture)
{
	GFX_Window* window = _gfx_window_get_current();
	if(!window) return;

	struct GFX_Texture* internal = (struct GFX_Texture*)texture;
	int old;

	if(window->ext[GFX_EXT_DIRECT_STATE_ACCESS])
	{
		window->renderer.GenerateTextureMipmap(internal->handle);
	}
	else
	{
		_gfx_binder_bind_texture(
			internal->handle,
			internal->target,
			0,
			&old,
			window
		);

		window->renderer.GenerateMipmap(internal->target);
	}
}
