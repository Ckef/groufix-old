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
#include "groufix/core/renderer.h"

#include <string.h>

#ifdef GFX_RENDERER_GL

/******************************************************/
static int _gfx_is_extension_supported(

		const char*  ext,
		GFX_Window*  window)
{
	GLint num;
	glGetIntegerv(GL_NUM_EXTENSIONS, &num);

	while(num) if(!strcmp(
		(const char*)window->renderer.GetStringi(GL_EXTENSIONS, --num),
		(const char*)ext))
	{
		return 1;
	}

	return 0;
}

/******************************************************/
void _gfx_renderer_load(void)
{
	/* Get current window and context */
	GFX_Window* window = _gfx_window_get_current();
	if(!window) return;

	/* Get OpenGL constants (a.k.a hardware limits) */
	GLint limit;

	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &limit);
		window->limits[GFX_LIM_MAX_BUFFER_PROPERTIES] = limit;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &limit);
		window->limits[GFX_LIM_MAX_COLOR_ATTACHMENTS] = limit;
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &limit);
		window->limits[GFX_LIM_MAX_COLOR_TARGETS] = limit;
	glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &limit);
		window->limits[GFX_LIM_MAX_CUBEMAP_SIZE] = limit;
	glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS, &limit);
		window->limits[GFX_LIM_MAX_FEEDBACK_BUFFERS] = limit;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &limit);
		window->limits[GFX_LIM_MAX_SAMPLER_PROPERTIES] = limit;
	glGetIntegerv(GL_MAX_SAMPLES, &limit);
		window->limits[GFX_LIM_MAX_SAMPLES] = limit;
	glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &limit);
		window->limits[GFX_LIM_MAX_TEXTURE_3D_SIZE] = limit;
	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &limit);
		window->limits[GFX_LIM_MAX_TEXTURE_LAYERS] = limit;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &limit);
		window->limits[GFX_LIM_MAX_TEXTURE_SIZE] = limit;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &limit);
		window->limits[GFX_LIM_MAX_VERTEX_ATTRIBS] = limit;

#if defined(GFX_GLES)

	/* Get OpenGL constants (a.k.a hardware limits) */
	window->limits[GFX_LIM_MAX_BUFFER_TEXTURE_SIZE] = 0;
	window->limits[GFX_LIM_MAX_PATCH_VERTICES] = 0;

	/* Default Extensions */
	window->flags[GFX_EXT_BUFFER_TEXTURE]              = 0;
	window->flags[GFX_EXT_DIRECT_STATE_ACCESS]         = 0;
	window->flags[GFX_EXT_GEOMETRY_SHADER]             = 0;
	window->flags[GFX_EXT_IMMUTABLE_TEXTURE]           = 1;
	window->flags[GFX_EXT_INSTANCED_ATTRIBUTES]        = 1;
	window->flags[GFX_EXT_INSTANCED_BASE_ATTRIBUTES]   = 0;
	window->flags[GFX_EXT_LAYERED_CUBEMAP]             = 0;
	window->flags[GFX_EXT_LAYERED_MULTISAMPLE_TEXTURE] = 0;
	window->flags[GFX_EXT_POLYGON_STATE]               = 0;
	window->flags[GFX_EXT_PROGRAM_BINARY]              = 1;
	window->flags[GFX_EXT_SEAMLESS_CUBEMAP]            = 0;
	window->flags[GFX_EXT_TESSELLATION_SHADER]         = 0;
	window->flags[GFX_EXT_TEXTURE_1D]                  = 0;

	/* GLES, assumes 3.0+ */
	window->renderer.ActiveTexture                     = glActiveTexture;
	window->renderer.AttachShader                      = glAttachShader;
	window->renderer.BeginTransformFeedback            = glBeginTransformFeedback;
	window->renderer.BindAttribLocation                = glBindAttribLocation;
	window->renderer.BindBuffer                        = glBindBuffer;
	window->renderer.BindBufferRange                   = glBindBufferRange;
	window->renderer.BindFramebuffer                   = glBindFramebuffer;
	window->renderer.BindTexture                       = glBindTexture;
	window->renderer.BindTextureUnit                   = glBindTextureUnit;
	window->renderer.BindVertexArray                   = glBindVertexArray;
	window->renderer.BlendEquationSeparate             = glBlendEquationSeparate;
	window->renderer.BlendFuncSeparate                 = glBlendFuncSeparate;
	window->renderer.BufferData                        = glBufferData;
	window->renderer.BufferSubData                     = glBufferSubData;
	window->renderer.Clear                             = glClear;
	window->renderer.CompileShader                     = glCompileShader;
	window->renderer.CopyBufferSubData                 = glCopyBufferSubData;
	window->renderer.CopyNamedBufferSubData            = _gfx_gl_copy_named_buffer_sub_data;
	window->renderer.CreateBuffers                     = _gfx_gl_create_buffers;
	window->renderer.CreateFramebuffers                = _gfx_gl_create_framebuffers;
	window->renderer.CreateProgram                     = glCreateProgram;
	window->renderer.CreateShader                      = glCreateShader;
	window->renderer.CreateTextures                    = _gfx_gl_create_textures;
	window->renderer.CreateVertexArrays                = _gfx_gl_create_vertex_arrays;
	window->renderer.CullFace                          = glCullFace;
	window->renderer.DeleteBuffers                     = glDeleteBuffers;
	window->renderer.DeleteFramebuffers                = glDeleteFramebuffers;
	window->renderer.DeleteProgram                     = glDeleteProgram;
	window->renderer.DeleteShader                      = glDeleteShader;
	window->renderer.DeleteTextures                    = glDeleteTextures;
	window->renderer.DeleteVertexArrays                = glDeleteVertexArrays;
	window->renderer.DepthFunc                         = glDepthFunc;
	window->renderer.DepthMask                         = glDepthMask;
	window->renderer.DetachShader                      = glDetachShader;
	window->renderer.Disable                           = glDisable;
	window->renderer.DisableVertexArrayAttrib          = _gfx_gl_disable_vertex_array_attrib;
	window->renderer.DisableVertexAttribArray          = glDisableVertexAttribArray;
	window->renderer.DrawArrays                        = glDrawArrays;
	window->renderer.DrawArraysInstanced               = glDrawArraysInstanced;
	window->renderer.DrawArraysInstancedBaseInstance   = _gfx_gl_draw_arrays_instanced_base_instance;
	window->renderer.DrawBuffers                       = glDrawBuffers;
	window->renderer.DrawElements                      = glDrawElements;
	window->renderer.DrawElementsInstanced             = glDrawElementsInstanced;
	window->renderer.DrawElementsInstancedBaseInstance = _gfx_gl_draw_elements_instanced_base_instance;
	window->renderer.Enable                            = glEnable;
	window->renderer.EnableVertexArrayAttrib           = _gfx_gl_enable_vertex_array_attrib;
	window->renderer.EnableVertexAttribArray           = glEnableVertexAttribArray;
	window->renderer.EndTransformFeedback              = glEndTransformFeedback;
	window->renderer.FramebufferTexture                = _gfx_gles_framebuffer_texture;
	window->renderer.FramebufferTexture1D              = _gfx_gles_framebuffer_texture_1d;
	window->renderer.FramebufferTexture2D              = glFramebufferTexture2D;
	window->renderer.FramebufferTextureLayer           = glFramebufferTextureLayer;
	window->renderer.GenBuffers                        = glGenBuffers;
	window->renderer.GenerateMipmap                    = glGenerateMipmap;
	window->renderer.GenerateTextureMipmap             = glGenerateTextureMipmap;
	window->renderer.GenFramebuffers                   = glGenFramebuffers;
	window->renderer.GenTextures                       = glGenTextures;
	window->renderer.GenVertexArrays                   = glGenVertexArrays;
	window->renderer.GetActiveUniform                  = glGetActiveUniform;
	window->renderer.GetActiveUniformBlockiv           = glGetActiveUniformBlockiv;
	window->renderer.GetActiveUniformsiv               = glGetActiveUniformsiv;
	window->renderer.GetBufferSubData                  = _gfx_gles_get_buffer_sub_data;
	window->renderer.GetError                          = glGetError;
	window->renderer.GetNamedBufferSubData             = _gfx_gl_get_named_buffer_sub_data;
	window->renderer.GetProgramBinary                  = glGetProgramBinary;
	window->renderer.GetProgramInfoLog                 = glGetProgramInfoLog;
	window->renderer.GetProgramiv                      = glGetProgramiv;
	window->renderer.GetShaderInfoLog                  = glGetShaderInfoLog;
	window->renderer.GetShaderiv                       = glGetShaderiv;
	window->renderer.GetShaderSource                   = glGetShaderSource;
	window->renderer.GetStringi                        = glGetStringi;
	window->renderer.GetUniformBlockIndex              = glGetUniformBlockIndex;
	window->renderer.GetUniformIndices                 = glGetUniformIndices;
	window->renderer.GetUniformLocation                = glGetUniformLocation;
	window->renderer.LinkProgram                       = glLinkProgram;
	window->renderer.MapBufferRange                    = glMapBufferRange;
	window->renderer.MapNamedBufferRange               = glMapNamedBufferRange;
	window->renderer.NamedBufferData                   = _gfx_gl_named_buffer_data;
	window->renderer.NamedBufferSubData                = _gfx_gl_named_buffer_sub_data;
	window->renderer.NamedFramebufferDrawBuffers       = _gfx_gl_named_framebuffer_draw_buffers;
	window->renderer.NamedFramebufferTexture           = _gfx_gl_named_framebuffer_texture;
	window->renderer.NamedFramebufferTexture1D         = _gfx_gles_named_framebuffer_texture_1d;
	window->renderer.NamedFramebufferTexture2D         = _gfx_gles_named_framebuffer_texture_2d;
	window->renderer.NamedFramebufferTextureLayer      = _gfx_gl_named_framebuffer_texture_layer;
	window->renderer.PatchParameteri                   = _gfx_gl_patch_parameter_i;
	window->renderer.PixelStorei                       = glPixelStorei;
	window->renderer.PolygonMode                       = _gfx_gles_polygon_mode;
	window->renderer.ProgramBinary                     = glProgramBinary;
	window->renderer.ProgramParameteri                 = glProgramParameteri;
	window->renderer.ShaderSource                      = glShaderSource;
	window->renderer.StencilFuncSeparate               = glStencilFuncSeparate;
	window->renderer.StencilOpSeparate                 = glStencilOpSeparate;
	window->renderer.TexBuffer                         = _gfx_gles_tex_buffer;
	window->renderer.TexImage1D                        = _gfx_gles_tex_image_1d;
	window->renderer.TexImage2D                        = glTexImage2D;
	window->renderer.TexImage2DMultisample             = _gfx_gles_tex_image_2d_multisample;
	window->renderer.TexImage3D                        = glTexImage3D;
	window->renderer.TexImage3DMultisample             = _gfx_gles_tex_image_3d_multisample;
	window->renderer.TexParameteri                     = glTexParameteri;
	window->renderer.TexStorage1D                      = _gfx_gles_tex_storage_1d;
	window->renderer.TexStorage2D                      = glTexStorage2D;
	window->renderer.TexStorage3D                      = glTexStorage3D;
	window->renderer.TexStorage3DMultisample           = _gfx_gles_tex_storage_3d_multisample;
	window->renderer.TexSubImage1D                     = _gfx_gles_tex_sub_image_1d;
	window->renderer.TexSubImage2D                     = glTexSubImage2D;
	window->renderer.TexSubImage3D                     = glTexSubImage3D;
	window->renderer.TextureBuffer                     = _gfx_gl_texture_buffer;
	window->renderer.TextureParameteri                 = _gfx_gl_texture_parameter_i;
	window->renderer.TextureStorage1D                  = _gfx_gl_texture_storage_1d;
	window->renderer.TextureStorage2D                  = _gfx_gl_texture_storage_2d;
	window->renderer.TextureStorage2DMultisample       = _gfx_gl_texture_storage_2d_multisample;
	window->renderer.TextureStorage3D                  = _gfx_gl_texture_storage_3d;
	window->renderer.TextureStorage3DMultisample       = _gfx_gl_texture_storage_3d_multisample;
	window->renderer.TextureSubImage1D                 = _gfx_gl_texture_sub_image_1d;
	window->renderer.TextureSubImage2D                 = _gfx_gl_texture_sub_image_2d;
	window->renderer.TextureSubImage3D                 = _gfx_gl_texture_sub_image_3d;
	window->renderer.TransformFeedbackVaryings         = glTransformFeedbackVaryings;
	window->renderer.Uniform1fv                        = glUniform1fv;
	window->renderer.Uniform1iv                        = glUniform1iv;
	window->renderer.Uniform1uiv                       = glUniform1uiv;
	window->renderer.Uniform2fv                        = glUniform2fv;
	window->renderer.Uniform2iv                        = glUniform2iv;
	window->renderer.Uniform2uiv                       = glUniform2uiv;
	window->renderer.Uniform3fv                        = glUniform3fv;
	window->renderer.Uniform3iv                        = glUniform3iv;
	window->renderer.Uniform3uiv                       = glUniform3uiv;
	window->renderer.Uniform4fv                        = glUniform4fv;
	window->renderer.Uniform4iv                        = glUniform4iv;
	window->renderer.Uniform4uiv                       = glUniform4uiv;
	window->renderer.UniformBlockBinding               = glUniformBlockBinding;
	window->renderer.UniformMatrix2fv                  = glUniformMatrix2fv;
	window->renderer.UniformMatrix3fv                  = glUniformMatrix3fv;
	window->renderer.UniformMatrix4fv                  = glUniformMatrix4fv;
	window->renderer.UnmapBuffer                       = glUnmapBuffer;
	window->renderer.UnmapNamedBuffer                  = _gfx_gl_unmap_named_buffer;
	window->renderer.UseProgram                        = glUseProgram;
	window->renderer.VertexAttribDivisor               = glVertexAttribDivisor;
	window->renderer.VertexAttribIPointer              = glVertexAttribIPointer;
	window->renderer.VertexAttribPointer               = glVertexAttribPointer;
	window->renderer.Viewport                          = glViewport;

	/* GFX_EXT_IMMUTABLE_MULTISAMPLE_TEXTURE */
	/* GFX_EXT_MULTISAMPLE_TEXTURE */
	if(
		window->context.major > 3 ||
		(window->context.major == 3 && window->context.minor > 0))
	{
		window->flags[GFX_EXT_IMMUTABLE_MULTISAMPLE_TEXTURE] = 1;
		window->flags[GFX_EXT_MULTISAMPLE_TEXTURE] = 1;

		window->renderer.TexStorage2DMultisample = glTexStorage2DMultisample;
	}

	else
	{
		window->flags[GFX_EXT_IMMUTABLE_MULTISAMPLE_TEXTURE] = 0;
		window->flags[GFX_EXT_MULTISAMPLE_TEXTURE] = 0;

		window->renderer.TexStorage2DMultisample = _gfx_gles_tex_storage_2d_multisample;
	}

#elif defined(GFX_GL)

	/* Settings */
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	/* Get OpenGL constants (a.k.a hardware limits) */
	glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &limit);
		window->limits[GFX_LIM_MAX_BUFFER_TEXTURE_SIZE] = limit;

	/* Default Extensions */
	window->flags[GFX_EXT_BUFFER_TEXTURE]              = 1;
	window->flags[GFX_EXT_GEOMETRY_SHADER]             = 1;
	window->flags[GFX_EXT_LAYERED_MULTISAMPLE_TEXTURE] = 1;
	window->flags[GFX_EXT_MULTISAMPLE_TEXTURE]         = 1;
	window->flags[GFX_EXT_POLYGON_STATE]               = 1;
	window->flags[GFX_EXT_SEAMLESS_CUBEMAP]            = 1;
	window->flags[GFX_EXT_TEXTURE_1D]                  = 1;

	/* Core, assumes 3.2+ context */
	window->renderer.ActiveTexture =
		(PFNGLACTIVETEXTUREPROC)_gfx_platform_get_proc_address("glActiveTexture");
	window->renderer.AttachShader =
		(PFNGLATTACHSHADERPROC)_gfx_platform_get_proc_address("glAttachShader");
	window->renderer.BeginTransformFeedback =
		(PFNGLBEGINTRANSFORMFEEDBACKPROC)_gfx_platform_get_proc_address("glBeginTransformFeedback");
	window->renderer.BindAttribLocation =
		(PFNGLBINDATTRIBLOCATIONPROC)_gfx_platform_get_proc_address("glBindAttribLocation");
	window->renderer.BindBuffer =
		(PFNGLBINDBUFFERPROC)_gfx_platform_get_proc_address("glBindBuffer");
	window->renderer.BindBufferRange =
		(PFNGLBINDBUFFERRANGEPROC)_gfx_platform_get_proc_address("glBindBufferRange");
	window->renderer.BindFramebuffer =
		(PFNGLBINDFRAMEBUFFERPROC)_gfx_platform_get_proc_address("glBindFramebuffer");
	window->renderer.BindTexture =
		(PFNGLBINDTEXTUREPROC)glBindTexture;
	window->renderer.BindVertexArray =
		(PFNGLBINDVERTEXARRAYPROC)_gfx_platform_get_proc_address("glBindVertexArray");
	window->renderer.BlendEquationSeparate =
		(PFNGLBLENDEQUATIONSEPARATEPROC)_gfx_platform_get_proc_address("glBlendEquationSeparate");
	window->renderer.BlendFuncSeparate =
		(PFNGLBLENDFUNCSEPARATEPROC)_gfx_platform_get_proc_address("glBlendFuncSeparate");
	window->renderer.BufferData =
		(PFNGLBUFFERDATAPROC)_gfx_platform_get_proc_address("glBufferData");
	window->renderer.BufferSubData =
		(PFNGLBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glBufferSubData");
	window->renderer.Clear =
		(PFNGLCLEARPROC)glClear;
	window->renderer.CompileShader =
		(PFNGLCOMPILESHADERPROC)_gfx_platform_get_proc_address("glCompileShader");
	window->renderer.CopyBufferSubData =
		(PFNGLCOPYBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glCopyBufferSubData");
	window->renderer.CreateProgram =
		(PFNGLCREATEPROGRAMPROC)_gfx_platform_get_proc_address("glCreateProgram");
	window->renderer.CreateShader =
		(PFNGLCREATESHADERPROC)_gfx_platform_get_proc_address("glCreateShader");
	window->renderer.CullFace =
		(PFNGLCULLFACEPROC)glCullFace;
	window->renderer.DeleteBuffers =
		(PFNGLDELETEBUFFERSPROC)_gfx_platform_get_proc_address("glDeleteBuffers");
	window->renderer.DeleteFramebuffers =
		(PFNGLDELETEFRAMEBUFFERSPROC)_gfx_platform_get_proc_address("glDeleteFramebuffers");
	window->renderer.DeleteProgram =
		(PFNGLDELETEPROGRAMPROC)_gfx_platform_get_proc_address("glDeleteProgram");
	window->renderer.DeleteShader =
		(PFNGLDELETESHADERPROC)_gfx_platform_get_proc_address("glDeleteShader");
	window->renderer.DeleteTextures =
		(PFNGLDELETETEXTURESPROC)glDeleteTextures;
	window->renderer.DeleteVertexArrays =
		(PFNGLDELETEVERTEXARRAYSPROC)_gfx_platform_get_proc_address("glDeleteVertexArrays");
	window->renderer.DepthFunc =
		(PFNGLDEPTHFUNCPROC)glDepthFunc;
	window->renderer.DepthMask =
		(PFNGLDEPTHMASKPROC)glDepthMask;
	window->renderer.DetachShader =
		(PFNGLDETACHSHADERPROC)_gfx_platform_get_proc_address("glDetachShader");
	window->renderer.Disable =
		(PFNGLDISABLEPROC)glDisable;
	window->renderer.DisableVertexAttribArray =
		(PFNGLDISABLEVERTEXATTRIBARRAYPROC)_gfx_platform_get_proc_address("glDisableVertexAttribArray");
	window->renderer.DrawArrays =
		(PFNGLDRAWARRAYSPROC)glDrawArrays;
	window->renderer.DrawArraysInstanced =
		(PFNGLDRAWARRAYSINSTANCEDPROC)_gfx_platform_get_proc_address("glDrawArraysInstanced");
	window->renderer.DrawBuffers =
		(PFNGLDRAWBUFFERSPROC)_gfx_platform_get_proc_address("glDrawBuffers");
	window->renderer.DrawElements =
		(PFNGLDRAWELEMENTSPROC)glDrawElements;
	window->renderer.DrawElementsInstanced =
		(PFNGLDRAWELEMENTSINSTANCEDPROC)_gfx_platform_get_proc_address("glDrawElementsInstanced");
	window->renderer.Enable =
		(PFNGLENABLEPROC)glEnable;
	window->renderer.EnableVertexAttribArray =
		(PFNGLENABLEVERTEXATTRIBARRAYPROC)_gfx_platform_get_proc_address("glEnableVertexAttribArray");
	window->renderer.EndTransformFeedback =
		(PFNGLENDTRANSFORMFEEDBACKPROC)_gfx_platform_get_proc_address("glEndTransformFeedback");
	window->renderer.FramebufferTexture =
		(PFNGLFRAMEBUFFERTEXTUREPROC)_gfx_platform_get_proc_address("glFramebufferTexture");
	window->renderer.FramebufferTexture1D =
		(PFNGLFRAMEBUFFERTEXTURE1DPROC)_gfx_platform_get_proc_address("glFramebufferTexture1D");
	window->renderer.FramebufferTexture2D =
		(PFNGLFRAMEBUFFERTEXTURE2DPROC)_gfx_platform_get_proc_address("glFramebufferTexture2D");
	window->renderer.FramebufferTextureLayer =
		(PFNGLFRAMEBUFFERTEXTURELAYERPROC)_gfx_platform_get_proc_address("glFramebufferTextureLayer");
	window->renderer.GenBuffers =
		(PFNGLGENBUFFERSPROC)_gfx_platform_get_proc_address("glGenBuffers");
	window->renderer.GenerateMipmap =
		(PFNGLGENERATEMIPMAPPROC)_gfx_platform_get_proc_address("glGenerateMipmap");
	window->renderer.GenFramebuffers =
		(PFNGLGENFRAMEBUFFERSPROC)_gfx_platform_get_proc_address("glGenFramebuffers");
	window->renderer.GenTextures =
		(PFNGLGENTEXTURESPROC)glGenTextures;
	window->renderer.GenVertexArrays =
		(PFNGLGENVERTEXARRAYSPROC)_gfx_platform_get_proc_address("glGenVertexArrays");
	window->renderer.GetActiveUniform =
		(PFNGLGETACTIVEUNIFORMPROC)_gfx_platform_get_proc_address("glGetActiveUniform");
	window->renderer.GetActiveUniformBlockiv =
		(PFNGLGETACTIVEUNIFORMBLOCKIVPROC)_gfx_platform_get_proc_address("glGetActiveUniformBlockiv");
	window->renderer.GetActiveUniformsiv =
		(PFNGLGETACTIVEUNIFORMSIVPROC)_gfx_platform_get_proc_address("glGetActiveUniformsiv");
	window->renderer.GetBufferSubData =
		(PFNGLGETBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glGetBufferSubData");
	window->renderer.GetError =
		(PFNGLGETERRORPROC)glGetError;
	window->renderer.GetProgramInfoLog =
		(PFNGLGETPROGRAMINFOLOGPROC)_gfx_platform_get_proc_address("glGetProgramInfoLog");
	window->renderer.GetProgramiv =
		(PFNGLGETPROGRAMIVPROC)_gfx_platform_get_proc_address("glGetProgramiv");
	window->renderer.GetShaderInfoLog =
		(PFNGLGETSHADERINFOLOGPROC)_gfx_platform_get_proc_address("glGetShaderInfoLog");
	window->renderer.GetShaderiv =
		(PFNGLGETSHADERIVPROC)_gfx_platform_get_proc_address("glGetShaderiv");
	window->renderer.GetShaderSource =
		(PFNGLGETSHADERSOURCEPROC)_gfx_platform_get_proc_address("glGetShaderSource");
	window->renderer.GetStringi =
		(PFNGLGETSTRINGIPROC)_gfx_platform_get_proc_address("glGetStringi");
	window->renderer.GetUniformBlockIndex =
		(PFNGLGETUNIFORMBLOCKINDEXPROC)_gfx_platform_get_proc_address("glGetUniformBlockIndex");
	window->renderer.GetUniformIndices =
		(PFNGLGETUNIFORMINDICESPROC)_gfx_platform_get_proc_address("glGetUniformIndices");
	window->renderer.GetUniformLocation =
		(PFNGLGETUNIFORMLOCATIONPROC)_gfx_platform_get_proc_address("glGetUniformLocation");
	window->renderer.LinkProgram =
		(PFNGLLINKPROGRAMPROC)_gfx_platform_get_proc_address("glLinkProgram");
	window->renderer.MapBufferRange =
		(PFNGLMAPBUFFERRANGEPROC)_gfx_platform_get_proc_address("glMapBufferRange");
	window->renderer.NamedFramebufferTexture1D =
		(GFX_NAMEDFRAMEBUFFERTEXTURE1DPROC)_gfx_gl_named_framebuffer_texture_1d;
	window->renderer.NamedFramebufferTexture2D =
		(GFX_NAMEDFRAMEBUFFERTEXTURE2DPROC)_gfx_gl_named_framebuffer_texture_2d;
	window->renderer.PixelStorei =
		(PFNGLPIXELSTOREIPROC)glPixelStorei;
	window->renderer.PolygonMode =
		(PFNGLPOLYGONMODEPROC)glPolygonMode;
	window->renderer.ShaderSource =
		(PFNGLSHADERSOURCEPROC)_gfx_platform_get_proc_address("glShaderSource");
	window->renderer.StencilFuncSeparate =
		(PFNGLSTENCILFUNCSEPARATEPROC)_gfx_platform_get_proc_address("glStencilFuncSeparate");
	window->renderer.StencilOpSeparate =
		(PFNGLSTENCILOPSEPARATEPROC)_gfx_platform_get_proc_address("glStencilOpSeparate");
	window->renderer.TexBuffer =
		(PFNGLTEXBUFFERPROC)_gfx_platform_get_proc_address("glTexBuffer");
	window->renderer.TexImage1D =
		(PFNGLTEXIMAGE1DPROC)glTexImage1D;
	window->renderer.TexImage2D =
		(PFNGLTEXIMAGE2DPROC)glTexImage2D;
	window->renderer.TexImage2DMultisample =
		(PFNGLTEXIMAGE2DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTexImage2DMultisample");
	window->renderer.TexImage3D =
		(PFNGLTEXIMAGE3DPROC)_gfx_platform_get_proc_address("glTexImage3D");
	window->renderer.TexImage3DMultisample =
		(PFNGLTEXIMAGE3DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTexImage3DMultisample");
	window->renderer.TexParameteri =
		(PFNGLTEXPARAMETERIPROC)glTexParameteri;
	window->renderer.TexSubImage1D =
		(PFNGLTEXSUBIMAGE1DPROC)glTexSubImage1D;
	window->renderer.TexSubImage2D =
		(PFNGLTEXSUBIMAGE2DPROC)glTexSubImage2D;
	window->renderer.TexSubImage3D =
		(PFNGLTEXSUBIMAGE3DPROC)_gfx_platform_get_proc_address("glTexSubImage3D");
	window->renderer.TransformFeedbackVaryings =
		(PFNGLTRANSFORMFEEDBACKVARYINGSPROC)_gfx_platform_get_proc_address("glTransformFeedbackVaryings");
	window->renderer.Uniform1fv =
		(PFNGLUNIFORM1FVPROC)_gfx_platform_get_proc_address("glUniform1fv");
	window->renderer.Uniform1iv =
		(PFNGLUNIFORM1IVPROC)_gfx_platform_get_proc_address("glUniform1iv");
	window->renderer.Uniform1uiv =
		(PFNGLUNIFORM1UIVPROC)_gfx_platform_get_proc_address("glUniform1uiv");
	window->renderer.Uniform2fv =
		(PFNGLUNIFORM2FVPROC)_gfx_platform_get_proc_address("glUniform2fv");
	window->renderer.Uniform2iv =
		(PFNGLUNIFORM2IVPROC)_gfx_platform_get_proc_address("glUniform2iv");
	window->renderer.Uniform2uiv =
		(PFNGLUNIFORM2UIVPROC)_gfx_platform_get_proc_address("glUniform2uiv");
	window->renderer.Uniform3fv =
		(PFNGLUNIFORM3FVPROC)_gfx_platform_get_proc_address("glUniform3fv");
	window->renderer.Uniform3iv =
		(PFNGLUNIFORM3IVPROC)_gfx_platform_get_proc_address("glUniform3iv");
	window->renderer.Uniform3uiv =
		(PFNGLUNIFORM3UIVPROC)_gfx_platform_get_proc_address("glUniform3uiv");
	window->renderer.Uniform4fv =
		(PFNGLUNIFORM4FVPROC)_gfx_platform_get_proc_address("glUniform4fv");
	window->renderer.Uniform4iv =
		(PFNGLUNIFORM4IVPROC)_gfx_platform_get_proc_address("glUniform4iv");
	window->renderer.Uniform4uiv =
		(PFNGLUNIFORM4UIVPROC)_gfx_platform_get_proc_address("glUniform4uiv");
	window->renderer.UniformBlockBinding =
		(PFNGLUNIFORMBLOCKBINDINGPROC)_gfx_platform_get_proc_address("glUniformBlockBinding");
	window->renderer.UniformMatrix2fv =
		(PFNGLUNIFORMMATRIX2FVPROC)_gfx_platform_get_proc_address("glUniformMatrix2fv");
	window->renderer.UniformMatrix3fv =
		(PFNGLUNIFORMMATRIX3FVPROC)_gfx_platform_get_proc_address("glUniformMatrix3fv");
	window->renderer.UniformMatrix4fv =
		(PFNGLUNIFORMMATRIX4FVPROC)_gfx_platform_get_proc_address("glUniformMatrix4fv");
	window->renderer.UnmapBuffer =
		(PFNGLUNMAPBUFFERPROC)_gfx_platform_get_proc_address("glUnmapBuffer");
	window->renderer.UseProgram =
		(PFNGLUSEPROGRAMPROC)_gfx_platform_get_proc_address("glUseProgram");
	window->renderer.VertexAttribIPointer =
		(PFNGLVERTEXATTRIBIPOINTERPROC)_gfx_platform_get_proc_address("glVertexAttribIPointer");
	window->renderer.VertexAttribPointer =
		(PFNGLVERTEXATTRIBPOINTERPROC)_gfx_platform_get_proc_address("glVertexAttribPointer");
	window->renderer.Viewport =
		(PFNGLVIEWPORTPROC)glViewport;

	/* GFX_EXT_DIRECT_STATE_ACCESS */
	if(
		window->context.major > 4 ||
		(window->context.major == 4 && window->context.minor > 4) ||
		_gfx_is_extension_supported("GL_ARB_direct_state_access", window))
	{
		window->flags[GFX_EXT_DIRECT_STATE_ACCESS] = 1;

		window->renderer.BindTextureUnit =
			(PFNGLBINDTEXTUREUNITPROC)_gfx_platform_get_proc_address("glBindTextureUnit");
		window->renderer.CopyNamedBufferSubData =
			(PFNGLCOPYNAMEDBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glCopyNamedBufferSubData");
		window->renderer.CreateBuffers =
			(PFNGLCREATEBUFFERSPROC)_gfx_platform_get_proc_address("glCreateBuffers");
		window->renderer.CreateFramebuffers =
			(PFNGLCREATEFRAMEBUFFERSPROC)_gfx_platform_get_proc_address("glCreateFramebuffers");
		window->renderer.CreateTextures =
			(PFNGLCREATETEXTURESPROC)_gfx_platform_get_proc_address("glCreateTextures");
		window->renderer.CreateVertexArrays =
			(PFNGLCREATEVERTEXARRAYSPROC)_gfx_platform_get_proc_address("glCreateVertexArrays");
		window->renderer.DisableVertexArrayAttrib =
			(PFNGLDISABLEVERTEXARRAYATTRIBPROC)_gfx_platform_get_proc_address("glDisableVertexArrayAttrib");
		window->renderer.EnableVertexArrayAttrib =
			(PFNGLENABLEVERTEXARRAYATTRIBPROC)_gfx_platform_get_proc_address("glEnableVertexArrayAttrib");
		window->renderer.GenerateTextureMipmap =
			(PFNGLGENERATETEXTUREMIPMAPPROC)_gfx_platform_get_proc_address("glGenerateTextureMipmap");
		window->renderer.GetNamedBufferSubData =
			(PFNGLGETNAMEDBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glGetNamedBufferSubData");
		window->renderer.MapNamedBufferRange =
			(PFNGLMAPNAMEDBUFFERRANGEPROC)_gfx_platform_get_proc_address("glMapNamedBufferRange");
		window->renderer.NamedBufferData =
			(PFNGLNAMEDBUFFERDATAPROC)_gfx_platform_get_proc_address("glNamedBufferData");
		window->renderer.NamedBufferSubData =
			(PFNGLNAMEDBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glNamedBufferSubData");
		window->renderer.NamedFramebufferDrawBuffers =
			(PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC)_gfx_platform_get_proc_address("glNamedFramebufferDrawBuffers");
		window->renderer.NamedFramebufferTexture =
			(PFNGLNAMEDFRAMEBUFFERTEXTUREPROC)_gfx_platform_get_proc_address("glNamedFramebufferTexture");
		window->renderer.NamedFramebufferTextureLayer =
			(PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC)_gfx_platform_get_proc_address("glNamedFramebufferTextureLayer");
		window->renderer.TextureBuffer =
			(PFNGLTEXTUREBUFFERPROC)_gfx_platform_get_proc_address("glTextureBuffer");
		window->renderer.TextureParameteri =
			(PFNGLTEXTUREPARAMETERIPROC)_gfx_platform_get_proc_address("glTextureParameteri");
		window->renderer.TextureStorage1D =
			(PFNGLTEXTURESTORAGE1DPROC)_gfx_platform_get_proc_address("glTextureStorage1D");
		window->renderer.TextureStorage2D =
			(PFNGLTEXTURESTORAGE2DPROC)_gfx_platform_get_proc_address("glTextureStorage2D");
		window->renderer.TextureStorage2DMultisample =
			(PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTextureStorage2DMultisample");
		window->renderer.TextureStorage3D =
			(PFNGLTEXTURESTORAGE3DPROC)_gfx_platform_get_proc_address("glTextureStorage3D");
		window->renderer.TextureStorage3DMultisample =
			(PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTextureStorage3DMultisample");
		window->renderer.TextureSubImage1D =
			(PFNGLTEXTURESUBIMAGE1DPROC)_gfx_platform_get_proc_address("glTextureSubImage1D");
		window->renderer.TextureSubImage2D =
			(PFNGLTEXTURESUBIMAGE2DPROC)_gfx_platform_get_proc_address("glTextureSubImage2D");
		window->renderer.TextureSubImage3D =
			(PFNGLTEXTURESUBIMAGE3DPROC)_gfx_platform_get_proc_address("glTextureSubImage3D");
		window->renderer.UnmapNamedBuffer =
			(PFNGLUNMAPNAMEDBUFFERPROC)_gfx_platform_get_proc_address("glUnmapNamedBuffer");
	}

	else
	{
		window->flags[GFX_EXT_DIRECT_STATE_ACCESS] = 0;

		window->renderer.BindTextureUnit              = _gfx_gl_bind_texture_unit;
		window->renderer.CopyNamedBufferSubData       = _gfx_gl_copy_named_buffer_sub_data;
		window->renderer.CreateBuffers                = _gfx_gl_create_buffers;
		window->renderer.CreateFramebuffers           = _gfx_gl_create_framebuffers;
		window->renderer.CreateTextures               = _gfx_gl_create_textures;
		window->renderer.CreateVertexArrays           = _gfx_gl_create_vertex_arrays;
		window->renderer.DisableVertexArrayAttrib     = _gfx_gl_disable_vertex_array_attrib;
		window->renderer.EnableVertexArrayAttrib      = _gfx_gl_enable_vertex_array_attrib;
		window->renderer.GenerateTextureMipmap        = _gfx_gl_generate_texture_mipmap;
		window->renderer.GetNamedBufferSubData        = _gfx_gl_get_named_buffer_sub_data;
		window->renderer.MapNamedBufferRange          = _gfx_gl_map_named_buffer_range;
		window->renderer.NamedBufferData              = _gfx_gl_named_buffer_data;
		window->renderer.NamedBufferSubData           = _gfx_gl_named_buffer_sub_data;
		window->renderer.NamedFramebufferDrawBuffers  = _gfx_gl_named_framebuffer_draw_buffers;
		window->renderer.NamedFramebufferTexture      = _gfx_gl_named_framebuffer_texture;
		window->renderer.NamedFramebufferTextureLayer = _gfx_gl_named_framebuffer_texture_layer;
		window->renderer.TextureBuffer                = _gfx_gl_texture_buffer;
		window->renderer.TextureParameteri            = _gfx_gl_texture_parameter_i;
		window->renderer.TextureStorage1D             = _gfx_gl_texture_storage_1d;
		window->renderer.TextureStorage2D             = _gfx_gl_texture_storage_2d;
		window->renderer.TextureStorage2DMultisample  = _gfx_gl_texture_storage_2d_multisample;
		window->renderer.TextureStorage3D             = _gfx_gl_texture_storage_3d;
		window->renderer.TextureStorage3DMultisample  = _gfx_gl_texture_storage_3d_multisample;
		window->renderer.TextureSubImage1D            = _gfx_gl_texture_sub_image_1d;
		window->renderer.TextureSubImage2D            = _gfx_gl_texture_sub_image_2d;
		window->renderer.TextureSubImage3D            = _gfx_gl_texture_sub_image_3d;
		window->renderer.UnmapNamedBuffer             = _gfx_gl_unmap_named_buffer;
	}

	/* GFX_EXT_IMMUTABLE_TEXTURE */
	if(
		window->context.major > 4 ||
		(window->context.major == 4 && window->context.minor > 1) ||
		_gfx_is_extension_supported("GL_ARB_texture_storage", window))
	{
		window->flags[GFX_EXT_IMMUTABLE_TEXTURE] = 1;

		window->renderer.TexStorage1D =
			(PFNGLTEXSTORAGE1DPROC)_gfx_platform_get_proc_address("glTexStorage1D");
		window->renderer.TexStorage2D =
			(PFNGLTEXSTORAGE2DPROC)_gfx_platform_get_proc_address("glTexStorage2D");
		window->renderer.TexStorage3D =
			(PFNGLTEXSTORAGE3DPROC)_gfx_platform_get_proc_address("glTexStorage3D");
	}

	else
	{
		window->flags[GFX_EXT_IMMUTABLE_TEXTURE] = 0;

		window->renderer.TexStorage1D = _gfx_gl_tex_storage_1d;
		window->renderer.TexStorage2D = _gfx_gl_tex_storage_2d;
		window->renderer.TexStorage3D = _gfx_gl_tex_storage_3d;
	}

	/* GFX_EXT_IMMUTABLE_MULTISAMPLE_TEXTURE */
	if(
		window->context.major > 4 ||
		(window->context.major == 4 && window->context.minor > 2) ||
		_gfx_is_extension_supported("GL_ARB_texture_storage_multisample", window))
	{
		window->flags[GFX_EXT_IMMUTABLE_MULTISAMPLE_TEXTURE] = 1;

		window->renderer.TexStorage2DMultisample =
			(PFNGLTEXSTORAGE2DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTexStorage2DMultisample");
		window->renderer.TexStorage3DMultisample =
			(PFNGLTEXSTORAGE3DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTexStorage3DMultisample");
	}

	else
	{
		window->flags[GFX_EXT_IMMUTABLE_MULTISAMPLE_TEXTURE] = 0;

		window->renderer.TexStorage2DMultisample = _gfx_gl_tex_storage_2d_multisample;
		window->renderer.TexStorage3DMultisample = _gfx_gl_tex_storage_3d_multisample;
	}

	/* GFX_EXT_INSTANCED_ATTRIBUTES */
	if(
		window->context.major > 3 ||
		(window->context.major == 3 && window->context.minor > 2))
	{
		window->flags[GFX_EXT_INSTANCED_ATTRIBUTES] = 1;

		window->renderer.VertexAttribDivisor =
			(PFNGLVERTEXATTRIBDIVISORPROC)_gfx_platform_get_proc_address("glVertexAttribDivisor");
	}

	else if(_gfx_is_extension_supported("GL_ARB_instanced_arrays", window))
	{
		window->flags[GFX_EXT_INSTANCED_ATTRIBUTES] = 1;

		window->renderer.VertexAttribDivisor =
			(PFNGLVERTEXATTRIBDIVISORPROC)_gfx_platform_get_proc_address("glVertexAttribDivisorARB");
	}

	else
	{
		window->flags[GFX_EXT_INSTANCED_ATTRIBUTES] = 0;

		window->renderer.VertexAttribDivisor = _gfx_gl_vertex_attrib_divisor;
	}

	/* GFX_EXT_INSTANCED_BASE_ATTRIBUTES */
	if(
		window->context.major > 4 ||
		(window->context.major == 4 && window->context.minor > 1) ||
		_gfx_is_extension_supported("GL_ARB_base_instance", window))
	{
		window->flags[GFX_EXT_INSTANCED_BASE_ATTRIBUTES] = 1;

		window->renderer.DrawArraysInstancedBaseInstance =
			(PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC)_gfx_platform_get_proc_address("glDrawArraysInstancedBaseInstance");
		window->renderer.DrawElementsInstancedBaseInstance =
			(PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC)_gfx_platform_get_proc_address("glDrawElementsInstancedBaseInstance");
	}

	else
	{
		window->flags[GFX_EXT_INSTANCED_BASE_ATTRIBUTES] = 0;

		window->renderer.DrawArraysInstancedBaseInstance = _gfx_gl_draw_arrays_instanced_base_instance;
		window->renderer.DrawElementsInstancedBaseInstance = _gfx_gl_draw_elements_instanced_base_instance;
	}

	/* GFX_EXT_LAYERED_CUBEMAP */
	if(
		window->context.major > 3 ||
		_gfx_is_extension_supported("GL_ARB_texture_cube_map_array", window))
	{
		window->flags[GFX_EXT_LAYERED_CUBEMAP] = 1;
	}

	else
	{
		window->flags[GFX_EXT_LAYERED_CUBEMAP] = 0;
	}

	/* GFX_EXT_PROGRAM_BINARY */
	if(
		window->context.major > 4 ||
		(window->context.major == 4 && window->context.minor > 0) ||
		_gfx_is_extension_supported("GL_ARB_get_program_binary", window))
	{
		window->flags[GFX_EXT_PROGRAM_BINARY] = 1;

		window->renderer.GetProgramBinary =
			(PFNGLGETPROGRAMBINARYPROC)_gfx_platform_get_proc_address("glGetProgramBinary");
		window->renderer.ProgramBinary =
			(PFNGLPROGRAMBINARYPROC)_gfx_platform_get_proc_address("glProgramBinary");
		window->renderer.ProgramParameteri =
			(PFNGLPROGRAMPARAMETERIPROC)_gfx_platform_get_proc_address("glProgramParameteri");
	}

	else
	{
		window->flags[GFX_EXT_PROGRAM_BINARY] = 0;

		window->renderer.GetProgramBinary  = _gfx_gl_get_program_binary;
		window->renderer.ProgramBinary     = _gfx_gl_program_binary;
		window->renderer.ProgramParameteri = _gfx_gl_program_parameter_i;
	}

	/* GFX_EXT_TESSELLATION_SHADER */
	if(
		window->context.major > 3 ||
		_gfx_is_extension_supported("GL_ARB_tessellation_shader", window))
	{
		glGetIntegerv(GL_MAX_PATCH_VERTICES, &limit);
		window->limits[GFX_LIM_MAX_PATCH_VERTICES] = limit;
		window->flags[GFX_EXT_TESSELLATION_SHADER] = 1;

		window->renderer.PatchParameteri =
			(PFNGLPATCHPARAMETERIPROC)_gfx_platform_get_proc_address("glPatchParameteri");
	}

	else
	{
		window->limits[GFX_LIM_MAX_PATCH_VERTICES] = 0;
		window->flags[GFX_EXT_TESSELLATION_SHADER] = 0;

		window->renderer.PatchParameteri = _gfx_gl_patch_parameter_i;
	}

#endif

	/* Set other defaults */
	window->renderer.fbos[0] = 0;
	window->renderer.fbos[1] = 0;
	window->renderer.vao     = 0;
	window->renderer.program = 0;
	window->renderer.post    = 0;

	_gfx_platform_window_get_size(
		window->handle,
		&window->renderer.width,
		&window->renderer.height
	);
	window->renderer.x = 0;
	window->renderer.y = 0;

	window->renderer.packAlignment = 0;
	window->renderer.unpackAlignment = 0;
	window->renderer.patchVertices = 0;

	window->renderer.uniformBuffers = NULL;
	window->renderer.textureUnits = NULL;
}

#endif // GFX_RENDERER_GL
