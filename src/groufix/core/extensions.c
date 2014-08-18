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
#include "groufix/core/pipeline/internal.h"
#include "groufix/core/errors.h"

#include <string.h>


/********************************************************
 * Context extensions
 *******************************************************/
static const char* _gfx_glsl_versions[] =
{
	"150\0",
	"300\0",
	"310\0",
	"330\0",
	"400\0",
	"410\0",
	"420\0",
	"430\0",
	"440\0"
};

/******************************************************/
static int _gfx_is_extension_supported(

		const char* ext)
{
	GLint num;
	glGetIntegerv(GL_NUM_EXTENSIONS, &num);

	while(num) if(!strcmp(
		(const char*)(GFX_EXT)->GetStringi(GL_EXTENSIONS, --num),
		(const char*)ext))
	{
		return 1;
	}

	return 0;
}

/******************************************************/
const char* _gfx_extensions_get_glsl(

		int  major,
		int  minor)
{
#ifdef GFX_GLES

	/* GLES context */
	switch(major)
	{
		case 3 : switch(minor)
		{
			case 0 : return _gfx_glsl_versions[1];
			case 1 : return _gfx_glsl_versions[2];
		}

		default : return NULL;
	}

#else

	/* GL context */
	switch(major)
	{
		case 3 : switch(minor)
		{
			case 2 : return _gfx_glsl_versions[0];
			case 3 : return _gfx_glsl_versions[3];
			default : return NULL;
		}
		case 4 : switch(minor)
		{
			case 0 : return _gfx_glsl_versions[4];
			case 1 : return _gfx_glsl_versions[5];
			case 2 : return _gfx_glsl_versions[6];
			case 3 : return _gfx_glsl_versions[7];
			case 4 : return _gfx_glsl_versions[8];
			default : return NULL;
		}

		default : return NULL;
	}

#endif
}

/******************************************************/
int _gfx_extensions_is_in_string(

		const char*  str,
		const char*  ext)
{
	/* Get extension length */
	size_t len = strlen(ext);
	if(!len) return 0;

	/* Try to find a complete match */
	char* found = strstr(str, ext);
	while(found)
	{
		char* end = found + len;
		if(*end == ' ' || *end == '\0')
		{
			/* To avoid segfault */
			if(found == str) return 1;
			if(*(found - 1) == ' ') return 1;
		}
		found = strstr(end, ext);
	}

	return 0;
}


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

static void APIENTRY _gfx_gl_bind_texture_unit(

		GLuint  unit,
		GLuint  texture)
{
	_gfx_gl_error_direct_state_access();
}

static void APIENTRY _gfx_gl_copy_named_buffer_sub_data(

		GLuint    readBuffer,
		GLuint    writeBuffer,
		GLintptr  readOffset,
		GLintptr  writeOffset,
		GLsizei   size)
{
	(GFX_EXT)->BindBuffer(GL_COPY_READ_BUFFER, readBuffer);
	(GFX_EXT)->BindBuffer(GL_COPY_WRITE_BUFFER, writeBuffer);

	(GFX_EXT)->CopyBufferSubData(
		GL_COPY_READ_BUFFER,
		GL_COPY_WRITE_BUFFER,
		readOffset,
		writeOffset,
		size
	);
}

static void APIENTRY _gfx_gl_create_buffers(

		GLsizei  n,
		GLuint*  buffers)
{
	(GFX_EXT)->GenBuffers(n, buffers);
}

static void APIENTRY _gfx_gl_create_framebuffers(

		GLsizei  n,
		GLuint*  ids)
{
	(GFX_EXT)->GenFramebuffers(n, ids);
}

static void APIENTRY _gfx_gl_create_textures(

		GLenum   target,
		GLsizei  n,
		GLuint*  textures)
{
	(GFX_EXT)->GenTextures(n, textures);
}

static void APIENTRY _gfx_gl_create_vertex_arrays(

		GLsizei  n,
		GLuint*  arrays)
{
	(GFX_EXT)->GenVertexArrays(n, arrays);
}

static void APIENTRY _gfx_gl_disable_vertex_array_attrib(

		GLuint  vao,
		GLuint  index)
{
	if((GFX_EXT)->vao != vao)
	{
		(GFX_EXT)->vao = vao;
		(GFX_EXT)->BindVertexArray(vao);
	}
	(GFX_EXT)->DisableVertexAttribArray(index);
}

static void APIENTRY _gfx_gl_draw_arrays_instanced_base_instance(

		GLenum   mode,
		GLint    first,
		GLsizei  count,
		GLsizei  primcount,
		GLuint   baseinstance)
{
	_gfx_gl_error_base_instance();
}

static void APIENTRY _gfx_gl_draw_elements_instanced_base_instance(

		GLenum         mode,
		GLsizei        count,
		GLenum         type,
		const GLvoid*  indices,
		GLsizei        primcount,
		GLuint         baseinstance)
{
	_gfx_gl_error_base_instance();
}

static void APIENTRY _gfx_gl_enable_vertex_array_attrib(

		GLuint  vao,
		GLuint  index)
{
	if((GFX_EXT)->vao != vao)
	{
		(GFX_EXT)->vao = vao;
		(GFX_EXT)->BindVertexArray(vao);
	}
	(GFX_EXT)->EnableVertexAttribArray(index);
}

static void APIENTRY _gfx_gl_generate_texture_mipmap(

		GLuint texture)
{
	_gfx_gl_error_direct_state_access();
}

static void APIENTRY _gfx_gl_get_named_buffer_sub_data(

		GLuint    buffer,
		GLintptr  offset,
		GLsizei   size,
		void*     data)
{
	(GFX_EXT)->BindBuffer(GL_ARRAY_BUFFER, buffer);
	(GFX_EXT)->GetBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

static void* APIENTRY _gfx_gl_map_named_buffer_range(

		GLuint      buffer,
		GLintptr    offset,
		GLsizei     length,
		GLbitfield  access)
{
	(GFX_EXT)->BindBuffer(GL_ARRAY_BUFFER, buffer);
	return (GFX_EXT)->MapBufferRange(GL_ARRAY_BUFFER, offset, length, access);
}

static void APIENTRY _gfx_gl_named_buffer_data(

		GLuint       buffer,
		GLsizei      size,
		const void*  data,
		GLenum       usage)
{
	(GFX_EXT)->BindBuffer(GL_ARRAY_BUFFER, buffer);
	(GFX_EXT)->BufferData(GL_ARRAY_BUFFER, size, data, usage);
}

static void APIENTRY _gfx_gl_named_buffer_sub_data(

		GLuint       buffer,
		GLintptr     offset,
		GLsizei      size,
		const void*  data)
{
	(GFX_EXT)->BindBuffer(GL_ARRAY_BUFFER, buffer);
	(GFX_EXT)->BufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

static void APIENTRY _gfx_gl_named_framebuffer_draw_buffers(

		GLuint         framebuffer,
		GLsizei        n,
		const GLenum*  bufs)
{
	if((GFX_EXT)->fbos[0] != framebuffer)
	{
		(GFX_EXT)->fbos[0] = framebuffer;
		(GFX_EXT)->BindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	}
	(GFX_EXT)->DrawBuffers(n, bufs);
}

static void APIENTRY _gfx_gl_named_framebuffer_texture(

		GLuint  framebuffer,
		GLenum  attach,
		GLuint  texture,
		GLint   level)
{
	if((GFX_EXT)->fbos[0] != framebuffer)
	{
		(GFX_EXT)->fbos[0] = framebuffer;
		(GFX_EXT)->BindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	}

	(GFX_EXT)->FramebufferTexture(
		GL_DRAW_FRAMEBUFFER,
		attach,
		texture,
		level
	);
}

static void APIENTRY _gfx_gl_named_framebuffer_texture_layer(

		GLuint  framebuffer,
		GLenum  attach,
		GLuint  texture,
		GLint   level,
		GLint   layer)
{
	if((GFX_EXT)->fbos[0] != framebuffer)
	{
		(GFX_EXT)->fbos[0] = framebuffer;
		(GFX_EXT)->BindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	}

	(GFX_EXT)->FramebufferTextureLayer(
		GL_DRAW_FRAMEBUFFER,
		attach,
		texture,
		level,
		layer
	);
}

static void APIENTRY _gfx_gl_patch_parameter_i(

		GLenum  pname,
		GLint   value)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_TESSELLATION_SHADER is incompatible with this context."
	);
}

static void APIENTRY _gfx_gl_texture_buffer(

		GLuint  texture,
		GLenum  format,
		GLuint  buffer)
{
	_gfx_gl_error_direct_state_access();
}

static void APIENTRY _gfx_gl_texture_parameter_i(

		GLuint  texture,
		GLenum  pname,
		GLint   param)
{
	_gfx_gl_error_direct_state_access();
}

static void APIENTRY _gfx_gl_texture_storage_1d(

		GLuint   texture,
		GLsizei  levels,
		GLenum   internalFormat,
		GLsizei  w)
{
	_gfx_gl_error_direct_state_access();
}

static void APIENTRY _gfx_gl_texture_storage_2d(

		GLuint   texture,
		GLsizei  levels,
		GLenum   internalFormat,
		GLsizei  w,
		GLsizei  h)
{
	_gfx_gl_error_direct_state_access();
}

static void APIENTRY _gfx_gl_texture_storage_2d_multisample(

		GLuint     texture,
		GLsizei    samples,
		GLenum     internalFormat,
		GLsizei    w,
		GLsizei    h,
		GLboolean  f)
{
	_gfx_gl_error_direct_state_access();
}

static void APIENTRY _gfx_gl_texture_storage_3d(

		GLuint   texture,
		GLsizei  levels,
		GLenum   internalFormat,
		GLsizei  w,
		GLsizei  h,
		GLsizei  d)
{
	_gfx_gl_error_direct_state_access();
}

static void APIENTRY _gfx_gl_texture_storage_3d_multisample(

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

static void APIENTRY _gfx_gl_texture_sub_image_1d(

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

static void APIENTRY _gfx_gl_texture_sub_image_2d(

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

static void APIENTRY _gfx_gl_texture_sub_image_3d(

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

static GLboolean APIENTRY _gfx_gl_unmap_named_buffer(

		GLuint buffer)
{
	(GFX_EXT)->BindBuffer(GL_ARRAY_BUFFER, buffer);
	return (GFX_EXT)->UnmapBuffer(GL_ARRAY_BUFFER);
}


#ifdef GFX_GLES

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

static void APIENTRY _gfx_gles_framebuffer_texture(

		GLenum  target,
		GLenum  attach,
		GLuint  texture,
		GLint   level)
{
	_gfx_gles_error_tex_buffer();
}

static void APIENTRY _gfx_gles_framebuffer_texture_1d(

		GLenum  target,
		GLenum  attach,
		GLenum  textarget,
		GLuint  texture,
		GLint   level)
{
	_gfx_gles_error_tex_1d();
}

static void APIENTRY _gfx_gles_get_buffer_sub_data(

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

static void APIENTRY _gfx_gles_named_framebuffer_texture_1d(

		GLuint  framebuffer,
		GLenum  attach,
		GLenum  textarget,
		GLuint  texture,
		GLint   level)
{
	if((GFX_EXT)->fbos[0] != framebuffer)
	{
		(GFX_EXT)->fbos[0] = framebuffer;
		(GFX_EXT)->BindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	}

	(GFX_EXT)->FramebufferTexture1D(
		GL_DRAW_FRAMEBUFFER,
		attach,
		textarget,
		texture,
		level
	);
}

static void APIENTRY _gfx_gles_named_framebuffer_texture_2d(

		GLuint  framebuffer,
		GLenum  attach,
		GLenum  textarget,
		GLuint  texture,
		GLint   level)
{
	if((GFX_EXT)->fbos[0] != framebuffer)
	{
		(GFX_EXT)->fbos[0] = framebuffer;
		(GFX_EXT)->BindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	}

	(GFX_EXT)->FramebufferTexture2D(
		GL_DRAW_FRAMEBUFFER,
		attach,
		textarget,
		texture,
		level
	);
}

static void APIENTRY _gfx_gles_polygon_mode(

		GLenum  face,
		GLenum  mode)
{
	/* Just ignore the call */
}

static void APIENTRY _gfx_gles_tex_buffer(

		GLenum  target,
		GLenum  internalFormat,
		GLuint  buffer)
{
	_gfx_gles_error_tex_buffer();
}

static void APIENTRY _gfx_gles_tex_image_1d(

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

static void APIENTRY _gfx_gles_tex_image_2d_multisample(

		GLenum     target,
		GLsizei    samples,
		GLenum     internalformat,
		GLsizei    w,
		GLsizei    h,
		GLboolean  f)
{
	_gfx_gles_error_multisample_tex();
}

static void APIENTRY _gfx_gles_tex_image_3d_multisample(

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

static void APIENTRY _gfx_gles_tex_storage_1d(

		GLenum   target,
		GLsizei  levels,
		GLenum   internalFormat,
		GLsizei  w)
{
	_gfx_gles_error_tex_1d();
}

static void APIENTRY _gfx_gles_tex_storage_2d_multisample(

		GLenum     target,
		GLsizei    samples,
		GLenum     internalFormat,
		GLsizei    w,
		GLsizei    h,
		GLboolean  f)
{
	_gfx_gles_error_multisample_tex();
}

static void APIENTRY _gfx_gles_tex_storage_3d_multisample(

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

static void APIENTRY _gfx_gles_tex_sub_image_1d(

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


#else

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

static void APIENTRY _gfx_gl_named_framebuffer_texture_1d(

		GLuint  framebuffer,
		GLenum  attach,
		GLenum  textarget,
		GLuint  texture,
		GLint   level)
{
	(GFX_EXT)->NamedFramebufferTexture(framebuffer, attach, texture, level);
}

static void APIENTRY _gfx_gl_named_framebuffer_texture_2d(

		GLuint  framebuffer,
		GLenum  attach,
		GLenum  textarget,
		GLuint  texture,
		GLint   level)
{
	(GFX_EXT)->NamedFramebufferTexture(framebuffer, attach, texture, level);
}

static void APIENTRY _gfx_gl_get_program_binary(

		GLuint    program,
		GLsizei   bufsize,
		GLsizei*  length,
		GLenum*   binaryFormat,
		void*     binary)
{
	if(length) *length = 0;

	_gfx_gl_error_program_binary();
}

static void APIENTRY _gfx_gl_program_binary(

		GLuint       program,
		GLenum       binaryFormat,
		const void*  binary,
		GLsizei      length)
{
	_gfx_gl_error_program_binary();
}

static void APIENTRY _gfx_gl_program_parameter_i(

		GLuint  program,
		GLenum  pname,
		GLint   value)
{
	_gfx_gl_error_program_binary();
}

static void APIENTRY _gfx_gl_tex_storage_1d(

		GLenum   target,
		GLsizei  levels,
		GLenum   internalFormat,
		GLsizei  w)
{
	GLsizei l;
	for(l = 0; l < levels; ++l)
	{
		(GFX_EXT)->TexImage1D(target, l, internalFormat, w, 0, GL_RED, GL_BYTE, NULL);
		w >>= (w > 1) ? 1 : 0;
	}
}

static void APIENTRY _gfx_gl_tex_storage_2d(

		GLenum   target,
		GLsizei  levels,
		GLenum   internalFormat,
		GLsizei  w,
		GLsizei  h)
{
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
			(GFX_EXT)->TexImage2D(face, l, internalFormat, w, h, 0, GL_RED, GL_BYTE, NULL);

		w >>= (w > 1) ? 1 : 0;
		h >>= (h > 1) ? hf : 0;
	}
}

static void APIENTRY _gfx_gl_tex_storage_2d_multisample(

		GLenum     target,
		GLsizei    samples,
		GLenum     internalFormat,
		GLsizei    w,
		GLsizei    h,
		GLboolean  f)
{
	(GFX_EXT)->TexImage2DMultisample(target, samples, internalFormat, w, h, f);
}

static void APIENTRY _gfx_gl_tex_storage_3d(

		GLenum   target,
		GLsizei  levels,
		GLenum   internalFormat,
		GLsizei  w,
		GLsizei  h,
		GLsizei  d)
{
	GLsizei df =
		(target == GL_TEXTURE_3D || target == GL_PROXY_TEXTURE_3D) ? 1 : 0;

	GLsizei l;
	for(l = 0; l < levels; ++l)
	{
		(GFX_EXT)->TexImage3D(target, l, internalFormat, w, h, d, 0, GL_RED, GL_BYTE, NULL);
		w >>= (w > 1) ? 1 : 0;
		h >>= (h > 1) ? 1 : 0;
		d >>= (d > 1) ? df : 0;
	}
}

static void APIENTRY _gfx_gl_tex_storage_3d_multisample(

		GLenum     target,
		GLsizei    samples,
		GLenum     internalFormat,
		GLsizei    w,
		GLsizei    h,
		GLsizei    d,
		GLboolean  f)
{
	(GFX_EXT)->TexImage3DMultisample(target, samples, internalFormat, w, h, d, f);
}

static void APIENTRY _gfx_gl_vertex_attrib_divisor(

		GLuint  index,
		GLuint  divisor)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_INSTANCED_ATTRIBUTES is incompatible with this context."
	);
}

#endif


/******************************************************/
void _gfx_extensions_load(void)
{
	/* Get current window and context */
	GFX_Window* window = _gfx_window_get_current();
	if(!window) return;

	/* Get OpenGL version */
	_gfx_platform_context_get(
		&window->context.major,
		&window->context.minor
	);

	/* Get OpenGL constants (a.k.a hardware limits) */
	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS,
		(GFX_EXT)->limits + GFX_LIM_MAX_BUFFER_PROPERTIES);
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS,
		(GFX_EXT)->limits + GFX_LIM_MAX_COLOR_ATTACHMENTS);
	glGetIntegerv(GL_MAX_DRAW_BUFFERS,
		(GFX_EXT)->limits + GFX_LIM_MAX_COLOR_TARGETS);
	glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE,
		(GFX_EXT)->limits + GFX_LIM_MAX_CUBEMAP_SIZE);
	glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS,
		(GFX_EXT)->limits + GFX_LIM_MAX_FEEDBACK_BUFFERS);
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
		(GFX_EXT)->limits + GFX_LIM_MAX_SAMPLER_PROPERTIES);
	glGetIntegerv(GL_MAX_SAMPLES,
		(GFX_EXT)->limits + GFX_LIM_MAX_SAMPLES);
	glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE,
		(GFX_EXT)->limits + GFX_LIM_MAX_TEXTURE_3D_SIZE);
	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS,
		(GFX_EXT)->limits + GFX_LIM_MAX_TEXTURE_LAYERS);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE,
		(GFX_EXT)->limits + GFX_LIM_MAX_TEXTURE_SIZE);
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS,
		(GFX_EXT)->limits + GFX_LIM_MAX_VERTEX_ATTRIBS);

#ifdef GFX_GLES

	/* Get OpenGL constants (a.k.a hardware limits) */
	(GFX_EXT)->limits[GFX_LIM_MAX_BUFFER_TEXTURE_SIZE] = 0;
	(GFX_EXT)->limits[GFX_LIM_MAX_PATCH_VERTICES] = 0;

	/* Default Extensions */
	(GFX_EXT)->flags[GFX_EXT_BUFFER_TEXTURE]              = 0;
	(GFX_EXT)->flags[GFX_EXT_DIRECT_STATE_ACCESS]         = 0;
	(GFX_EXT)->flags[GFX_EXT_GEOMETRY_SHADER]             = 0;
	(GFX_EXT)->flags[GFX_EXT_IMMUTABLE_TEXTURE]           = 1;
	(GFX_EXT)->flags[GFX_EXT_INSTANCED_ATTRIBUTES]        = 1;
	(GFX_EXT)->flags[GFX_EXT_INSTANCED_BASE_ATTRIBUTES]   = 0;
	(GFX_EXT)->flags[GFX_EXT_LAYERED_CUBEMAP]             = 0;
	(GFX_EXT)->flags[GFX_EXT_LAYERED_MULTISAMPLE_TEXTURE] = 0;
	(GFX_EXT)->flags[GFX_EXT_POLYGON_STATE]               = 0;
	(GFX_EXT)->flags[GFX_EXT_PROGRAM_BINARY]              = 1;
	(GFX_EXT)->flags[GFX_EXT_SEAMLESS_CUBEMAP]            = 0;
	(GFX_EXT)->flags[GFX_EXT_TESSELLATION_SHADER]         = 0;
	(GFX_EXT)->flags[GFX_EXT_TEXTURE_1D]                  = 0;

	/* GLES, assumes 3.0+ */
	(GFX_EXT)->ActiveTexture                     = glActiveTexture;
	(GFX_EXT)->AttachShader                      = glAttachShader;
	(GFX_EXT)->BeginTransformFeedback            = glBeginTransformFeedback;
	(GFX_EXT)->BindAttribLocation                = glBindAttribLocation;
	(GFX_EXT)->BindBuffer                        = glBindBuffer;
	(GFX_EXT)->BindBufferRange                   = glBindBufferRange;
	(GFX_EXT)->BindFramebuffer                   = glBindFramebuffer;
	(GFX_EXT)->BindTexture                       = glBindTexture;
	(GFX_EXT)->BindTextureUnit                   = glBindTextureUnit;
	(GFX_EXT)->BindVertexArray                   = glBindVertexArray;
	(GFX_EXT)->BlendEquationSeparate             = glBlendEquationSeparate;
	(GFX_EXT)->BlendFuncSeparate                 = glBlendFuncSeparate;
	(GFX_EXT)->BufferData                        = glBufferData;
	(GFX_EXT)->BufferSubData                     = glBufferSubData;
	(GFX_EXT)->Clear                             = glClear;
	(GFX_EXT)->CompileShader                     = glCompileShader;
	(GFX_EXT)->CopyBufferSubData                 = glCopyBufferSubData;
	(GFX_EXT)->CopyNamedBufferSubData            = _gfx_gl_copy_named_buffer_sub_data;
	(GFX_EXT)->CreateBuffers                     = _gfx_gl_create_buffers;
	(GFX_EXT)->CreateFramebuffers                = _gfx_gl_create_framebuffers;
	(GFX_EXT)->CreateProgram                     = glCreateProgram;
	(GFX_EXT)->CreateShader                      = glCreateShader;
	(GFX_EXT)->CreateTextures                    = _gfx_gl_create_textures;
	(GFX_EXT)->CreateVertexArrays                = _gfx_gl_create_vertex_arrays;
	(GFX_EXT)->CullFace                          = glCullFace;
	(GFX_EXT)->DeleteBuffers                     = glDeleteBuffers;
	(GFX_EXT)->DeleteFramebuffers                = glDeleteFramebuffers;
	(GFX_EXT)->DeleteProgram                     = glDeleteProgram;
	(GFX_EXT)->DeleteShader                      = glDeleteShader;
	(GFX_EXT)->DeleteTextures                    = glDeleteTextures;
	(GFX_EXT)->DeleteVertexArrays                = glDeleteVertexArrays;
	(GFX_EXT)->DepthFunc                         = glDepthFunc;
	(GFX_EXT)->DepthMask                         = glDepthMask;
	(GFX_EXT)->DetachShader                      = glDetachShader;
	(GFX_EXT)->Disable                           = glDisable;
	(GFX_EXT)->DisableVertexArrayAttrib          = _gfx_gl_disable_vertex_array_attrib;
	(GFX_EXT)->DisableVertexAttribArray          = glDisableVertexAttribArray;
	(GFX_EXT)->DrawArrays                        = glDrawArrays;
	(GFX_EXT)->DrawArraysInstanced               = glDrawArraysInstanced;
	(GFX_EXT)->DrawArraysInstancedBaseInstance   = _gfx_gl_draw_arrays_instanced_base_instance;
	(GFX_EXT)->DrawBuffers                       = glDrawBuffers;
	(GFX_EXT)->DrawElements                      = glDrawElements;
	(GFX_EXT)->DrawElementsInstanced             = glDrawElementsInstanced;
	(GFX_EXT)->DrawElementsInstancedBaseInstance = _gfx_gl_draw_elements_instanced_base_instance;
	(GFX_EXT)->Enable                            = glEnable;
	(GFX_EXT)->EnableVertexArrayAttrib           = _gfx_gl_enable_vertex_array_attrib;
	(GFX_EXT)->EnableVertexAttribArray           = glEnableVertexAttribArray;
	(GFX_EXT)->EndTransformFeedback              = glEndTransformFeedback;
	(GFX_EXT)->FramebufferTexture                = _gfx_gles_framebuffer_texture;
	(GFX_EXT)->FramebufferTexture1D              = _gfx_gles_framebuffer_texture_1d;
	(GFX_EXT)->FramebufferTexture2D              = glFramebufferTexture2D;
	(GFX_EXT)->FramebufferTextureLayer           = glFramebufferTextureLayer;
	(GFX_EXT)->GenBuffers                        = glGenBuffers;
	(GFX_EXT)->GenerateMipmap                    = glGenerateMipmap;
	(GFX_EXT)->GenerateTextureMipmap             = glGenerateTextureMipmap;
	(GFX_EXT)->GenFramebuffers                   = glGenFramebuffers;
	(GFX_EXT)->GenTextures                       = glGenTextures;
	(GFX_EXT)->GenVertexArrays                   = glGenVertexArrays;
	(GFX_EXT)->GetActiveUniform                  = glGetActiveUniform;
	(GFX_EXT)->GetActiveUniformBlockiv           = glGetActiveUniformBlockiv;
	(GFX_EXT)->GetActiveUniformsiv               = glGetActiveUniformsiv;
	(GFX_EXT)->GetBufferSubData                  = _gfx_gles_get_buffer_sub_data;
	(GFX_EXT)->GetError                          = glGetError;
	(GFX_EXT)->GetNamedBufferSubData             = _gfx_gl_get_named_buffer_sub_data;
	(GFX_EXT)->GetProgramBinary                  = glGetProgramBinary;
	(GFX_EXT)->GetProgramInfoLog                 = glGetProgramInfoLog;
	(GFX_EXT)->GetProgramiv                      = glGetProgramiv;
	(GFX_EXT)->GetShaderInfoLog                  = glGetShaderInfoLog;
	(GFX_EXT)->GetShaderiv                       = glGetShaderiv;
	(GFX_EXT)->GetShaderSource                   = glGetShaderSource;
	(GFX_EXT)->GetStringi                        = glGetStringi;
	(GFX_EXT)->GetUniformBlockIndex              = glGetUniformBlockIndex;
	(GFX_EXT)->GetUniformIndices                 = glGetUniformIndices;
	(GFX_EXT)->GetUniformLocation                = glGetUniformLocation;
	(GFX_EXT)->LinkProgram                       = glLinkProgram;
	(GFX_EXT)->MapBufferRange                    = glMapBufferRange;
	(GFX_EXT)->MapNamedBufferRange               = glMapNamedBufferRange;
	(GFX_EXT)->NamedBufferData                   = _gfx_gl_named_buffer_data;
	(GFX_EXT)->NamedBufferSubData                = _gfx_gl_named_buffer_sub_data;
	(GFX_EXT)->NamedFramebufferDrawBuffers       = _gfx_gl_named_framebuffer_draw_buffers;
	(GFX_EXT)->NamedFramebufferTexture           = _gfx_gl_named_framebuffer_texture;
	(GFX_EXT)->NamedFramebufferTexture1D         = _gfx_gles_named_framebuffer_texture_1d;
	(GFX_EXT)->NamedFramebufferTexture2D         = _gfx_gles_named_framebuffer_texture_2d;
	(GFX_EXT)->NamedFramebufferTextureLayer      = _gfx_gl_named_framebuffer_texture_layer;
	(GFX_EXT)->PatchParameteri                   = _gfx_gl_patch_parameter_i;
	(GFX_EXT)->PixelStorei                       = glPixelStorei;
	(GFX_EXT)->PolygonMode                       = _gfx_gles_polygon_mode;
	(GFX_EXT)->ProgramBinary                     = glProgramBinary;
	(GFX_EXT)->ProgramParameteri                 = glProgramParameteri;
	(GFX_EXT)->ShaderSource                      = glShaderSource;
	(GFX_EXT)->StencilFuncSeparate               = glStencilFuncSeparate;
	(GFX_EXT)->StencilOpSeparate                 = glStencilOpSeparate;
	(GFX_EXT)->TexBuffer                         = _gfx_gles_tex_buffer;
	(GFX_EXT)->TexImage1D                        = _gfx_gles_tex_image_1d;
	(GFX_EXT)->TexImage2D                        = glTexImage2D;
	(GFX_EXT)->TexImage2DMultisample             = _gfx_gles_tex_image_2d_multisample;
	(GFX_EXT)->TexImage3D                        = glTexImage3D;
	(GFX_EXT)->TexImage3DMultisample             = _gfx_gles_tex_image_3d_multisample;
	(GFX_EXT)->TexParameteri                     = glTexParameteri;
	(GFX_EXT)->TexStorage1D                      = _gfx_gles_tex_storage_1d;
	(GFX_EXT)->TexStorage2D                      = glTexStorage2D;
	(GFX_EXT)->TexStorage3D                      = glTexStorage3D;
	(GFX_EXT)->TexStorage3DMultisample           = _gfx_gles_tex_storage_3d_multisample;
	(GFX_EXT)->TexSubImage1D                     = _gfx_gles_tex_sub_image_1d;
	(GFX_EXT)->TexSubImage2D                     = glTexSubImage2D;
	(GFX_EXT)->TexSubImage3D                     = glTexSubImage3D;
	(GFX_EXT)->TextureBuffer                     = _gfx_gl_texture_buffer;
	(GFX_EXT)->TextureParameteri                 = _gfx_gl_texture_parameter_i;
	(GFX_EXT)->TextureStorage1D                  = _gfx_gl_texture_storage_1d;
	(GFX_EXT)->TextureStorage2D                  = _gfx_gl_texture_storage_2d;
	(GFX_EXT)->TextureStorage2DMultisample       = _gfx_gl_texture_storage_2d_multisample;
	(GFX_EXT)->TextureStorage3D                  = _gfx_gl_texture_storage_3d;
	(GFX_EXT)->TextureStorage3DMultisample       = _gfx_gl_texture_storage_3d_multisample;
	(GFX_EXT)->TextureSubImage1D                 = _gfx_gl_texture_sub_image_1d;
	(GFX_EXT)->TextureSubImage2D                 = _gfx_gl_texture_sub_image_2d;
	(GFX_EXT)->TextureSubImage3D                 = _gfx_gl_texture_sub_image_3d;
	(GFX_EXT)->TransformFeedbackVaryings         = glTransformFeedbackVaryings;
	(GFX_EXT)->Uniform1fv                        = glUniform1fv;
	(GFX_EXT)->Uniform1iv                        = glUniform1iv;
	(GFX_EXT)->Uniform1uiv                       = glUniform1uiv;
	(GFX_EXT)->Uniform2fv                        = glUniform2fv;
	(GFX_EXT)->Uniform2iv                        = glUniform2iv;
	(GFX_EXT)->Uniform2uiv                       = glUniform2uiv;
	(GFX_EXT)->Uniform3fv                        = glUniform3fv;
	(GFX_EXT)->Uniform3iv                        = glUniform3iv;
	(GFX_EXT)->Uniform3uiv                       = glUniform3uiv;
	(GFX_EXT)->Uniform4fv                        = glUniform4fv;
	(GFX_EXT)->Uniform4iv                        = glUniform4iv;
	(GFX_EXT)->Uniform4uiv                       = glUniform4uiv;
	(GFX_EXT)->UniformBlockBinding               = glUniformBlockBinding;
	(GFX_EXT)->UniformMatrix2fv                  = glUniformMatrix2fv;
	(GFX_EXT)->UniformMatrix3fv                  = glUniformMatrix3fv;
	(GFX_EXT)->UniformMatrix4fv                  = glUniformMatrix4fv;
	(GFX_EXT)->UnmapBuffer                       = glUnmapBuffer;
	(GFX_EXT)->UnmapNamedBuffer                  = _gfx_gl_unmap_named_buffer;
	(GFX_EXT)->UseProgram                        = glUseProgram;
	(GFX_EXT)->VertexAttribDivisor               = glVertexAttribDivisor;
	(GFX_EXT)->VertexAttribIPointer              = glVertexAttribIPointer;
	(GFX_EXT)->VertexAttribPointer               = glVertexAttribPointer;
	(GFX_EXT)->Viewport                          = glViewport;

	/* GFX_EXT_IMMUTABLE_MULTISAMPLE_TEXTURE */
	/* GFX_EXT_MULTISAMPLE_TEXTURE */
	if(
		window->context.major > 3 ||
		(window->context.major == 3 && window->context.minor > 0))
	{
		(GFX_EXT)->flags[GFX_EXT_IMMUTABLE_MULTISAMPLE_TEXTURE] = 1;
		(GFX_EXT)->flags[GFX_EXT_MULTISAMPLE_TEXTURE] = 1;
		(GFX_EXT)->TexStorage2DMultisample = glTexStorage2DMultisample;
	}

	else
	{
		(GFX_EXT)->flags[GFX_EXT_IMMUTABLE_MULTISAMPLE_TEXTURE] = 0;
		(GFX_EXT)->flags[GFX_EXT_MULTISAMPLE_TEXTURE] = 0;
		(GFX_EXT)->TexStorage2DMultisample = _gfx_gles_tex_storage_2d_multisample;
	}

#else

	/* Settings */
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	/* Get OpenGL constants (a.k.a hardware limits) */
	glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE,
		(GFX_EXT)->limits + GFX_LIM_MAX_BUFFER_TEXTURE_SIZE);

	/* Default Extensions */
	(GFX_EXT)->flags[GFX_EXT_BUFFER_TEXTURE]              = 1;
	(GFX_EXT)->flags[GFX_EXT_GEOMETRY_SHADER]             = 1;
	(GFX_EXT)->flags[GFX_EXT_LAYERED_MULTISAMPLE_TEXTURE] = 1;
	(GFX_EXT)->flags[GFX_EXT_MULTISAMPLE_TEXTURE]         = 1;
	(GFX_EXT)->flags[GFX_EXT_POLYGON_STATE]               = 1;
	(GFX_EXT)->flags[GFX_EXT_SEAMLESS_CUBEMAP]            = 1;
	(GFX_EXT)->flags[GFX_EXT_TEXTURE_1D]                  = 1;

	/* Core, assumes 3.2+ context */
	(GFX_EXT)->ActiveTexture =
		(PFNGLACTIVETEXTUREPROC)_gfx_platform_get_proc_address("glActiveTexture");
	(GFX_EXT)->AttachShader =
		(PFNGLATTACHSHADERPROC)_gfx_platform_get_proc_address("glAttachShader");
	(GFX_EXT)->BeginTransformFeedback =
		(PFNGLBEGINTRANSFORMFEEDBACKPROC)_gfx_platform_get_proc_address("glBeginTransformFeedback");
	(GFX_EXT)->BindAttribLocation =
		(PFNGLBINDATTRIBLOCATIONPROC)_gfx_platform_get_proc_address("glBindAttribLocation");
	(GFX_EXT)->BindBuffer =
		(PFNGLBINDBUFFERPROC)_gfx_platform_get_proc_address("glBindBuffer");
	(GFX_EXT)->BindBufferRange =
		(PFNGLBINDBUFFERRANGEPROC)_gfx_platform_get_proc_address("glBindBufferRange");
	(GFX_EXT)->BindFramebuffer =
		(PFNGLBINDFRAMEBUFFERPROC)_gfx_platform_get_proc_address("glBindFramebuffer");
	(GFX_EXT)->BindTexture =
		(PFNGLBINDTEXTUREPROC)glBindTexture;
	(GFX_EXT)->BindVertexArray =
		(PFNGLBINDVERTEXARRAYPROC)_gfx_platform_get_proc_address("glBindVertexArray");
	(GFX_EXT)->BlendEquationSeparate =
		(PFNGLBLENDEQUATIONSEPARATEPROC)_gfx_platform_get_proc_address("glBlendEquationSeparate");
	(GFX_EXT)->BlendFuncSeparate =
		(PFNGLBLENDFUNCSEPARATEPROC)_gfx_platform_get_proc_address("glBlendFuncSeparate");
	(GFX_EXT)->BufferData =
		(PFNGLBUFFERDATAPROC)_gfx_platform_get_proc_address("glBufferData");
	(GFX_EXT)->BufferSubData =
		(PFNGLBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glBufferSubData");
	(GFX_EXT)->Clear =
		(PFNGLCLEARPROC)glClear;
	(GFX_EXT)->CompileShader =
		(PFNGLCOMPILESHADERPROC)_gfx_platform_get_proc_address("glCompileShader");
	(GFX_EXT)->CopyBufferSubData =
		(PFNGLCOPYBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glCopyBufferSubData");
	(GFX_EXT)->CreateProgram =
		(PFNGLCREATEPROGRAMPROC)_gfx_platform_get_proc_address("glCreateProgram");
	(GFX_EXT)->CreateShader =
		(PFNGLCREATESHADERPROC)_gfx_platform_get_proc_address("glCreateShader");
	(GFX_EXT)->CullFace =
		(PFNGLCULLFACEPROC)glCullFace;
	(GFX_EXT)->DeleteBuffers =
		(PFNGLDELETEBUFFERSPROC)_gfx_platform_get_proc_address("glDeleteBuffers");
	(GFX_EXT)->DeleteFramebuffers =
		(PFNGLDELETEFRAMEBUFFERSPROC)_gfx_platform_get_proc_address("glDeleteFramebuffers");
	(GFX_EXT)->DeleteProgram =
		(PFNGLDELETEPROGRAMPROC)_gfx_platform_get_proc_address("glDeleteProgram");
	(GFX_EXT)->DeleteShader =
		(PFNGLDELETESHADERPROC)_gfx_platform_get_proc_address("glDeleteShader");
	(GFX_EXT)->DeleteTextures =
		(PFNGLDELETETEXTURESPROC)glDeleteTextures;
	(GFX_EXT)->DeleteVertexArrays =
		(PFNGLDELETEVERTEXARRAYSPROC)_gfx_platform_get_proc_address("glDeleteVertexArrays");
	(GFX_EXT)->DepthFunc =
		(PFNGLDEPTHFUNCPROC)glDepthFunc;
	(GFX_EXT)->DepthMask =
		(PFNGLDEPTHMASKPROC)glDepthMask;
	(GFX_EXT)->DetachShader =
		(PFNGLDETACHSHADERPROC)_gfx_platform_get_proc_address("glDetachShader");
	(GFX_EXT)->Disable =
		(PFNGLDISABLEPROC)glDisable;
	(GFX_EXT)->DisableVertexAttribArray =
		(PFNGLDISABLEVERTEXATTRIBARRAYPROC)_gfx_platform_get_proc_address("glDisableVertexAttribArray");
	(GFX_EXT)->DrawArrays =
		(PFNGLDRAWARRAYSPROC)glDrawArrays;
	(GFX_EXT)->DrawArraysInstanced =
		(PFNGLDRAWARRAYSINSTANCEDPROC)_gfx_platform_get_proc_address("glDrawArraysInstanced");
	(GFX_EXT)->DrawBuffers =
		(PFNGLDRAWBUFFERSPROC)_gfx_platform_get_proc_address("glDrawBuffers");
	(GFX_EXT)->DrawElements =
		(PFNGLDRAWELEMENTSPROC)glDrawElements;
	(GFX_EXT)->DrawElementsInstanced =
		(PFNGLDRAWELEMENTSINSTANCEDPROC)_gfx_platform_get_proc_address("glDrawElementsInstanced");
	(GFX_EXT)->Enable =
		(PFNGLENABLEPROC)glEnable;
	(GFX_EXT)->EnableVertexAttribArray =
		(PFNGLENABLEVERTEXATTRIBARRAYPROC)_gfx_platform_get_proc_address("glEnableVertexAttribArray");
	(GFX_EXT)->EndTransformFeedback =
		(PFNGLENDTRANSFORMFEEDBACKPROC)_gfx_platform_get_proc_address("glEndTransformFeedback");
	(GFX_EXT)->FramebufferTexture =
		(PFNGLFRAMEBUFFERTEXTUREPROC)_gfx_platform_get_proc_address("glFramebufferTexture");
	(GFX_EXT)->FramebufferTexture1D =
		(PFNGLFRAMEBUFFERTEXTURE1DPROC)_gfx_platform_get_proc_address("glFramebufferTexture1D");
	(GFX_EXT)->FramebufferTexture2D =
		(PFNGLFRAMEBUFFERTEXTURE2DPROC)_gfx_platform_get_proc_address("glFramebufferTexture2D");
	(GFX_EXT)->FramebufferTextureLayer =
		(PFNGLFRAMEBUFFERTEXTURELAYERPROC)_gfx_platform_get_proc_address("glFramebufferTextureLayer");
	(GFX_EXT)->GenBuffers =
		(PFNGLGENBUFFERSPROC)_gfx_platform_get_proc_address("glGenBuffers");
	(GFX_EXT)->GenerateMipmap =
		(PFNGLGENERATEMIPMAPPROC)_gfx_platform_get_proc_address("glGenerateMipmap");
	(GFX_EXT)->GenFramebuffers =
		(PFNGLGENFRAMEBUFFERSPROC)_gfx_platform_get_proc_address("glGenFramebuffers");
	(GFX_EXT)->GenTextures =
		(PFNGLGENTEXTURESPROC)glGenTextures;
	(GFX_EXT)->GenVertexArrays =
		(PFNGLGENVERTEXARRAYSPROC)_gfx_platform_get_proc_address("glGenVertexArrays");
	(GFX_EXT)->GetActiveUniform =
		(PFNGLGETACTIVEUNIFORMPROC)_gfx_platform_get_proc_address("glGetActiveUniform");
	(GFX_EXT)->GetActiveUniformBlockiv =
		(PFNGLGETACTIVEUNIFORMBLOCKIVPROC)_gfx_platform_get_proc_address("glGetActiveUniformBlockiv");
	(GFX_EXT)->GetActiveUniformsiv =
		(PFNGLGETACTIVEUNIFORMSIVPROC)_gfx_platform_get_proc_address("glGetActiveUniformsiv");
	(GFX_EXT)->GetBufferSubData =
		(PFNGLGETBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glGetBufferSubData");
	(GFX_EXT)->GetError =
		(PFNGLGETERRORPROC)glGetError;
	(GFX_EXT)->GetProgramInfoLog =
		(PFNGLGETPROGRAMINFOLOGPROC)_gfx_platform_get_proc_address("glGetProgramInfoLog");
	(GFX_EXT)->GetProgramiv =
		(PFNGLGETPROGRAMIVPROC)_gfx_platform_get_proc_address("glGetProgramiv");
	(GFX_EXT)->GetShaderInfoLog =
		(PFNGLGETSHADERINFOLOGPROC)_gfx_platform_get_proc_address("glGetShaderInfoLog");
	(GFX_EXT)->GetShaderiv =
		(PFNGLGETSHADERIVPROC)_gfx_platform_get_proc_address("glGetShaderiv");
	(GFX_EXT)->GetShaderSource =
		(PFNGLGETSHADERSOURCEPROC)_gfx_platform_get_proc_address("glGetShaderSource");
	(GFX_EXT)->GetStringi =
		(PFNGLGETSTRINGIPROC)_gfx_platform_get_proc_address("glGetStringi");
	(GFX_EXT)->GetUniformBlockIndex =
		(PFNGLGETUNIFORMBLOCKINDEXPROC)_gfx_platform_get_proc_address("glGetUniformBlockIndex");
	(GFX_EXT)->GetUniformIndices =
		(PFNGLGETUNIFORMINDICESPROC)_gfx_platform_get_proc_address("glGetUniformIndices");
	(GFX_EXT)->GetUniformLocation =
		(PFNGLGETUNIFORMLOCATIONPROC)_gfx_platform_get_proc_address("glGetUniformLocation");
	(GFX_EXT)->LinkProgram =
		(PFNGLLINKPROGRAMPROC)_gfx_platform_get_proc_address("glLinkProgram");
	(GFX_EXT)->MapBufferRange =
		(PFNGLMAPBUFFERRANGEPROC)_gfx_platform_get_proc_address("glMapBufferRange");
	(GFX_EXT)->NamedFramebufferTexture1D =
		(GFX_NAMEDFRAMEBUFFERTEXTURE1DPROC)_gfx_gl_named_framebuffer_texture_1d;
	(GFX_EXT)->NamedFramebufferTexture2D =
		(GFX_NAMEDFRAMEBUFFERTEXTURE2DPROC)_gfx_gl_named_framebuffer_texture_2d;
	(GFX_EXT)->PixelStorei =
		(PFNGLPIXELSTOREIPROC)glPixelStorei;
	(GFX_EXT)->PolygonMode =
		(PFNGLPOLYGONMODEPROC)glPolygonMode;
	(GFX_EXT)->ShaderSource =
		(PFNGLSHADERSOURCEPROC)_gfx_platform_get_proc_address("glShaderSource");
	(GFX_EXT)->StencilFuncSeparate =
		(PFNGLSTENCILFUNCSEPARATEPROC)_gfx_platform_get_proc_address("glStencilFuncSeparate");
	(GFX_EXT)->StencilOpSeparate =
		(PFNGLSTENCILOPSEPARATEPROC)_gfx_platform_get_proc_address("glStencilOpSeparate");
	(GFX_EXT)->TexBuffer =
		(PFNGLTEXBUFFERPROC)_gfx_platform_get_proc_address("glTexBuffer");
	(GFX_EXT)->TexImage1D =
		(PFNGLTEXIMAGE1DPROC)glTexImage1D;
	(GFX_EXT)->TexImage2D =
		(PFNGLTEXIMAGE2DPROC)glTexImage2D;
	(GFX_EXT)->TexImage2DMultisample =
		(PFNGLTEXIMAGE2DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTexImage2DMultisample");
	(GFX_EXT)->TexImage3D =
		(PFNGLTEXIMAGE3DPROC)_gfx_platform_get_proc_address("glTexImage3D");
	(GFX_EXT)->TexImage3DMultisample =
		(PFNGLTEXIMAGE3DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTexImage3DMultisample");
	(GFX_EXT)->TexParameteri =
		(PFNGLTEXPARAMETERIPROC)glTexParameteri;
	(GFX_EXT)->TexSubImage1D =
		(PFNGLTEXSUBIMAGE1DPROC)glTexSubImage1D;
	(GFX_EXT)->TexSubImage2D =
		(PFNGLTEXSUBIMAGE2DPROC)glTexSubImage2D;
	(GFX_EXT)->TexSubImage3D =
		(PFNGLTEXSUBIMAGE3DPROC)_gfx_platform_get_proc_address("glTexSubImage3D");
	(GFX_EXT)->TransformFeedbackVaryings =
		(PFNGLTRANSFORMFEEDBACKVARYINGSPROC)_gfx_platform_get_proc_address("glTransformFeedbackVaryings");
	(GFX_EXT)->Uniform1fv =
		(PFNGLUNIFORM1FVPROC)_gfx_platform_get_proc_address("glUniform1fv");
	(GFX_EXT)->Uniform1iv =
		(PFNGLUNIFORM1IVPROC)_gfx_platform_get_proc_address("glUniform1iv");
	(GFX_EXT)->Uniform1uiv =
		(PFNGLUNIFORM1UIVPROC)_gfx_platform_get_proc_address("glUniform1uiv");
	(GFX_EXT)->Uniform2fv =
		(PFNGLUNIFORM2FVPROC)_gfx_platform_get_proc_address("glUniform2fv");
	(GFX_EXT)->Uniform2iv =
		(PFNGLUNIFORM2IVPROC)_gfx_platform_get_proc_address("glUniform2iv");
	(GFX_EXT)->Uniform2uiv =
		(PFNGLUNIFORM2UIVPROC)_gfx_platform_get_proc_address("glUniform2uiv");
	(GFX_EXT)->Uniform3fv =
		(PFNGLUNIFORM3FVPROC)_gfx_platform_get_proc_address("glUniform3fv");
	(GFX_EXT)->Uniform3iv =
		(PFNGLUNIFORM3IVPROC)_gfx_platform_get_proc_address("glUniform3iv");
	(GFX_EXT)->Uniform3uiv =
		(PFNGLUNIFORM3UIVPROC)_gfx_platform_get_proc_address("glUniform3uiv");
	(GFX_EXT)->Uniform4fv =
		(PFNGLUNIFORM4FVPROC)_gfx_platform_get_proc_address("glUniform4fv");
	(GFX_EXT)->Uniform4iv =
		(PFNGLUNIFORM4IVPROC)_gfx_platform_get_proc_address("glUniform4iv");
	(GFX_EXT)->Uniform4uiv =
		(PFNGLUNIFORM4UIVPROC)_gfx_platform_get_proc_address("glUniform4uiv");
	(GFX_EXT)->UniformBlockBinding =
		(PFNGLUNIFORMBLOCKBINDINGPROC)_gfx_platform_get_proc_address("glUniformBlockBinding");
	(GFX_EXT)->UniformMatrix2fv =
		(PFNGLUNIFORMMATRIX2FVPROC)_gfx_platform_get_proc_address("glUniformMatrix2fv");
	(GFX_EXT)->UniformMatrix3fv =
		(PFNGLUNIFORMMATRIX3FVPROC)_gfx_platform_get_proc_address("glUniformMatrix3fv");
	(GFX_EXT)->UniformMatrix4fv =
		(PFNGLUNIFORMMATRIX4FVPROC)_gfx_platform_get_proc_address("glUniformMatrix4fv");
	(GFX_EXT)->UnmapBuffer =
		(PFNGLUNMAPBUFFERPROC)_gfx_platform_get_proc_address("glUnmapBuffer");
	(GFX_EXT)->UseProgram =
		(PFNGLUSEPROGRAMPROC)_gfx_platform_get_proc_address("glUseProgram");
	(GFX_EXT)->VertexAttribIPointer =
		(PFNGLVERTEXATTRIBIPOINTERPROC)_gfx_platform_get_proc_address("glVertexAttribIPointer");
	(GFX_EXT)->VertexAttribPointer =
		(PFNGLVERTEXATTRIBPOINTERPROC)_gfx_platform_get_proc_address("glVertexAttribPointer");
	(GFX_EXT)->Viewport =
		(PFNGLVIEWPORTPROC)glViewport;

	/* GFX_EXT_DIRECT_STATE_ACCESS */
	if(
		window->context.major > 4 ||
		(window->context.major == 4 && window->context.minor > 4) ||
		_gfx_is_extension_supported("GL_ARB_direct_state_access"))
	{
		(GFX_EXT)->flags[GFX_EXT_DIRECT_STATE_ACCESS] = 1;
		(GFX_EXT)->BindTextureUnit =
			(PFNGLBINDTEXTUREUNITPROC)_gfx_platform_get_proc_address("glBindTextureUnit");
		(GFX_EXT)->CopyNamedBufferSubData =
			(PFNGLCOPYNAMEDBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glCopyNamedBufferSubData");
		(GFX_EXT)->CreateBuffers =
			(PFNGLCREATEBUFFERSPROC)_gfx_platform_get_proc_address("glCreateBuffers");
		(GFX_EXT)->CreateFramebuffers =
			(PFNGLCREATEFRAMEBUFFERSPROC)_gfx_platform_get_proc_address("glCreateFramebuffers");
		(GFX_EXT)->CreateTextures =
			(PFNGLCREATETEXTURESPROC)_gfx_platform_get_proc_address("glCreateTextures");
		(GFX_EXT)->CreateVertexArrays =
			(PFNGLCREATEVERTEXARRAYSPROC)_gfx_platform_get_proc_address("glCreateVertexArrays");
		(GFX_EXT)->DisableVertexArrayAttrib =
			(PFNGLDISABLEVERTEXARRAYATTRIBPROC)_gfx_platform_get_proc_address("glDisableVertexArrayAttrib");
		(GFX_EXT)->EnableVertexArrayAttrib =
			(PFNGLENABLEVERTEXARRAYATTRIBPROC)_gfx_platform_get_proc_address("glEnableVertexArrayAttrib");
		(GFX_EXT)->GenerateTextureMipmap =
			(PFNGLGENERATETEXTUREMIPMAPPROC)_gfx_platform_get_proc_address("glGenerateTextureMipmap");
		(GFX_EXT)->GetNamedBufferSubData =
			(PFNGLGETNAMEDBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glGetNamedBufferSubData");
		(GFX_EXT)->MapNamedBufferRange =
			(PFNGLMAPNAMEDBUFFERRANGEPROC)_gfx_platform_get_proc_address("glMapNamedBufferRange");
		(GFX_EXT)->NamedBufferData =
			(PFNGLNAMEDBUFFERDATAPROC)_gfx_platform_get_proc_address("glNamedBufferData");
		(GFX_EXT)->NamedBufferSubData =
			(PFNGLNAMEDBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glNamedBufferSubData");
		(GFX_EXT)->NamedFramebufferDrawBuffers =
			(PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC)_gfx_platform_get_proc_address("glNamedFramebufferDrawBuffers");
		(GFX_EXT)->NamedFramebufferTexture =
			(PFNGLNAMEDFRAMEBUFFERTEXTUREPROC)_gfx_platform_get_proc_address("glNamedFramebufferTexture");
		(GFX_EXT)->NamedFramebufferTextureLayer =
			(PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC)_gfx_platform_get_proc_address("glNamedFramebufferTextureLayer");
		(GFX_EXT)->TextureBuffer =
			(PFNGLTEXTUREBUFFERPROC)_gfx_platform_get_proc_address("glTextureBuffer");
		(GFX_EXT)->TextureParameteri =
			(PFNGLTEXTUREPARAMETERIPROC)_gfx_platform_get_proc_address("glTextureParameteri");
		(GFX_EXT)->TextureStorage1D =
			(PFNGLTEXTURESTORAGE1DPROC)_gfx_platform_get_proc_address("glTextureStorage1D");
		(GFX_EXT)->TextureStorage2D =
			(PFNGLTEXTURESTORAGE2DPROC)_gfx_platform_get_proc_address("glTextureStorage2D");
		(GFX_EXT)->TextureStorage2DMultisample =
			(PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTextureStorage2DMultisample");
		(GFX_EXT)->TextureStorage3D =
			(PFNGLTEXTURESTORAGE3DPROC)_gfx_platform_get_proc_address("glTextureStorage3D");
		(GFX_EXT)->TextureStorage3DMultisample =
			(PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTextureStorage3DMultisample");
		(GFX_EXT)->TextureSubImage1D =
			(PFNGLTEXTURESUBIMAGE1DPROC)_gfx_platform_get_proc_address("glTextureSubImage1D");
		(GFX_EXT)->TextureSubImage2D =
			(PFNGLTEXTURESUBIMAGE2DPROC)_gfx_platform_get_proc_address("glTextureSubImage2D");
		(GFX_EXT)->TextureSubImage3D =
			(PFNGLTEXTURESUBIMAGE3DPROC)_gfx_platform_get_proc_address("glTextureSubImage3D");
		(GFX_EXT)->UnmapNamedBuffer =
			(PFNGLUNMAPNAMEDBUFFERPROC)_gfx_platform_get_proc_address("glUnmapNamedBuffer");
	}

	else
	{
		(GFX_EXT)->flags[GFX_EXT_DIRECT_STATE_ACCESS] = 0;
		(GFX_EXT)->BindTextureUnit              = _gfx_gl_bind_texture_unit;
		(GFX_EXT)->CopyNamedBufferSubData       = _gfx_gl_copy_named_buffer_sub_data;
		(GFX_EXT)->CreateBuffers                = _gfx_gl_create_buffers;
		(GFX_EXT)->CreateFramebuffers           = _gfx_gl_create_framebuffers;
		(GFX_EXT)->CreateTextures               = _gfx_gl_create_textures;
		(GFX_EXT)->CreateVertexArrays           = _gfx_gl_create_vertex_arrays;
		(GFX_EXT)->DisableVertexArrayAttrib     = _gfx_gl_disable_vertex_array_attrib;
		(GFX_EXT)->EnableVertexArrayAttrib      = _gfx_gl_enable_vertex_array_attrib;
		(GFX_EXT)->GenerateTextureMipmap        = _gfx_gl_generate_texture_mipmap;
		(GFX_EXT)->GetNamedBufferSubData        = _gfx_gl_get_named_buffer_sub_data;
		(GFX_EXT)->MapNamedBufferRange          = _gfx_gl_map_named_buffer_range;
		(GFX_EXT)->NamedBufferData              = _gfx_gl_named_buffer_data;
		(GFX_EXT)->NamedBufferSubData           = _gfx_gl_named_buffer_sub_data;
		(GFX_EXT)->NamedFramebufferDrawBuffers  = _gfx_gl_named_framebuffer_draw_buffers;
		(GFX_EXT)->NamedFramebufferTexture      = _gfx_gl_named_framebuffer_texture;
		(GFX_EXT)->NamedFramebufferTextureLayer = _gfx_gl_named_framebuffer_texture_layer;
		(GFX_EXT)->TextureBuffer                = _gfx_gl_texture_buffer;
		(GFX_EXT)->TextureParameteri            = _gfx_gl_texture_parameter_i;
		(GFX_EXT)->TextureStorage1D             = _gfx_gl_texture_storage_1d;
		(GFX_EXT)->TextureStorage2D             = _gfx_gl_texture_storage_2d;
		(GFX_EXT)->TextureStorage2DMultisample  = _gfx_gl_texture_storage_2d_multisample;
		(GFX_EXT)->TextureStorage3D             = _gfx_gl_texture_storage_3d;
		(GFX_EXT)->TextureStorage3DMultisample  = _gfx_gl_texture_storage_3d_multisample;
		(GFX_EXT)->TextureSubImage1D            = _gfx_gl_texture_sub_image_1d;
		(GFX_EXT)->TextureSubImage2D            = _gfx_gl_texture_sub_image_2d;
		(GFX_EXT)->TextureSubImage3D            = _gfx_gl_texture_sub_image_3d;
		(GFX_EXT)->UnmapNamedBuffer             = _gfx_gl_unmap_named_buffer;
	}

	/* GFX_EXT_IMMUTABLE_TEXTURE */
	if(
		window->context.major > 4 ||
		(window->context.major == 4 && window->context.minor > 1) ||
		_gfx_is_extension_supported("GL_ARB_texture_storage"))
	{
		(GFX_EXT)->flags[GFX_EXT_IMMUTABLE_TEXTURE] = 1;
		(GFX_EXT)->TexStorage1D =
			(PFNGLTEXSTORAGE1DPROC)_gfx_platform_get_proc_address("glTexStorage1D");
		(GFX_EXT)->TexStorage2D =
			(PFNGLTEXSTORAGE2DPROC)_gfx_platform_get_proc_address("glTexStorage2D");
		(GFX_EXT)->TexStorage3D =
			(PFNGLTEXSTORAGE3DPROC)_gfx_platform_get_proc_address("glTexStorage3D");
	}

	else
	{
		(GFX_EXT)->flags[GFX_EXT_IMMUTABLE_TEXTURE] = 0;
		(GFX_EXT)->TexStorage1D = _gfx_gl_tex_storage_1d;
		(GFX_EXT)->TexStorage2D = _gfx_gl_tex_storage_2d;
		(GFX_EXT)->TexStorage3D = _gfx_gl_tex_storage_3d;
	}

	/* GFX_EXT_IMMUTABLE_MULTISAMPLE_TEXTURE */
	if(
		window->context.major > 4 ||
		(window->context.major == 4 && window->context.minor > 2) ||
		_gfx_is_extension_supported("GL_ARB_texture_storage_multisample"))
	{
		(GFX_EXT)->flags[GFX_EXT_IMMUTABLE_MULTISAMPLE_TEXTURE] = 1;
		(GFX_EXT)->TexStorage2DMultisample =
			(PFNGLTEXSTORAGE2DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTexStorage2DMultisample");
		(GFX_EXT)->TexStorage3DMultisample =
			(PFNGLTEXSTORAGE3DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTexStorage3DMultisample");
	}

	else
	{
		(GFX_EXT)->flags[GFX_EXT_IMMUTABLE_MULTISAMPLE_TEXTURE] = 0;
		(GFX_EXT)->TexStorage2DMultisample = _gfx_gl_tex_storage_2d_multisample;
		(GFX_EXT)->TexStorage3DMultisample = _gfx_gl_tex_storage_3d_multisample;
	}

	/* GFX_EXT_INSTANCED_ATTRIBUTES */
	if(
		window->context.major > 3 ||
		(window->context.major == 3 && window->context.minor > 2))
	{
		(GFX_EXT)->flags[GFX_EXT_INSTANCED_ATTRIBUTES] = 1;
		(GFX_EXT)->VertexAttribDivisor =
			(PFNGLVERTEXATTRIBDIVISORPROC)_gfx_platform_get_proc_address("glVertexAttribDivisor");
	}

	else if(_gfx_is_extension_supported("GL_ARB_instanced_arrays"))
	{
		(GFX_EXT)->flags[GFX_EXT_INSTANCED_ATTRIBUTES] = 1;
		(GFX_EXT)->VertexAttribDivisor =
			(PFNGLVERTEXATTRIBDIVISORPROC)_gfx_platform_get_proc_address("glVertexAttribDivisorARB");
	}

	else
	{
		(GFX_EXT)->flags[GFX_EXT_INSTANCED_ATTRIBUTES] = 0;
		(GFX_EXT)->VertexAttribDivisor = _gfx_gl_vertex_attrib_divisor;
	}

	/* GFX_EXT_INSTANCED_BASE_ATTRIBUTES */
	if(
		window->context.major > 4 ||
		(window->context.major == 4 && window->context.minor > 1) ||
		_gfx_is_extension_supported("GL_ARB_base_instance"))
	{
		(GFX_EXT)->flags[GFX_EXT_INSTANCED_BASE_ATTRIBUTES] = 1;
		(GFX_EXT)->DrawArraysInstancedBaseInstance =
			(PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC)_gfx_platform_get_proc_address("glDrawArraysInstancedBaseInstance");
		(GFX_EXT)->DrawElementsInstancedBaseInstance =
			(PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC)_gfx_platform_get_proc_address("glDrawElementsInstancedBaseInstance");
	}

	else
	{
		(GFX_EXT)->flags[GFX_EXT_INSTANCED_BASE_ATTRIBUTES] = 0;
		(GFX_EXT)->DrawArraysInstancedBaseInstance = _gfx_gl_draw_arrays_instanced_base_instance;
		(GFX_EXT)->DrawElementsInstancedBaseInstance = _gfx_gl_draw_elements_instanced_base_instance;
	}

	/* GFX_EXT_LAYERED_CUBEMAP */
	if(
		window->context.major > 3 ||
		_gfx_is_extension_supported("GL_ARB_texture_cube_map_array"))
	{
		(GFX_EXT)->flags[GFX_EXT_LAYERED_CUBEMAP] = 1;
	}

	else
	{
		(GFX_EXT)->flags[GFX_EXT_LAYERED_CUBEMAP] = 0;
	}

	/* GFX_EXT_PROGRAM_BINARY */
	if(
		window->context.major > 4 ||
		(window->context.major == 4 && window->context.minor > 0) ||
		_gfx_is_extension_supported("GL_ARB_get_program_binary"))
	{
		(GFX_EXT)->flags[GFX_EXT_PROGRAM_BINARY] = 1;
		(GFX_EXT)->GetProgramBinary =
			(PFNGLGETPROGRAMBINARYPROC)_gfx_platform_get_proc_address("glGetProgramBinary");
		(GFX_EXT)->ProgramBinary =
			(PFNGLPROGRAMBINARYPROC)_gfx_platform_get_proc_address("glProgramBinary");
		(GFX_EXT)->ProgramParameteri =
			(PFNGLPROGRAMPARAMETERIPROC)_gfx_platform_get_proc_address("glProgramParameteri");
	}

	else
	{
		(GFX_EXT)->flags[GFX_EXT_PROGRAM_BINARY] = 0;
		(GFX_EXT)->GetProgramBinary  = _gfx_gl_get_program_binary;
		(GFX_EXT)->ProgramBinary     = _gfx_gl_program_binary;
		(GFX_EXT)->ProgramParameteri = _gfx_gl_program_parameter_i;
	}

	/* GFX_EXT_TESSELLATION_SHADER */
	if(
		window->context.major > 3 ||
		_gfx_is_extension_supported("GL_ARB_tessellation_shader"))
	{
		glGetIntegerv(
			GL_MAX_PATCH_VERTICES,
			(GFX_EXT)->limits + GFX_LIM_MAX_PATCH_VERTICES
		);
		(GFX_EXT)->flags[GFX_EXT_TESSELLATION_SHADER] = 1;

		(GFX_EXT)->PatchParameteri =
			(PFNGLPATCHPARAMETERIPROC)_gfx_platform_get_proc_address("glPatchParameteri");
	}

	else
	{
		(GFX_EXT)->limits[GFX_LIM_MAX_PATCH_VERTICES] = 0;
		(GFX_EXT)->flags[GFX_EXT_TESSELLATION_SHADER] = 0;

		(GFX_EXT)->PatchParameteri = _gfx_gl_patch_parameter_i;
	}

#endif

	/* Set default state */
	_gfx_states_set_default(&(GFX_EXT)->state);
	_gfx_states_force_set(&(GFX_EXT)->state);

	/* Set other defaults */
	(GFX_EXT)->fbos[0] = 0;
	(GFX_EXT)->fbos[1] = 0;
	(GFX_EXT)->vao = 0;
	(GFX_EXT)->program = 0;

	_gfx_platform_window_get_size(
		window->handle,
		&(GFX_EXT)->width,
		&(GFX_EXT)->height
	);
	(GFX_EXT)->x = 0;
	(GFX_EXT)->y = 0;

	(GFX_EXT)->packAlignment = 0;
	(GFX_EXT)->unpackAlignment = 0;
	(GFX_EXT)->patchVertices = 0;

	(GFX_EXT)->uniformBuffers = NULL;
	(GFX_EXT)->textureUnits = NULL;
}
