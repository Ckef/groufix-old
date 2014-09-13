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

		const char* ext)
{
	GLint num;
	glGetIntegerv(GL_NUM_EXTENSIONS, &num);

	while(num) if(!strcmp(
		(const char*)(GFX_RND).GetStringi(GL_EXTENSIONS, --num),
		(const char*)ext))
	{
		return 1;
	}

	return 0;
}

/******************************************************/
void _gfx_renderer_load(void)
{
	/* Get current (GFX_WND) and context */
	if(!GFX_WND) return;

	/* Get OpenGL constants (a.k.a hardware limits) */
	GLint limit;

	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &limit);
		(GFX_WND)->limits[GFX_LIM_MAX_BUFFER_PROPERTIES] = limit;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &limit);
		(GFX_WND)->limits[GFX_LIM_MAX_COLOR_ATTACHMENTS] = limit;
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &limit);
		(GFX_WND)->limits[GFX_LIM_MAX_COLOR_TARGETS] = limit;
	glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &limit);
		(GFX_WND)->limits[GFX_LIM_MAX_CUBEMAP_SIZE] = limit;
	glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS, &limit);
		(GFX_WND)->limits[GFX_LIM_MAX_FEEDBACK_BUFFERS] = limit;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &limit);
		(GFX_WND)->limits[GFX_LIM_MAX_SAMPLER_PROPERTIES] = limit;
	glGetIntegerv(GL_MAX_SAMPLES, &limit);
		(GFX_WND)->limits[GFX_LIM_MAX_SAMPLES] = limit;
	glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &limit);
		(GFX_WND)->limits[GFX_LIM_MAX_TEXTURE_3D_SIZE] = limit;
	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &limit);
		(GFX_WND)->limits[GFX_LIM_MAX_TEXTURE_LAYERS] = limit;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &limit);
		(GFX_WND)->limits[GFX_LIM_MAX_TEXTURE_SIZE] = limit;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &limit);
		(GFX_WND)->limits[GFX_LIM_MAX_VERTEX_ATTRIBS] = limit;

#if defined(GFX_GLES)

	/* Get OpenGL constants (a.k.a hardware limits) */
	(GFX_WND)->limits[GFX_LIM_MAX_BUFFER_TEXTURE_SIZE] = 0;
	(GFX_WND)->limits[GFX_LIM_MAX_PATCH_VERTICES] = 0;

	/* Default Extensions */
	(GFX_WND)->flags[GFX_EXT_BUFFER_TEXTURE]              = 0;
	(GFX_WND)->flags[GFX_EXT_DIRECT_STATE_ACCESS]         = 0;
	(GFX_WND)->flags[GFX_EXT_GEOMETRY_SHADER]             = 0;
	(GFX_WND)->flags[GFX_EXT_IMMUTABLE_TEXTURE]           = 1;
	(GFX_WND)->flags[GFX_EXT_INSTANCED_ATTRIBUTES]        = 1;
	(GFX_WND)->flags[GFX_EXT_INSTANCED_BASE_ATTRIBUTES]   = 0;
	(GFX_WND)->flags[GFX_EXT_LAYERED_CUBEMAP]             = 0;
	(GFX_WND)->flags[GFX_EXT_LAYERED_MULTISAMPLE_TEXTURE] = 0;
	(GFX_WND)->flags[GFX_EXT_POLYGON_STATE]               = 0;
	(GFX_WND)->flags[GFX_EXT_PROGRAM_BINARY]              = 1;
	(GFX_WND)->flags[GFX_EXT_SEAMLESS_CUBEMAP]            = 0;
	(GFX_WND)->flags[GFX_EXT_TESSELLATION_SHADER]         = 0;
	(GFX_WND)->flags[GFX_EXT_TEXTURE_1D]                  = 0;

	/* GLES, assumes 3.0+ */
	(GFX_RND).ActiveTexture                     = glActiveTexture;
	(GFX_RND).AttachShader                      = glAttachShader;
	(GFX_RND).BeginTransformFeedback            = glBeginTransformFeedback;
	(GFX_RND).BindAttribLocation                = glBindAttribLocation;
	(GFX_RND).BindBuffer                        = glBindBuffer;
	(GFX_RND).BindBufferRange                   = glBindBufferRange;
	(GFX_RND).BindFramebuffer                   = glBindFramebuffer;
	(GFX_RND).BindTexture                       = glBindTexture;
	(GFX_RND).BindTextureUnit                   = glBindTextureUnit;
	(GFX_RND).BindVertexArray                   = glBindVertexArray;
	(GFX_RND).BlendEquationSeparate             = glBlendEquationSeparate;
	(GFX_RND).BlendFuncSeparate                 = glBlendFuncSeparate;
	(GFX_RND).BufferData                        = glBufferData;
	(GFX_RND).BufferSubData                     = glBufferSubData;
	(GFX_RND).Clear                             = glClear;
	(GFX_RND).CompileShader                     = glCompileShader;
	(GFX_RND).CopyBufferSubData                 = glCopyBufferSubData;
	(GFX_RND).CopyNamedBufferSubData            = _gfx_gl_copy_named_buffer_sub_data;
	(GFX_RND).CreateBuffers                     = _gfx_gl_create_buffers;
	(GFX_RND).CreateFramebuffers                = _gfx_gl_create_framebuffers;
	(GFX_RND).CreateProgram                     = glCreateProgram;
	(GFX_RND).CreateShader                      = glCreateShader;
	(GFX_RND).CreateTextures                    = _gfx_gl_create_textures;
	(GFX_RND).CreateVertexArrays                = _gfx_gl_create_vertex_arrays;
	(GFX_RND).CullFace                          = glCullFace;
	(GFX_RND).DeleteBuffers                     = glDeleteBuffers;
	(GFX_RND).DeleteFramebuffers                = glDeleteFramebuffers;
	(GFX_RND).DeleteProgram                     = glDeleteProgram;
	(GFX_RND).DeleteShader                      = glDeleteShader;
	(GFX_RND).DeleteTextures                    = glDeleteTextures;
	(GFX_RND).DeleteVertexArrays                = glDeleteVertexArrays;
	(GFX_RND).DepthFunc                         = glDepthFunc;
	(GFX_RND).DepthMask                         = glDepthMask;
	(GFX_RND).DetachShader                      = glDetachShader;
	(GFX_RND).Disable                           = glDisable;
	(GFX_RND).DisableVertexArrayAttrib          = _gfx_gl_disable_vertex_array_attrib;
	(GFX_RND).DisableVertexAttribArray          = glDisableVertexAttribArray;
	(GFX_RND).DrawArrays                        = glDrawArrays;
	(GFX_RND).DrawArraysInstanced               = glDrawArraysInstanced;
	(GFX_RND).DrawArraysInstancedBaseInstance   = _gfx_gl_draw_arrays_instanced_base_instance;
	(GFX_RND).DrawBuffers                       = glDrawBuffers;
	(GFX_RND).DrawElements                      = glDrawElements;
	(GFX_RND).DrawElementsInstanced             = glDrawElementsInstanced;
	(GFX_RND).DrawElementsInstancedBaseInstance = _gfx_gl_draw_elements_instanced_base_instance;
	(GFX_RND).Enable                            = glEnable;
	(GFX_RND).EnableVertexArrayAttrib           = _gfx_gl_enable_vertex_array_attrib;
	(GFX_RND).EnableVertexAttribArray           = glEnableVertexAttribArray;
	(GFX_RND).EndTransformFeedback              = glEndTransformFeedback;
	(GFX_RND).FramebufferTexture                = _gfx_gles_framebuffer_texture;
	(GFX_RND).FramebufferTexture1D              = _gfx_gles_framebuffer_texture_1d;
	(GFX_RND).FramebufferTexture2D              = glFramebufferTexture2D;
	(GFX_RND).FramebufferTextureLayer           = glFramebufferTextureLayer;
	(GFX_RND).GenBuffers                        = glGenBuffers;
	(GFX_RND).GenerateMipmap                    = glGenerateMipmap;
	(GFX_RND).GenerateTextureMipmap             = glGenerateTextureMipmap;
	(GFX_RND).GenFramebuffers                   = glGenFramebuffers;
	(GFX_RND).GenTextures                       = glGenTextures;
	(GFX_RND).GenVertexArrays                   = glGenVertexArrays;
	(GFX_RND).GetActiveUniform                  = glGetActiveUniform;
	(GFX_RND).GetActiveUniformBlockiv           = glGetActiveUniformBlockiv;
	(GFX_RND).GetActiveUniformsiv               = glGetActiveUniformsiv;
	(GFX_RND).GetBufferSubData                  = _gfx_gles_get_buffer_sub_data;
	(GFX_RND).GetError                          = glGetError;
	(GFX_RND).GetNamedBufferSubData             = _gfx_gl_get_named_buffer_sub_data;
	(GFX_RND).GetProgramBinary                  = glGetProgramBinary;
	(GFX_RND).GetProgramInfoLog                 = glGetProgramInfoLog;
	(GFX_RND).GetProgramiv                      = glGetProgramiv;
	(GFX_RND).GetShaderInfoLog                  = glGetShaderInfoLog;
	(GFX_RND).GetShaderiv                       = glGetShaderiv;
	(GFX_RND).GetShaderSource                   = glGetShaderSource;
	(GFX_RND).GetStringi                        = glGetStringi;
	(GFX_RND).GetUniformBlockIndex              = glGetUniformBlockIndex;
	(GFX_RND).GetUniformIndices                 = glGetUniformIndices;
	(GFX_RND).GetUniformLocation                = glGetUniformLocation;
	(GFX_RND).LinkProgram                       = glLinkProgram;
	(GFX_RND).MapBufferRange                    = glMapBufferRange;
	(GFX_RND).MapNamedBufferRange               = glMapNamedBufferRange;
	(GFX_RND).NamedBufferData                   = _gfx_gl_named_buffer_data;
	(GFX_RND).NamedBufferSubData                = _gfx_gl_named_buffer_sub_data;
	(GFX_RND).NamedFramebufferDrawBuffers       = _gfx_gl_named_framebuffer_draw_buffers;
	(GFX_RND).NamedFramebufferTexture           = _gfx_gl_named_framebuffer_texture;
	(GFX_RND).NamedFramebufferTexture1D         = _gfx_gles_named_framebuffer_texture_1d;
	(GFX_RND).NamedFramebufferTexture2D         = _gfx_gles_named_framebuffer_texture_2d;
	(GFX_RND).NamedFramebufferTextureLayer      = _gfx_gl_named_framebuffer_texture_layer;
	(GFX_RND).PatchParameteri                   = _gfx_gl_patch_parameter_i;
	(GFX_RND).PixelStorei                       = glPixelStorei;
	(GFX_RND).PolygonMode                       = _gfx_gles_polygon_mode;
	(GFX_RND).ProgramBinary                     = glProgramBinary;
	(GFX_RND).ProgramParameteri                 = glProgramParameteri;
	(GFX_RND).ShaderSource                      = glShaderSource;
	(GFX_RND).StencilFuncSeparate               = glStencilFuncSeparate;
	(GFX_RND).StencilOpSeparate                 = glStencilOpSeparate;
	(GFX_RND).TexBuffer                         = _gfx_gles_tex_buffer;
	(GFX_RND).TexImage1D                        = _gfx_gles_tex_image_1d;
	(GFX_RND).TexImage2D                        = glTexImage2D;
	(GFX_RND).TexImage2DMultisample             = _gfx_gles_tex_image_2d_multisample;
	(GFX_RND).TexImage3D                        = glTexImage3D;
	(GFX_RND).TexImage3DMultisample             = _gfx_gles_tex_image_3d_multisample;
	(GFX_RND).TexParameteri                     = glTexParameteri;
	(GFX_RND).TexStorage1D                      = _gfx_gles_tex_storage_1d;
	(GFX_RND).TexStorage2D                      = glTexStorage2D;
	(GFX_RND).TexStorage3D                      = glTexStorage3D;
	(GFX_RND).TexStorage3DMultisample           = _gfx_gles_tex_storage_3d_multisample;
	(GFX_RND).TexSubImage1D                     = _gfx_gles_tex_sub_image_1d;
	(GFX_RND).TexSubImage2D                     = glTexSubImage2D;
	(GFX_RND).TexSubImage3D                     = glTexSubImage3D;
	(GFX_RND).TextureBuffer                     = _gfx_gl_texture_buffer;
	(GFX_RND).TextureParameteri                 = _gfx_gl_texture_parameter_i;
	(GFX_RND).TextureStorage1D                  = _gfx_gl_texture_storage_1d;
	(GFX_RND).TextureStorage2D                  = _gfx_gl_texture_storage_2d;
	(GFX_RND).TextureStorage2DMultisample       = _gfx_gl_texture_storage_2d_multisample;
	(GFX_RND).TextureStorage3D                  = _gfx_gl_texture_storage_3d;
	(GFX_RND).TextureStorage3DMultisample       = _gfx_gl_texture_storage_3d_multisample;
	(GFX_RND).TextureSubImage1D                 = _gfx_gl_texture_sub_image_1d;
	(GFX_RND).TextureSubImage2D                 = _gfx_gl_texture_sub_image_2d;
	(GFX_RND).TextureSubImage3D                 = _gfx_gl_texture_sub_image_3d;
	(GFX_RND).TransformFeedbackVaryings         = glTransformFeedbackVaryings;
	(GFX_RND).Uniform1fv                        = glUniform1fv;
	(GFX_RND).Uniform1iv                        = glUniform1iv;
	(GFX_RND).Uniform1uiv                       = glUniform1uiv;
	(GFX_RND).Uniform2fv                        = glUniform2fv;
	(GFX_RND).Uniform2iv                        = glUniform2iv;
	(GFX_RND).Uniform2uiv                       = glUniform2uiv;
	(GFX_RND).Uniform3fv                        = glUniform3fv;
	(GFX_RND).Uniform3iv                        = glUniform3iv;
	(GFX_RND).Uniform3uiv                       = glUniform3uiv;
	(GFX_RND).Uniform4fv                        = glUniform4fv;
	(GFX_RND).Uniform4iv                        = glUniform4iv;
	(GFX_RND).Uniform4uiv                       = glUniform4uiv;
	(GFX_RND).UniformBlockBinding               = glUniformBlockBinding;
	(GFX_RND).UniformMatrix2fv                  = glUniformMatrix2fv;
	(GFX_RND).UniformMatrix3fv                  = glUniformMatrix3fv;
	(GFX_RND).UniformMatrix4fv                  = glUniformMatrix4fv;
	(GFX_RND).UnmapBuffer                       = glUnmapBuffer;
	(GFX_RND).UnmapNamedBuffer                  = _gfx_gl_unmap_named_buffer;
	(GFX_RND).UseProgram                        = glUseProgram;
	(GFX_RND).VertexAttribDivisor               = glVertexAttribDivisor;
	(GFX_RND).VertexAttribIPointer              = glVertexAttribIPointer;
	(GFX_RND).VertexAttribPointer               = glVertexAttribPointer;
	(GFX_RND).Viewport                          = glViewport;

	/* GFX_EXT_IMMUTABLE_MULTISAMPLE_TEXTURE */
	/* GFX_EXT_MULTISAMPLE_TEXTURE */
	if(
		(GFX_WND)->context.major > 3 ||
		((GFX_WND)->context.major == 3 && (GFX_WND)->context.minor > 0))
	{
		(GFX_WND)->flags[GFX_EXT_IMMUTABLE_MULTISAMPLE_TEXTURE] = 1;
		(GFX_WND)->flags[GFX_EXT_MULTISAMPLE_TEXTURE] = 1;

		(GFX_RND).TexStorage2DMultisample = glTexStorage2DMultisample;
	}

	else
	{
		(GFX_WND)->flags[GFX_EXT_IMMUTABLE_MULTISAMPLE_TEXTURE] = 0;
		(GFX_WND)->flags[GFX_EXT_MULTISAMPLE_TEXTURE] = 0;

		(GFX_RND).TexStorage2DMultisample = _gfx_gles_tex_storage_2d_multisample;
	}

#elif defined(GFX_GL)

	/* Settings */
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	/* Get OpenGL constants (a.k.a hardware limits) */
	glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &limit);
		(GFX_WND)->limits[GFX_LIM_MAX_BUFFER_TEXTURE_SIZE] = limit;

	/* Default Extensions */
	(GFX_WND)->flags[GFX_EXT_BUFFER_TEXTURE]              = 1;
	(GFX_WND)->flags[GFX_EXT_GEOMETRY_SHADER]             = 1;
	(GFX_WND)->flags[GFX_EXT_LAYERED_MULTISAMPLE_TEXTURE] = 1;
	(GFX_WND)->flags[GFX_EXT_MULTISAMPLE_TEXTURE]         = 1;
	(GFX_WND)->flags[GFX_EXT_POLYGON_STATE]               = 1;
	(GFX_WND)->flags[GFX_EXT_SEAMLESS_CUBEMAP]            = 1;
	(GFX_WND)->flags[GFX_EXT_TEXTURE_1D]                  = 1;

	/* Core, assumes 3.2+ context */
	(GFX_RND).ActiveTexture =
		(PFNGLACTIVETEXTUREPROC)_gfx_platform_get_proc_address("glActiveTexture");
	(GFX_RND).AttachShader =
		(PFNGLATTACHSHADERPROC)_gfx_platform_get_proc_address("glAttachShader");
	(GFX_RND).BeginTransformFeedback =
		(PFNGLBEGINTRANSFORMFEEDBACKPROC)_gfx_platform_get_proc_address("glBeginTransformFeedback");
	(GFX_RND).BindAttribLocation =
		(PFNGLBINDATTRIBLOCATIONPROC)_gfx_platform_get_proc_address("glBindAttribLocation");
	(GFX_RND).BindBuffer =
		(PFNGLBINDBUFFERPROC)_gfx_platform_get_proc_address("glBindBuffer");
	(GFX_RND).BindBufferRange =
		(PFNGLBINDBUFFERRANGEPROC)_gfx_platform_get_proc_address("glBindBufferRange");
	(GFX_RND).BindFramebuffer =
		(PFNGLBINDFRAMEBUFFERPROC)_gfx_platform_get_proc_address("glBindFramebuffer");
	(GFX_RND).BindTexture =
		(PFNGLBINDTEXTUREPROC)glBindTexture;
	(GFX_RND).BindVertexArray =
		(PFNGLBINDVERTEXARRAYPROC)_gfx_platform_get_proc_address("glBindVertexArray");
	(GFX_RND).BlendEquationSeparate =
		(PFNGLBLENDEQUATIONSEPARATEPROC)_gfx_platform_get_proc_address("glBlendEquationSeparate");
	(GFX_RND).BlendFuncSeparate =
		(PFNGLBLENDFUNCSEPARATEPROC)_gfx_platform_get_proc_address("glBlendFuncSeparate");
	(GFX_RND).BufferData =
		(PFNGLBUFFERDATAPROC)_gfx_platform_get_proc_address("glBufferData");
	(GFX_RND).BufferSubData =
		(PFNGLBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glBufferSubData");
	(GFX_RND).Clear =
		(PFNGLCLEARPROC)glClear;
	(GFX_RND).CompileShader =
		(PFNGLCOMPILESHADERPROC)_gfx_platform_get_proc_address("glCompileShader");
	(GFX_RND).CopyBufferSubData =
		(PFNGLCOPYBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glCopyBufferSubData");
	(GFX_RND).CreateProgram =
		(PFNGLCREATEPROGRAMPROC)_gfx_platform_get_proc_address("glCreateProgram");
	(GFX_RND).CreateShader =
		(PFNGLCREATESHADERPROC)_gfx_platform_get_proc_address("glCreateShader");
	(GFX_RND).CullFace =
		(PFNGLCULLFACEPROC)glCullFace;
	(GFX_RND).DeleteBuffers =
		(PFNGLDELETEBUFFERSPROC)_gfx_platform_get_proc_address("glDeleteBuffers");
	(GFX_RND).DeleteFramebuffers =
		(PFNGLDELETEFRAMEBUFFERSPROC)_gfx_platform_get_proc_address("glDeleteFramebuffers");
	(GFX_RND).DeleteProgram =
		(PFNGLDELETEPROGRAMPROC)_gfx_platform_get_proc_address("glDeleteProgram");
	(GFX_RND).DeleteShader =
		(PFNGLDELETESHADERPROC)_gfx_platform_get_proc_address("glDeleteShader");
	(GFX_RND).DeleteTextures =
		(PFNGLDELETETEXTURESPROC)glDeleteTextures;
	(GFX_RND).DeleteVertexArrays =
		(PFNGLDELETEVERTEXARRAYSPROC)_gfx_platform_get_proc_address("glDeleteVertexArrays");
	(GFX_RND).DepthFunc =
		(PFNGLDEPTHFUNCPROC)glDepthFunc;
	(GFX_RND).DepthMask =
		(PFNGLDEPTHMASKPROC)glDepthMask;
	(GFX_RND).DetachShader =
		(PFNGLDETACHSHADERPROC)_gfx_platform_get_proc_address("glDetachShader");
	(GFX_RND).Disable =
		(PFNGLDISABLEPROC)glDisable;
	(GFX_RND).DisableVertexAttribArray =
		(PFNGLDISABLEVERTEXATTRIBARRAYPROC)_gfx_platform_get_proc_address("glDisableVertexAttribArray");
	(GFX_RND).DrawArrays =
		(PFNGLDRAWARRAYSPROC)glDrawArrays;
	(GFX_RND).DrawArraysInstanced =
		(PFNGLDRAWARRAYSINSTANCEDPROC)_gfx_platform_get_proc_address("glDrawArraysInstanced");
	(GFX_RND).DrawBuffers =
		(PFNGLDRAWBUFFERSPROC)_gfx_platform_get_proc_address("glDrawBuffers");
	(GFX_RND).DrawElements =
		(PFNGLDRAWELEMENTSPROC)glDrawElements;
	(GFX_RND).DrawElementsInstanced =
		(PFNGLDRAWELEMENTSINSTANCEDPROC)_gfx_platform_get_proc_address("glDrawElementsInstanced");
	(GFX_RND).Enable =
		(PFNGLENABLEPROC)glEnable;
	(GFX_RND).EnableVertexAttribArray =
		(PFNGLENABLEVERTEXATTRIBARRAYPROC)_gfx_platform_get_proc_address("glEnableVertexAttribArray");
	(GFX_RND).EndTransformFeedback =
		(PFNGLENDTRANSFORMFEEDBACKPROC)_gfx_platform_get_proc_address("glEndTransformFeedback");
	(GFX_RND).FramebufferTexture =
		(PFNGLFRAMEBUFFERTEXTUREPROC)_gfx_platform_get_proc_address("glFramebufferTexture");
	(GFX_RND).FramebufferTexture1D =
		(PFNGLFRAMEBUFFERTEXTURE1DPROC)_gfx_platform_get_proc_address("glFramebufferTexture1D");
	(GFX_RND).FramebufferTexture2D =
		(PFNGLFRAMEBUFFERTEXTURE2DPROC)_gfx_platform_get_proc_address("glFramebufferTexture2D");
	(GFX_RND).FramebufferTextureLayer =
		(PFNGLFRAMEBUFFERTEXTURELAYERPROC)_gfx_platform_get_proc_address("glFramebufferTextureLayer");
	(GFX_RND).GenBuffers =
		(PFNGLGENBUFFERSPROC)_gfx_platform_get_proc_address("glGenBuffers");
	(GFX_RND).GenerateMipmap =
		(PFNGLGENERATEMIPMAPPROC)_gfx_platform_get_proc_address("glGenerateMipmap");
	(GFX_RND).GenFramebuffers =
		(PFNGLGENFRAMEBUFFERSPROC)_gfx_platform_get_proc_address("glGenFramebuffers");
	(GFX_RND).GenTextures =
		(PFNGLGENTEXTURESPROC)glGenTextures;
	(GFX_RND).GenVertexArrays =
		(PFNGLGENVERTEXARRAYSPROC)_gfx_platform_get_proc_address("glGenVertexArrays");
	(GFX_RND).GetActiveUniform =
		(PFNGLGETACTIVEUNIFORMPROC)_gfx_platform_get_proc_address("glGetActiveUniform");
	(GFX_RND).GetActiveUniformBlockiv =
		(PFNGLGETACTIVEUNIFORMBLOCKIVPROC)_gfx_platform_get_proc_address("glGetActiveUniformBlockiv");
	(GFX_RND).GetActiveUniformsiv =
		(PFNGLGETACTIVEUNIFORMSIVPROC)_gfx_platform_get_proc_address("glGetActiveUniformsiv");
	(GFX_RND).GetBufferSubData =
		(PFNGLGETBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glGetBufferSubData");
	(GFX_RND).GetError =
		(PFNGLGETERRORPROC)glGetError;
	(GFX_RND).GetProgramInfoLog =
		(PFNGLGETPROGRAMINFOLOGPROC)_gfx_platform_get_proc_address("glGetProgramInfoLog");
	(GFX_RND).GetProgramiv =
		(PFNGLGETPROGRAMIVPROC)_gfx_platform_get_proc_address("glGetProgramiv");
	(GFX_RND).GetShaderInfoLog =
		(PFNGLGETSHADERINFOLOGPROC)_gfx_platform_get_proc_address("glGetShaderInfoLog");
	(GFX_RND).GetShaderiv =
		(PFNGLGETSHADERIVPROC)_gfx_platform_get_proc_address("glGetShaderiv");
	(GFX_RND).GetShaderSource =
		(PFNGLGETSHADERSOURCEPROC)_gfx_platform_get_proc_address("glGetShaderSource");
	(GFX_RND).GetStringi =
		(PFNGLGETSTRINGIPROC)_gfx_platform_get_proc_address("glGetStringi");
	(GFX_RND).GetUniformBlockIndex =
		(PFNGLGETUNIFORMBLOCKINDEXPROC)_gfx_platform_get_proc_address("glGetUniformBlockIndex");
	(GFX_RND).GetUniformIndices =
		(PFNGLGETUNIFORMINDICESPROC)_gfx_platform_get_proc_address("glGetUniformIndices");
	(GFX_RND).GetUniformLocation =
		(PFNGLGETUNIFORMLOCATIONPROC)_gfx_platform_get_proc_address("glGetUniformLocation");
	(GFX_RND).LinkProgram =
		(PFNGLLINKPROGRAMPROC)_gfx_platform_get_proc_address("glLinkProgram");
	(GFX_RND).MapBufferRange =
		(PFNGLMAPBUFFERRANGEPROC)_gfx_platform_get_proc_address("glMapBufferRange");
	(GFX_RND).NamedFramebufferTexture1D =
		(GFX_NAMEDFRAMEBUFFERTEXTURE1DPROC)_gfx_gl_named_framebuffer_texture_1d;
	(GFX_RND).NamedFramebufferTexture2D =
		(GFX_NAMEDFRAMEBUFFERTEXTURE2DPROC)_gfx_gl_named_framebuffer_texture_2d;
	(GFX_RND).PixelStorei =
		(PFNGLPIXELSTOREIPROC)glPixelStorei;
	(GFX_RND).PolygonMode =
		(PFNGLPOLYGONMODEPROC)glPolygonMode;
	(GFX_RND).ShaderSource =
		(PFNGLSHADERSOURCEPROC)_gfx_platform_get_proc_address("glShaderSource");
	(GFX_RND).StencilFuncSeparate =
		(PFNGLSTENCILFUNCSEPARATEPROC)_gfx_platform_get_proc_address("glStencilFuncSeparate");
	(GFX_RND).StencilOpSeparate =
		(PFNGLSTENCILOPSEPARATEPROC)_gfx_platform_get_proc_address("glStencilOpSeparate");
	(GFX_RND).TexBuffer =
		(PFNGLTEXBUFFERPROC)_gfx_platform_get_proc_address("glTexBuffer");
	(GFX_RND).TexImage1D =
		(PFNGLTEXIMAGE1DPROC)glTexImage1D;
	(GFX_RND).TexImage2D =
		(PFNGLTEXIMAGE2DPROC)glTexImage2D;
	(GFX_RND).TexImage2DMultisample =
		(PFNGLTEXIMAGE2DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTexImage2DMultisample");
	(GFX_RND).TexImage3D =
		(PFNGLTEXIMAGE3DPROC)_gfx_platform_get_proc_address("glTexImage3D");
	(GFX_RND).TexImage3DMultisample =
		(PFNGLTEXIMAGE3DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTexImage3DMultisample");
	(GFX_RND).TexParameteri =
		(PFNGLTEXPARAMETERIPROC)glTexParameteri;
	(GFX_RND).TexSubImage1D =
		(PFNGLTEXSUBIMAGE1DPROC)glTexSubImage1D;
	(GFX_RND).TexSubImage2D =
		(PFNGLTEXSUBIMAGE2DPROC)glTexSubImage2D;
	(GFX_RND).TexSubImage3D =
		(PFNGLTEXSUBIMAGE3DPROC)_gfx_platform_get_proc_address("glTexSubImage3D");
	(GFX_RND).TransformFeedbackVaryings =
		(PFNGLTRANSFORMFEEDBACKVARYINGSPROC)_gfx_platform_get_proc_address("glTransformFeedbackVaryings");
	(GFX_RND).Uniform1fv =
		(PFNGLUNIFORM1FVPROC)_gfx_platform_get_proc_address("glUniform1fv");
	(GFX_RND).Uniform1iv =
		(PFNGLUNIFORM1IVPROC)_gfx_platform_get_proc_address("glUniform1iv");
	(GFX_RND).Uniform1uiv =
		(PFNGLUNIFORM1UIVPROC)_gfx_platform_get_proc_address("glUniform1uiv");
	(GFX_RND).Uniform2fv =
		(PFNGLUNIFORM2FVPROC)_gfx_platform_get_proc_address("glUniform2fv");
	(GFX_RND).Uniform2iv =
		(PFNGLUNIFORM2IVPROC)_gfx_platform_get_proc_address("glUniform2iv");
	(GFX_RND).Uniform2uiv =
		(PFNGLUNIFORM2UIVPROC)_gfx_platform_get_proc_address("glUniform2uiv");
	(GFX_RND).Uniform3fv =
		(PFNGLUNIFORM3FVPROC)_gfx_platform_get_proc_address("glUniform3fv");
	(GFX_RND).Uniform3iv =
		(PFNGLUNIFORM3IVPROC)_gfx_platform_get_proc_address("glUniform3iv");
	(GFX_RND).Uniform3uiv =
		(PFNGLUNIFORM3UIVPROC)_gfx_platform_get_proc_address("glUniform3uiv");
	(GFX_RND).Uniform4fv =
		(PFNGLUNIFORM4FVPROC)_gfx_platform_get_proc_address("glUniform4fv");
	(GFX_RND).Uniform4iv =
		(PFNGLUNIFORM4IVPROC)_gfx_platform_get_proc_address("glUniform4iv");
	(GFX_RND).Uniform4uiv =
		(PFNGLUNIFORM4UIVPROC)_gfx_platform_get_proc_address("glUniform4uiv");
	(GFX_RND).UniformBlockBinding =
		(PFNGLUNIFORMBLOCKBINDINGPROC)_gfx_platform_get_proc_address("glUniformBlockBinding");
	(GFX_RND).UniformMatrix2fv =
		(PFNGLUNIFORMMATRIX2FVPROC)_gfx_platform_get_proc_address("glUniformMatrix2fv");
	(GFX_RND).UniformMatrix3fv =
		(PFNGLUNIFORMMATRIX3FVPROC)_gfx_platform_get_proc_address("glUniformMatrix3fv");
	(GFX_RND).UniformMatrix4fv =
		(PFNGLUNIFORMMATRIX4FVPROC)_gfx_platform_get_proc_address("glUniformMatrix4fv");
	(GFX_RND).UnmapBuffer =
		(PFNGLUNMAPBUFFERPROC)_gfx_platform_get_proc_address("glUnmapBuffer");
	(GFX_RND).UseProgram =
		(PFNGLUSEPROGRAMPROC)_gfx_platform_get_proc_address("glUseProgram");
	(GFX_RND).VertexAttribIPointer =
		(PFNGLVERTEXATTRIBIPOINTERPROC)_gfx_platform_get_proc_address("glVertexAttribIPointer");
	(GFX_RND).VertexAttribPointer =
		(PFNGLVERTEXATTRIBPOINTERPROC)_gfx_platform_get_proc_address("glVertexAttribPointer");
	(GFX_RND).Viewport =
		(PFNGLVIEWPORTPROC)glViewport;

	/* GFX_EXT_DIRECT_STATE_ACCESS */
	if(
		(GFX_WND)->context.major > 4 ||
		((GFX_WND)->context.major == 4 && (GFX_WND)->context.minor > 4) ||
		_gfx_is_extension_supported("GL_ARB_direct_state_access"))
	{
		(GFX_WND)->flags[GFX_EXT_DIRECT_STATE_ACCESS] = 1;

		(GFX_RND).BindTextureUnit =
			(PFNGLBINDTEXTUREUNITPROC)_gfx_platform_get_proc_address("glBindTextureUnit");
		(GFX_RND).CopyNamedBufferSubData =
			(PFNGLCOPYNAMEDBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glCopyNamedBufferSubData");
		(GFX_RND).CreateBuffers =
			(PFNGLCREATEBUFFERSPROC)_gfx_platform_get_proc_address("glCreateBuffers");
		(GFX_RND).CreateFramebuffers =
			(PFNGLCREATEFRAMEBUFFERSPROC)_gfx_platform_get_proc_address("glCreateFramebuffers");
		(GFX_RND).CreateTextures =
			(PFNGLCREATETEXTURESPROC)_gfx_platform_get_proc_address("glCreateTextures");
		(GFX_RND).CreateVertexArrays =
			(PFNGLCREATEVERTEXARRAYSPROC)_gfx_platform_get_proc_address("glCreateVertexArrays");
		(GFX_RND).DisableVertexArrayAttrib =
			(PFNGLDISABLEVERTEXARRAYATTRIBPROC)_gfx_platform_get_proc_address("glDisableVertexArrayAttrib");
		(GFX_RND).EnableVertexArrayAttrib =
			(PFNGLENABLEVERTEXARRAYATTRIBPROC)_gfx_platform_get_proc_address("glEnableVertexArrayAttrib");
		(GFX_RND).GenerateTextureMipmap =
			(PFNGLGENERATETEXTUREMIPMAPPROC)_gfx_platform_get_proc_address("glGenerateTextureMipmap");
		(GFX_RND).GetNamedBufferSubData =
			(PFNGLGETNAMEDBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glGetNamedBufferSubData");
		(GFX_RND).MapNamedBufferRange =
			(PFNGLMAPNAMEDBUFFERRANGEPROC)_gfx_platform_get_proc_address("glMapNamedBufferRange");
		(GFX_RND).NamedBufferData =
			(PFNGLNAMEDBUFFERDATAPROC)_gfx_platform_get_proc_address("glNamedBufferData");
		(GFX_RND).NamedBufferSubData =
			(PFNGLNAMEDBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glNamedBufferSubData");
		(GFX_RND).NamedFramebufferDrawBuffers =
			(PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC)_gfx_platform_get_proc_address("glNamedFramebufferDrawBuffers");
		(GFX_RND).NamedFramebufferTexture =
			(PFNGLNAMEDFRAMEBUFFERTEXTUREPROC)_gfx_platform_get_proc_address("glNamedFramebufferTexture");
		(GFX_RND).NamedFramebufferTextureLayer =
			(PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC)_gfx_platform_get_proc_address("glNamedFramebufferTextureLayer");
		(GFX_RND).TextureBuffer =
			(PFNGLTEXTUREBUFFERPROC)_gfx_platform_get_proc_address("glTextureBuffer");
		(GFX_RND).TextureParameteri =
			(PFNGLTEXTUREPARAMETERIPROC)_gfx_platform_get_proc_address("glTextureParameteri");
		(GFX_RND).TextureStorage1D =
			(PFNGLTEXTURESTORAGE1DPROC)_gfx_platform_get_proc_address("glTextureStorage1D");
		(GFX_RND).TextureStorage2D =
			(PFNGLTEXTURESTORAGE2DPROC)_gfx_platform_get_proc_address("glTextureStorage2D");
		(GFX_RND).TextureStorage2DMultisample =
			(PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTextureStorage2DMultisample");
		(GFX_RND).TextureStorage3D =
			(PFNGLTEXTURESTORAGE3DPROC)_gfx_platform_get_proc_address("glTextureStorage3D");
		(GFX_RND).TextureStorage3DMultisample =
			(PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTextureStorage3DMultisample");
		(GFX_RND).TextureSubImage1D =
			(PFNGLTEXTURESUBIMAGE1DPROC)_gfx_platform_get_proc_address("glTextureSubImage1D");
		(GFX_RND).TextureSubImage2D =
			(PFNGLTEXTURESUBIMAGE2DPROC)_gfx_platform_get_proc_address("glTextureSubImage2D");
		(GFX_RND).TextureSubImage3D =
			(PFNGLTEXTURESUBIMAGE3DPROC)_gfx_platform_get_proc_address("glTextureSubImage3D");
		(GFX_RND).UnmapNamedBuffer =
			(PFNGLUNMAPNAMEDBUFFERPROC)_gfx_platform_get_proc_address("glUnmapNamedBuffer");
	}

	else
	{
		(GFX_WND)->flags[GFX_EXT_DIRECT_STATE_ACCESS] = 0;

		(GFX_RND).BindTextureUnit              = _gfx_gl_bind_texture_unit;
		(GFX_RND).CopyNamedBufferSubData       = _gfx_gl_copy_named_buffer_sub_data;
		(GFX_RND).CreateBuffers                = _gfx_gl_create_buffers;
		(GFX_RND).CreateFramebuffers           = _gfx_gl_create_framebuffers;
		(GFX_RND).CreateTextures               = _gfx_gl_create_textures;
		(GFX_RND).CreateVertexArrays           = _gfx_gl_create_vertex_arrays;
		(GFX_RND).DisableVertexArrayAttrib     = _gfx_gl_disable_vertex_array_attrib;
		(GFX_RND).EnableVertexArrayAttrib      = _gfx_gl_enable_vertex_array_attrib;
		(GFX_RND).GenerateTextureMipmap        = _gfx_gl_generate_texture_mipmap;
		(GFX_RND).GetNamedBufferSubData        = _gfx_gl_get_named_buffer_sub_data;
		(GFX_RND).MapNamedBufferRange          = _gfx_gl_map_named_buffer_range;
		(GFX_RND).NamedBufferData              = _gfx_gl_named_buffer_data;
		(GFX_RND).NamedBufferSubData           = _gfx_gl_named_buffer_sub_data;
		(GFX_RND).NamedFramebufferDrawBuffers  = _gfx_gl_named_framebuffer_draw_buffers;
		(GFX_RND).NamedFramebufferTexture      = _gfx_gl_named_framebuffer_texture;
		(GFX_RND).NamedFramebufferTextureLayer = _gfx_gl_named_framebuffer_texture_layer;
		(GFX_RND).TextureBuffer                = _gfx_gl_texture_buffer;
		(GFX_RND).TextureParameteri            = _gfx_gl_texture_parameter_i;
		(GFX_RND).TextureStorage1D             = _gfx_gl_texture_storage_1d;
		(GFX_RND).TextureStorage2D             = _gfx_gl_texture_storage_2d;
		(GFX_RND).TextureStorage2DMultisample  = _gfx_gl_texture_storage_2d_multisample;
		(GFX_RND).TextureStorage3D             = _gfx_gl_texture_storage_3d;
		(GFX_RND).TextureStorage3DMultisample  = _gfx_gl_texture_storage_3d_multisample;
		(GFX_RND).TextureSubImage1D            = _gfx_gl_texture_sub_image_1d;
		(GFX_RND).TextureSubImage2D            = _gfx_gl_texture_sub_image_2d;
		(GFX_RND).TextureSubImage3D            = _gfx_gl_texture_sub_image_3d;
		(GFX_RND).UnmapNamedBuffer             = _gfx_gl_unmap_named_buffer;
	}

	/* GFX_EXT_IMMUTABLE_TEXTURE */
	if(
		(GFX_WND)->context.major > 4 ||
		((GFX_WND)->context.major == 4 && (GFX_WND)->context.minor > 1) ||
		_gfx_is_extension_supported("GL_ARB_texture_storage"))
	{
		(GFX_WND)->flags[GFX_EXT_IMMUTABLE_TEXTURE] = 1;

		(GFX_RND).TexStorage1D =
			(PFNGLTEXSTORAGE1DPROC)_gfx_platform_get_proc_address("glTexStorage1D");
		(GFX_RND).TexStorage2D =
			(PFNGLTEXSTORAGE2DPROC)_gfx_platform_get_proc_address("glTexStorage2D");
		(GFX_RND).TexStorage3D =
			(PFNGLTEXSTORAGE3DPROC)_gfx_platform_get_proc_address("glTexStorage3D");
	}

	else
	{
		(GFX_WND)->flags[GFX_EXT_IMMUTABLE_TEXTURE] = 0;

		(GFX_RND).TexStorage1D = _gfx_gl_tex_storage_1d;
		(GFX_RND).TexStorage2D = _gfx_gl_tex_storage_2d;
		(GFX_RND).TexStorage3D = _gfx_gl_tex_storage_3d;
	}

	/* GFX_EXT_IMMUTABLE_MULTISAMPLE_TEXTURE */
	if(
		(GFX_WND)->context.major > 4 ||
		((GFX_WND)->context.major == 4 && (GFX_WND)->context.minor > 2) ||
		_gfx_is_extension_supported("GL_ARB_texture_storage_multisample"))
	{
		(GFX_WND)->flags[GFX_EXT_IMMUTABLE_MULTISAMPLE_TEXTURE] = 1;

		(GFX_RND).TexStorage2DMultisample =
			(PFNGLTEXSTORAGE2DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTexStorage2DMultisample");
		(GFX_RND).TexStorage3DMultisample =
			(PFNGLTEXSTORAGE3DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTexStorage3DMultisample");
	}

	else
	{
		(GFX_WND)->flags[GFX_EXT_IMMUTABLE_MULTISAMPLE_TEXTURE] = 0;

		(GFX_RND).TexStorage2DMultisample = _gfx_gl_tex_storage_2d_multisample;
		(GFX_RND).TexStorage3DMultisample = _gfx_gl_tex_storage_3d_multisample;
	}

	/* GFX_EXT_INSTANCED_ATTRIBUTES */
	if(
		(GFX_WND)->context.major > 3 ||
		((GFX_WND)->context.major == 3 && (GFX_WND)->context.minor > 2))
	{
		(GFX_WND)->flags[GFX_EXT_INSTANCED_ATTRIBUTES] = 1;

		(GFX_RND).VertexAttribDivisor =
			(PFNGLVERTEXATTRIBDIVISORPROC)_gfx_platform_get_proc_address("glVertexAttribDivisor");
	}

	else if(_gfx_is_extension_supported("GL_ARB_instanced_arrays"))
	{
		(GFX_WND)->flags[GFX_EXT_INSTANCED_ATTRIBUTES] = 1;

		(GFX_RND).VertexAttribDivisor =
			(PFNGLVERTEXATTRIBDIVISORPROC)_gfx_platform_get_proc_address("glVertexAttribDivisorARB");
	}

	else
	{
		(GFX_WND)->flags[GFX_EXT_INSTANCED_ATTRIBUTES] = 0;

		(GFX_RND).VertexAttribDivisor = _gfx_gl_vertex_attrib_divisor;
	}

	/* GFX_EXT_INSTANCED_BASE_ATTRIBUTES */
	if(
		(GFX_WND)->context.major > 4 ||
		((GFX_WND)->context.major == 4 && (GFX_WND)->context.minor > 1) ||
		_gfx_is_extension_supported("GL_ARB_base_instance"))
	{
		(GFX_WND)->flags[GFX_EXT_INSTANCED_BASE_ATTRIBUTES] = 1;

		(GFX_RND).DrawArraysInstancedBaseInstance =
			(PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC)_gfx_platform_get_proc_address("glDrawArraysInstancedBaseInstance");
		(GFX_RND).DrawElementsInstancedBaseInstance =
			(PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC)_gfx_platform_get_proc_address("glDrawElementsInstancedBaseInstance");
	}

	else
	{
		(GFX_WND)->flags[GFX_EXT_INSTANCED_BASE_ATTRIBUTES] = 0;

		(GFX_RND).DrawArraysInstancedBaseInstance = _gfx_gl_draw_arrays_instanced_base_instance;
		(GFX_RND).DrawElementsInstancedBaseInstance = _gfx_gl_draw_elements_instanced_base_instance;
	}

	/* GFX_EXT_LAYERED_CUBEMAP */
	if(
		(GFX_WND)->context.major > 3 ||
		_gfx_is_extension_supported("GL_ARB_texture_cube_map_array"))
	{
		(GFX_WND)->flags[GFX_EXT_LAYERED_CUBEMAP] = 1;
	}

	else
	{
		(GFX_WND)->flags[GFX_EXT_LAYERED_CUBEMAP] = 0;
	}

	/* GFX_EXT_PROGRAM_BINARY */
	if(
		(GFX_WND)->context.major > 4 ||
		((GFX_WND)->context.major == 4 && (GFX_WND)->context.minor > 0) ||
		_gfx_is_extension_supported("GL_ARB_get_program_binary"))
	{
		(GFX_WND)->flags[GFX_EXT_PROGRAM_BINARY] = 1;

		(GFX_RND).GetProgramBinary =
			(PFNGLGETPROGRAMBINARYPROC)_gfx_platform_get_proc_address("glGetProgramBinary");
		(GFX_RND).ProgramBinary =
			(PFNGLPROGRAMBINARYPROC)_gfx_platform_get_proc_address("glProgramBinary");
		(GFX_RND).ProgramParameteri =
			(PFNGLPROGRAMPARAMETERIPROC)_gfx_platform_get_proc_address("glProgramParameteri");
	}

	else
	{
		(GFX_WND)->flags[GFX_EXT_PROGRAM_BINARY] = 0;

		(GFX_RND).GetProgramBinary  = _gfx_gl_get_program_binary;
		(GFX_RND).ProgramBinary     = _gfx_gl_program_binary;
		(GFX_RND).ProgramParameteri = _gfx_gl_program_parameter_i;
	}

	/* GFX_EXT_TESSELLATION_SHADER */
	if(
		(GFX_WND)->context.major > 3 ||
		_gfx_is_extension_supported("GL_ARB_tessellation_shader"))
	{
		glGetIntegerv(GL_MAX_PATCH_VERTICES, &limit);
		(GFX_WND)->limits[GFX_LIM_MAX_PATCH_VERTICES] = limit;
		(GFX_WND)->flags[GFX_EXT_TESSELLATION_SHADER] = 1;

		(GFX_RND).PatchParameteri =
			(PFNGLPATCHPARAMETERIPROC)_gfx_platform_get_proc_address("glPatchParameteri");
	}

	else
	{
		(GFX_WND)->limits[GFX_LIM_MAX_PATCH_VERTICES] = 0;
		(GFX_WND)->flags[GFX_EXT_TESSELLATION_SHADER] = 0;

		(GFX_RND).PatchParameteri = _gfx_gl_patch_parameter_i;
	}

#endif

	/* Set other defaults */
	(GFX_RND).fbos[0] = 0;
	(GFX_RND).fbos[1] = 0;
	(GFX_RND).vao     = 0;
	(GFX_RND).program = 0;
	(GFX_RND).post    = 0;

	_gfx_platform_window_get_size(
		(GFX_WND)->handle,
		&(GFX_RND).width,
		&(GFX_RND).height
	);
	(GFX_RND).x = 0;
	(GFX_RND).y = 0;

	(GFX_RND).packAlignment = 0;
	(GFX_RND).unpackAlignment = 0;
	(GFX_RND).patchVertices = 0;

	(GFX_RND).uniformBuffers = NULL;
	(GFX_RND).textureUnits = NULL;
}

#endif // GFX_RENDERER_GL
