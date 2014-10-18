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

#define GL_GLEXT_PROTOTYPES
#include "groufix/core/renderer.h"

#include <stdlib.h>
#include <string.h>

#ifdef GFX_RENDERER_GL

/******************************************************/
static int _gfx_is_extension_supported(

		const char* ext,
		GFX_WIND_ARG)
{
	GLint num;
	glGetIntegerv(GL_NUM_EXTENSIONS, &num);

	while(num) if(!strcmp(
		(const char*)GFX_REND_GET.GetStringi(GL_EXTENSIONS, --num),
		(const char*)ext))
	{
		return 1;
	}

	return 0;
}

/******************************************************/
void _gfx_renderer_load(void)
{
	GFX_WIND_INIT();

	/* Get viewport size */
	_gfx_platform_window_get_size(
		GFX_WIND_GET.handle,
		&GFX_REND_GET.viewport.width,
		&GFX_REND_GET.viewport.height
	);

	/* Defaults */
	GFX_WIND_GET.lim[GFX_LIM_MAX_VERTEX_BUFFERS] = GFX_GL_DEF_MAX_VERTEX_BUFFERS;
	GFX_WIND_GET.lim[GFX_LIM_MAX_VERTEX_STRIDE] = GFX_GL_DEF_MAX_VERTEX_STRIDE;

	/* Get OpenGL constants (a.k.a hardware limits) */
	GLint limit;

	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &limit);
		GFX_WIND_GET.lim[GFX_LIM_MAX_BUFFER_PROPERTIES] = limit;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &limit);
		GFX_WIND_GET.lim[GFX_LIM_MAX_COLOR_ATTACHMENTS] = limit;
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &limit);
		GFX_WIND_GET.lim[GFX_LIM_MAX_COLOR_TARGETS] = limit;
	glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &limit);
		GFX_WIND_GET.lim[GFX_LIM_MAX_CUBEMAP_SIZE] = limit;
	glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS, &limit);
		GFX_WIND_GET.lim[GFX_LIM_MAX_FEEDBACK_BUFFERS] = limit;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &limit);
		GFX_WIND_GET.lim[GFX_LIM_MAX_SAMPLER_PROPERTIES] = limit;
	glGetIntegerv(GL_MAX_SAMPLES, &limit);
		GFX_WIND_GET.lim[GFX_LIM_MAX_SAMPLES] = limit;
	glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &limit);
		GFX_WIND_GET.lim[GFX_LIM_MAX_TEXTURE_3D_SIZE] = limit;
	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &limit);
		GFX_WIND_GET.lim[GFX_LIM_MAX_TEXTURE_LAYERS] = limit;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &limit);
		GFX_WIND_GET.lim[GFX_LIM_MAX_TEXTURE_SIZE] = limit;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &limit);
		GFX_WIND_GET.lim[GFX_LIM_MAX_VERTEX_ATTRIBS] = limit;

#if defined(GFX_GLES)

	/* Default Extensions */
	GFX_WIND_GET.ext[GFX_EXT_IMMUTABLE_TEXTURE]    = 1;
	GFX_WIND_GET.ext[GFX_EXT_INSTANCED_ATTRIBUTES] = 1;
	GFX_WIND_GET.ext[GFX_EXT_PROGRAM_BINARY]       = 1;

	/* GLES, assumes 3.0+ */
	GFX_REND_GET.ActiveTexture                     = glActiveTexture;
	GFX_REND_GET.AttachShader                      = glAttachShader;
	GFX_REND_GET.BeginTransformFeedback            = glBeginTransformFeedback;
	GFX_REND_GET.BindAttribLocation                = glBindAttribLocation;
	GFX_REND_GET.BindBuffer                        = glBindBuffer;
	GFX_REND_GET.BindBufferRange                   = glBindBufferRange;
	GFX_REND_GET.BindFramebuffer                   = glBindFramebuffer;
	GFX_REND_GET.BindTexture                       = glBindTexture;
	GFX_REND_GET.BindTextureUnit                   = glBindTextureUnit;
	GFX_REND_GET.BindVertexArray                   = glBindVertexArray;
	GFX_REND_GET.BlendEquationSeparate             = glBlendEquationSeparate;
	GFX_REND_GET.BlendFuncSeparate                 = glBlendFuncSeparate;
	GFX_REND_GET.BufferData                        = glBufferData;
	GFX_REND_GET.BufferSubData                     = glBufferSubData;
	GFX_REND_GET.Clear                             = glClear;
	GFX_REND_GET.CompileShader                     = glCompileShader;
	GFX_REND_GET.CopyBufferSubData                 = glCopyBufferSubData;
	GFX_REND_GET.CopyNamedBufferSubData            = _gfx_gl_copy_named_buffer_sub_data;
	GFX_REND_GET.CreateBuffers                     = _gfx_gl_create_buffers;
	GFX_REND_GET.CreateFramebuffers                = _gfx_gl_create_framebuffers;
	GFX_REND_GET.CreateProgram                     = glCreateProgram;
	GFX_REND_GET.CreateShader                      = glCreateShader;
	GFX_REND_GET.CreateTextures                    = _gfx_gl_create_textures;
	GFX_REND_GET.CreateVertexArrays                = _gfx_gl_create_vertex_arrays;
	GFX_REND_GET.CullFace                          = glCullFace;
	GFX_REND_GET.DeleteBuffers                     = glDeleteBuffers;
	GFX_REND_GET.DeleteFramebuffers                = glDeleteFramebuffers;
	GFX_REND_GET.DeleteProgram                     = glDeleteProgram;
	GFX_REND_GET.DeleteShader                      = glDeleteShader;
	GFX_REND_GET.DeleteTextures                    = glDeleteTextures;
	GFX_REND_GET.DeleteVertexArrays                = glDeleteVertexArrays;
	GFX_REND_GET.DepthFunc                         = glDepthFunc;
	GFX_REND_GET.DepthMask                         = glDepthMask;
	GFX_REND_GET.DetachShader                      = glDetachShader;
	GFX_REND_GET.Disable                           = glDisable;
	GFX_REND_GET.DisableVertexArrayAttrib          = _gfx_gl_disable_vertex_array_attrib;
	GFX_REND_GET.DisableVertexAttribArray          = glDisableVertexAttribArray;
	GFX_REND_GET.DrawArrays                        = glDrawArrays;
	GFX_REND_GET.DrawArraysInstanced               = glDrawArraysInstanced;
	GFX_REND_GET.DrawArraysInstancedBaseInstance   = _gfx_gl_draw_arrays_instanced_base_instance;
	GFX_REND_GET.DrawBuffers                       = glDrawBuffers;
	GFX_REND_GET.DrawElements                      = glDrawElements;
	GFX_REND_GET.DrawElementsInstanced             = glDrawElementsInstanced;
	GFX_REND_GET.DrawElementsInstancedBaseInstance = _gfx_gl_draw_elements_instanced_base_instance;
	GFX_REND_GET.Enable                            = glEnable;
	GFX_REND_GET.EnableVertexArrayAttrib           = _gfx_gl_enable_vertex_array_attrib;
	GFX_REND_GET.EnableVertexAttribArray           = glEnableVertexAttribArray;
	GFX_REND_GET.EndTransformFeedback              = glEndTransformFeedback;
	GFX_REND_GET.FramebufferTexture                = _gfx_gles_framebuffer_texture;
	GFX_REND_GET.FramebufferTexture1D              = _gfx_gles_framebuffer_texture_1d;
	GFX_REND_GET.FramebufferTexture2D              = glFramebufferTexture2D;
	GFX_REND_GET.FramebufferTextureLayer           = glFramebufferTextureLayer;
	GFX_REND_GET.GenBuffers                        = glGenBuffers;
	GFX_REND_GET.GenerateMipmap                    = glGenerateMipmap;
	GFX_REND_GET.GenerateTextureMipmap             = glGenerateTextureMipmap;
	GFX_REND_GET.GenFramebuffers                   = glGenFramebuffers;
	GFX_REND_GET.GenTextures                       = glGenTextures;
	GFX_REND_GET.GenVertexArrays                   = glGenVertexArrays;
	GFX_REND_GET.GetActiveUniform                  = glGetActiveUniform;
	GFX_REND_GET.GetActiveUniformBlockiv           = glGetActiveUniformBlockiv;
	GFX_REND_GET.GetActiveUniformsiv               = glGetActiveUniformsiv;
	GFX_REND_GET.GetBufferSubData                  = _gfx_gles_get_buffer_sub_data;
	GFX_REND_GET.GetError                          = glGetError;
	GFX_REND_GET.GetNamedBufferSubData             = _gfx_gl_get_named_buffer_sub_data;
	GFX_REND_GET.GetProgramBinary                  = glGetProgramBinary;
	GFX_REND_GET.GetProgramInfoLog                 = glGetProgramInfoLog;
	GFX_REND_GET.GetProgramiv                      = glGetProgramiv;
	GFX_REND_GET.GetShaderInfoLog                  = glGetShaderInfoLog;
	GFX_REND_GET.GetShaderiv                       = glGetShaderiv;
	GFX_REND_GET.GetShaderSource                   = glGetShaderSource;
	GFX_REND_GET.GetStringi                        = glGetStringi;
	GFX_REND_GET.GetUniformBlockIndex              = glGetUniformBlockIndex;
	GFX_REND_GET.GetUniformIndices                 = glGetUniformIndices;
	GFX_REND_GET.GetUniformLocation                = glGetUniformLocation;
	GFX_REND_GET.LinkProgram                       = glLinkProgram;
	GFX_REND_GET.MapBufferRange                    = glMapBufferRange;
	GFX_REND_GET.MapNamedBufferRange               = glMapNamedBufferRange;
	GFX_REND_GET.NamedBufferData                   = _gfx_gl_named_buffer_data;
	GFX_REND_GET.NamedBufferSubData                = _gfx_gl_named_buffer_sub_data;
	GFX_REND_GET.NamedFramebufferDrawBuffers       = _gfx_gl_named_framebuffer_draw_buffers;
	GFX_REND_GET.NamedFramebufferTexture           = _gfx_gl_named_framebuffer_texture;
	GFX_REND_GET.NamedFramebufferTexture1D         = _gfx_gles_named_framebuffer_texture_1d;
	GFX_REND_GET.NamedFramebufferTexture2D         = _gfx_gles_named_framebuffer_texture_2d;
	GFX_REND_GET.NamedFramebufferTextureLayer      = _gfx_gl_named_framebuffer_texture_layer;
	GFX_REND_GET.PatchParameteri                   = _gfx_gl_patch_parameter_i;
	GFX_REND_GET.PixelStorei                       = glPixelStorei;
	GFX_REND_GET.PolygonMode                       = _gfx_gles_polygon_mode;
	GFX_REND_GET.ProgramBinary                     = glProgramBinary;
	GFX_REND_GET.ProgramParameteri                 = glProgramParameteri;
	GFX_REND_GET.ShaderSource                      = glShaderSource;
	GFX_REND_GET.StencilFuncSeparate               = glStencilFuncSeparate;
	GFX_REND_GET.StencilOpSeparate                 = glStencilOpSeparate;
	GFX_REND_GET.TexBuffer                         = _gfx_gles_tex_buffer;
	GFX_REND_GET.TexImage1D                        = _gfx_gles_tex_image_1d;
	GFX_REND_GET.TexImage2D                        = glTexImage2D;
	GFX_REND_GET.TexImage2DMultisample             = _gfx_gles_tex_image_2d_multisample;
	GFX_REND_GET.TexImage3D                        = glTexImage3D;
	GFX_REND_GET.TexImage3DMultisample             = _gfx_gles_tex_image_3d_multisample;
	GFX_REND_GET.TexParameteri                     = glTexParameteri;
	GFX_REND_GET.TexStorage1D                      = _gfx_gles_tex_storage_1d;
	GFX_REND_GET.TexStorage2D                      = glTexStorage2D;
	GFX_REND_GET.TexStorage3D                      = glTexStorage3D;
	GFX_REND_GET.TexStorage3DMultisample           = _gfx_gles_tex_storage_3d_multisample;
	GFX_REND_GET.TexSubImage1D                     = _gfx_gles_tex_sub_image_1d;
	GFX_REND_GET.TexSubImage2D                     = glTexSubImage2D;
	GFX_REND_GET.TexSubImage3D                     = glTexSubImage3D;
	GFX_REND_GET.TextureBuffer                     = _gfx_gl_texture_buffer;
	GFX_REND_GET.TextureParameteri                 = _gfx_gl_texture_parameter_i;
	GFX_REND_GET.TextureStorage1D                  = _gfx_gl_texture_storage_1d;
	GFX_REND_GET.TextureStorage2D                  = _gfx_gl_texture_storage_2d;
	GFX_REND_GET.TextureStorage2DMultisample       = _gfx_gl_texture_storage_2d_multisample;
	GFX_REND_GET.TextureStorage3D                  = _gfx_gl_texture_storage_3d;
	GFX_REND_GET.TextureStorage3DMultisample       = _gfx_gl_texture_storage_3d_multisample;
	GFX_REND_GET.TextureSubImage1D                 = _gfx_gl_texture_sub_image_1d;
	GFX_REND_GET.TextureSubImage2D                 = _gfx_gl_texture_sub_image_2d;
	GFX_REND_GET.TextureSubImage3D                 = _gfx_gl_texture_sub_image_3d;
	GFX_REND_GET.TransformFeedbackVaryings         = glTransformFeedbackVaryings;
	GFX_REND_GET.Uniform1fv                        = glUniform1fv;
	GFX_REND_GET.Uniform1iv                        = glUniform1iv;
	GFX_REND_GET.Uniform1uiv                       = glUniform1uiv;
	GFX_REND_GET.Uniform2fv                        = glUniform2fv;
	GFX_REND_GET.Uniform2iv                        = glUniform2iv;
	GFX_REND_GET.Uniform2uiv                       = glUniform2uiv;
	GFX_REND_GET.Uniform3fv                        = glUniform3fv;
	GFX_REND_GET.Uniform3iv                        = glUniform3iv;
	GFX_REND_GET.Uniform3uiv                       = glUniform3uiv;
	GFX_REND_GET.Uniform4fv                        = glUniform4fv;
	GFX_REND_GET.Uniform4iv                        = glUniform4iv;
	GFX_REND_GET.Uniform4uiv                       = glUniform4uiv;
	GFX_REND_GET.UniformBlockBinding               = glUniformBlockBinding;
	GFX_REND_GET.UniformMatrix2fv                  = glUniformMatrix2fv;
	GFX_REND_GET.UniformMatrix3fv                  = glUniformMatrix3fv;
	GFX_REND_GET.UniformMatrix4fv                  = glUniformMatrix4fv;
	GFX_REND_GET.UnmapBuffer                       = glUnmapBuffer;
	GFX_REND_GET.UnmapNamedBuffer                  = _gfx_gl_unmap_named_buffer;
	GFX_REND_GET.UseProgram                        = glUseProgram;
	GFX_REND_GET.VertexAttribDivisor               = glVertexAttribDivisor;
	GFX_REND_GET.VertexAttribIPointer              = glVertexAttribIPointer;
	GFX_REND_GET.VertexAttribPointer               = glVertexAttribPointer;
	GFX_REND_GET.Viewport                          = glViewport;

	/* GFX_EXT_IMMUTABLE_MULTISAMPLE_TEXTURE */
	/* GFX_EXT_MULTISAMPLE_TEXTURE */
	if(
		GFX_WIND_GET.context.major > 3 ||
		(GFX_WIND_GET.context.major == 3 && GFX_WIND_GET.context.minor > 0))
	{
		GFX_WIND_GET.ext[GFX_EXT_IMMUTABLE_MULTISAMPLE_TEXTURE] = 1;
		GFX_WIND_GET.ext[GFX_EXT_MULTISAMPLE_TEXTURE] = 1;

		GFX_REND_GET.TexStorage2DMultisample = glTexStorage2DMultisample;
	}

	else
	{
		GFX_REND_GET.TexStorage2DMultisample = _gfx_gles_tex_storage_2d_multisample;
	}

#elif defined(GFX_GL)

	/* Settings */
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	/* Get OpenGL constants (a.k.a hardware limits) */
	glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &limit);
		GFX_WIND_GET.lim[GFX_LIM_MAX_BUFFER_TEXTURE_SIZE] = limit;

	/* Default Extensions */
	GFX_WIND_GET.ext[GFX_EXT_BUFFER_TEXTURE]              = 1;
	GFX_WIND_GET.ext[GFX_EXT_GEOMETRY_SHADER]             = 1;
	GFX_WIND_GET.ext[GFX_EXT_LAYERED_MULTISAMPLE_TEXTURE] = 1;
	GFX_WIND_GET.ext[GFX_EXT_MULTISAMPLE_TEXTURE]         = 1;
	GFX_WIND_GET.ext[GFX_EXT_POLYGON_STATE]               = 1;
	GFX_WIND_GET.ext[GFX_EXT_SEAMLESS_CUBEMAP]            = 1;
	GFX_WIND_GET.ext[GFX_EXT_TEXTURE_1D]                  = 1;

	/* Core, assumes 3.2+ context */
	GFX_REND_GET.ActiveTexture =
		(PFNGLACTIVETEXTUREPROC)_gfx_platform_get_proc_address("glActiveTexture");
	GFX_REND_GET.AttachShader =
		(PFNGLATTACHSHADERPROC)_gfx_platform_get_proc_address("glAttachShader");
	GFX_REND_GET.BeginTransformFeedback =
		(PFNGLBEGINTRANSFORMFEEDBACKPROC)_gfx_platform_get_proc_address("glBeginTransformFeedback");
	GFX_REND_GET.BindAttribLocation =
		(PFNGLBINDATTRIBLOCATIONPROC)_gfx_platform_get_proc_address("glBindAttribLocation");
	GFX_REND_GET.BindBuffer =
		(PFNGLBINDBUFFERPROC)_gfx_platform_get_proc_address("glBindBuffer");
	GFX_REND_GET.BindBufferRange =
		(PFNGLBINDBUFFERRANGEPROC)_gfx_platform_get_proc_address("glBindBufferRange");
	GFX_REND_GET.BindFramebuffer =
		(PFNGLBINDFRAMEBUFFERPROC)_gfx_platform_get_proc_address("glBindFramebuffer");
	GFX_REND_GET.BindTexture =
		(PFNGLBINDTEXTUREPROC)glBindTexture;
	GFX_REND_GET.BindVertexArray =
		(PFNGLBINDVERTEXARRAYPROC)_gfx_platform_get_proc_address("glBindVertexArray");
	GFX_REND_GET.BlendEquationSeparate =
		(PFNGLBLENDEQUATIONSEPARATEPROC)_gfx_platform_get_proc_address("glBlendEquationSeparate");
	GFX_REND_GET.BlendFuncSeparate =
		(PFNGLBLENDFUNCSEPARATEPROC)_gfx_platform_get_proc_address("glBlendFuncSeparate");
	GFX_REND_GET.BufferData =
		(PFNGLBUFFERDATAPROC)_gfx_platform_get_proc_address("glBufferData");
	GFX_REND_GET.BufferSubData =
		(PFNGLBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glBufferSubData");
	GFX_REND_GET.Clear =
		(PFNGLCLEARPROC)glClear;
	GFX_REND_GET.CompileShader =
		(PFNGLCOMPILESHADERPROC)_gfx_platform_get_proc_address("glCompileShader");
	GFX_REND_GET.CopyBufferSubData =
		(PFNGLCOPYBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glCopyBufferSubData");
	GFX_REND_GET.CreateProgram =
		(PFNGLCREATEPROGRAMPROC)_gfx_platform_get_proc_address("glCreateProgram");
	GFX_REND_GET.CreateShader =
		(PFNGLCREATESHADERPROC)_gfx_platform_get_proc_address("glCreateShader");
	GFX_REND_GET.CullFace =
		(PFNGLCULLFACEPROC)glCullFace;
	GFX_REND_GET.DeleteBuffers =
		(PFNGLDELETEBUFFERSPROC)_gfx_platform_get_proc_address("glDeleteBuffers");
	GFX_REND_GET.DeleteFramebuffers =
		(PFNGLDELETEFRAMEBUFFERSPROC)_gfx_platform_get_proc_address("glDeleteFramebuffers");
	GFX_REND_GET.DeleteProgram =
		(PFNGLDELETEPROGRAMPROC)_gfx_platform_get_proc_address("glDeleteProgram");
	GFX_REND_GET.DeleteShader =
		(PFNGLDELETESHADERPROC)_gfx_platform_get_proc_address("glDeleteShader");
	GFX_REND_GET.DeleteTextures =
		(PFNGLDELETETEXTURESPROC)glDeleteTextures;
	GFX_REND_GET.DeleteVertexArrays =
		(PFNGLDELETEVERTEXARRAYSPROC)_gfx_platform_get_proc_address("glDeleteVertexArrays");
	GFX_REND_GET.DepthFunc =
		(PFNGLDEPTHFUNCPROC)glDepthFunc;
	GFX_REND_GET.DepthMask =
		(PFNGLDEPTHMASKPROC)glDepthMask;
	GFX_REND_GET.DetachShader =
		(PFNGLDETACHSHADERPROC)_gfx_platform_get_proc_address("glDetachShader");
	GFX_REND_GET.Disable =
		(PFNGLDISABLEPROC)glDisable;
	GFX_REND_GET.DisableVertexAttribArray =
		(PFNGLDISABLEVERTEXATTRIBARRAYPROC)_gfx_platform_get_proc_address("glDisableVertexAttribArray");
	GFX_REND_GET.DrawArrays =
		(PFNGLDRAWARRAYSPROC)glDrawArrays;
	GFX_REND_GET.DrawArraysInstanced =
		(PFNGLDRAWARRAYSINSTANCEDPROC)_gfx_platform_get_proc_address("glDrawArraysInstanced");
	GFX_REND_GET.DrawBuffers =
		(PFNGLDRAWBUFFERSPROC)_gfx_platform_get_proc_address("glDrawBuffers");
	GFX_REND_GET.DrawElements =
		(PFNGLDRAWELEMENTSPROC)glDrawElements;
	GFX_REND_GET.DrawElementsInstanced =
		(PFNGLDRAWELEMENTSINSTANCEDPROC)_gfx_platform_get_proc_address("glDrawElementsInstanced");
	GFX_REND_GET.Enable =
		(PFNGLENABLEPROC)glEnable;
	GFX_REND_GET.EnableVertexAttribArray =
		(PFNGLENABLEVERTEXATTRIBARRAYPROC)_gfx_platform_get_proc_address("glEnableVertexAttribArray");
	GFX_REND_GET.EndTransformFeedback =
		(PFNGLENDTRANSFORMFEEDBACKPROC)_gfx_platform_get_proc_address("glEndTransformFeedback");
	GFX_REND_GET.FramebufferTexture =
		(PFNGLFRAMEBUFFERTEXTUREPROC)_gfx_platform_get_proc_address("glFramebufferTexture");
	GFX_REND_GET.FramebufferTexture1D =
		(PFNGLFRAMEBUFFERTEXTURE1DPROC)_gfx_platform_get_proc_address("glFramebufferTexture1D");
	GFX_REND_GET.FramebufferTexture2D =
		(PFNGLFRAMEBUFFERTEXTURE2DPROC)_gfx_platform_get_proc_address("glFramebufferTexture2D");
	GFX_REND_GET.FramebufferTextureLayer =
		(PFNGLFRAMEBUFFERTEXTURELAYERPROC)_gfx_platform_get_proc_address("glFramebufferTextureLayer");
	GFX_REND_GET.GenBuffers =
		(PFNGLGENBUFFERSPROC)_gfx_platform_get_proc_address("glGenBuffers");
	GFX_REND_GET.GenerateMipmap =
		(PFNGLGENERATEMIPMAPPROC)_gfx_platform_get_proc_address("glGenerateMipmap");
	GFX_REND_GET.GenFramebuffers =
		(PFNGLGENFRAMEBUFFERSPROC)_gfx_platform_get_proc_address("glGenFramebuffers");
	GFX_REND_GET.GenTextures =
		(PFNGLGENTEXTURESPROC)glGenTextures;
	GFX_REND_GET.GenVertexArrays =
		(PFNGLGENVERTEXARRAYSPROC)_gfx_platform_get_proc_address("glGenVertexArrays");
	GFX_REND_GET.GetActiveUniform =
		(PFNGLGETACTIVEUNIFORMPROC)_gfx_platform_get_proc_address("glGetActiveUniform");
	GFX_REND_GET.GetActiveUniformBlockiv =
		(PFNGLGETACTIVEUNIFORMBLOCKIVPROC)_gfx_platform_get_proc_address("glGetActiveUniformBlockiv");
	GFX_REND_GET.GetActiveUniformsiv =
		(PFNGLGETACTIVEUNIFORMSIVPROC)_gfx_platform_get_proc_address("glGetActiveUniformsiv");
	GFX_REND_GET.GetBufferSubData =
		(PFNGLGETBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glGetBufferSubData");
	GFX_REND_GET.GetError =
		(PFNGLGETERRORPROC)glGetError;
	GFX_REND_GET.GetProgramInfoLog =
		(PFNGLGETPROGRAMINFOLOGPROC)_gfx_platform_get_proc_address("glGetProgramInfoLog");
	GFX_REND_GET.GetProgramiv =
		(PFNGLGETPROGRAMIVPROC)_gfx_platform_get_proc_address("glGetProgramiv");
	GFX_REND_GET.GetShaderInfoLog =
		(PFNGLGETSHADERINFOLOGPROC)_gfx_platform_get_proc_address("glGetShaderInfoLog");
	GFX_REND_GET.GetShaderiv =
		(PFNGLGETSHADERIVPROC)_gfx_platform_get_proc_address("glGetShaderiv");
	GFX_REND_GET.GetShaderSource =
		(PFNGLGETSHADERSOURCEPROC)_gfx_platform_get_proc_address("glGetShaderSource");
	GFX_REND_GET.GetStringi =
		(PFNGLGETSTRINGIPROC)_gfx_platform_get_proc_address("glGetStringi");
	GFX_REND_GET.GetUniformBlockIndex =
		(PFNGLGETUNIFORMBLOCKINDEXPROC)_gfx_platform_get_proc_address("glGetUniformBlockIndex");
	GFX_REND_GET.GetUniformIndices =
		(PFNGLGETUNIFORMINDICESPROC)_gfx_platform_get_proc_address("glGetUniformIndices");
	GFX_REND_GET.GetUniformLocation =
		(PFNGLGETUNIFORMLOCATIONPROC)_gfx_platform_get_proc_address("glGetUniformLocation");
	GFX_REND_GET.LinkProgram =
		(PFNGLLINKPROGRAMPROC)_gfx_platform_get_proc_address("glLinkProgram");
	GFX_REND_GET.MapBufferRange =
		(PFNGLMAPBUFFERRANGEPROC)_gfx_platform_get_proc_address("glMapBufferRange");
	GFX_REND_GET.NamedFramebufferTexture1D =
		(GFX_NAMEDFRAMEBUFFERTEXTURE1DPROC)_gfx_gl_named_framebuffer_texture_1d;
	GFX_REND_GET.NamedFramebufferTexture2D =
		(GFX_NAMEDFRAMEBUFFERTEXTURE2DPROC)_gfx_gl_named_framebuffer_texture_2d;
	GFX_REND_GET.PixelStorei =
		(PFNGLPIXELSTOREIPROC)glPixelStorei;
	GFX_REND_GET.PolygonMode =
		(PFNGLPOLYGONMODEPROC)glPolygonMode;
	GFX_REND_GET.ShaderSource =
		(PFNGLSHADERSOURCEPROC)_gfx_platform_get_proc_address("glShaderSource");
	GFX_REND_GET.StencilFuncSeparate =
		(PFNGLSTENCILFUNCSEPARATEPROC)_gfx_platform_get_proc_address("glStencilFuncSeparate");
	GFX_REND_GET.StencilOpSeparate =
		(PFNGLSTENCILOPSEPARATEPROC)_gfx_platform_get_proc_address("glStencilOpSeparate");
	GFX_REND_GET.TexBuffer =
		(PFNGLTEXBUFFERPROC)_gfx_platform_get_proc_address("glTexBuffer");
	GFX_REND_GET.TexImage1D =
		(PFNGLTEXIMAGE1DPROC)glTexImage1D;
	GFX_REND_GET.TexImage2D =
		(PFNGLTEXIMAGE2DPROC)glTexImage2D;
	GFX_REND_GET.TexImage2DMultisample =
		(PFNGLTEXIMAGE2DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTexImage2DMultisample");
	GFX_REND_GET.TexImage3D =
		(PFNGLTEXIMAGE3DPROC)_gfx_platform_get_proc_address("glTexImage3D");
	GFX_REND_GET.TexImage3DMultisample =
		(PFNGLTEXIMAGE3DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTexImage3DMultisample");
	GFX_REND_GET.TexParameteri =
		(PFNGLTEXPARAMETERIPROC)glTexParameteri;
	GFX_REND_GET.TexSubImage1D =
		(PFNGLTEXSUBIMAGE1DPROC)glTexSubImage1D;
	GFX_REND_GET.TexSubImage2D =
		(PFNGLTEXSUBIMAGE2DPROC)glTexSubImage2D;
	GFX_REND_GET.TexSubImage3D =
		(PFNGLTEXSUBIMAGE3DPROC)_gfx_platform_get_proc_address("glTexSubImage3D");
	GFX_REND_GET.TransformFeedbackVaryings =
		(PFNGLTRANSFORMFEEDBACKVARYINGSPROC)_gfx_platform_get_proc_address("glTransformFeedbackVaryings");
	GFX_REND_GET.Uniform1fv =
		(PFNGLUNIFORM1FVPROC)_gfx_platform_get_proc_address("glUniform1fv");
	GFX_REND_GET.Uniform1iv =
		(PFNGLUNIFORM1IVPROC)_gfx_platform_get_proc_address("glUniform1iv");
	GFX_REND_GET.Uniform1uiv =
		(PFNGLUNIFORM1UIVPROC)_gfx_platform_get_proc_address("glUniform1uiv");
	GFX_REND_GET.Uniform2fv =
		(PFNGLUNIFORM2FVPROC)_gfx_platform_get_proc_address("glUniform2fv");
	GFX_REND_GET.Uniform2iv =
		(PFNGLUNIFORM2IVPROC)_gfx_platform_get_proc_address("glUniform2iv");
	GFX_REND_GET.Uniform2uiv =
		(PFNGLUNIFORM2UIVPROC)_gfx_platform_get_proc_address("glUniform2uiv");
	GFX_REND_GET.Uniform3fv =
		(PFNGLUNIFORM3FVPROC)_gfx_platform_get_proc_address("glUniform3fv");
	GFX_REND_GET.Uniform3iv =
		(PFNGLUNIFORM3IVPROC)_gfx_platform_get_proc_address("glUniform3iv");
	GFX_REND_GET.Uniform3uiv =
		(PFNGLUNIFORM3UIVPROC)_gfx_platform_get_proc_address("glUniform3uiv");
	GFX_REND_GET.Uniform4fv =
		(PFNGLUNIFORM4FVPROC)_gfx_platform_get_proc_address("glUniform4fv");
	GFX_REND_GET.Uniform4iv =
		(PFNGLUNIFORM4IVPROC)_gfx_platform_get_proc_address("glUniform4iv");
	GFX_REND_GET.Uniform4uiv =
		(PFNGLUNIFORM4UIVPROC)_gfx_platform_get_proc_address("glUniform4uiv");
	GFX_REND_GET.UniformBlockBinding =
		(PFNGLUNIFORMBLOCKBINDINGPROC)_gfx_platform_get_proc_address("glUniformBlockBinding");
	GFX_REND_GET.UniformMatrix2fv =
		(PFNGLUNIFORMMATRIX2FVPROC)_gfx_platform_get_proc_address("glUniformMatrix2fv");
	GFX_REND_GET.UniformMatrix3fv =
		(PFNGLUNIFORMMATRIX3FVPROC)_gfx_platform_get_proc_address("glUniformMatrix3fv");
	GFX_REND_GET.UniformMatrix4fv =
		(PFNGLUNIFORMMATRIX4FVPROC)_gfx_platform_get_proc_address("glUniformMatrix4fv");
	GFX_REND_GET.UnmapBuffer =
		(PFNGLUNMAPBUFFERPROC)_gfx_platform_get_proc_address("glUnmapBuffer");
	GFX_REND_GET.UseProgram =
		(PFNGLUSEPROGRAMPROC)_gfx_platform_get_proc_address("glUseProgram");
	GFX_REND_GET.VertexAttribIPointer =
		(PFNGLVERTEXATTRIBIPOINTERPROC)_gfx_platform_get_proc_address("glVertexAttribIPointer");
	GFX_REND_GET.VertexAttribPointer =
		(PFNGLVERTEXATTRIBPOINTERPROC)_gfx_platform_get_proc_address("glVertexAttribPointer");
	GFX_REND_GET.Viewport =
		(PFNGLVIEWPORTPROC)glViewport;

	/* GFX_EXT_DIRECT_STATE_ACCESS */
	if(
		GFX_WIND_GET.context.major > 4 ||
		(GFX_WIND_GET.context.major == 4 && GFX_WIND_GET.context.minor > 4) ||
		_gfx_is_extension_supported("GL_ARB_direct_state_access", GFX_WIND_AS_ARG))
	{
		GFX_WIND_GET.ext[GFX_EXT_DIRECT_STATE_ACCESS] = 1;

		GFX_REND_GET.BindTextureUnit =
			(PFNGLBINDTEXTUREUNITPROC)_gfx_platform_get_proc_address("glBindTextureUnit");
		GFX_REND_GET.CopyNamedBufferSubData =
			(PFNGLCOPYNAMEDBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glCopyNamedBufferSubData");
		GFX_REND_GET.CreateBuffers =
			(PFNGLCREATEBUFFERSPROC)_gfx_platform_get_proc_address("glCreateBuffers");
		GFX_REND_GET.CreateFramebuffers =
			(PFNGLCREATEFRAMEBUFFERSPROC)_gfx_platform_get_proc_address("glCreateFramebuffers");
		GFX_REND_GET.CreateTextures =
			(PFNGLCREATETEXTURESPROC)_gfx_platform_get_proc_address("glCreateTextures");
		GFX_REND_GET.CreateVertexArrays =
			(PFNGLCREATEVERTEXARRAYSPROC)_gfx_platform_get_proc_address("glCreateVertexArrays");
		GFX_REND_GET.DisableVertexArrayAttrib =
			(PFNGLDISABLEVERTEXARRAYATTRIBPROC)_gfx_platform_get_proc_address("glDisableVertexArrayAttrib");
		GFX_REND_GET.EnableVertexArrayAttrib =
			(PFNGLENABLEVERTEXARRAYATTRIBPROC)_gfx_platform_get_proc_address("glEnableVertexArrayAttrib");
		GFX_REND_GET.GenerateTextureMipmap =
			(PFNGLGENERATETEXTUREMIPMAPPROC)_gfx_platform_get_proc_address("glGenerateTextureMipmap");
		GFX_REND_GET.GetNamedBufferSubData =
			(PFNGLGETNAMEDBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glGetNamedBufferSubData");
		GFX_REND_GET.MapNamedBufferRange =
			(PFNGLMAPNAMEDBUFFERRANGEPROC)_gfx_platform_get_proc_address("glMapNamedBufferRange");
		GFX_REND_GET.NamedBufferData =
			(PFNGLNAMEDBUFFERDATAPROC)_gfx_platform_get_proc_address("glNamedBufferData");
		GFX_REND_GET.NamedBufferSubData =
			(PFNGLNAMEDBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glNamedBufferSubData");
		GFX_REND_GET.NamedFramebufferDrawBuffers =
			(PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC)_gfx_platform_get_proc_address("glNamedFramebufferDrawBuffers");
		GFX_REND_GET.NamedFramebufferTexture =
			(PFNGLNAMEDFRAMEBUFFERTEXTUREPROC)_gfx_platform_get_proc_address("glNamedFramebufferTexture");
		GFX_REND_GET.NamedFramebufferTextureLayer =
			(PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC)_gfx_platform_get_proc_address("glNamedFramebufferTextureLayer");
		GFX_REND_GET.TextureBuffer =
			(PFNGLTEXTUREBUFFERPROC)_gfx_platform_get_proc_address("glTextureBuffer");
		GFX_REND_GET.TextureParameteri =
			(PFNGLTEXTUREPARAMETERIPROC)_gfx_platform_get_proc_address("glTextureParameteri");
		GFX_REND_GET.TextureStorage1D =
			(PFNGLTEXTURESTORAGE1DPROC)_gfx_platform_get_proc_address("glTextureStorage1D");
		GFX_REND_GET.TextureStorage2D =
			(PFNGLTEXTURESTORAGE2DPROC)_gfx_platform_get_proc_address("glTextureStorage2D");
		GFX_REND_GET.TextureStorage2DMultisample =
			(PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTextureStorage2DMultisample");
		GFX_REND_GET.TextureStorage3D =
			(PFNGLTEXTURESTORAGE3DPROC)_gfx_platform_get_proc_address("glTextureStorage3D");
		GFX_REND_GET.TextureStorage3DMultisample =
			(PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTextureStorage3DMultisample");
		GFX_REND_GET.TextureSubImage1D =
			(PFNGLTEXTURESUBIMAGE1DPROC)_gfx_platform_get_proc_address("glTextureSubImage1D");
		GFX_REND_GET.TextureSubImage2D =
			(PFNGLTEXTURESUBIMAGE2DPROC)_gfx_platform_get_proc_address("glTextureSubImage2D");
		GFX_REND_GET.TextureSubImage3D =
			(PFNGLTEXTURESUBIMAGE3DPROC)_gfx_platform_get_proc_address("glTextureSubImage3D");
		GFX_REND_GET.UnmapNamedBuffer =
			(PFNGLUNMAPNAMEDBUFFERPROC)_gfx_platform_get_proc_address("glUnmapNamedBuffer");
	}

	else
	{
		GFX_REND_GET.BindTextureUnit              = _gfx_gl_bind_texture_unit;
		GFX_REND_GET.CopyNamedBufferSubData       = _gfx_gl_copy_named_buffer_sub_data;
		GFX_REND_GET.CreateBuffers                = _gfx_gl_create_buffers;
		GFX_REND_GET.CreateFramebuffers           = _gfx_gl_create_framebuffers;
		GFX_REND_GET.CreateTextures               = _gfx_gl_create_textures;
		GFX_REND_GET.CreateVertexArrays           = _gfx_gl_create_vertex_arrays;
		GFX_REND_GET.DisableVertexArrayAttrib     = _gfx_gl_disable_vertex_array_attrib;
		GFX_REND_GET.EnableVertexArrayAttrib      = _gfx_gl_enable_vertex_array_attrib;
		GFX_REND_GET.GenerateTextureMipmap        = _gfx_gl_generate_texture_mipmap;
		GFX_REND_GET.GetNamedBufferSubData        = _gfx_gl_get_named_buffer_sub_data;
		GFX_REND_GET.MapNamedBufferRange          = _gfx_gl_map_named_buffer_range;
		GFX_REND_GET.NamedBufferData              = _gfx_gl_named_buffer_data;
		GFX_REND_GET.NamedBufferSubData           = _gfx_gl_named_buffer_sub_data;
		GFX_REND_GET.NamedFramebufferDrawBuffers  = _gfx_gl_named_framebuffer_draw_buffers;
		GFX_REND_GET.NamedFramebufferTexture      = _gfx_gl_named_framebuffer_texture;
		GFX_REND_GET.NamedFramebufferTextureLayer = _gfx_gl_named_framebuffer_texture_layer;
		GFX_REND_GET.TextureBuffer                = _gfx_gl_texture_buffer;
		GFX_REND_GET.TextureParameteri            = _gfx_gl_texture_parameter_i;
		GFX_REND_GET.TextureStorage1D             = _gfx_gl_texture_storage_1d;
		GFX_REND_GET.TextureStorage2D             = _gfx_gl_texture_storage_2d;
		GFX_REND_GET.TextureStorage2DMultisample  = _gfx_gl_texture_storage_2d_multisample;
		GFX_REND_GET.TextureStorage3D             = _gfx_gl_texture_storage_3d;
		GFX_REND_GET.TextureStorage3DMultisample  = _gfx_gl_texture_storage_3d_multisample;
		GFX_REND_GET.TextureSubImage1D            = _gfx_gl_texture_sub_image_1d;
		GFX_REND_GET.TextureSubImage2D            = _gfx_gl_texture_sub_image_2d;
		GFX_REND_GET.TextureSubImage3D            = _gfx_gl_texture_sub_image_3d;
		GFX_REND_GET.UnmapNamedBuffer             = _gfx_gl_unmap_named_buffer;
	}

	/* GFX_EXT_IMMUTABLE_TEXTURE */
	if(
		GFX_WIND_GET.context.major > 4 ||
		(GFX_WIND_GET.context.major == 4 && GFX_WIND_GET.context.minor > 1) ||
		_gfx_is_extension_supported("GL_ARB_texture_storage", GFX_WIND_AS_ARG))
	{
		GFX_WIND_GET.ext[GFX_EXT_IMMUTABLE_TEXTURE] = 1;

		GFX_REND_GET.TexStorage1D =
			(PFNGLTEXSTORAGE1DPROC)_gfx_platform_get_proc_address("glTexStorage1D");
		GFX_REND_GET.TexStorage2D =
			(PFNGLTEXSTORAGE2DPROC)_gfx_platform_get_proc_address("glTexStorage2D");
		GFX_REND_GET.TexStorage3D =
			(PFNGLTEXSTORAGE3DPROC)_gfx_platform_get_proc_address("glTexStorage3D");
	}

	else
	{
		GFX_REND_GET.TexStorage1D = _gfx_gl_tex_storage_1d;
		GFX_REND_GET.TexStorage2D = _gfx_gl_tex_storage_2d;
		GFX_REND_GET.TexStorage3D = _gfx_gl_tex_storage_3d;
	}

	/* GFX_EXT_IMMUTABLE_MULTISAMPLE_TEXTURE */
	if(
		GFX_WIND_GET.context.major > 4 ||
		(GFX_WIND_GET.context.major == 4 && GFX_WIND_GET.context.minor > 2) ||
		_gfx_is_extension_supported("GL_ARB_texture_storage_multisample", GFX_WIND_AS_ARG))
	{
		GFX_WIND_GET.ext[GFX_EXT_IMMUTABLE_MULTISAMPLE_TEXTURE] = 1;

		GFX_REND_GET.TexStorage2DMultisample =
			(PFNGLTEXSTORAGE2DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTexStorage2DMultisample");
		GFX_REND_GET.TexStorage3DMultisample =
			(PFNGLTEXSTORAGE3DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTexStorage3DMultisample");
	}

	else
	{
		GFX_REND_GET.TexStorage2DMultisample = _gfx_gl_tex_storage_2d_multisample;
		GFX_REND_GET.TexStorage3DMultisample = _gfx_gl_tex_storage_3d_multisample;
	}

	/* GFX_EXT_INSTANCED_ATTRIBUTES */
	if(
		GFX_WIND_GET.context.major > 3 ||
		(GFX_WIND_GET.context.major == 3 && GFX_WIND_GET.context.minor > 2))
	{
		GFX_WIND_GET.ext[GFX_EXT_INSTANCED_ATTRIBUTES] = 1;

		GFX_REND_GET.VertexAttribDivisor =
			(PFNGLVERTEXATTRIBDIVISORPROC)_gfx_platform_get_proc_address("glVertexAttribDivisor");
	}

	else if(_gfx_is_extension_supported("GL_ARB_instanced_arrays", GFX_WIND_AS_ARG))
	{
		GFX_WIND_GET.ext[GFX_EXT_INSTANCED_ATTRIBUTES] = 1;

		GFX_REND_GET.VertexAttribDivisor =
			(PFNGLVERTEXATTRIBDIVISORPROC)_gfx_platform_get_proc_address("glVertexAttribDivisorARB");
	}

	else
	{
		GFX_REND_GET.VertexAttribDivisor = _gfx_gl_vertex_attrib_divisor;
	}

	/* GFX_EXT_INSTANCED_BASE_ATTRIBUTES */
	if(
		GFX_WIND_GET.context.major > 4 ||
		(GFX_WIND_GET.context.major == 4 && GFX_WIND_GET.context.minor > 1) ||
		_gfx_is_extension_supported("GL_ARB_base_instance", GFX_WIND_AS_ARG))
	{
		GFX_WIND_GET.ext[GFX_EXT_INSTANCED_BASE_ATTRIBUTES] = 1;

		GFX_REND_GET.DrawArraysInstancedBaseInstance =
			(PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC)_gfx_platform_get_proc_address("glDrawArraysInstancedBaseInstance");
		GFX_REND_GET.DrawElementsInstancedBaseInstance =
			(PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC)_gfx_platform_get_proc_address("glDrawElementsInstancedBaseInstance");
	}

	else
	{
		GFX_REND_GET.DrawArraysInstancedBaseInstance = _gfx_gl_draw_arrays_instanced_base_instance;
		GFX_REND_GET.DrawElementsInstancedBaseInstance = _gfx_gl_draw_elements_instanced_base_instance;
	}

	/* GFX_EXT_LAYERED_CUBEMAP */
	if(
		GFX_WIND_GET.context.major > 3 ||
		_gfx_is_extension_supported("GL_ARB_texture_cube_map_array", GFX_WIND_AS_ARG))
	{
		GFX_WIND_GET.ext[GFX_EXT_LAYERED_CUBEMAP] = 1;
	}

	/* GFX_EXT_PROGRAM_BINARY */
	if(
		GFX_WIND_GET.context.major > 4 ||
		(GFX_WIND_GET.context.major == 4 && GFX_WIND_GET.context.minor > 0) ||
		_gfx_is_extension_supported("GL_ARB_get_program_binary", GFX_WIND_AS_ARG))
	{
		GFX_WIND_GET.ext[GFX_EXT_PROGRAM_BINARY] = 1;

		GFX_REND_GET.GetProgramBinary =
			(PFNGLGETPROGRAMBINARYPROC)_gfx_platform_get_proc_address("glGetProgramBinary");
		GFX_REND_GET.ProgramBinary =
			(PFNGLPROGRAMBINARYPROC)_gfx_platform_get_proc_address("glProgramBinary");
		GFX_REND_GET.ProgramParameteri =
			(PFNGLPROGRAMPARAMETERIPROC)_gfx_platform_get_proc_address("glProgramParameteri");
	}

	else
	{
		GFX_REND_GET.GetProgramBinary  = _gfx_gl_get_program_binary;
		GFX_REND_GET.ProgramBinary     = _gfx_gl_program_binary;
		GFX_REND_GET.ProgramParameteri = _gfx_gl_program_parameter_i;
	}

	/* GFX_EXT_TESSELLATION_SHADER */
	if(
		GFX_WIND_GET.context.major > 3 ||
		_gfx_is_extension_supported("GL_ARB_tessellation_shader", GFX_WIND_AS_ARG))
	{
		glGetIntegerv(GL_MAX_PATCH_VERTICES, &limit);
		GFX_WIND_GET.lim[GFX_LIM_MAX_PATCH_VERTICES] = limit;
		GFX_WIND_GET.ext[GFX_EXT_TESSELLATION_SHADER] = 1;

		GFX_REND_GET.PatchParameteri =
			(PFNGLPATCHPARAMETERIPROC)_gfx_platform_get_proc_address("glPatchParameteri");
	}

	else
	{
		GFX_REND_GET.PatchParameteri = _gfx_gl_patch_parameter_i;
	}

#endif

}

/******************************************************/
void _gfx_renderer_unload(void)
{
	GFX_WIND_INIT();

	/* Free binding points */
	free(GFX_REND_GET.uniformBuffers);
	free(GFX_REND_GET.textureUnits);

	GFX_REND_GET.uniformBuffers = NULL;
	GFX_REND_GET.textureUnits = NULL;
}

#endif // GFX_RENDERER_GL
