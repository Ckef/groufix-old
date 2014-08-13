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

static void _gfx_gl_copy_named_buffer_sub_data(

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

static void _gfx_gl_create_buffers(

		GLsizei  n,
		GLuint*  buffers)
{
	(GFX_EXT)->GenBuffers(n, buffers);
}

static void _gfx_gl_create_framebuffers(

		GLsizei  n,
		GLuint*  ids)
{
	(GFX_EXT)->GenFramebuffers(n, ids);
}

static void _gfx_gl_create_textures(

		GLenum   target,
		GLsizei  n,
		GLuint*  textures)
{
	(GFX_EXT)->GenTextures(n, textures);
}

static void _gfx_gl_base_instance_error(void)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_INSTANCED_BASE_ATTRIBUTES is incompatible with this context."
	);
}

static void _gfx_gl_draw_arrays_instanced_base_instance(

		GLenum   mode,
		GLint    first,
		GLsizei  count,
		GLsizei  primcount,
		GLuint   baseinstance)
{
	_gfx_gl_base_instance_error();
}

static void _gfx_gl_draw_elements_instanced_base_instance(

		GLenum         mode,
		GLsizei        count,
		GLenum         type,
		const GLvoid*  indices,
		GLsizei        primcount,
		GLuint         baseinstance)
{
	_gfx_gl_base_instance_error();
}

static void _gfx_gl_get_named_buffer_sub_data(

		GLuint    buffer,
		GLintptr  offset,
		GLsizei   size,
		void*     data)
{
	(GFX_EXT)->BindBuffer(GL_ARRAY_BUFFER, buffer);
	(GFX_EXT)->GetBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

static void _gfx_gl_map_named_buffer_range(

		GLuint      buffer,
		GLintptr    offset,
		GLsizei     length,
		GLbitfield  access)
{
	(GFX_EXT)->BindBuffer(GL_ARRAY_BUFFER, buffer);
	(GFX_EXT)->MapBufferRange(GL_ARRAY_BUFFER, offset, length, access);
}

static void _gfx_gl_named_buffer_data(

		GLuint       buffer,
		GLsizei      size,
		const void*  data,
		GLenum       usage)
{
	(GFX_EXT)->BindBuffer(GL_ARRAY_BUFFER, buffer);
	(GFX_EXT)->BufferData(GL_ARRAY_BUFFER, size, data, usage);
}

static void _gfx_gl_named_buffer_sub_data(

		GLuint       buffer,
		GLintptr     offset,
		GLsizei      size,
		const void*  data)
{
	(GFX_EXT)->BindBuffer(GL_ARRAY_BUFFER, buffer);
	(GFX_EXT)->BufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

static void _gfx_gl_named_framebuffer_draw_buffers(

		GLuint         framebuffer,
		GLsizei        n,
		const GLenum*  bufs)
{
	(GFX_EXT)->fbos[0] = framebuffer;
	(GFX_EXT)->BindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	(GFX_EXT)->DrawBuffers(n, bufs);
}

static void _gfx_gl_named_framebuffer_texture(

		GLuint  framebuffer,
		GLenum  attach,
		GLuint  texture,
		GLint   level)
{
	(GFX_EXT)->fbos[0] = framebuffer;
	(GFX_EXT)->BindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);

	(GFX_EXT)->FramebufferTexture(
		GL_DRAW_FRAMEBUFFER,
		attach,
		texture,
		level
	);
}

static void _gfx_gl_named_framebuffer_texture_layer(

		GLuint  framebuffer,
		GLenum  attach,
		GLuint  texture,
		GLint   level,
		GLint   layer)
{
	(GFX_EXT)->fbos[0] = framebuffer;
	(GFX_EXT)->BindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);

	(GFX_EXT)->FramebufferTextureLayer(
		GL_DRAW_FRAMEBUFFER,
		attach,
		texture,
		level,
		layer
	);
}

static void _gfx_gl_patch_parameter_i(

		GLenum  pname,
		GLint   value)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_TESSELLATION_SHADER is incompatible with this context."
	);
}

static void _gfx_gl_direct_state_access_error(void)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_DIRECT_STATE_ACCESS is incompatible with this context."
	);
}

static void _gfx_gl_texture_buffer(

		GLuint  texture,
		GLenum  format,
		GLuint  buffer)
{
	_gfx_gl_direct_state_access_error();
}

static GLboolean _gfx_gl_unmap_named_buffer(

		GLuint buffer)
{
	(GFX_EXT)->BindBuffer(GL_ARRAY_BUFFER, buffer);
	return (GFX_EXT)->UnmapBuffer(GL_ARRAY_BUFFER);
}


#ifdef GFX_GLES

/********************************************************
 * GL ES emulators
 *******************************************************/

static void _gfx_gles_tex_buffer_error(void)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_BUFFER_TEXTURE is incompatible with this context."
	);
}

static void _gfx_gles_framebuffer_texture(

		GLenum  target,
		GLenum  attach,
		GLuint  texture,
		GLint   level)
{
	_gfx_gles_tex_buffer_error();
}

static void _gfx_gles_framebuffer_texture_1d(

		GLenum  target,
		GLenum  attach,
		GLenum  textarget,
		GLuint  texture,
		GLint   level)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_1D_TEXTURE is incompatible with this context."
	);
}

static void _gfx_gles_get_buffer_sub_data(

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

static void _gfx_gles_named_framebuffer_texture_1d(

		GLuint  framebuffer,
		GLenum  attach,
		GLenum  textarget,
		GLuint  texture,
		GLint   level)
{
	(GFX_EXT)->fbos[0] = framebuffer;
	(GFX_EXT)->BindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);

	(GFX_EXT)->FramebufferTexture1D(
		GL_DRAW_FRAMEBUFFER,
		attach,
		textarget,
		texture,
		level
	);
}

static void _gfx_gles_named_framebuffer_texture_2d(

		GLuint  framebuffer,
		GLenum  attach,
		GLenum  textarget,
		GLuint  texture,
		GLint   level)
{
	(GFX_EXT)->fbos[0] = framebuffer;
	(GFX_EXT)->BindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);

	(GFX_EXT)->FramebufferTexture2D(
		GL_DRAW_FRAMEBUFFER,
		attach,
		textarget,
		texture,
		level
	);
}

static void _gfx_gles_polygon_mode(

		GLenum  face,
		GLenum  mode)
{
	/* Just ignore the call */
}

static void _gfx_gles_tex_buffer(

		GLenum  target,
		GLenum  internalFormat,
		GLuint  buffer)
{
	_gfx_gles_tex_buffer_error();
}

static void _gfx_gles_tex_1d_error(void)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_TEXTURE_1D is incompatible with this context."
	);
}

static void _gfx_gles_tex_image_1d(

		GLenum         target,
		GLint          level,
		GLint          internalFormat,
		GLsizei        w,
		GLint          b,
		GLenum         format,
		GLenum         type,
		const GLvoid*  data)
{
	_gfx_gles_tex_1d_error();
}

static void _gfx_gles_tex_sub_image_1d(

		GLenum         target,
		GLint          level,
		GLint          xoff,
		GLsizei        w,
		GLenum         format,
		GLenum         type,
		const GLvoid*  data)
{
	_gfx_gles_tex_1d_error();
}

static void _gfx_gles_multisample_tex_error(void)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_MULTISAMPLE_TEXTURE is incompatible with this context."
	);
}

static void _gfx_gles_tex_image_2d_multisample(

		GLenum     target,
		GLsizei    samples,
		GLenum     internalformat,
		GLsizei    w,
		GLsizei    h,
		GLboolean  f)
{
	_gfx_gles_multisample_tex_error();
}

static void _gfx_gles_tex_image_3d_multisample(

		GLenum     target,
		GLsizei    samples,
		GLenum     internalformat,
		GLsizei    w,
		GLsizei    h,
		GLsizei    d,
		GLboolean  f)
{
	_gfx_gles_multisample_tex_error();
}


#else

/********************************************************
 * GL core emulators
 *******************************************************/

static void _gfx_gl_named_framebuffer_texture_1d(

		GLuint  framebuffer,
		GLenum  attach,
		GLenum  textarget,
		GLuint  texture,
		GLint   level)
{
	(GFX_EXT)->NamedFramebufferTexture(framebuffer, attach, texture, level);
}

static void _gfx_gl_named_framebuffer_texture_2d(

		GLuint  framebuffer,
		GLenum  attach,
		GLenum  textarget,
		GLuint  texture,
		GLint   level)
{
	(GFX_EXT)->NamedFramebufferTexture(framebuffer, attach, texture, level);
}

static void _gfx_gl_program_binary_error(void)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_PROGRAM_BINARY is incompatible with this context."
	);
}

static void _gfx_gl_get_program_binary(

		GLuint    program,
		GLsizei   bufsize,
		GLsizei*  length,
		GLenum*   binaryFormat,
		void*     binary)
{
	if(length) *length = 0;

	_gfx_gl_program_binary_error();
}

static void _gfx_gl_program_binary(

		GLuint       program,
		GLenum       binaryFormat,
		const void*  binary,
		GLsizei      length)
{
	_gfx_gl_program_binary_error();
}

static void _gfx_gl_program_parameter_i(

		GLuint  program,
		GLenum  pname,
		GLint   value)
{
	_gfx_gl_program_binary_error();
}

static void _gfx_gl_vertex_attrib_divisor(

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

	GFX_Extensions* ext = GFX_EXT;

	/* Get OpenGL version */
	_gfx_platform_context_get(
		&window->context.major,
		&window->context.minor
	);

	/* Get OpenGL constants (a.k.a hardware limits) */
	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS,
		ext->limits + GFX_LIM_MAX_BUFFER_PROPERTIES);
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS,
		ext->limits + GFX_LIM_MAX_COLOR_ATTACHMENTS);
	glGetIntegerv(GL_MAX_DRAW_BUFFERS,
		ext->limits + GFX_LIM_MAX_COLOR_TARGETS);
	glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE,
		ext->limits + GFX_LIM_MAX_CUBEMAP_SIZE);
	glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS,
		ext->limits + GFX_LIM_MAX_FEEDBACK_BUFFERS);
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
		ext->limits + GFX_LIM_MAX_SAMPLER_PROPERTIES);
	glGetIntegerv(GL_MAX_SAMPLES,
		ext->limits + GFX_LIM_MAX_SAMPLES);
	glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE,
		ext->limits + GFX_LIM_MAX_TEXTURE_3D_SIZE);
	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS,
		ext->limits + GFX_LIM_MAX_TEXTURE_LAYERS);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE,
		ext->limits + GFX_LIM_MAX_TEXTURE_SIZE);
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS,
		ext->limits + GFX_LIM_MAX_VERTEX_ATTRIBS);

#ifdef GFX_GLES

	/* Get OpenGL constants (a.k.a hardware limits) */
	ext->limits[GFX_LIM_MAX_BUFFER_TEXTURE_SIZE] = 0;
	ext->limits[GFX_LIM_MAX_PATCH_VERTICES] = 0;

	/* Default Extensions */
	ext->flags[GFX_EXT_BUFFER_TEXTURE]            = 0;
	ext->flags[GFX_EXT_DIRECT_STATE_ACCESS]       = 0;
	ext->flags[GFX_EXT_GEOMETRY_SHADER]           = 0;
	ext->flags[GFX_EXT_INSTANCED_ATTRIBUTES]      = 1;
	ext->flags[GFX_EXT_INSTANCED_BASE_ATTRIBUTES] = 0;
	ext->flags[GFX_EXT_LAYERED_CUBEMAP]           = 0;
	ext->flags[GFX_EXT_MULTISAMPLE_TEXTURE]       = 0;
	ext->flags[GFX_EXT_POLYGON_STATE]             = 0;
	ext->flags[GFX_EXT_PROGRAM_BINARY]            = 1;
	ext->flags[GFX_EXT_SEAMLESS_CUBEMAP]          = 0;
	ext->flags[GFX_EXT_TESSELLATION_SHADER]       = 0;
	ext->flags[GFX_EXT_TEXTURE_1D]                = 0;

	/* GLES, assumes 3.0+ */
	ext->ActiveTexture                     = glActiveTexture;
	ext->AttachShader                      = glAttachShader;
	ext->BeginTransformFeedback            = glBeginTransformFeedback;
	ext->BindAttribLocation                = glBindAttribLocation;
	ext->BindBuffer                        = glBindBuffer;
	ext->BindBufferRange                   = glBindBufferRange;
	ext->BindFramebuffer                   = glBindFramebuffer;
	ext->BindTexture                       = glBindTexture;
	ext->BindVertexArray                   = glBindVertexArray;
	ext->BlendEquationSeparate             = glBlendEquationSeparate;
	ext->BlendFuncSeparate                 = glBlendFuncSeparate;
	ext->BufferData                        = glBufferData;
	ext->BufferSubData                     = glBufferSubData;
	ext->Clear                             = glClear;
	ext->CompileShader                     = glCompileShader;
	ext->CopyBufferSubData                 = glCopyBufferSubData;
	ext->CopyNamedBufferSubData            = _gfx_gl_copy_named_buffer_sub_data;
	ext->CreateBuffers                     = _gfx_gl_create_buffers;
	ext->CreateFramebuffers                = _gfx_gl_create_framebuffers;
	ext->CreateProgram                     = glCreateProgram;
	ext->CreateShader                      = glCreateShader;
	ext->CreateTextures                    = glCreateTextures;
	ext->CullFace                          = glCullFace;
	ext->DeleteBuffers                     = glDeleteBuffers;
	ext->DeleteFramebuffers                = glDeleteFramebuffers;
	ext->DeleteProgram                     = glDeleteProgram;
	ext->DeleteShader                      = glDeleteShader;
	ext->DeleteTextures                    = glDeleteTextures;
	ext->DeleteVertexArrays                = glDeleteVertexArrays;
	ext->DepthFunc                         = glDepthFunc;
	ext->DepthMask                         = glDepthMask;
	ext->DetachShader                      = glDetachShader;
	ext->Disable                           = glDisable;
	ext->DisableVertexAttribArray          = glDisableVertexAttribArray;
	ext->DrawArrays                        = glDrawArrays;
	ext->DrawArraysInstanced               = glDrawArraysInstanced;
	ext->DrawArraysInstancedBaseInstance   = _gfx_gl_draw_arrays_instanced_base_instance;
	ext->DrawBuffers                       = glDrawBuffers;
	ext->DrawElements                      = glDrawElements;
	ext->DrawElementsInstanced             = glDrawElementsInstanced;
	ext->DrawElementsInstancedBaseInstance = _gfx_gl_draw_elements_instanced_base_instance;
	ext->Enable                            = glEnable;
	ext->EnableVertexAttribArray           = glEnableVertexAttribArray;
	ext->EndTransformFeedback              = glEndTransformFeedback;
	ext->FramebufferTexture                = _gfx_gles_framebuffer_texture;
	ext->FramebufferTexture1D              = _gfx_gles_framebuffer_texture_1d;
	ext->FramebufferTexture2D              = glFramebufferTexture2D;
	ext->FramebufferTextureLayer           = glFramebufferTextureLayer;
	ext->GenBuffers                        = glGenBuffers;
	ext->GenerateMipmap                    = glGenerateMipmap;
	ext->GenFramebuffers                   = glGenFramebuffers;
	ext->GenTextures                       = glGenTextures;
	ext->GenVertexArrays                   = glGenVertexArrays;
	ext->GetActiveUniform                  = glGetActiveUniform;
	ext->GetActiveUniformBlockiv           = glGetActiveUniformBlockiv;
	ext->GetActiveUniformsiv               = glGetActiveUniformsiv;
	ext->GetBufferSubData                  = _gfx_gles_get_buffer_sub_data;
	ext->GetError                          = glGetError;
	ext->GetNamedBufferSubData             = _gfx_gl_get_named_buffer_sub_data;
	ext->GetProgramBinary                  = glGetProgramBinary;
	ext->GetProgramInfoLog                 = glGetProgramInfoLog;
	ext->GetProgramiv                      = glGetProgramiv;
	ext->GetShaderInfoLog                  = glGetShaderInfoLog;
	ext->GetShaderiv                       = glGetShaderiv;
	ext->GetShaderSource                   = glGetShaderSource;
	ext->GetUniformBlockIndex              = glGetUniformBlockIndex;
	ext->GetUniformIndices                 = glGetUniformIndices;
	ext->GetUniformLocation                = glGetUniformLocation;
	ext->LinkProgram                       = glLinkProgram;
	ext->MapBufferRange                    = glMapBufferRange;
	ext->MapNamedBufferRange               = glMapNamedBufferRange;
	ext->NamedBufferData                   = _gfx_gl_named_buffer_data;
	ext->NamedBufferSubData                = _gfx_gl_named_buffer_sub_data;
	ext->NamedFramebufferDrawBuffers       = _gfx_gl_named_framebuffer_draw_buffers;
	ext->NamedFramebufferTexture           = _gfx_gl_named_framebuffer_texture;
	ext->NamedFramebufferTexture1D         = _gfx_gles_named_framebuffer_texture_1d;
	ext->NamedFramebufferTexture2D         = _gfx_gles_named_framebuffer_texture_2d;
	ext->NamedFramebufferTextureLayer      = _gfx_gl_named_framebuffer_texture_layer;
	ext->PatchParameteri                   = _gfx_gl_patch_parameter_i;
	ext->PixelStorei                       = glPixelStorei;
	ext->PolygonMode                       = _gfx_gles_polygon_mode;
	ext->ProgramBinary                     = glProgramBinary;
	ext->ProgramParameteri                 = glProgramParameteri;
	ext->ShaderSource                      = glShaderSource;
	ext->StencilFuncSeparate               = glStencilFuncSeparate;
	ext->StencilOpSeparate                 = glStencilOpSeparate;
	ext->TexBuffer                         = _gfx_gles_tex_buffer;
	ext->TexImage1D                        = _gfx_gles_tex_image_1d;
	ext->TexImage2D                        = glTexImage2D;
	ext->TexImage2DMultisample             = _gfx_gles_tex_image_2d_multisample;
	ext->TexImage3D                        = glTexImage3D;
	ext->TexImage3DMultisample             = _gfx_gles_tex_image_3d_multisample;
	ext->TexParameteri                     = glTexParameteri;
	ext->TexSubImage1D                     = _gfx_gles_tex_sub_image_1d;
	ext->TexSubImage2D                     = glTexSubImage2D;
	ext->TexSubImage3D                     = glTexSubImage3D;
	ext->TextureBuffer                     = _gfx_gl_texture_buffer;
	ext->TransformFeedbackVaryings         = glTransformFeedbackVaryings;
	ext->Uniform1fv                        = glUniform1fv;
	ext->Uniform1iv                        = glUniform1iv;
	ext->Uniform1uiv                       = glUniform1uiv;
	ext->Uniform2fv                        = glUniform2fv;
	ext->Uniform2iv                        = glUniform2iv;
	ext->Uniform2uiv                       = glUniform2uiv;
	ext->Uniform3fv                        = glUniform3fv;
	ext->Uniform3iv                        = glUniform3iv;
	ext->Uniform3uiv                       = glUniform3uiv;
	ext->Uniform4fv                        = glUniform4fv;
	ext->Uniform4iv                        = glUniform4iv;
	ext->Uniform4uiv                       = glUniform4uiv;
	ext->UniformBlockBinding               = glUniformBlockBinding;
	ext->UniformMatrix2fv                  = glUniformMatrix2fv;
	ext->UniformMatrix3fv                  = glUniformMatrix3fv;
	ext->UniformMatrix4fv                  = glUniformMatrix4fv;
	ext->UnmapBuffer                       = glUnmapBuffer;
	ext->UnmapNamedBuffer                  = _gfx_gl_unmap_named_buffer;
	ext->UseProgram                        = glUseProgram;
	ext->VertexAttribDivisor               = glVertexAttribDivisor;
	ext->VertexAttribIPointer              = glVertexAttribIPointer;
	ext->VertexAttribPointer               = glVertexAttribPointer;
	ext->Viewport                          = glViewport;

#else

	/* Settings */
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	/* Get OpenGL constants (a.k.a hardware limits) */
	glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE,
		ext->limits + GFX_LIM_MAX_BUFFER_TEXTURE_SIZE);

	/* Default Extensions */
	ext->flags[GFX_EXT_BUFFER_TEXTURE]      = 1;
	ext->flags[GFX_EXT_GEOMETRY_SHADER]     = 1;
	ext->flags[GFX_EXT_MULTISAMPLE_TEXTURE] = 1;
	ext->flags[GFX_EXT_POLYGON_STATE]       = 1;
	ext->flags[GFX_EXT_SEAMLESS_CUBEMAP]    = 1;
	ext->flags[GFX_EXT_TEXTURE_1D]          = 1;

	/* Core, assumes 3.2+ context */
	ext->ActiveTexture             = (PFNGLACTIVETEXTUREPROC)             _gfx_platform_get_proc_address("glActiveTexture");
	ext->AttachShader              = (PFNGLATTACHSHADERPROC)              _gfx_platform_get_proc_address("glAttachShader");
	ext->BeginTransformFeedback    = (PFNGLBEGINTRANSFORMFEEDBACKPROC)    _gfx_platform_get_proc_address("glBeginTransformFeedback");
	ext->BindAttribLocation        = (PFNGLBINDATTRIBLOCATIONPROC)        _gfx_platform_get_proc_address("glBindAttribLocation");
	ext->BindBuffer                = (PFNGLBINDBUFFERPROC)                _gfx_platform_get_proc_address("glBindBuffer");
	ext->BindBufferRange           = (PFNGLBINDBUFFERRANGEPROC)           _gfx_platform_get_proc_address("glBindBufferRange");
	ext->BindFramebuffer           = (PFNGLBINDFRAMEBUFFERPROC)           _gfx_platform_get_proc_address("glBindFramebuffer");
	ext->BindTexture               = (PFNGLBINDTEXTUREPROC)               glBindTexture;
	ext->BindVertexArray           = (PFNGLBINDVERTEXARRAYPROC)           _gfx_platform_get_proc_address("glBindVertexArray");
	ext->BlendEquationSeparate     = (PFNGLBLENDEQUATIONSEPARATEPROC)     _gfx_platform_get_proc_address("glBlendEquationSeparate");
	ext->BlendFuncSeparate         = (PFNGLBLENDFUNCSEPARATEPROC)         _gfx_platform_get_proc_address("glBlendFuncSeparate");
	ext->BufferData                = (PFNGLBUFFERDATAPROC)                _gfx_platform_get_proc_address("glBufferData");
	ext->BufferSubData             = (PFNGLBUFFERSUBDATAPROC)             _gfx_platform_get_proc_address("glBufferSubData");
	ext->Clear                     = (PFNGLCLEARPROC)                     glClear;
	ext->CompileShader             = (PFNGLCOMPILESHADERPROC)             _gfx_platform_get_proc_address("glCompileShader");
	ext->CopyBufferSubData         = (PFNGLCOPYBUFFERSUBDATAPROC)         _gfx_platform_get_proc_address("glCopyBufferSubData");
	ext->CreateProgram             = (PFNGLCREATEPROGRAMPROC)             _gfx_platform_get_proc_address("glCreateProgram");
	ext->CreateShader              = (PFNGLCREATESHADERPROC)              _gfx_platform_get_proc_address("glCreateShader");
	ext->CullFace                  = (PFNGLCULLFACEPROC)                  glCullFace;
	ext->DeleteBuffers             = (PFNGLDELETEBUFFERSPROC)             _gfx_platform_get_proc_address("glDeleteBuffers");
	ext->DeleteFramebuffers        = (PFNGLDELETEFRAMEBUFFERSPROC)        _gfx_platform_get_proc_address("glDeleteFramebuffers");
	ext->DeleteProgram             = (PFNGLDELETEPROGRAMPROC)             _gfx_platform_get_proc_address("glDeleteProgram");
	ext->DeleteShader              = (PFNGLDELETESHADERPROC)              _gfx_platform_get_proc_address("glDeleteShader");
	ext->DeleteTextures            = (PFNGLDELETETEXTURESPROC)            glDeleteTextures;
	ext->DeleteVertexArrays        = (PFNGLDELETEVERTEXARRAYSPROC)        _gfx_platform_get_proc_address("glDeleteVertexArrays");
	ext->DepthFunc                 = (PFNGLDEPTHFUNCPROC)                 glDepthFunc;
	ext->DepthMask                 = (PFNGLDEPTHMASKPROC)                 glDepthMask;
	ext->DetachShader              = (PFNGLDETACHSHADERPROC)              _gfx_platform_get_proc_address("glDetachShader");
	ext->Disable                   = (PFNGLDISABLEPROC)                   glDisable;
	ext->DisableVertexAttribArray  = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)  _gfx_platform_get_proc_address("glDisableVertexAttribArray");
	ext->DrawArrays                = (PFNGLDRAWARRAYSPROC)                glDrawArrays;
	ext->DrawArraysInstanced       = (PFNGLDRAWARRAYSINSTANCEDPROC)       _gfx_platform_get_proc_address("glDrawArraysInstanced");
	ext->DrawBuffers               = (PFNGLDRAWBUFFERSPROC)               _gfx_platform_get_proc_address("glDrawBuffers");
	ext->DrawElements              = (PFNGLDRAWELEMENTSPROC)              glDrawElements;
	ext->DrawElementsInstanced     = (PFNGLDRAWELEMENTSINSTANCEDPROC)     _gfx_platform_get_proc_address("glDrawElementsInstanced");
	ext->Enable                    = (PFNGLENABLEPROC)                    glEnable;
	ext->EnableVertexAttribArray   = (PFNGLENABLEVERTEXATTRIBARRAYPROC)   _gfx_platform_get_proc_address("glEnableVertexAttribArray");
	ext->EndTransformFeedback      = (PFNGLENDTRANSFORMFEEDBACKPROC)      _gfx_platform_get_proc_address("glEndTransformFeedback");
	ext->FramebufferTexture        = (PFNGLFRAMEBUFFERTEXTUREPROC)        _gfx_platform_get_proc_address("glFramebufferTexture");
	ext->FramebufferTexture1D      = (PFNGLFRAMEBUFFERTEXTURE1DPROC)      _gfx_platform_get_proc_address("glFramebufferTexture1D");
	ext->FramebufferTexture2D      = (PFNGLFRAMEBUFFERTEXTURE2DPROC)      _gfx_platform_get_proc_address("glFramebufferTexture2D");
	ext->FramebufferTextureLayer   = (PFNGLFRAMEBUFFERTEXTURELAYERPROC)   _gfx_platform_get_proc_address("glFramebufferTextureLayer");
	ext->GenBuffers                = (PFNGLGENBUFFERSPROC)                _gfx_platform_get_proc_address("glGenBuffers");
	ext->GenerateMipmap            = (PFNGLGENERATEMIPMAPPROC)            _gfx_platform_get_proc_address("glGenerateMipmap");
	ext->GenFramebuffers           = (PFNGLGENFRAMEBUFFERSPROC)           _gfx_platform_get_proc_address("glGenFramebuffers");
	ext->GenTextures               = (PFNGLGENTEXTURESPROC)               glGenTextures;
	ext->GenVertexArrays           = (PFNGLGENVERTEXARRAYSPROC)           _gfx_platform_get_proc_address("glGenVertexArrays");
	ext->GetActiveUniform          = (PFNGLGETACTIVEUNIFORMPROC)          _gfx_platform_get_proc_address("glGetActiveUniform");
	ext->GetActiveUniformBlockiv   = (PFNGLGETACTIVEUNIFORMBLOCKIVPROC)   _gfx_platform_get_proc_address("glGetActiveUniformBlockiv");
	ext->GetActiveUniformsiv       = (PFNGLGETACTIVEUNIFORMSIVPROC)       _gfx_platform_get_proc_address("glGetActiveUniformsiv");
	ext->GetBufferSubData          = (PFNGLGETBUFFERSUBDATAPROC)          _gfx_platform_get_proc_address("glGetBufferSubData");
	ext->GetError                  = (PFNGLGETERRORPROC)                  glGetError;
	ext->GetProgramInfoLog         = (PFNGLGETPROGRAMINFOLOGPROC)         _gfx_platform_get_proc_address("glGetProgramInfoLog");
	ext->GetProgramiv              = (PFNGLGETPROGRAMIVPROC)              _gfx_platform_get_proc_address("glGetProgramiv");
	ext->GetShaderInfoLog          = (PFNGLGETSHADERINFOLOGPROC)          _gfx_platform_get_proc_address("glGetShaderInfoLog");
	ext->GetShaderiv               = (PFNGLGETSHADERIVPROC)               _gfx_platform_get_proc_address("glGetShaderiv");
	ext->GetShaderSource           = (PFNGLGETSHADERSOURCEPROC)           _gfx_platform_get_proc_address("glGetShaderSource");
	ext->GetUniformBlockIndex      = (PFNGLGETUNIFORMBLOCKINDEXPROC)      _gfx_platform_get_proc_address("glGetUniformBlockIndex");
	ext->GetUniformIndices         = (PFNGLGETUNIFORMINDICESPROC)         _gfx_platform_get_proc_address("glGetUniformIndices");
	ext->GetUniformLocation        = (PFNGLGETUNIFORMLOCATIONPROC)        _gfx_platform_get_proc_address("glGetUniformLocation");
	ext->LinkProgram               = (PFNGLLINKPROGRAMPROC)               _gfx_platform_get_proc_address("glLinkProgram");
	ext->MapBufferRange            = (PFNGLMAPBUFFERRANGEPROC)            _gfx_platform_get_proc_address("glMapBufferRange");
	ext->NamedFramebufferTexture1D = (GFX_NAMEDFRAMEBUFFERTEXTURE1DPROC)  _gfx_gl_named_framebuffer_texture_1d;
	ext->NamedFramebufferTexture2D = (GFX_NAMEDFRAMEBUFFERTEXTURE2DPROC)  _gfx_gl_named_framebuffer_texture_2d;
	ext->PixelStorei               = (PFNGLPIXELSTOREIPROC)               glPixelStorei;
	ext->PolygonMode               = (PFNGLPOLYGONMODEPROC)               glPolygonMode;
	ext->ShaderSource              = (PFNGLSHADERSOURCEPROC)              _gfx_platform_get_proc_address("glShaderSource");
	ext->StencilFuncSeparate       = (PFNGLSTENCILFUNCSEPARATEPROC)       _gfx_platform_get_proc_address("glStencilFuncSeparate");
	ext->StencilOpSeparate         = (PFNGLSTENCILOPSEPARATEPROC)         _gfx_platform_get_proc_address("glStencilOpSeparate");
	ext->TexBuffer                 = (PFNGLTEXBUFFERPROC)                 _gfx_platform_get_proc_address("glTexBuffer");
	ext->TexImage1D                = (PFNGLTEXIMAGE1DPROC)                glTexImage1D;
	ext->TexImage2D                = (PFNGLTEXIMAGE2DPROC)                glTexImage2D;
	ext->TexImage2DMultisample     = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)     _gfx_platform_get_proc_address("glTexImage2DMultisample");
	ext->TexImage3D                = (PFNGLTEXIMAGE3DPROC)                _gfx_platform_get_proc_address("glTexImage3D");
	ext->TexImage3DMultisample     = (PFNGLTEXIMAGE3DMULTISAMPLEPROC)     _gfx_platform_get_proc_address("glTexImage3DMultisample");
	ext->TexParameteri             = (PFNGLTEXPARAMETERIPROC)             glTexParameteri;
	ext->TexSubImage1D             = (PFNGLTEXSUBIMAGE1DPROC)             glTexSubImage1D;
	ext->TexSubImage2D             = (PFNGLTEXSUBIMAGE2DPROC)             glTexSubImage2D;
	ext->TexSubImage3D             = (PFNGLTEXSUBIMAGE3DPROC)             _gfx_platform_get_proc_address("glTexSubImage3D");
	ext->TransformFeedbackVaryings = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC) _gfx_platform_get_proc_address("glTransformFeedbackVaryings");
	ext->Uniform1fv                = (PFNGLUNIFORM1FVPROC)                _gfx_platform_get_proc_address("glUniform1fv");
	ext->Uniform1iv                = (PFNGLUNIFORM1IVPROC)                _gfx_platform_get_proc_address("glUniform1iv");
	ext->Uniform1uiv               = (PFNGLUNIFORM1UIVPROC)               _gfx_platform_get_proc_address("glUniform1uiv");
	ext->Uniform2fv                = (PFNGLUNIFORM2FVPROC)                _gfx_platform_get_proc_address("glUniform2fv");
	ext->Uniform2iv                = (PFNGLUNIFORM2IVPROC)                _gfx_platform_get_proc_address("glUniform2iv");
	ext->Uniform2uiv               = (PFNGLUNIFORM2UIVPROC)               _gfx_platform_get_proc_address("glUniform2uiv");
	ext->Uniform3fv                = (PFNGLUNIFORM3FVPROC)                _gfx_platform_get_proc_address("glUniform3fv");
	ext->Uniform3iv                = (PFNGLUNIFORM3IVPROC)                _gfx_platform_get_proc_address("glUniform3iv");
	ext->Uniform3uiv               = (PFNGLUNIFORM3UIVPROC)               _gfx_platform_get_proc_address("glUniform3uiv");
	ext->Uniform4fv                = (PFNGLUNIFORM4FVPROC)                _gfx_platform_get_proc_address("glUniform4fv");
	ext->Uniform4iv                = (PFNGLUNIFORM4IVPROC)                _gfx_platform_get_proc_address("glUniform4iv");
	ext->Uniform4uiv               = (PFNGLUNIFORM4UIVPROC)               _gfx_platform_get_proc_address("glUniform4uiv");
	ext->UniformBlockBinding       = (PFNGLUNIFORMBLOCKBINDINGPROC)       _gfx_platform_get_proc_address("glUniformBlockBinding");
	ext->UniformMatrix2fv          = (PFNGLUNIFORMMATRIX2FVPROC)          _gfx_platform_get_proc_address("glUniformMatrix2fv");
	ext->UniformMatrix3fv          = (PFNGLUNIFORMMATRIX3FVPROC)          _gfx_platform_get_proc_address("glUniformMatrix3fv");
	ext->UniformMatrix4fv          = (PFNGLUNIFORMMATRIX4FVPROC)          _gfx_platform_get_proc_address("glUniformMatrix4fv");
	ext->UnmapBuffer               = (PFNGLUNMAPBUFFERPROC)               _gfx_platform_get_proc_address("glUnmapBuffer");
	ext->UseProgram                = (PFNGLUSEPROGRAMPROC)                _gfx_platform_get_proc_address("glUseProgram");
	ext->VertexAttribIPointer      = (PFNGLVERTEXATTRIBIPOINTERPROC)      _gfx_platform_get_proc_address("glVertexAttribIPointer");
	ext->VertexAttribPointer       = (PFNGLVERTEXATTRIBPOINTERPROC)       _gfx_platform_get_proc_address("glVertexAttribPointer");
	ext->Viewport                  = (PFNGLVIEWPORTPROC)                  glViewport;

	/* GFX_EXT_DIRECT_STATE_ACCESS */
	if(
		window->context.major > 4 ||
		(window->context.major == 4 && window->context.minor > 4))
	{
		ext->flags[GFX_EXT_DIRECT_STATE_ACCESS] = 1;

		ext->CopyNamedBufferSubData = (PFNGLCOPYNAMEDBUFFERSUBDATAPROC)
			_gfx_platform_get_proc_address("glCopyNamedBufferSubData");
		ext->CreateBuffers = (PFNGLCREATEBUFFERSPROC)
			_gfx_platform_get_proc_address("glCreateBuffers");
		ext->CreateFramebuffers = (PFNGLCREATEFRAMEBUFFERSPROC)
			_gfx_platform_get_proc_address("glCreateFramebuffers");
		ext->CreateTextures = (PFNGLCREATETEXTURESPROC)
			_gfx_platform_get_proc_address("glCreateTextures");
		ext->GetNamedBufferSubData = (PFNGLGETNAMEDBUFFERSUBDATAPROC)
			_gfx_platform_get_proc_address("glGetNamedBufferSubData");
		ext->MapNamedBufferRange = (PFNGLMAPNAMEDBUFFERRANGEPROC)
			_gfx_platform_get_proc_address("glMapNamedBufferRange");
		ext->NamedBufferData = (PFNGLNAMEDBUFFERDATAPROC)
			_gfx_platform_get_proc_address("glNamedBufferData");
		ext->NamedBufferSubData = (PFNGLNAMEDBUFFERSUBDATAPROC)
			_gfx_platform_get_proc_address("glNamedBufferSubData");
		ext->NamedFramebufferDrawBuffers = (PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC)
			_gfx_platform_get_proc_address("glNamedFramebufferDrawBuffers");
		ext->NamedFramebufferTexture = (PFNGLNAMEDFRAMEBUFFERTEXTUREPROC)
			_gfx_platform_get_proc_address("glNamedFramebufferTexture");
		ext->NamedFramebufferTextureLayer = (PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC)
			_gfx_platform_get_proc_address("glNamedFramebufferTextureLayer");
		ext->TextureBuffer = (PFNGLTEXTUREBUFFERPROC)
			_gfx_platform_get_proc_address("glTextureBuffer");
		ext->UnmapNamedBuffer = (PFNGLUNMAPNAMEDBUFFERPROC)
			_gfx_platform_get_proc_address("glUnmapNamedBuffer");
	}

	else if(_gfx_platform_is_extension_supported(window->handle, "GL_ARB_direct_state_access"))
	{
		ext->flags[GFX_EXT_DIRECT_STATE_ACCESS] = 1;

		ext->CopyNamedBufferSubData = (PFNGLCOPYNAMEDBUFFERSUBDATAPROC)
			_gfx_platform_get_proc_address("glCopyNamedBufferSubDataARB");
		ext->CreateBuffers = (PFNGLCREATEBUFFERSPROC)
			_gfx_platform_get_proc_address("glCreateBuffersARB");
		ext->CreateFramebuffers = (PFNGLCREATEFRAMEBUFFERSPROC)
			_gfx_platform_get_proc_address("glCreateFramebuffersARB");
		ext->CreateTextures = (PFNGLCREATETEXTURESPROC)
			_gfx_platform_get_proc_address("glCreateTexturesARB");
		ext->GetNamedBufferSubData = (PFNGLGETNAMEDBUFFERSUBDATAPROC)
			_gfx_platform_get_proc_address("glGetNamedBufferSubDataARB");
		ext->MapNamedBufferRange = (PFNGLMAPNAMEDBUFFERRANGEPROC)
			_gfx_platform_get_proc_address("glMapNamedBufferRangeARB");
		ext->NamedBufferData = (PFNGLNAMEDBUFFERDATAPROC)
			_gfx_platform_get_proc_address("glNamedBufferDataARB");
		ext->NamedBufferSubData = (PFNGLNAMEDBUFFERSUBDATAPROC)
			_gfx_platform_get_proc_address("glNamedBufferSubDataARB");
		ext->NamedFramebufferDrawBuffers = (PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC)
			_gfx_platform_get_proc_address("glNamedFramebufferDrawBuffersARB");
		ext->NamedFramebufferTexture = (PFNGLNAMEDFRAMEBUFFERTEXTUREPROC)
			_gfx_platform_get_proc_address("glNamedFramebufferTextureARB");
		ext->NamedFramebufferTextureLayer = (PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC)
			_gfx_platform_get_proc_address("glNamedFramebufferTextureLayerARB");
		ext->TextureBuffer = (PFNGLTEXTUREBUFFERPROC)
			_gfx_platform_get_proc_address("glTextureBufferARB");
		ext->UnmapNamedBuffer = (PFNGLUNMAPNAMEDBUFFERPROC)
			_gfx_platform_get_proc_address("glUnmapNamedBufferARB");
	}

	else
	{
		ext->flags[GFX_EXT_DIRECT_STATE_ACCESS] = 0;

		ext->CopyNamedBufferSubData = (PFNGLCOPYNAMEDBUFFERSUBDATAPROC)
			_gfx_gl_copy_named_buffer_sub_data;
		ext->CreateBuffers = (PFNGLCREATEBUFFERSPROC)
			_gfx_gl_create_buffers;
		ext->CreateFramebuffers = (PFNGLCREATEFRAMEBUFFERSPROC)
			_gfx_gl_create_framebuffers;
		ext->CreateTextures = (PFNGLCREATETEXTURESPROC)
			_gfx_gl_create_textures;
		ext->GetNamedBufferSubData = (PFNGLGETNAMEDBUFFERSUBDATAPROC)
			_gfx_gl_get_named_buffer_sub_data;
		ext->MapNamedBufferRange = (PFNGLMAPNAMEDBUFFERRANGEPROC)
			_gfx_gl_map_named_buffer_range;
		ext->NamedBufferData = (PFNGLNAMEDBUFFERDATAPROC)
			_gfx_gl_named_buffer_data;
		ext->NamedBufferSubData = (PFNGLNAMEDBUFFERSUBDATAPROC)
			_gfx_gl_named_buffer_sub_data;
		ext->NamedFramebufferDrawBuffers = (PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC)
			_gfx_gl_named_framebuffer_draw_buffers;
		ext->NamedFramebufferTexture = (PFNGLNAMEDFRAMEBUFFERTEXTUREPROC)
			_gfx_gl_named_framebuffer_texture;
		ext->NamedFramebufferTextureLayer = (PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC)
			_gfx_gl_named_framebuffer_texture_layer;
		ext->TextureBuffer = (PFNGLTEXTUREBUFFERPROC)
			_gfx_gl_texture_buffer;
		ext->UnmapNamedBuffer = (PFNGLUNMAPNAMEDBUFFERPROC)
			_gfx_gl_unmap_named_buffer;
	}

	/* GFX_EXT_INSTANCED_ATTRIBUTES */
	if(
		window->context.major > 3 ||
		(window->context.major == 3 && window->context.minor > 2))
	{
		ext->flags[GFX_EXT_INSTANCED_ATTRIBUTES] = 1;

		ext->VertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)
			_gfx_platform_get_proc_address("glVertexAttribDivisor");
	}

	else if(_gfx_platform_is_extension_supported(window->handle, "GL_ARB_instanced_arrays"))
	{
		ext->flags[GFX_EXT_INSTANCED_ATTRIBUTES] = 1;

		ext->VertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)
			_gfx_platform_get_proc_address("glVertexAttribDivisorARB");
	}

	else
	{
		ext->flags[GFX_EXT_INSTANCED_ATTRIBUTES] = 0;

		ext->VertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)
			_gfx_gl_vertex_attrib_divisor;
	}

	/* GFX_EXT_INSTANCED_BASE_ATTRIBUTES */
	if(
		window->context.major > 4 ||
		(window->context.major == 4 && window->context.minor > 1))
	{
		ext->flags[GFX_EXT_INSTANCED_BASE_ATTRIBUTES] = 1;

		ext->DrawArraysInstancedBaseInstance = (PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC)
			_gfx_platform_get_proc_address("glDrawArraysInstancedBaseInstance");
		ext->DrawElementsInstancedBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC)
			_gfx_platform_get_proc_address("glDrawElementsInstancedBaseInstance");
	}

	else if(_gfx_platform_is_extension_supported(window->handle, "GL_ARB_base_instance"))
	{
		ext->flags[GFX_EXT_INSTANCED_BASE_ATTRIBUTES] = 1;

		ext->DrawArraysInstancedBaseInstance = (PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC)
			_gfx_platform_get_proc_address("glDrawArraysInstancedBaseInstanceARB");
		ext->DrawElementsInstancedBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC)
			_gfx_platform_get_proc_address("glDrawElementsInstancedBaseInstaceARB");
	}

	else
	{
		ext->flags[GFX_EXT_INSTANCED_BASE_ATTRIBUTES] = 0;

		ext->DrawArraysInstancedBaseInstance = (PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC)
			_gfx_gl_draw_arrays_instanced_base_instance;
		ext->DrawElementsInstancedBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC)
			_gfx_gl_draw_elements_instanced_base_instance;
	}

	/* GFX_EXT_LAYERED_CUBEMAP */
	if(
		window->context.major > 3 ||
		_gfx_platform_is_extension_supported(window->handle, "GL_ARB_texture_cube_map_array"))
	{
		ext->flags[GFX_EXT_LAYERED_CUBEMAP] = 1;
	}

	else
	{
		ext->flags[GFX_EXT_LAYERED_CUBEMAP] = 0;
	}

	/* GFX_EXT_PROGRAM_BINARY */
	if(
		window->context.major > 4 ||
		(window->context.major == 4 && window->context.minor > 0))
	{
		ext->flags[GFX_EXT_PROGRAM_BINARY] = 1;

		ext->GetProgramBinary = (PFNGLGETPROGRAMBINARYPROC)
			_gfx_platform_get_proc_address("glGetProgramBinary");
		ext->ProgramBinary = (PFNGLPROGRAMBINARYPROC)
			_gfx_platform_get_proc_address("glProgramBinary");
		ext->ProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC)
			_gfx_platform_get_proc_address("glProgramParameteri");
	}

	else if(_gfx_platform_is_extension_supported(window->handle, "GL_ARB_get_program_binary"))
	{
		ext->flags[GFX_EXT_PROGRAM_BINARY] = 1;

		ext->GetProgramBinary = (PFNGLGETPROGRAMBINARYPROC)
			_gfx_platform_get_proc_address("glGetProgramBinaryARB");
		ext->ProgramBinary = (PFNGLPROGRAMBINARYPROC)
			_gfx_platform_get_proc_address("glProgramBinaryARB");
		ext->ProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC)
			_gfx_platform_get_proc_address("glProgramParameteriARB");
	}

	else
	{
		ext->flags[GFX_EXT_PROGRAM_BINARY] = 0;

		ext->GetProgramBinary = (PFNGLGETPROGRAMBINARYPROC)
			_gfx_gl_get_program_binary;
		ext->ProgramBinary = (PFNGLPROGRAMBINARYPROC)
			_gfx_gl_program_binary;
		ext->ProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC)
			_gfx_gl_program_parameter_i;
	}

	/* GFX_EXT_TESSELLATION_SHADER */
	if(window->context.major > 3)
	{
		glGetIntegerv(GL_MAX_PATCH_VERTICES,
			ext->limits + GFX_LIM_MAX_PATCH_VERTICES);

		ext->flags[GFX_EXT_TESSELLATION_SHADER] = 1;

		ext->PatchParameteri = (PFNGLPATCHPARAMETERIPROC)
			_gfx_platform_get_proc_address("glPatchParameteri");
	}

	else if(_gfx_platform_is_extension_supported(window->handle, "GL_ARB_tessellation_shader"))
	{
		glGetIntegerv(GL_MAX_PATCH_VERTICES,
			ext->limits + GFX_LIM_MAX_PATCH_VERTICES);

		ext->flags[GFX_EXT_TESSELLATION_SHADER] = 1;

		ext->PatchParameteri = (PFNGLPATCHPARAMETERIPROC)
			_gfx_platform_get_proc_address("glPatchParameteriARB");
	}

	else
	{
		ext->limits[GFX_LIM_MAX_PATCH_VERTICES] = 0;
		ext->flags[GFX_EXT_TESSELLATION_SHADER] = 0;

		ext->PatchParameteri = (PFNGLPATCHPARAMETERIPROC)
			_gfx_gl_patch_parameter_i;
	}

#endif

	/* Set default state */
	_gfx_states_set_default(&ext->state);
	_gfx_states_force_set(&ext->state);

	/* Set other defaults */
	ext->fbos[0] = 0;
	ext->fbos[1] = 0;
	ext->layout = 0;
	ext->program = 0;

	_gfx_platform_window_get_size(
		window->handle,
		&ext->width,
		&ext->height
	);
	ext->x = 0;
	ext->y = 0;

	ext->packAlignment = 0;
	ext->unpackAlignment = 0;
	ext->patchVertices = 0;

	ext->uniformBuffers = NULL;
	ext->textureUnits = NULL;
}
