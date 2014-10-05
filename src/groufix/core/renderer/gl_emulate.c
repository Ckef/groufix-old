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

#define GL_GLEXT_PROTOTYPES
#include "groufix/core/errors.h"
#include "groufix/core/renderer.h"

#ifdef GFX_RENDERER_GL


/********************************************************
 * GL core & GL ES emulators
 *******************************************************/

static void _gfx_gl_error_base_instance(void)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_INSTANCED_BASE_ATTRIBUTES is incompatible with this context."
	);
}

static void _gfx_gl_error_direct_state_access(void)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_DIRECT_STATE_ACCESS is incompatible with this context."
	);
}

void APIENTRY _gfx_gl_bind_texture_unit(

		GLuint  unit,
		GLuint  texture)
{
	_gfx_gl_error_direct_state_access();
}

void APIENTRY _gfx_gl_copy_named_buffer_sub_data(

		GLuint    readBuffer,
		GLuint    writeBuffer,
		GLintptr  readOffset,
		GLintptr  writeOffset,
		GLsizei   size)
{
	GFX_WIND_INIT_UNSAFE;

	GFX_REND_GET.BindBuffer(GL_COPY_READ_BUFFER, readBuffer);
	GFX_REND_GET.BindBuffer(GL_COPY_WRITE_BUFFER, writeBuffer);

	GFX_REND_GET.CopyBufferSubData(
		GL_COPY_READ_BUFFER,
		GL_COPY_WRITE_BUFFER,
		readOffset,
		writeOffset,
		size
	);
}

void APIENTRY _gfx_gl_create_buffers(

		GLsizei  n,
		GLuint*  buffers)
{
	GFX_WIND_INIT_UNSAFE;
	GFX_REND_GET.GenBuffers(n, buffers);
}

void APIENTRY _gfx_gl_create_framebuffers(

		GLsizei  n,
		GLuint*  ids)
{
	GFX_WIND_INIT_UNSAFE;
	GFX_REND_GET.GenFramebuffers(n, ids);
}

void APIENTRY _gfx_gl_create_textures(

		GLenum   target,
		GLsizei  n,
		GLuint*  textures)
{
	GFX_WIND_INIT_UNSAFE;
	GFX_REND_GET.GenTextures(n, textures);
}

void APIENTRY _gfx_gl_create_vertex_arrays(

		GLsizei  n,
		GLuint*  arrays)
{
	GFX_WIND_INIT_UNSAFE;
	GFX_REND_GET.GenVertexArrays(n, arrays);
}

void APIENTRY _gfx_gl_disable_vertex_array_attrib(

		GLuint  vao,
		GLuint  index)
{
	GFX_WIND_INIT_UNSAFE;

	if(GFX_REND_GET.vao != vao)
	{
		GFX_REND_GET.vao = vao;
		GFX_REND_GET.BindVertexArray(vao);
	}

	GFX_REND_GET.DisableVertexAttribArray(index);
}

void APIENTRY _gfx_gl_draw_arrays_instanced_base_instance(

		GLenum   mode,
		GLint    first,
		GLsizei  count,
		GLsizei  primcount,
		GLuint   baseinstance)
{
	_gfx_gl_error_base_instance();
}

void APIENTRY _gfx_gl_draw_elements_instanced_base_instance(

		GLenum         mode,
		GLsizei        count,
		GLenum         type,
		const GLvoid*  indices,
		GLsizei        primcount,
		GLuint         baseinstance)
{
	_gfx_gl_error_base_instance();
}

void APIENTRY _gfx_gl_enable_vertex_array_attrib(

		GLuint  vao,
		GLuint  index)
{
	GFX_WIND_INIT_UNSAFE;

	if(GFX_REND_GET.vao != vao)
	{
		GFX_REND_GET.vao = vao;
		GFX_REND_GET.BindVertexArray(vao);
	}

	GFX_REND_GET.EnableVertexAttribArray(index);
}

void APIENTRY _gfx_gl_generate_texture_mipmap(

		GLuint texture)
{
	_gfx_gl_error_direct_state_access();
}

void APIENTRY _gfx_gl_get_named_buffer_sub_data(

		GLuint    buffer,
		GLintptr  offset,
		GLsizei   size,
		void*     data)
{
	GFX_WIND_INIT_UNSAFE;

	GFX_REND_GET.BindBuffer(GL_ARRAY_BUFFER, buffer);
	GFX_REND_GET.GetBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void* APIENTRY _gfx_gl_map_named_buffer_range(

		GLuint      buffer,
		GLintptr    offset,
		GLsizei     length,
		GLbitfield  access)
{
	GFX_WIND_INIT_UNSAFE;

	GFX_REND_GET.BindBuffer(GL_ARRAY_BUFFER, buffer);
	return GFX_REND_GET.MapBufferRange(GL_ARRAY_BUFFER, offset, length, access);
}

void APIENTRY _gfx_gl_named_buffer_data(

		GLuint       buffer,
		GLsizei      size,
		const void*  data,
		GLenum       usage)
{
	GFX_WIND_INIT_UNSAFE;

	GFX_REND_GET.BindBuffer(GL_ARRAY_BUFFER, buffer);
	GFX_REND_GET.BufferData(GL_ARRAY_BUFFER, size, data, usage);
}

void APIENTRY _gfx_gl_named_buffer_sub_data(

		GLuint       buffer,
		GLintptr     offset,
		GLsizei      size,
		const void*  data)
{
	GFX_WIND_INIT_UNSAFE;

	GFX_REND_GET.BindBuffer(GL_ARRAY_BUFFER, buffer);
	GFX_REND_GET.BufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void APIENTRY _gfx_gl_named_framebuffer_draw_buffers(

		GLuint         framebuffer,
		GLsizei        n,
		const GLenum*  bufs)
{
	GFX_WIND_INIT_UNSAFE;

	if(GFX_REND_GET.fbos[0] != framebuffer)
	{
		GFX_REND_GET.fbos[0] = framebuffer;
		GFX_REND_GET.BindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	}

	GFX_REND_GET.DrawBuffers(n, bufs);
}

void APIENTRY _gfx_gl_named_framebuffer_texture(

		GLuint  framebuffer,
		GLenum  attach,
		GLuint  texture,
		GLint   level)
{
	GFX_WIND_INIT_UNSAFE;

	if(GFX_REND_GET.fbos[0] != framebuffer)
	{
		GFX_REND_GET.fbos[0] = framebuffer;
		GFX_REND_GET.BindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	}

	GFX_REND_GET.FramebufferTexture(
		GL_DRAW_FRAMEBUFFER,
		attach,
		texture,
		level
	);
}

void APIENTRY _gfx_gl_named_framebuffer_texture_layer(

		GLuint  framebuffer,
		GLenum  attach,
		GLuint  texture,
		GLint   level,
		GLint   layer)
{
	GFX_WIND_INIT_UNSAFE;

	if(GFX_REND_GET.fbos[0] != framebuffer)
	{
		GFX_REND_GET.fbos[0] = framebuffer;
		GFX_REND_GET.BindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	}

	GFX_REND_GET.FramebufferTextureLayer(
		GL_DRAW_FRAMEBUFFER,
		attach,
		texture,
		level,
		layer
	);
}

void APIENTRY _gfx_gl_patch_parameter_i(

		GLenum  pname,
		GLint   value)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_TESSELLATION_SHADER is incompatible with this context."
	);
}

void APIENTRY _gfx_gl_texture_buffer(

		GLuint  texture,
		GLenum  format,
		GLuint  buffer)
{
	_gfx_gl_error_direct_state_access();
}

void APIENTRY _gfx_gl_texture_parameter_i(

		GLuint  texture,
		GLenum  pname,
		GLint   param)
{
	_gfx_gl_error_direct_state_access();
}

void APIENTRY _gfx_gl_texture_storage_1d(

		GLuint   texture,
		GLsizei  levels,
		GLenum   internalFormat,
		GLsizei  w)
{
	_gfx_gl_error_direct_state_access();
}

void APIENTRY _gfx_gl_texture_storage_2d(

		GLuint   texture,
		GLsizei  levels,
		GLenum   internalFormat,
		GLsizei  w,
		GLsizei  h)
{
	_gfx_gl_error_direct_state_access();
}

void APIENTRY _gfx_gl_texture_storage_2d_multisample(

		GLuint     texture,
		GLsizei    samples,
		GLenum     internalFormat,
		GLsizei    w,
		GLsizei    h,
		GLboolean  f)
{
	_gfx_gl_error_direct_state_access();
}

void APIENTRY _gfx_gl_texture_storage_3d(

		GLuint   texture,
		GLsizei  levels,
		GLenum   internalFormat,
		GLsizei  w,
		GLsizei  h,
		GLsizei  d)
{
	_gfx_gl_error_direct_state_access();
}

void APIENTRY _gfx_gl_texture_storage_3d_multisample(

		GLuint     texture,
		GLsizei    samples,
		GLenum     internalFormat,
		GLsizei    w,
		GLsizei    h,
		GLsizei    d,
		GLboolean  f)
{
	_gfx_gl_error_direct_state_access();
}

void APIENTRY _gfx_gl_texture_sub_image_1d(

		GLuint       texture,
		GLint        level,
		GLint        xoffset,
		GLsizei      w,
		GLenum       format,
		GLenum       type,
		const void*  pixels)
{
	_gfx_gl_error_direct_state_access();
}

void APIENTRY _gfx_gl_texture_sub_image_2d(

		GLuint       texture,
		GLint        level,
		GLint        xoffset,
		GLint        yoffset,
		GLsizei      w,
		GLsizei      h,
		GLenum       format,
		GLenum       type,
		const void*  pixels)
{
	_gfx_gl_error_direct_state_access();
}

void APIENTRY _gfx_gl_texture_sub_image_3d(

		GLuint       texture,
		GLint        level,
		GLint        xoffset,
		GLint        yoffset,
		GLint        zoffset,
		GLsizei      w,
		GLsizei      h,
		GLsizei      d,
		GLenum       format,
		GLenum       type,
		const void*  pixels)
{
	_gfx_gl_error_direct_state_access();
}

GLboolean APIENTRY _gfx_gl_unmap_named_buffer(

		GLuint buffer)
{
	GFX_WIND_INIT_UNSAFE;

	GFX_REND_GET.BindBuffer(GL_ARRAY_BUFFER, buffer);
	return GFX_REND_GET.UnmapBuffer(GL_ARRAY_BUFFER);
}


#if defined(GFX_GLES)

/********************************************************
 * GL ES emulators
 *******************************************************/

static void _gfx_gles_error_tex_buffer(void)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_BUFFER_TEXTURE is incompatible with this context."
	);
}

static void _gfx_gles_error_tex_1d(void)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_TEXTURE_1D is incompatible with this context."
	);
}

static void _gfx_gles_error_multisample_tex(void)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_MULTISAMPLE_TEXTURE is incompatible with this context."
	);
}

static void _gfx_gles_error_layered_multisample_tex(void)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_LAYERED_MULTISAMPLE_TEXTURE is incompatible with this context."
	);
}

void APIENTRY _gfx_gles_framebuffer_texture(

		GLenum  target,
		GLenum  attach,
		GLuint  texture,
		GLint   level)
{
	_gfx_gles_error_tex_buffer();
}

void APIENTRY _gfx_gles_framebuffer_texture_1d(

		GLenum  target,
		GLenum  attach,
		GLenum  textarget,
		GLuint  texture,
		GLint   level)
{
	_gfx_gles_error_tex_1d();
}

void APIENTRY _gfx_gles_get_buffer_sub_data(

		GLenum      target,
		GLintptr    offset,
		GLsizeiptr  size,
		GLvoid*     data)
{
	void* map = glMapBufferRange(
		target,
		offset,
		size,
		GL_MAP_READ_BIT
	);

	if(map)
	{
		memcpy(data, map, size);

		if(!glUnmapBuffer(target)) gfx_errors_push(
			GFX_ERROR_MEMORY_CORRUPTION,
			"Reading from a buffer might have corrupted its memory."
		);
	}
}

void APIENTRY _gfx_gles_named_framebuffer_texture_1d(

		GLuint  framebuffer,
		GLenum  attach,
		GLenum  textarget,
		GLuint  texture,
		GLint   level)
{
	GFX_WIND_INIT_UNSAFE;

	if(GFX_REND_GET.fbos[0] != framebuffer)
	{
		GFX_REND_GET.fbos[0] = framebuffer;
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	}

	GFX_REND_GET.FramebufferTexture1D(
		GL_DRAW_FRAMEBUFFER,
		attach,
		textarget,
		texture,
		level
	);
}

void APIENTRY _gfx_gles_named_framebuffer_texture_2d(

		GLuint  framebuffer,
		GLenum  attach,
		GLenum  textarget,
		GLuint  texture,
		GLint   level)
{
	GFX_WIND_INIT_UNSAFE;

	if(GFX_REND_GET.fbos[0] != framebuffer)
	{
		GFX_REND_GET.fbos[0] = framebuffer;
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	}

	glFramebufferTexture2D(
		GL_DRAW_FRAMEBUFFER,
		attach,
		textarget,
		texture,
		level
	);
}

void APIENTRY _gfx_gles_polygon_mode(

		GLenum  face,
		GLenum  mode)
{
	/* Just ignore the call */
}

void APIENTRY _gfx_gles_tex_buffer(

		GLenum  target,
		GLenum  internalFormat,
		GLuint  buffer)
{
	_gfx_gles_error_tex_buffer();
}

void APIENTRY _gfx_gles_tex_image_1d(

		GLenum         target,
		GLint          level,
		GLint          internalFormat,
		GLsizei        w,
		GLint          b,
		GLenum         format,
		GLenum         type,
		const GLvoid*  data)
{
	_gfx_gles_error_tex_1d();
}

void APIENTRY _gfx_gles_tex_image_2d_multisample(

		GLenum     target,
		GLsizei    samples,
		GLenum     internalformat,
		GLsizei    w,
		GLsizei    h,
		GLboolean  f)
{
	_gfx_gles_error_multisample_tex();
}

void APIENTRY _gfx_gles_tex_image_3d_multisample(

		GLenum     target,
		GLsizei    samples,
		GLenum     internalformat,
		GLsizei    w,
		GLsizei    h,
		GLsizei    d,
		GLboolean  f)
{
	_gfx_gles_error_layered_multisample_tex();
}

void APIENTRY _gfx_gles_tex_storage_1d(

		GLenum   target,
		GLsizei  levels,
		GLenum   internalFormat,
		GLsizei  w)
{
	_gfx_gles_error_tex_1d();
}

void APIENTRY _gfx_gles_tex_storage_2d_multisample(

		GLenum     target,
		GLsizei    samples,
		GLenum     internalFormat,
		GLsizei    w,
		GLsizei    h,
		GLboolean  f)
{
	_gfx_gles_error_multisample_tex();
}

void APIENTRY _gfx_gles_tex_storage_3d_multisample(

		GLenum     target,
		GLsizei    samples,
		GLenum     internalFormat,
		GLsizei    w,
		GLsizei    h,
		GLsizei    d,
		GLboolean  f)
{
	_gfx_gles_error_layered_multisample_tex();
}

void APIENTRY _gfx_gles_tex_sub_image_1d(

		GLenum         target,
		GLint          level,
		GLint          xoff,
		GLsizei        w,
		GLenum         format,
		GLenum         type,
		const GLvoid*  data)
{
	_gfx_gles_error_tex_1d();
}


#elif defined(GFX_GL)

/********************************************************
 * GL core emulators
 *******************************************************/

static void _gfx_gl_error_program_binary(void)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_PROGRAM_BINARY is incompatible with this context."
	);
}

void APIENTRY _gfx_gl_named_framebuffer_texture_1d(

		GLuint  framebuffer,
		GLenum  attach,
		GLenum  textarget,
		GLuint  texture,
		GLint   level)
{
	GFX_WIND_INIT_UNSAFE;
	GFX_REND_GET.NamedFramebufferTexture(framebuffer, attach, texture, level);
}

void APIENTRY _gfx_gl_named_framebuffer_texture_2d(

		GLuint  framebuffer,
		GLenum  attach,
		GLenum  textarget,
		GLuint  texture,
		GLint   level)
{
	GFX_WIND_INIT_UNSAFE;
	GFX_REND_GET.NamedFramebufferTexture(framebuffer, attach, texture, level);
}

void APIENTRY _gfx_gl_get_program_binary(

		GLuint    program,
		GLsizei   bufsize,
		GLsizei*  length,
		GLenum*   binaryFormat,
		void*     binary)
{
	if(length) *length = 0;

	_gfx_gl_error_program_binary();
}

void APIENTRY _gfx_gl_program_binary(

		GLuint       program,
		GLenum       binaryFormat,
		const void*  binary,
		GLsizei      length)
{
	_gfx_gl_error_program_binary();
}

void APIENTRY _gfx_gl_program_parameter_i(

		GLuint  program,
		GLenum  pname,
		GLint   value)
{
	_gfx_gl_error_program_binary();
}

void APIENTRY _gfx_gl_tex_storage_1d(

		GLenum   target,
		GLsizei  levels,
		GLenum   internalFormat,
		GLsizei  w)
{
	GFX_WIND_INIT_UNSAFE;

	GLsizei l;
	for(l = 0; l < levels; ++l)
	{
		GFX_REND_GET.TexImage1D(target, l, internalFormat, w, 0, GL_RED, GL_BYTE, NULL);
		w >>= (w > 1) ? 1 : 0;
	}
}

void APIENTRY _gfx_gl_tex_storage_2d(

		GLenum   target,
		GLsizei  levels,
		GLenum   internalFormat,
		GLsizei  w,
		GLsizei  h)
{
	GFX_WIND_INIT_UNSAFE;

	GLsizei hf =
		(target == GL_TEXTURE_1D || target == GL_TEXTURE_1D_ARRAY) ? 0 : 1;
	GLenum first =
		(target == GL_TEXTURE_CUBE_MAP) ? GL_TEXTURE_CUBE_MAP_POSITIVE_X : target;
	GLenum last =
		(target == GL_TEXTURE_CUBE_MAP) ? GL_TEXTURE_CUBE_MAP_NEGATIVE_Z : target;

	GLsizei l;
	for(l = 0; l < levels; ++l)
	{
		GLenum face;
		for(face = first; face <= last; ++face)
			GFX_REND_GET.TexImage2D(face, l, internalFormat, w, h, 0, GL_RED, GL_BYTE, NULL);

		w >>= (w > 1) ? 1 : 0;
		h >>= (h > 1) ? hf : 0;
	}
}

void APIENTRY _gfx_gl_tex_storage_2d_multisample(

		GLenum     target,
		GLsizei    samples,
		GLenum     internalFormat,
		GLsizei    w,
		GLsizei    h,
		GLboolean  f)
{
	GFX_WIND_INIT_UNSAFE;
	GFX_REND_GET.TexImage2DMultisample(target, samples, internalFormat, w, h, f);
}

void APIENTRY _gfx_gl_tex_storage_3d(

		GLenum   target,
		GLsizei  levels,
		GLenum   internalFormat,
		GLsizei  w,
		GLsizei  h,
		GLsizei  d)
{
	GFX_WIND_INIT_UNSAFE;

	GLsizei df =
		(target == GL_TEXTURE_3D || target == GL_PROXY_TEXTURE_3D) ? 1 : 0;

	GLsizei l;
	for(l = 0; l < levels; ++l)
	{
		GFX_REND_GET.TexImage3D(target, l, internalFormat, w, h, d, 0, GL_RED, GL_BYTE, NULL);
		w >>= (w > 1) ? 1 : 0;
		h >>= (h > 1) ? 1 : 0;
		d >>= (d > 1) ? df : 0;
	}
}

void APIENTRY _gfx_gl_tex_storage_3d_multisample(

		GLenum     target,
		GLsizei    samples,
		GLenum     internalFormat,
		GLsizei    w,
		GLsizei    h,
		GLsizei    d,
		GLboolean  f)
{
	GFX_WIND_INIT_UNSAFE;
	GFX_REND_GET.TexImage3DMultisample(target, samples, internalFormat, w, h, d, f);
}

void APIENTRY _gfx_gl_vertex_attrib_divisor(

		GLuint  index,
		GLuint  divisor)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_INSTANCED_ATTRIBUTES is incompatible with this context."
	);
}

#endif

#endif // GFX_RENDERER_GL
