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

#include "groufix/core/internal.h"

#include <limits.h>
#include <stdlib.h>

/******************************************************/
/* Internal Texture */
typedef struct GFX_Texture
{
	/* Super class */
	GFXTexture texture;

	/* Hidden data */
	GFX_RenderObjectID  id;
	GLuint              handle;  /* OpenGL handle */
	GLenum              target;
	GLint               format;  /* Internal format */
	GLuint              buffer;

} GFX_Texture;


/******************************************************/
static inline unsigned char _gfx_texture_get_num_mipmaps(

		GFXTextureType  type,
		size_t          w,
		size_t          h,
		size_t          d)
{
	/* Get correct dimensions */
	if(type == GFX_TEXTURE_2D || type == GFX_CUBEMAP)
		d = 1;

	/* Calculate number of mipmaps */
	size_t max = w > h ? (w > d ? w : d) : (h > d ? h : d);

	unsigned char num = 0;
	while(max >>= 1 && num < UCHAR_MAX) ++num;

	return num;
}

/******************************************************/
static int _gfx_texture_eval_target(

		GLenum target,
		GFX_CONT_ARG)
{
	switch(target)
	{
		/* GFX_EXT_BUFFER_TEXTURE */
		case GL_TEXTURE_BUFFER :

			if(!GFX_CONT_GET.ext[GFX_EXT_BUFFER_TEXTURE])
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

			if(!GFX_CONT_GET.ext[GFX_EXT_MULTISAMPLE_TEXTURE])
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

			if(!GFX_CONT_GET.ext[GFX_EXT_LAYERED_MULTISAMPLE_TEXTURE])
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

			if(!GFX_CONT_GET.ext[GFX_EXT_LAYERED_CUBEMAP])
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

		void*               object,
		GFX_RenderObjectID  id)
{
	GFX_Texture* texture = (GFX_Texture*)object;

	texture->id = id;
	texture->handle = 0;
	texture->buffer = 0;
}

/******************************************************/
static void _gfx_texture_obj_save_restore(

		void*               object,
		GFX_RenderObjectID  id)
{
	GFX_Texture* texture = (GFX_Texture*)object;
	texture->id = id;
}

/******************************************************/
/* vtable for render object part of the texture */
static GFX_RenderObjectFuncs _gfx_texture_obj_funcs =
{
	_gfx_texture_obj_free,
	_gfx_texture_obj_save_restore,
	_gfx_texture_obj_save_restore
};

/******************************************************/
static GFX_Texture* _gfx_texture_alloc(

		GLenum            target,
		GFXTextureFormat  format,
		GFX_CONT_ARG)
{
	/* Validate type & format */
	GLint form = _gfx_texture_eval_internal_format(format);
	if(form < 0 || !_gfx_texture_eval_target(target, GFX_CONT_AS_ARG))
		return NULL;

	/* Create new texture */
	GFX_Texture* tex = calloc(1, sizeof(GFX_Texture));
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
	tex->id = _gfx_render_object_register(
		&GFX_CONT_GET.objects,
		tex,
		&_gfx_texture_obj_funcs
	);

	if(!tex->id.id)
	{
		free(tex);
		return NULL;
	}

	/* Allocate OGL resources */
	GFX_REND_GET.CreateTextures(target, 1, &tex->handle);

	tex->target = target;
	tex->format = form;
	tex->texture.samples = 1;

	if(!GFX_REND_GET.intExt[GFX_INT_EXT_DIRECT_STATE_ACCESS])
	{
		/* Bind it to a unit if no direct state access */
		_gfx_binder_bind_texture(
			tex->handle,
			tex->target,
			0,
			GFX_CONT_AS_ARG
		);
	}

	return tex;
}

/******************************************************/
static void _gfx_texture_set_mipmaps(

		const GFX_Texture*  tex,
		unsigned char       mips,
		GFX_CONT_ARG)
{
	if(GFX_REND_GET.intExt[GFX_INT_EXT_DIRECT_STATE_ACCESS])
	{
		GFX_REND_GET.TextureParameteri(
			tex->handle, GL_TEXTURE_BASE_LEVEL, 0);
		GFX_REND_GET.TextureParameteri(
			tex->handle, GL_TEXTURE_MAX_LEVEL, mips);
	}
	else
	{
		/* Assumes it is already bound */
		GFX_REND_GET.TexParameteri(
			tex->target, GL_TEXTURE_BASE_LEVEL, 0);
		GFX_REND_GET.TexParameteri(
			tex->target, GL_TEXTURE_MAX_LEVEL, mips);
	}
}

/******************************************************/
static void _gfx_texture_set_storage(

		GFX_Texture*  tex,
		size_t        width,
		size_t        height,
		size_t        depth,
		GFX_CONT_ARG)
{
	tex->texture.width  = width;
	tex->texture.height = height;
	tex->texture.depth  = depth;

	/* Allocate storage */
	if(GFX_REND_GET.intExt[GFX_INT_EXT_DIRECT_STATE_ACCESS])
	{
		switch(tex->target)
		{
			case GL_TEXTURE_2D :
			case GL_TEXTURE_CUBE_MAP :
				GFX_REND_GET.TextureStorage2D(
					tex->handle,
					tex->texture.mipmaps + 1,
					tex->format,
					width,
					height
				);
				break;

			case GL_TEXTURE_2D_ARRAY :
			case GL_TEXTURE_3D :
				GFX_REND_GET.TextureStorage3D(
					tex->handle,
					tex->texture.mipmaps + 1,
					tex->format,
					width,
					height,
					depth
				);
				break;

			case GL_TEXTURE_2D_MULTISAMPLE :
				GFX_REND_GET.TextureStorage2DMultisample(
					tex->handle,
					tex->texture.samples,
					tex->format,
					width,
					height,
					GL_FALSE
				);
				break;

			case GL_TEXTURE_2D_MULTISAMPLE_ARRAY :
				GFX_REND_GET.TextureStorage3DMultisample(
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
				GFX_REND_GET.TextureStorage3D(
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
		/* Assumes it is already bound */
		switch(tex->target)
		{
			case GL_TEXTURE_2D :
			case GL_TEXTURE_CUBE_MAP :
				GFX_REND_GET.TexStorage2D(
					tex->target,
					tex->texture.mipmaps + 1,
					tex->format,
					width,
					height
				);
				break;

			case GL_TEXTURE_2D_ARRAY :
			case GL_TEXTURE_3D :
				GFX_REND_GET.TexStorage3D(
					tex->target,
					tex->texture.mipmaps + 1,
					tex->format,
					width,
					height,
					depth
				);
				break;

			case GL_TEXTURE_2D_MULTISAMPLE :
				GFX_REND_GET.TexStorage2DMultisample(
					tex->target,
					tex->texture.samples,
					tex->format,
					width,
					height,
					GL_FALSE
				);
				break;

			case GL_TEXTURE_2D_MULTISAMPLE_ARRAY :
				GFX_REND_GET.TexStorage3DMultisample(
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
				GFX_REND_GET.TexStorage3D(
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
	return ((const GFX_Texture*)texture)->handle;
}

/******************************************************/
GLenum _gfx_texture_get_internal_target(

		const GFXTexture* texture)
{
	return ((const GFX_Texture*)texture)->target;
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
	GFX_CONT_INIT(NULL);

	/* Get target */
	GLenum target;
	switch(type)
	{
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
	GFX_Texture* tex = _gfx_texture_alloc(
		target,
		format,
		GFX_CONT_AS_ARG
	);

	if(!tex) return NULL;

	/* Limit mipmaps */
	unsigned char maxMips =
		_gfx_texture_get_num_mipmaps(type, width, height, depth);
	mipmaps =
		(mipmaps < 0 || mipmaps > maxMips) ? maxMips : mipmaps;

	tex->texture.type = type;
	tex->texture.mipmaps = mipmaps;

	/* Set parameters */
	_gfx_texture_set_mipmaps(
		tex,
		mipmaps,
		GFX_CONT_AS_ARG
	);

	/* Allocate data */
	_gfx_texture_set_storage(
		tex,
		width,
		height,
		depth,
		GFX_CONT_AS_ARG
	);

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
	GFX_CONT_INIT(NULL);

	GLenum target = (depth > 1) ?
		GL_TEXTURE_2D_MULTISAMPLE_ARRAY : GL_TEXTURE_2D_MULTISAMPLE;

	/* Allocate texture */
	GFX_Texture* tex = _gfx_texture_alloc(
		target,
		format,
		GFX_CONT_AS_ARG
	);

	if(!tex) return NULL;

	/* Limit samples */
	int maxSamples = GFX_CONT_GET.lim[GFX_LIM_MAX_SAMPLES];

	tex->texture.type =
		GFX_TEXTURE_2D;
	tex->texture.samples =
		(samples > maxSamples) ?
		maxSamples : (samples < 2 ? 2 : samples);

	/* Set parameters */
	_gfx_texture_set_mipmaps(
		tex,
		0,
		GFX_CONT_AS_ARG
	);

	/* Allocate data */
	_gfx_texture_set_storage(
		tex,
		width,
		height,
		depth,
		GFX_CONT_AS_ARG
	);

	return (GFXTexture*)tex;
}

/******************************************************/
GFXTexture* gfx_texture_create_buffer_link(

		GFXTextureFormat  format,
		const GFXBuffer*  buffer)
{
	GFX_CONT_INIT(NULL);

	if(
		_gfx_is_data_type_packed(format.type) ||
		format.interpret == GFX_INTERPRET_DEPTH)
	{
		return NULL;
	}

	/* Allocate texture */
	GFX_Texture* tex = _gfx_texture_alloc(
		GL_TEXTURE_BUFFER,
		format,
		GFX_CONT_AS_ARG
	);

	if(!tex) return NULL;

	tex->buffer = _gfx_buffer_get_handle(buffer);
	tex->texture.type = GFX_TEXTURE_2D;

	/* Compute dimensions */
	tex->texture.width =
		format.components * _gfx_sizeof_data_type(format.type);
	if(tex->texture.width)
		tex->texture.width = buffer->size / tex->texture.width;

	int max = GFX_CONT_GET.lim[GFX_LIM_MAX_BUFFER_TEXTURE_SIZE];
	tex->texture.width = tex->texture.width > max ? max : tex->texture.width;

	tex->texture.height = 1;
	tex->texture.depth  = 1;

	/* Set parameters */
	_gfx_texture_set_mipmaps(
		tex,
		0,
		GFX_CONT_AS_ARG
	);

	/* Link buffer */
	if(GFX_REND_GET.intExt[GFX_INT_EXT_DIRECT_STATE_ACCESS])
	{
		GFX_REND_GET.TextureBuffer(
			tex->handle, tex->format, tex->buffer);
	}
	else
	{
		/* Assumes it is already bound */
		GFX_REND_GET.TexBuffer(
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
		GFX_CONT_INIT_UNSAFE;

		GFX_Texture* internal = (GFX_Texture*)texture;

		/* Unregister as object */
		_gfx_render_object_unregister(internal->id);

		if(!GFX_CONT_EQ(NULL))
		{
			/* Delete texture */
			_gfx_binder_unbind_texture(internal->handle, GFX_CONT_AS_ARG);
			GFX_REND_GET.DeleteTextures(1, &internal->handle);
		}

		free(texture);
	}
}

/******************************************************/
GFXTextureFormat gfx_texture_get_format(

		const GFXTexture* texture)
{
	const GFX_Texture* internal = (const GFX_Texture*)texture;
	return _gfx_texture_format_from_internal(internal->format);
}

/******************************************************/
void gfx_texture_write(

		GFXTextureImage          image,
		const GFXPixelTransfer*  transfer,
		const void*              data)
{
	GFX_CONT_INIT();

	const GFX_Texture* tex = (const GFX_Texture*)image.texture;

	if(tex->buffer)
	{
		/* Write indirectly to the linked buffer */
		size_t size = transfer->format.components *
			_gfx_sizeof_data_type(transfer->format.type);

		GFX_REND_GET.NamedBufferSubData(
			tex->buffer,
			transfer->xOffset * size,
			transfer->width * size,
			data);
	}
	else
	{
		/* Upload texture data */
		_gfx_states_set_pixel_unpack_alignment(
			transfer->alignment,
			GFX_CONT_AS_ARG);

		GLint pixForm =
			_gfx_texture_eval_pixel_format(transfer->format);
		GLenum pixType =
			_gfx_is_data_type_packed(transfer->format.type) ?
			transfer->format.type.packed :
			transfer->format.type.unpacked;

		/* Say wut? */
		if(pixForm < 0) return;

		if(GFX_REND_GET.intExt[GFX_INT_EXT_DIRECT_STATE_ACCESS])
		{
			switch(tex->target)
			{
				case GL_TEXTURE_2D :
					GFX_REND_GET.TextureSubImage2D(
						tex->handle,
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
					GFX_REND_GET.TextureSubImage3D(
						tex->handle,
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
					GFX_REND_GET.TextureSubImage3D(
						tex->handle,
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
					GFX_REND_GET.TextureSubImage3D(
						tex->handle,
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
					GFX_REND_GET.TextureSubImage3D(
						tex->handle,
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
			_gfx_binder_bind_texture(
				tex->handle,
				tex->target,
				0,
				GFX_CONT_AS_ARG
			);

			switch(tex->target)
			{
				case GL_TEXTURE_2D :
					GFX_REND_GET.TexSubImage2D(
						tex->target,
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
					GFX_REND_GET.TexSubImage3D(
						tex->target,
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
					GFX_REND_GET.TexSubImage3D(
						tex->target,
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
					GFX_REND_GET.TexSubImage3D(
						tex->target,
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
					GFX_REND_GET.TexSubImage3D(
						tex->target,
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
	GFX_CONT_INIT();

	const GFX_Texture* tex = (const GFX_Texture*)image.texture;

	if(tex->buffer)
	{
		/* Copy the buffer data to the linked buffer */
		size_t size = transfer->format.components *
			_gfx_sizeof_data_type(transfer->format.type);

		GFX_REND_GET.CopyNamedBufferSubData(
			_gfx_buffer_get_handle(buffer),
			tex->buffer,
			offset,
			transfer->xOffset * size,
			transfer->width * size);
	}
	else
	{
		/* Bind buffer as unpack pixel buffer before performing the copy */
		GFX_REND_GET.BindBuffer(
			GL_PIXEL_UNPACK_BUFFER,
			_gfx_buffer_get_handle(buffer));

		gfx_texture_write(
			image,
			transfer,
			GFX_UINT_TO_VOID(offset));

		/* Also unbind for future transfers */
		GFX_REND_GET.BindBuffer(
			GL_PIXEL_UNPACK_BUFFER,
			0);
	}
}

/******************************************************/
void gfx_texture_generate_mipmaps(

		const GFXTexture* texture)
{
	GFX_CONT_INIT();

	const GFX_Texture* internal = (const GFX_Texture*)texture;

	if(GFX_REND_GET.intExt[GFX_INT_EXT_DIRECT_STATE_ACCESS])
	{
		GFX_REND_GET.GenerateTextureMipmap(internal->handle);
	}
	else
	{
		/* Bind it to a unit if no direct state access */
		_gfx_binder_bind_texture(
			internal->handle,
			internal->target,
			0,
			GFX_CONT_AS_ARG
		);

		GFX_REND_GET.GenerateMipmap(internal->target);
	}
}
