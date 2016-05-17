/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel.
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


/* Compatibility defines */
#ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY
	#define GL_MAX_TEXTURE_MAX_ANISOTROPY  0x84ff
#endif


/******************************************************/
static int _gfx_gl_is_extension_supported(

		const char* ext,
		GFX_CONT_ARG)
{
	GLint num;
	glGetIntegerv(GL_NUM_EXTENSIONS, &num);

	while(num) if(!strcmp(ext,
		(const char*)GFX_REND_GET.GetStringi(GL_EXTENSIONS, --num)))
	{
		return 1;
	}

	return 0;
}

/******************************************************/
void _gfx_renderer_load(

		GFX_CONT_ARG)
{
	/* Get viewport size */
	_gfx_platform_window_get_size(
		GFX_CONT_GET.handle,
		&GFX_REND_GET.viewport.width,
		&GFX_REND_GET.viewport.height
	);

	/* Defaults */
	GFX_CONT_GET.lim[GFX_LIM_MAX_ANISOTROPY] =
		GFX_GL_DEF_MAX_ANISOTROPY;
	GFX_CONT_GET.lim[GFX_LIM_MAX_VERTEX_ATTRIB_OFFSET] =
		GFX_GL_DEF_MAX_VERTEX_ATTRIB_OFFSET;
	GFX_CONT_GET.lim[GFX_LIM_MAX_VERTEX_BUFFERS] =
		GFX_GL_DEF_MAX_VERTEX_BUFFERS;
	GFX_CONT_GET.lim[GFX_LIM_MAX_VERTEX_STRIDE] =
		GFX_GL_DEF_MAX_VERTEX_STRIDE;

	/* Get OpenGL constants */
	GLint limit;

	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &limit),
		GFX_CONT_GET.lim[GFX_LIM_MAX_BUFFER_PROPERTIES] = limit;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &limit),
		GFX_CONT_GET.lim[GFX_LIM_MAX_COLOR_ATTACHMENTS] = limit;
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &limit),
		GFX_CONT_GET.lim[GFX_LIM_MAX_COLOR_TARGETS] = limit;
	glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &limit),
		GFX_CONT_GET.lim[GFX_LIM_MAX_CUBEMAP_SIZE] = limit;
	glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS, &limit),
		GFX_CONT_GET.lim[GFX_LIM_MAX_FEEDBACK_BUFFERS] = limit;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &limit),
		GFX_CONT_GET.lim[GFX_LIM_MAX_SAMPLER_PROPERTIES] = limit;
	glGetIntegerv(GL_MAX_SAMPLES, &limit),
		GFX_CONT_GET.lim[GFX_LIM_MAX_SAMPLES] = limit;
	glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &limit),
		GFX_CONT_GET.lim[GFX_LIM_MAX_TEXTURE_3D_SIZE] = limit;
	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &limit),
		GFX_CONT_GET.lim[GFX_LIM_MAX_TEXTURE_LAYERS] = limit;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &limit),
		GFX_CONT_GET.lim[GFX_LIM_MAX_TEXTURE_SIZE] = limit;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &limit),
		GFX_CONT_GET.lim[GFX_LIM_MAX_VERTEX_ATTRIBS] = limit;

#if defined(GFX_GLES)

	/* Default Extensions */
	GFX_CONT_GET.ext[GFX_EXT_INSTANCED_ATTRIBUTES]   = 1;
	GFX_CONT_GET.ext[GFX_EXT_PROGRAM_BINARY]         = 1;
	GFX_REND_GET.intExt[GFX_INT_EXT_SAMPLER_OBJECTS] = 1;
	GFX_REND_GET.intExt[GFX_INT_EXT_TEXTURE_STORAGE] = 1;

	/* GLES, assumes 3.0+ */
	GFX_REND_GET.ActiveTexture                               = glActiveTexture;
	GFX_REND_GET.AttachShader                                = glAttachShader;
	GFX_REND_GET.BeginTransformFeedback                      = glBeginTransformFeedback;
	GFX_REND_GET.BindAttribLocation                          = glBindAttribLocation;
	GFX_REND_GET.BindBuffer                                  = glBindBuffer;
	GFX_REND_GET.BindBufferBase                              = glBindBufferBase;
	GFX_REND_GET.BindBufferRange                             = glBindBufferRange;
	GFX_REND_GET.BindBuffersRange                            = _gfx_gl_bind_buffers_range;
	GFX_REND_GET.BindFramebuffer                             = glBindFramebuffer;
	GFX_REND_GET.BindProgramPipeline                         = _gfx_gl_bind_program_pipeline;
	GFX_REND_GET.BindSampler                                 = glBindSampler;
	GFX_REND_GET.BindTexture                                 = glBindTexture;
	GFX_REND_GET.BindTextureUnit                             = _gfx_gl_bind_texture_unit;
	GFX_REND_GET.BindVertexArray                             = glBindVertexArray;
	GFX_REND_GET.BindVertexBuffer                            = _gfx_gl_bind_vertex_buffer;
	GFX_REND_GET.BlendEquationSeparate                       = glBlendEquationSeparate;
	GFX_REND_GET.BlendFuncSeparate                           = glBlendFuncSeparate;
	GFX_REND_GET.BufferData                                  = glBufferData;
	GFX_REND_GET.BufferStorage                               = _gfx_gl_buffer_storage;
	GFX_REND_GET.BufferSubData                               = glBufferSubData;
	GFX_REND_GET.Clear                                       = glClear;
	GFX_REND_GET.CompileShader                               = glCompileShader;
	GFX_REND_GET.CopyBufferSubData                           = glCopyBufferSubData;
	GFX_REND_GET.CopyNamedBufferSubData                      = _gfx_gl_copy_named_buffer_sub_data;
	GFX_REND_GET.CreateBuffers                               = _gfx_gl_create_buffers;
	GFX_REND_GET.CreateFramebuffers                          = _gfx_gl_create_framebuffers;
	GFX_REND_GET.CreateProgram                               = glCreateProgram;
	GFX_REND_GET.CreateProgramPipelines                      = _gfx_gl_create_program_pipelines;
	GFX_REND_GET.CreateSamplers                              = _gfx_gl_create_samplers;
	GFX_REND_GET.CreateShader                                = glCreateShader;
	GFX_REND_GET.CreateTextures                              = _gfx_gl_create_textures;
	GFX_REND_GET.CreateVertexArrays                          = _gfx_gl_create_vertex_arrays;
	GFX_REND_GET.CullFace                                    = glCullFace;
	GFX_REND_GET.DebugMessageCallback                        = _gfx_gl_debug_message_callback;
	GFX_REND_GET.DebugMessageControl                         = _gfx_gl_debug_message_control;
	GFX_REND_GET.DeleteBuffers                               = glDeleteBuffers;
	GFX_REND_GET.DeleteFramebuffers                          = glDeleteFramebuffers;
	GFX_REND_GET.DeleteProgram                               = glDeleteProgram;
	GFX_REND_GET.DeleteProgramPipelines                      = _gfx_gl_delete_program_pipelines;
	GFX_REND_GET.DeleteSamplers                              = glDeleteSamplers;
	GFX_REND_GET.DeleteShader                                = glDeleteShader;
	GFX_REND_GET.DeleteTextures                              = glDeleteTextures;
	GFX_REND_GET.DeleteVertexArrays                          = glDeleteVertexArrays;
	GFX_REND_GET.DepthFunc                                   = glDepthFunc;
	GFX_REND_GET.DepthMask                                   = glDepthMask;
	GFX_REND_GET.DetachShader                                = glDetachShader;
	GFX_REND_GET.Disable                                     = glDisable;
	GFX_REND_GET.DisableVertexArrayAttrib                    = _gfx_gl_disable_vertex_array_attrib;
	GFX_REND_GET.DisableVertexAttribArray                    = glDisableVertexAttribArray;
	GFX_REND_GET.DrawArrays                                  = glDrawArrays;
	GFX_REND_GET.DrawArraysInstanced                         = glDrawArraysInstanced;
	GFX_REND_GET.DrawArraysInstancedBaseInstance             = _gfx_gl_draw_arrays_instanced_base_instance;
	GFX_REND_GET.DrawBuffers                                 = glDrawBuffers;
	GFX_REND_GET.DrawElements                                = glDrawElements;
	GFX_REND_GET.DrawElementsBaseVertex                      = _gfx_gles_draw_elements_base_vertex;
	GFX_REND_GET.DrawElementsInstanced                       = glDrawElementsInstanced;
	GFX_REND_GET.DrawElementsInstancedBaseInstance           = _gfx_gl_draw_elements_instanced_base_instance;
	GFX_REND_GET.DrawElementsInstancedBaseVertex             = _gfx_gles_draw_elements_instanced_base_vertex;
	GFX_REND_GET.DrawElementsInstancedBaseVertexBaseInstance = _gfx_gl_draw_elements_instanced_base_vertex_base_instance;
	GFX_REND_GET.Enable                                      = glEnable;
	GFX_REND_GET.EnableVertexArrayAttrib                     = _gfx_gl_enable_vertex_array_attrib;
	GFX_REND_GET.EnableVertexAttribArray                     = glEnableVertexAttribArray;
	GFX_REND_GET.EndTransformFeedback                        = glEndTransformFeedback;
	GFX_REND_GET.Flush                                       = glFlush;
	GFX_REND_GET.FramebufferTexture                          = _gfx_gles_framebuffer_texture;
	GFX_REND_GET.FramebufferTexture2D                        = glFramebufferTexture2D;
	GFX_REND_GET.FramebufferTextureLayer                     = glFramebufferTextureLayer;
	GFX_REND_GET.GenBuffers                                  = glGenBuffers;
	GFX_REND_GET.GenerateMipmap                              = glGenerateMipmap;
	GFX_REND_GET.GenerateTextureMipmap                       = _gfx_gl_generate_texture_mipmap;
	GFX_REND_GET.GenFramebuffers                             = glGenFramebuffers;
	GFX_REND_GET.GenProgramPipelines                         = _gfx_gl_gen_program_pipelines;
	GFX_REND_GET.GenSamplers                                 = glGenSamplers;
	GFX_REND_GET.GenTextures                                 = glGenTextures;
	GFX_REND_GET.GenVertexArrays                             = glGenVertexArrays;
	GFX_REND_GET.GetActiveUniform                            = glGetActiveUniform;
	GFX_REND_GET.GetActiveUniformBlockiv                     = glGetActiveUniformBlockiv;
	GFX_REND_GET.GetActiveUniformsiv                         = glGetActiveUniformsiv;
	GFX_REND_GET.GetBufferSubData                            = _gfx_gles_get_buffer_sub_data;
	GFX_REND_GET.GetNamedBufferSubData                       = _gfx_gl_get_named_buffer_sub_data;
	GFX_REND_GET.GetProgramBinary                            = glGetProgramBinary;
	GFX_REND_GET.GetProgramInfoLog                           = glGetProgramInfoLog;
	GFX_REND_GET.GetProgramiv                                = glGetProgramiv;
	GFX_REND_GET.GetShaderInfoLog                            = glGetShaderInfoLog;
	GFX_REND_GET.GetShaderiv                                 = glGetShaderiv;
	GFX_REND_GET.GetShaderSource                             = glGetShaderSource;
	GFX_REND_GET.GetStringi                                  = glGetStringi;
	GFX_REND_GET.GetUniformBlockIndex                        = glGetUniformBlockIndex;
	GFX_REND_GET.GetUniformIndices                           = glGetUniformIndices;
	GFX_REND_GET.GetUniformLocation                          = glGetUniformLocation;
	GFX_REND_GET.InvalidateBufferSubData                     = _gfx_gl_invalidate_buffer_sub_data;
	GFX_REND_GET.LinkProgram                                 = glLinkProgram;
	GFX_REND_GET.MapBufferRange                              = glMapBufferRange;
	GFX_REND_GET.MapNamedBufferRange                         = _gfx_gl_map_named_buffer_range;
	GFX_REND_GET.NamedBufferData                             = _gfx_gl_named_buffer_data;
	GFX_REND_GET.NamedBufferStorage                          = _gfx_gl_named_buffer_storage;
	GFX_REND_GET.NamedBufferSubData                          = _gfx_gl_named_buffer_sub_data;
	GFX_REND_GET.NamedFramebufferDrawBuffers                 = _gfx_gl_named_framebuffer_draw_buffers;
	GFX_REND_GET.NamedFramebufferTexture                     = _gfx_gl_named_framebuffer_texture;
	GFX_REND_GET.NamedFramebufferTexture2D                   = _gfx_gles_named_framebuffer_texture_2d;
	GFX_REND_GET.NamedFramebufferTextureLayer                = _gfx_gl_named_framebuffer_texture_layer;
	GFX_REND_GET.PatchParameteri                             = _gfx_gl_patch_parameter_i;
	GFX_REND_GET.PixelStorei                                 = glPixelStorei;
	GFX_REND_GET.PolygonMode                                 = _gfx_gles_polygon_mode;
	GFX_REND_GET.ProgramBinary                               = glProgramBinary;
	GFX_REND_GET.ProgramParameteri                           = glProgramParameteri;
	GFX_REND_GET.ProgramUniform1fv                           = _gfx_gl_program_uniform_1fv;
	GFX_REND_GET.ProgramUniform1iv                           = _gfx_gl_program_uniform_1iv;
	GFX_REND_GET.ProgramUniform1uiv                          = _gfx_gl_program_uniform_1uiv;
	GFX_REND_GET.ProgramUniform2fv                           = _gfx_gl_program_uniform_2fv;
	GFX_REND_GET.ProgramUniform2iv                           = _gfx_gl_program_uniform_2iv;
	GFX_REND_GET.ProgramUniform2uiv                          = _gfx_gl_program_uniform_2uiv;
	GFX_REND_GET.ProgramUniform3fv                           = _gfx_gl_program_uniform_3fv;
	GFX_REND_GET.ProgramUniform3iv                           = _gfx_gl_program_uniform_3iv;
	GFX_REND_GET.ProgramUniform3uiv                          = _gfx_gl_program_uniform_3uiv;
	GFX_REND_GET.ProgramUniform4fv                           = _gfx_gl_program_uniform_4fv;
	GFX_REND_GET.ProgramUniform4iv                           = _gfx_gl_program_uniform_4iv;
	GFX_REND_GET.ProgramUniform4uiv                          = _gfx_gl_program_uniform_4uiv;
	GFX_REND_GET.ProgramUniformMatrix2fv                     = _gfx_gl_program_uniform_matrix_2fv;
	GFX_REND_GET.ProgramUniformMatrix3fv                     = _gfx_gl_program_uniform_matrix_3fv;
	GFX_REND_GET.ProgramUniformMatrix4fv                     = _gfx_gl_program_uniform_matrix_4fv;
	GFX_REND_GET.SamplerParameterf                           = glSamplerParameterf;
	GFX_REND_GET.SamplerParameteri                           = glSamplerParameteri;
	GFX_REND_GET.ShaderSource                                = glShaderSource;
	GFX_REND_GET.StencilFuncSeparate                         = glStencilFuncSeparate;
	GFX_REND_GET.StencilOpSeparate                           = glStencilOpSeparate;
	GFX_REND_GET.TexBuffer                                   = _gfx_gles_tex_buffer;
	GFX_REND_GET.TexImage2D                                  = glTexImage2D;
	GFX_REND_GET.TexImage2DMultisample                       = _gfx_gles_tex_image_2d_multisample;
	GFX_REND_GET.TexImage3D                                  = glTexImage3D;
	GFX_REND_GET.TexImage3DMultisample                       = _gfx_gles_tex_image_3d_multisample;
	GFX_REND_GET.TexParameterf                               = glTexParameterf;
	GFX_REND_GET.TexParameteri                               = glTexParameteri;
	GFX_REND_GET.TexStorage2D                                = glTexStorage2D;
	GFX_REND_GET.TexStorage2DMultisample                     = _gfx_gles_tex_storage_2d_multisample;
	GFX_REND_GET.TexStorage3D                                = glTexStorage3D;
	GFX_REND_GET.TexStorage3DMultisample                     = _gfx_gles_tex_storage_3d_multisample;
	GFX_REND_GET.TexSubImage2D                               = glTexSubImage2D;
	GFX_REND_GET.TexSubImage3D                               = glTexSubImage3D;
	GFX_REND_GET.TextureBuffer                               = _gfx_gl_texture_buffer;
	GFX_REND_GET.TextureParameterf                           = _gfx_gl_texture_parameter_f;
	GFX_REND_GET.TextureParameteri                           = _gfx_gl_texture_parameter_i;
	GFX_REND_GET.TextureStorage2D                            = _gfx_gl_texture_storage_2d;
	GFX_REND_GET.TextureStorage2DMultisample                 = _gfx_gl_texture_storage_2d_multisample;
	GFX_REND_GET.TextureStorage3D                            = _gfx_gl_texture_storage_3d;
	GFX_REND_GET.TextureStorage3DMultisample                 = _gfx_gl_texture_storage_3d_multisample;
	GFX_REND_GET.TextureSubImage2D                           = _gfx_gl_texture_sub_image_2d;
	GFX_REND_GET.TextureSubImage3D                           = _gfx_gl_texture_sub_image_3d;
	GFX_REND_GET.TransformFeedbackVaryings                   = glTransformFeedbackVaryings;
	GFX_REND_GET.Uniform1fv                                  = glUniform1fv;
	GFX_REND_GET.Uniform1iv                                  = glUniform1iv;
	GFX_REND_GET.Uniform1uiv                                 = glUniform1uiv;
	GFX_REND_GET.Uniform2fv                                  = glUniform2fv;
	GFX_REND_GET.Uniform2iv                                  = glUniform2iv;
	GFX_REND_GET.Uniform2uiv                                 = glUniform2uiv;
	GFX_REND_GET.Uniform3fv                                  = glUniform3fv;
	GFX_REND_GET.Uniform3iv                                  = glUniform3iv;
	GFX_REND_GET.Uniform3uiv                                 = glUniform3uiv;
	GFX_REND_GET.Uniform4fv                                  = glUniform4fv;
	GFX_REND_GET.Uniform4iv                                  = glUniform4iv;
	GFX_REND_GET.Uniform4uiv                                 = glUniform4uiv;
	GFX_REND_GET.UniformBlockBinding                         = glUniformBlockBinding;
	GFX_REND_GET.UniformMatrix2fv                            = glUniformMatrix2fv;
	GFX_REND_GET.UniformMatrix3fv                            = glUniformMatrix3fv;
	GFX_REND_GET.UniformMatrix4fv                            = glUniformMatrix4fv;
	GFX_REND_GET.UnmapBuffer                                 = glUnmapBuffer;
	GFX_REND_GET.UnmapNamedBuffer                            = _gfx_gl_unmap_named_buffer;
	GFX_REND_GET.UseProgram                                  = glUseProgram;
	GFX_REND_GET.UseProgramStages                            = _gfx_gl_use_program_stages;
	GFX_REND_GET.VertexArrayAttribBinding                    = _gfx_gl_vertex_array_attrib_binding;
	GFX_REND_GET.VertexArrayAttribFormat                     = _gfx_gl_vertex_array_attrib_format;
	GFX_REND_GET.VertexArrayAttribIFormat                    = _gfx_gl_vertex_array_attrib_i_format;
	GFX_REND_GET.VertexArrayBindingDivisor                   = _gfx_gl_vertex_array_binding_divisor;
	GFX_REND_GET.VertexArrayElementBuffer                    = _gfx_gl_vertex_array_element_buffer;
	GFX_REND_GET.VertexArrayVertexBuffer                     = _gfx_gl_vertex_array_vertex_buffer;
	GFX_REND_GET.VertexAttribBinding                         = _gfx_gl_vertex_attrib_binding;
	GFX_REND_GET.VertexAttribDivisor                         = glVertexAttribDivisor;
	GFX_REND_GET.VertexAttribFormat                          = _gfx_gl_vertex_attrib_format;
	GFX_REND_GET.VertexAttribIFormat                         = _gfx_gl_vertex_attrib_i_format;
	GFX_REND_GET.VertexAttribIPointer                        = glVertexAttribIPointer;
	GFX_REND_GET.VertexAttribPointer                         = glVertexAttribPointer;
	GFX_REND_GET.VertexBindingDivisor                        = _gfx_gl_vertex_binding_divisor;
	GFX_REND_GET.Viewport                                    = glViewport;

	/* GFX_EXT_ANISOTROPIC_FILTER */
	if(_gfx_gl_is_extension_supported("GL_EXT_texture_filter_anisotropic", GFX_CONT_AS_ARG))
	{
		GFX_CONT_GET.ext[GFX_EXT_ANISOTROPIC_FILTER] = 1;

		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &limit),
			GFX_CONT_GET.lim[GFX_LIM_MAX_ANISOTROPY] = limit;
	}

	/* GFX_INT_EXT_BUFFER_STORAGE */
	if(_gfx_gl_is_extension_supported("GL_EXT_buffer_storage", GFX_CONT_AS_ARG))
	{
		GFX_REND_GET.intExt[GFX_INT_BUFFER_STORAGE] = 1;

		GFX_REND_GET.BufferStorage =
			(GFX_BUFFERSTORAGEPROC)_gfx_platform_get_proc_address("glBufferStorageEXT");
	}

	/* GFX_INT_EXT_DEBUG_OUTPUT */
	if(
		GFX_CONT_GET.version.major > 3 ||
		(GFX_CONT_GET.version.major == 3 && GFX_CONT_GET.version.minor > 1))
	{
		GFX_REND_GET.intExt[GFX_INT_EXT_DEBUG_OUTPUT] = 1;

		GFX_REND_GET.DebugMessageCallback = glDebugMessageCallback;
		GFX_REND_GET.DebugMessageControl  = glDebugMessageControl;
	}

	else if(_gfx_gl_is_extension_supported("GL_KHR_debug", GFX_CONT_AS_ARG))
	{
		GFX_REND_GET.intExt[GFX_INT_EXT_DEBUG_OUTPUT] = 1;

		GFX_REND_GET.DebugMessageCallback =
			(GFX_DEBUGMESSAGECALLBACKPROC)_gfx_platform_get_proc_address("glDebugMessageCallbackKHR");
		GFX_REND_GET.DebugMessageControl =
			(GFX_DEBUGMESSAGECONTROLPROC)_gfx_platform_get_proc_address("glDebugMessageControlKHR");
	}

	/* GFX_EXT_GEOMETRY_SHADER */
	if(
		_gfx_gl_is_extension_supported("GL_OES_geometry_shader", GFX_CONT_AS_ARG) ||
		_gfx_gl_is_extension_supported("GL_EXT_geometry_shader", GFX_CONT_AS_ARG))
	{
		GFX_CONT_GET.ext[GFX_EXT_GEOMETRY_SHADER] = 1;
	}

	/* GFX_EXT_MULTISAMPLE_TEXTURE */
	/* GFX_INT_EXT_TEXTURE_STORAGE_MULTISAMPLE */
	if(
		GFX_CONT_GET.version.major > 3 ||
		(GFX_CONT_GET.version.major == 3 && GFX_CONT_GET.version.minor > 0))
	{
		GFX_CONT_GET.ext[GFX_EXT_MULTISAMPLE_TEXTURE] = 1;
		GFX_REND_GET.intExt[GFX_INT_EXT_TEXTURE_STORAGE_MULTISAMPLE] = 1;

		GFX_REND_GET.TexStorage2DMultisample = glTexStorage2DMultisample;
	}

	/* GFX_EXT_INSTANCED_BASE_ATTRIBUTES */
	if(_gfx_gl_is_extension_supported("GL_EXT_base_instance", GFX_CONT_AS_ARG))
	{
		GFX_CONT_GET.ext[GFX_EXT_INSTANCED_BASE_ATTRIBUTES] = 1;

		GFX_REND_GET.DrawArraysInstancedBaseInstance =
			(GFX_DRAWARRAYSINSTANCEDBASEINSTANCEPROC)_gfx_platform_get_proc_address("glDrawArraysInstancedBaseInstanceEXT");
		GFX_REND_GET.DrawElementsInstancedBaseInstance =
			(GFX_DRAWELEMENTSINSTANCEDBASEINSTANCEPROC)_gfx_platform_get_proc_address("glDrawElementsInstancedBaseInstanceEXT");
		GFX_REND_GET.DrawElementsInstancedBaseVertexBaseInstance =
			(GFX_DRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC)_gfx_platform_get_proc_address("glDrawElementsInstancedBaseVertexBaseInstanceEXT");
	}

	/* GFX_EXT_POLYGON_STATE */
	if(_gfx_gl_is_extension_supported("GL_NV_polygon_mode", GFX_CONT_AS_ARG))
	{
		GFX_CONT_GET.ext[GFX_EXT_POLYGON_STATE] = 1;

		GFX_REND_GET.PolygonMode =
			(GFX_POLYGONMODEPROC)_gfx_platform_get_proc_address("glPolygonModeNV");
	}

	/* GFX_EXT_PROGRAM_MAP */
	if(
		GFX_CONT_GET.version.major > 3 ||
		(GFX_CONT_GET.version.major == 3 && GFX_CONT_GET.version.minor > 0))
	{
		GFX_CONT_GET.ext[GFX_EXT_PROGRAM_MAP] = 1;

		GFX_REND_GET.BindProgramPipeline     = glBindProgramPipeline;
		GFX_REND_GET.DeleteProgramPipelines  = glDeleteProgramPipelines;
		GFX_REND_GET.GenProgramPipelines     = glGenProgramPipelines;
		GFX_REND_GET.ProgramUniform1fv       = glProgramUniform1fv;
		GFX_REND_GET.ProgramUniform1iv       = glProgramUniform1iv;
		GFX_REND_GET.ProgramUniform1uiv      = glProgramUniform1uiv;
		GFX_REND_GET.ProgramUniform2fv       = glProgramUniform2fv;
		GFX_REND_GET.ProgramUniform2iv       = glProgramUniform2iv;
		GFX_REND_GET.ProgramUniform2uiv      = glProgramUniform2uiv;
		GFX_REND_GET.ProgramUniform3fv       = glProgramUniform3fv;
		GFX_REND_GET.ProgramUniform3iv       = glProgramUniform3iv;
		GFX_REND_GET.ProgramUniform3uiv      = glProgramUniform3uiv;
		GFX_REND_GET.ProgramUniform4fv       = glProgramUniform4fv;
		GFX_REND_GET.ProgramUniform4iv       = glProgramUniform4iv;
		GFX_REND_GET.ProgramUniform4uiv      = glProgramUniform4uiv;
		GFX_REND_GET.ProgramUniformMatrix2fv = glProgramUniformMatrix2fv;
		GFX_REND_GET.ProgramUniformMatrix3fv = glProgramUniformMatrix3fv;
		GFX_REND_GET.ProgramUniformMatrix4fv = glProgramUniformMatrix4fv;
		GFX_REND_GET.UseProgramStages        = glUseProgramStages;
	}

	else if(_gfx_gl_is_extension_supported("GL_EXT_separate_shader_objects", GFX_CONT_AS_ARG))
	{
		GFX_CONT_GET.ext[GFX_EXT_PROGRAM_MAP] = 1;

		GFX_REND_GET.BindProgramPipeline =
			(GFX_BINDPROGRAMPIPELINEPROC)_gfx_platform_get_proc_address("glBindProgramPipelineEXT");
		GFX_REND_GET.DeleteProgramPipelines =
			(GFX_DELETEPROGRAMPIPELINESPROC)_gfx_platform_get_proc_address("glDeleteProgramPipelinesEXT");
		GFX_REND_GET.GenProgramPipelines =
			(GFX_GENPROGRAMPIPELINESPROC)_gfx_platform_get_proc_address("glGenProgramPipelinesEXT");
		GFX_REND_GET.ProgramUniform1fv =
			(GFX_PROGRAMUNIFORM1FVPROC)_gfx_platform_get_proc_address("glProgramUniform1fvEXT");
		GFX_REND_GET.ProgramUniform1iv =
			(GFX_PROGRAMUNIFORM1IVPROC)_gfx_platform_get_proc_address("glProgramUniform1ivEXT");
		GFX_REND_GET.ProgramUniform1uiv =
			(GFX_PROGRAMUNIFORM1UIVPROC)_gfx_platform_get_proc_address("glProgramUniform1uivEXT");
		GFX_REND_GET.ProgramUniform2fv =
			(GFX_PROGRAMUNIFORM2FVPROC)_gfx_platform_get_proc_address("glProgramUniform2fvEXT");
		GFX_REND_GET.ProgramUniform2iv =
			(GFX_PROGRAMUNIFORM2IVPROC)_gfx_platform_get_proc_address("glProgramUniform2ivEXT");
		GFX_REND_GET.ProgramUniform2uiv =
			(GFX_PROGRAMUNIFORM2UIVPROC)_gfx_platform_get_proc_address("glProgramUniform2uivEXT");
		GFX_REND_GET.ProgramUniform3fv =
			(GFX_PROGRAMUNIFORM3FVPROC)_gfx_platform_get_proc_address("glProgramUniform3fvEXT");
		GFX_REND_GET.ProgramUniform3iv =
			(GFX_PROGRAMUNIFORM3IVPROC)_gfx_platform_get_proc_address("glProgramUniform3ivEXT");
		GFX_REND_GET.ProgramUniform3uiv =
			(GFX_PROGRAMUNIFORM3UIVPROC)_gfx_platform_get_proc_address("glProgramUniform3uivEXT");
		GFX_REND_GET.ProgramUniform4fv =
			(GFX_PROGRAMUNIFORM4FVPROC)_gfx_platform_get_proc_address("glProgramUniform4fvEXT");
		GFX_REND_GET.ProgramUniform4iv =
			(GFX_PROGRAMUNIFORM4IVPROC)_gfx_platform_get_proc_address("glProgramUniform4ivEXT");
		GFX_REND_GET.ProgramUniform4uiv =
			(GFX_PROGRAMUNIFORM4UIVPROC)_gfx_platform_get_proc_address("glProgramUniform4uivEXT");
		GFX_REND_GET.ProgramUniformMatrix2fv =
			(GFX_PROGRAMUNIFORMMATRIX2FVPROC)_gfx_platform_get_proc_address("glProgramUniformMatrix2fvEXT");
		GFX_REND_GET.ProgramUniformMatrix3fv =
			(GFX_PROGRAMUNIFORMMATRIX3FVPROC)_gfx_platform_get_proc_address("glProgramUniformMatrix3fvEXT");
		GFX_REND_GET.ProgramUniformMatrix4fv =
			(GFX_PROGRAMUNIFORMMATRIX4FVPROC)_gfx_platform_get_proc_address("glProgramUniformMatrix4fvEXT");
		GFX_REND_GET.UseProgramStages =
			(GFX_USEPROGRAMSTAGESPROC)_gfx_platform_get_proc_address("glUseProgramStagesEXT");
	}

	/* GFX_EXT_TESSELLATION_SHADER */
	if(
		GFX_CONT_GET.version.major > 3 ||
		(GFX_CONT_GET.version.major == 3 && GFX_CONT_GET.version.minor > 1))
	{
		GFX_CONT_GET.ext[GFX_EXT_TESSELLATION_SHADER] = 1;

		glGetIntegerv(GL_MAX_PATCH_VERTICES, &limit),
			GFX_CONT_GET.lim[GFX_LIM_MAX_PATCH_VERTICES] = limit;

		GFX_REND_GET.PatchParameteri = glPatchParameteri;
	}

	else if(_gfx_gl_is_extension_supported("GL_OES_tessellation_shader", GFX_CONT_AS_ARG))
	{
		GFX_CONT_GET.ext[GFX_EXT_TESSELLATION_SHADER] = 1;

		glGetIntegerv(GL_MAX_PATCH_VERTICES, &limit),
			GFX_CONT_GET.lim[GFX_LIM_MAX_PATCH_VERTICES] = limit;

		GFX_REND_GET.PatchParameteri =
			(GFX_PATCHPARAMETERIPROC)_gfx_platform_get_proc_address("glPatchParameteriOES");
	}

	else if(_gfx_gl_is_extension_supported("GL_EXT_tessellation_shader", GFX_CONT_AS_ARG))
	{
		GFX_CONT_GET.ext[GFX_EXT_TESSELLATION_SHADER] = 1;

		glGetIntegerv(GL_MAX_PATCH_VERTICES, &limit),
			GFX_CONT_GET.lim[GFX_LIM_MAX_PATCH_VERTICES] = limit;

		GFX_REND_GET.PatchParameteri =
			(GFX_PATCHPARAMETERIPROC)_gfx_platform_get_proc_address("glPatchParameteriEXT");
	}

	/* GFX_EXT_VERTEX_BASE */
	if(_gfx_gl_is_extension_supported("GL_OES_draw_elements_base_vertex", GFX_CONT_AS_ARG))
	{
		GFX_CONT_GET.ext[GFX_EXT_VERTEX_BASE_INDICES] = 1;

		GFX_REND_GET.DrawElementsBaseVertex =
			(GFX_DRAWELEMENTSBASEVERTEXPROC)_gfx_platform_get_proc_address("glDrawElementsBaseVertexOES");
		GFX_REND_GET.DrawElementsInstancedBaseVertex =
			(GFX_DRAWELEMENTSINSTANCEDBASEVERTEXPROC)_gfx_platform_get_proc_address("glDrawElementsInstancedBaseVertexOES");
	}

	else if(_gfx_gl_is_extension_supported("GL_EXT_draw_elements_base_vertex", GFX_CONT_AS_ARG))
	{
		GFX_CONT_GET.ext[GFX_EXT_VERTEX_BASE_INDICES] = 1;

		GFX_REND_GET.DrawElementsBaseVertex =
			(GFX_DRAWELEMENTSBASEVERTEXPROC)_gfx_platform_get_proc_address("glDrawElementsBaseVertexEXT");
		GFX_REND_GET.DrawElementsInstancedBaseVertex =
			(GFX_DRAWELEMENTSINSTANCEDBASEVERTEXPROC)_gfx_platform_get_proc_address("glDrawElementsInstancedBaseVertexEXT");
	}

	/* GFX_INT_EXT_VERTEX_ATTRIB_BINDING */
	if(
		GFX_CONT_GET.version.major > 3 ||
		(GFX_CONT_GET.version.major == 3 && GFX_CONT_GET.version.minor > 0))
	{
		GFX_REND_GET.intExt[GFX_INT_EXT_VERTEX_ATTRIB_BINDING] = 1;

		glGetIntegerv(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, &limit),
			GFX_CONT_GET.lim[GFX_LIM_MAX_VERTEX_ATTRIB_OFFSET] = limit;
		glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, &limit),
			GFX_CONT_GET.lim[GFX_LIM_MAX_VERTEX_BUFFERS] = limit;

		GFX_REND_GET.BindVertexBuffer     = glBindVertexBuffer;
		GFX_REND_GET.VertexAttribBinding  = glVertexAttribBinding;
		GFX_REND_GET.VertexAttribFormat   = glVertexAttribFormat;
		GFX_REND_GET.VertexAttribIFormat  = glVertexAttribIFormat;
		GFX_REND_GET.VertexBindingDivisor = glVertexBindingDivisor;
	}

#elif defined(GFX_GL)

	/* Settings */
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	/* Get OpenGL constants (a.k.a hardware limits) */
	glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &limit),
		GFX_CONT_GET.lim[GFX_LIM_MAX_BUFFER_TEXTURE_SIZE] = limit;

	/* Default Extensions */
	GFX_REND_GET.intExt[GFX_INT_EXT_BUFFER_READ]          = 1;
	GFX_CONT_GET.ext[GFX_EXT_BUFFER_TEXTURE]              = 1;
	GFX_CONT_GET.ext[GFX_EXT_GEOMETRY_SHADER]             = 1;
	GFX_CONT_GET.ext[GFX_EXT_LAYERED_MULTISAMPLE_TEXTURE] = 1;
	GFX_CONT_GET.ext[GFX_EXT_MULTISAMPLE_TEXTURE]         = 1;
	GFX_CONT_GET.ext[GFX_EXT_POLYGON_STATE]               = 1;
	GFX_CONT_GET.ext[GFX_EXT_SEAMLESS_CUBEMAP]            = 1;
	GFX_CONT_GET.ext[GFX_EXT_VERTEX_BASE_INDICES]         = 1;

	/* Core, assumes 3.2+ */
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
	GFX_REND_GET.BindBufferBase =
		(PFNGLBINDBUFFERBASEPROC)_gfx_platform_get_proc_address("glBindBufferBase");
	GFX_REND_GET.BindBufferRange =
		(PFNGLBINDBUFFERRANGEPROC)_gfx_platform_get_proc_address("glBindBufferRange");
	GFX_REND_GET.BindBuffersRange =
		(PFNGLBINDBUFFERSRANGEPROC)_gfx_gl_bind_buffers_range;
	GFX_REND_GET.BindFramebuffer =
		(PFNGLBINDFRAMEBUFFERPROC)_gfx_platform_get_proc_address("glBindFramebuffer");
	GFX_REND_GET.BindProgramPipeline =
		(PFNGLBINDPROGRAMPIPELINEPROC)_gfx_gl_bind_program_pipeline;
	GFX_REND_GET.BindSampler =
		(PFNGLBINDSAMPLERPROC)_gfx_gl_bind_sampler;
	GFX_REND_GET.BindTexture =
		(PFNGLBINDTEXTUREPROC)glBindTexture;
	GFX_REND_GET.BindTextureUnit =
		(PFNGLBINDTEXTUREUNITPROC)_gfx_gl_bind_texture_unit;
	GFX_REND_GET.BindVertexArray =
		(PFNGLBINDVERTEXARRAYPROC)_gfx_platform_get_proc_address("glBindVertexArray");
	GFX_REND_GET.BindVertexBuffer =
		(PFNGLBINDVERTEXBUFFERPROC)_gfx_gl_bind_vertex_buffer;
	GFX_REND_GET.BlendEquationSeparate =
		(PFNGLBLENDEQUATIONSEPARATEPROC)_gfx_platform_get_proc_address("glBlendEquationSeparate");
	GFX_REND_GET.BlendFuncSeparate =
		(PFNGLBLENDFUNCSEPARATEPROC)_gfx_platform_get_proc_address("glBlendFuncSeparate");
	GFX_REND_GET.BufferData =
		(PFNGLBUFFERDATAPROC)_gfx_platform_get_proc_address("glBufferData");
	GFX_REND_GET.BufferStorage =
		(PFNGLBUFFERSTORAGEPROC)_gfx_gl_buffer_storage;
	GFX_REND_GET.BufferSubData =
		(PFNGLBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glBufferSubData");
	GFX_REND_GET.Clear =
		(PFNGLCLEARPROC)glClear;
	GFX_REND_GET.CompileShader =
		(PFNGLCOMPILESHADERPROC)_gfx_platform_get_proc_address("glCompileShader");
	GFX_REND_GET.CopyBufferSubData =
		(PFNGLCOPYBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glCopyBufferSubData");
	GFX_REND_GET.CopyNamedBufferSubData =
		(PFNGLCOPYNAMEDBUFFERSUBDATAPROC)_gfx_gl_copy_named_buffer_sub_data;
	GFX_REND_GET.CreateBuffers =
		(PFNGLCREATEBUFFERSPROC)_gfx_gl_create_buffers;
	GFX_REND_GET.CreateFramebuffers =
		(PFNGLCREATEFRAMEBUFFERSPROC)_gfx_gl_create_framebuffers;
	GFX_REND_GET.CreateProgram =
		(PFNGLCREATEPROGRAMPROC)_gfx_platform_get_proc_address("glCreateProgram");
	GFX_REND_GET.CreateProgramPipelines =
		(PFNGLCREATEPROGRAMPIPELINESPROC)_gfx_gl_create_program_pipelines;
	GFX_REND_GET.CreateSamplers =
		(PFNGLCREATESAMPLERSPROC)_gfx_gl_create_samplers;
	GFX_REND_GET.CreateShader =
		(PFNGLCREATESHADERPROC)_gfx_platform_get_proc_address("glCreateShader");
	GFX_REND_GET.CreateTextures =
		(PFNGLCREATETEXTURESPROC)_gfx_gl_create_textures;
	GFX_REND_GET.CreateVertexArrays =
		(PFNGLCREATEVERTEXARRAYSPROC)_gfx_gl_create_vertex_arrays;
	GFX_REND_GET.CullFace =
		(PFNGLCULLFACEPROC)glCullFace;
	GFX_REND_GET.DebugMessageCallback =
		(PFNGLDEBUGMESSAGECALLBACKPROC)_gfx_gl_debug_message_callback;
	GFX_REND_GET.DebugMessageControl =
		(PFNGLDEBUGMESSAGECONTROLPROC)_gfx_gl_debug_message_control;
	GFX_REND_GET.DeleteBuffers =
		(PFNGLDELETEBUFFERSPROC)_gfx_platform_get_proc_address("glDeleteBuffers");
	GFX_REND_GET.DeleteFramebuffers =
		(PFNGLDELETEFRAMEBUFFERSPROC)_gfx_platform_get_proc_address("glDeleteFramebuffers");
	GFX_REND_GET.DeleteProgram =
		(PFNGLDELETEPROGRAMPROC)_gfx_platform_get_proc_address("glDeleteProgram");
	GFX_REND_GET.DeleteProgramPipelines =
		(PFNGLDELETEPROGRAMPIPELINESPROC)_gfx_gl_delete_program_pipelines;
	GFX_REND_GET.DeleteSamplers =
		(PFNGLDELETESAMPLERSPROC)_gfx_gl_delete_samplers;
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
	GFX_REND_GET.DisableVertexArrayAttrib =
		(PFNGLDISABLEVERTEXARRAYATTRIBPROC)_gfx_gl_disable_vertex_array_attrib;
	GFX_REND_GET.DisableVertexAttribArray =
		(PFNGLDISABLEVERTEXATTRIBARRAYPROC)_gfx_platform_get_proc_address("glDisableVertexAttribArray");
	GFX_REND_GET.DrawArrays =
		(PFNGLDRAWARRAYSPROC)glDrawArrays;
	GFX_REND_GET.DrawArraysInstanced =
		(PFNGLDRAWARRAYSINSTANCEDPROC)_gfx_platform_get_proc_address("glDrawArraysInstanced");
	GFX_REND_GET.DrawArraysInstancedBaseInstance =
		(PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC)_gfx_gl_draw_arrays_instanced_base_instance;
	GFX_REND_GET.DrawBuffers =
		(PFNGLDRAWBUFFERSPROC)_gfx_platform_get_proc_address("glDrawBuffers");
	GFX_REND_GET.DrawElements =
		(PFNGLDRAWELEMENTSPROC)glDrawElements;
	GFX_REND_GET.DrawElementsBaseVertex =
		(PFNGLDRAWELEMENTSBASEVERTEXPROC)_gfx_platform_get_proc_address("glDrawElementsBaseVertex");
	GFX_REND_GET.DrawElementsInstanced =
		(PFNGLDRAWELEMENTSINSTANCEDPROC)_gfx_platform_get_proc_address("glDrawElementsInstanced");
	GFX_REND_GET.DrawElementsInstancedBaseInstance =
		(PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC)_gfx_gl_draw_elements_instanced_base_instance;
	GFX_REND_GET.DrawElementsInstancedBaseVertex =
		(PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC)_gfx_platform_get_proc_address("glDrawElementsInstancedBaseVertex");
	GFX_REND_GET.DrawElementsInstancedBaseVertexBaseInstance =
		(PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC)_gfx_gl_draw_elements_instanced_base_vertex_base_instance;
	GFX_REND_GET.Enable =
		(PFNGLENABLEPROC)glEnable;
	GFX_REND_GET.EnableVertexArrayAttrib =
		(PFNGLENABLEVERTEXARRAYATTRIBPROC)_gfx_gl_enable_vertex_array_attrib;
	GFX_REND_GET.EnableVertexAttribArray =
		(PFNGLENABLEVERTEXATTRIBARRAYPROC)_gfx_platform_get_proc_address("glEnableVertexAttribArray");
	GFX_REND_GET.EndTransformFeedback =
		(PFNGLENDTRANSFORMFEEDBACKPROC)_gfx_platform_get_proc_address("glEndTransformFeedback");
	GFX_REND_GET.Flush =
		(PFNGLFLUSHPROC)glFlush;
	GFX_REND_GET.FramebufferTexture =
		(PFNGLFRAMEBUFFERTEXTUREPROC)_gfx_platform_get_proc_address("glFramebufferTexture");
	GFX_REND_GET.FramebufferTexture2D =
		(PFNGLFRAMEBUFFERTEXTURE2DPROC)_gfx_platform_get_proc_address("glFramebufferTexture2D");
	GFX_REND_GET.FramebufferTextureLayer =
		(PFNGLFRAMEBUFFERTEXTURELAYERPROC)_gfx_platform_get_proc_address("glFramebufferTextureLayer");
	GFX_REND_GET.GenBuffers =
		(PFNGLGENBUFFERSPROC)_gfx_platform_get_proc_address("glGenBuffers");
	GFX_REND_GET.GenerateMipmap =
		(PFNGLGENERATEMIPMAPPROC)_gfx_platform_get_proc_address("glGenerateMipmap");
	GFX_REND_GET.GenerateTextureMipmap =
		(PFNGLGENERATETEXTUREMIPMAPPROC)_gfx_gl_generate_texture_mipmap;
	GFX_REND_GET.GenFramebuffers =
		(PFNGLGENFRAMEBUFFERSPROC)_gfx_platform_get_proc_address("glGenFramebuffers");
	GFX_REND_GET.GenProgramPipelines =
		(PFNGLGENPROGRAMPIPELINESPROC)_gfx_gl_gen_program_pipelines;
	GFX_REND_GET.GenSamplers =
		(PFNGLGENSAMPLERSPROC)_gfx_gl_gen_samplers;
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
	GFX_REND_GET.GetNamedBufferSubData =
		(PFNGLGETNAMEDBUFFERSUBDATAPROC)_gfx_gl_get_named_buffer_sub_data;
	GFX_REND_GET.GetProgramBinary =
		(PFNGLGETPROGRAMBINARYPROC)_gfx_gl_get_program_binary;
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
	GFX_REND_GET.InvalidateBufferSubData =
		(PFNGLINVALIDATEBUFFERSUBDATAPROC)_gfx_gl_invalidate_buffer_sub_data;
	GFX_REND_GET.LinkProgram =
		(PFNGLLINKPROGRAMPROC)_gfx_platform_get_proc_address("glLinkProgram");
	GFX_REND_GET.MapBufferRange =
		(PFNGLMAPBUFFERRANGEPROC)_gfx_platform_get_proc_address("glMapBufferRange");
	GFX_REND_GET.MapNamedBufferRange =
		(PFNGLMAPNAMEDBUFFERRANGEPROC)_gfx_gl_map_named_buffer_range;
	GFX_REND_GET.NamedBufferData =
		(PFNGLNAMEDBUFFERDATAPROC)_gfx_gl_named_buffer_data;
	GFX_REND_GET.NamedBufferStorage =
		(PFNGLNAMEDBUFFERSTORAGEPROC)_gfx_gl_named_buffer_storage;
	GFX_REND_GET.NamedBufferSubData =
		(PFNGLNAMEDBUFFERSUBDATAPROC)_gfx_gl_named_buffer_sub_data;
	GFX_REND_GET.NamedFramebufferDrawBuffers =
		(PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC)_gfx_gl_named_framebuffer_draw_buffers;
	GFX_REND_GET.NamedFramebufferTexture =
		(PFNGLNAMEDFRAMEBUFFERTEXTUREPROC)_gfx_gl_named_framebuffer_texture;
	GFX_REND_GET.NamedFramebufferTexture2D =
		(GFX_NAMEDFRAMEBUFFERTEXTURE2DPROC)_gfx_gl_named_framebuffer_texture_2d;
	GFX_REND_GET.NamedFramebufferTextureLayer =
		(PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC)_gfx_gl_named_framebuffer_texture_layer;
	GFX_REND_GET.PatchParameteri =
		(PFNGLPATCHPARAMETERIPROC)_gfx_gl_patch_parameter_i;
	GFX_REND_GET.PixelStorei =
		(PFNGLPIXELSTOREIPROC)glPixelStorei;
	GFX_REND_GET.PolygonMode =
		(PFNGLPOLYGONMODEPROC)glPolygonMode;
	GFX_REND_GET.ProgramBinary =
		(PFNGLPROGRAMBINARYPROC)_gfx_gl_program_binary;
	GFX_REND_GET.ProgramParameteri =
		(PFNGLPROGRAMPARAMETERIPROC)_gfx_gl_program_parameter_i;
	GFX_REND_GET.ProgramUniform1fv =
		(GFX_PROGRAMUNIFORM1FVPROC)_gfx_gl_program_uniform_1fv;
	GFX_REND_GET.ProgramUniform1iv =
		(GFX_PROGRAMUNIFORM1IVPROC)_gfx_gl_program_uniform_1iv;
	GFX_REND_GET.ProgramUniform1uiv =
		(GFX_PROGRAMUNIFORM1UIVPROC)_gfx_gl_program_uniform_1uiv;
	GFX_REND_GET.ProgramUniform2fv =
		(GFX_PROGRAMUNIFORM2FVPROC)_gfx_gl_program_uniform_2fv;
	GFX_REND_GET.ProgramUniform2iv =
		(GFX_PROGRAMUNIFORM2IVPROC)_gfx_gl_program_uniform_2iv;
	GFX_REND_GET.ProgramUniform2uiv =
		(GFX_PROGRAMUNIFORM2UIVPROC)_gfx_gl_program_uniform_2uiv;
	GFX_REND_GET.ProgramUniform3fv =
		(GFX_PROGRAMUNIFORM3FVPROC)_gfx_gl_program_uniform_3fv;
	GFX_REND_GET.ProgramUniform3iv =
		(GFX_PROGRAMUNIFORM3IVPROC)_gfx_gl_program_uniform_3iv;
	GFX_REND_GET.ProgramUniform3uiv =
		(GFX_PROGRAMUNIFORM3UIVPROC)_gfx_gl_program_uniform_3uiv;
	GFX_REND_GET.ProgramUniform4fv =
		(GFX_PROGRAMUNIFORM4FVPROC)_gfx_gl_program_uniform_4fv;
	GFX_REND_GET.ProgramUniform4iv =
		(GFX_PROGRAMUNIFORM4IVPROC)_gfx_gl_program_uniform_4iv;
	GFX_REND_GET.ProgramUniform4uiv =
		(GFX_PROGRAMUNIFORM4UIVPROC)_gfx_gl_program_uniform_4uiv;
	GFX_REND_GET.ProgramUniformMatrix2fv =
		(GFX_PROGRAMUNIFORMMATRIX2FVPROC)_gfx_gl_program_uniform_matrix_2fv;
	GFX_REND_GET.ProgramUniformMatrix3fv =
		(GFX_PROGRAMUNIFORMMATRIX3FVPROC)_gfx_gl_program_uniform_matrix_3fv;
	GFX_REND_GET.ProgramUniformMatrix4fv =
		(GFX_PROGRAMUNIFORMMATRIX4FVPROC)_gfx_gl_program_uniform_matrix_4fv;
	GFX_REND_GET.SamplerParameterf =
		(PFNGLSAMPLERPARAMETERFPROC)_gfx_gl_sampler_parameter_f;
	GFX_REND_GET.SamplerParameteri =
		(PFNGLSAMPLERPARAMETERIPROC)_gfx_gl_sampler_parameter_i;
	GFX_REND_GET.ShaderSource =
		(PFNGLSHADERSOURCEPROC)_gfx_platform_get_proc_address("glShaderSource");
	GFX_REND_GET.StencilFuncSeparate =
		(PFNGLSTENCILFUNCSEPARATEPROC)_gfx_platform_get_proc_address("glStencilFuncSeparate");
	GFX_REND_GET.StencilOpSeparate =
		(PFNGLSTENCILOPSEPARATEPROC)_gfx_platform_get_proc_address("glStencilOpSeparate");
	GFX_REND_GET.TexBuffer =
		(PFNGLTEXBUFFERPROC)_gfx_platform_get_proc_address("glTexBuffer");
	GFX_REND_GET.TexImage2D =
		(PFNGLTEXIMAGE2DPROC)glTexImage2D;
	GFX_REND_GET.TexImage2DMultisample =
		(PFNGLTEXIMAGE2DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTexImage2DMultisample");
	GFX_REND_GET.TexImage3D =
		(PFNGLTEXIMAGE3DPROC)_gfx_platform_get_proc_address("glTexImage3D");
	GFX_REND_GET.TexImage3DMultisample =
		(PFNGLTEXIMAGE3DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTexImage3DMultisample");
	GFX_REND_GET.TexParameterf =
		(PFNGLTEXPARAMETERFPROC)glTexParameterf;
	GFX_REND_GET.TexParameteri =
		(PFNGLTEXPARAMETERIPROC)glTexParameteri;
	GFX_REND_GET.TexStorage2D =
		(PFNGLTEXSTORAGE2DPROC)_gfx_gl_tex_storage_2d;
	GFX_REND_GET.TexStorage2DMultisample =
		(PFNGLTEXSTORAGE2DMULTISAMPLEPROC)_gfx_gl_tex_storage_2d_multisample;
	GFX_REND_GET.TexStorage3D =
		(PFNGLTEXSTORAGE3DPROC)_gfx_gl_tex_storage_3d;
	GFX_REND_GET.TexStorage3DMultisample =
		(PFNGLTEXSTORAGE3DMULTISAMPLEPROC)_gfx_gl_tex_storage_3d_multisample;
	GFX_REND_GET.TexSubImage2D =
		(PFNGLTEXSUBIMAGE2DPROC)glTexSubImage2D;
	GFX_REND_GET.TexSubImage3D =
		(PFNGLTEXSUBIMAGE3DPROC)_gfx_platform_get_proc_address("glTexSubImage3D");
	GFX_REND_GET.TextureBuffer =
		(PFNGLTEXTUREBUFFERPROC)_gfx_gl_texture_buffer;
	GFX_REND_GET.TextureParameterf =
		(PFNGLTEXTUREPARAMETERFPROC)_gfx_gl_texture_parameter_f;
	GFX_REND_GET.TextureParameteri =
		(PFNGLTEXTUREPARAMETERIPROC)_gfx_gl_texture_parameter_i;
	GFX_REND_GET.TextureStorage2D =
		(PFNGLTEXTURESTORAGE2DPROC)_gfx_gl_texture_storage_2d;
	GFX_REND_GET.TextureStorage2DMultisample =
		(PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC)_gfx_gl_texture_storage_2d_multisample;
	GFX_REND_GET.TextureStorage3D =
		(PFNGLTEXTURESTORAGE3DPROC)_gfx_gl_texture_storage_3d;
	GFX_REND_GET.TextureStorage3DMultisample =
		(PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC)_gfx_gl_texture_storage_3d_multisample;
	GFX_REND_GET.TextureSubImage2D =
		(PFNGLTEXTURESUBIMAGE2DPROC)_gfx_gl_texture_sub_image_2d;
	GFX_REND_GET.TextureSubImage3D =
		(PFNGLTEXTURESUBIMAGE3DPROC)_gfx_gl_texture_sub_image_3d;
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
	GFX_REND_GET.UnmapNamedBuffer =
		(PFNGLUNMAPNAMEDBUFFERPROC)_gfx_gl_unmap_named_buffer;
	GFX_REND_GET.UseProgram =
		(PFNGLUSEPROGRAMPROC)_gfx_platform_get_proc_address("glUseProgram");
	GFX_REND_GET.UseProgramStages =
		(PFNGLUSEPROGRAMSTAGESPROC)_gfx_gl_use_program_stages;
	GFX_REND_GET.VertexArrayAttribBinding =
		(PFNGLVERTEXARRAYATTRIBBINDINGPROC)_gfx_gl_vertex_array_attrib_binding;
	GFX_REND_GET.VertexArrayAttribFormat =
		(PFNGLVERTEXARRAYATTRIBFORMATPROC)_gfx_gl_vertex_array_attrib_format;
	GFX_REND_GET.VertexArrayAttribIFormat =
		(PFNGLVERTEXARRAYATTRIBIFORMATPROC)_gfx_gl_vertex_array_attrib_i_format;
	GFX_REND_GET.VertexArrayBindingDivisor =
		(PFNGLVERTEXARRAYBINDINGDIVISORPROC)_gfx_gl_vertex_array_binding_divisor;
	GFX_REND_GET.VertexArrayElementBuffer =
		(PFNGLVERTEXARRAYELEMENTBUFFERPROC)_gfx_gl_vertex_array_element_buffer;
	GFX_REND_GET.VertexArrayVertexBuffer =
		(PFNGLVERTEXARRAYVERTEXBUFFERPROC)_gfx_gl_vertex_array_vertex_buffer;
	GFX_REND_GET.VertexAttribBinding  = _gfx_gl_vertex_attrib_binding;
	GFX_REND_GET.VertexAttribDivisor =
		(PFNGLVERTEXATTRIBDIVISORPROC)_gfx_gl_vertex_attrib_divisor;
	GFX_REND_GET.VertexAttribFormat =
		(PFNGLVERTEXATTRIBFORMATPROC)_gfx_gl_vertex_attrib_format;
	GFX_REND_GET.VertexAttribIFormat =
		(PFNGLVERTEXATTRIBIFORMATPROC)_gfx_gl_vertex_attrib_i_format;
	GFX_REND_GET.VertexAttribIPointer =
		(PFNGLVERTEXATTRIBIPOINTERPROC)_gfx_platform_get_proc_address("glVertexAttribIPointer");
	GFX_REND_GET.VertexAttribPointer =
		(PFNGLVERTEXATTRIBPOINTERPROC)_gfx_platform_get_proc_address("glVertexAttribPointer");
	GFX_REND_GET.VertexBindingDivisor =
		(PFNGLVERTEXBINDINGDIVISORPROC)_gfx_gl_vertex_binding_divisor;
	GFX_REND_GET.Viewport =
		(PFNGLVIEWPORTPROC)glViewport;

	/* GFX_EXT_ANISOTROPIC_FILTER */
	if(_gfx_gl_is_extension_supported("GL_EXT_texture_filter_anisotropic", GFX_CONT_AS_ARG))
	{
		GFX_CONT_GET.ext[GFX_EXT_ANISOTROPIC_FILTER] = 1;

		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &limit),
			GFX_CONT_GET.lim[GFX_LIM_MAX_ANISOTROPY] = limit;
	}

	/* GFX_INT_EXT_BUFFER_INVALIDATION */
	if(
		GFX_CONT_GET.version.major > 4 ||
		(GFX_CONT_GET.version.major == 4 && GFX_CONT_GET.version.minor > 2) ||
		_gfx_gl_is_extension_supported("GL_ARB_invalidate_subdata", GFX_CONT_AS_ARG))
	{
		GFX_REND_GET.intExt[GFX_INT_EXT_BUFFER_INVALIDATION] = 1;

		GFX_REND_GET.InvalidateBufferSubData =
			(PFNGLINVALIDATEBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glInvalidateBufferSubData");
	}

	/* GFX_INT_EXT_BUFFER_STORAGE */
	if(
		GFX_CONT_GET.version.major > 4 ||
		(GFX_CONT_GET.version.major == 4 && GFX_CONT_GET.version.minor > 3) ||
		_gfx_gl_is_extension_supported("GL_ARB_buffer_storage", GFX_CONT_AS_ARG))
	{
		GFX_REND_GET.intExt[GFX_INT_EXT_BUFFER_STORAGE] = 1;

		GFX_REND_GET.BufferStorage =
			(PFNGLBUFFERSTORAGEPROC)_gfx_platform_get_proc_address("glBufferStorage");
	}

	/* GFX_INT_EXT_DEBUG_OUTPUT */
	if(
		GFX_CONT_GET.version.major > 4 ||
		(GFX_CONT_GET.version.major == 4 && GFX_CONT_GET.version.minor > 2) ||
		_gfx_gl_is_extension_supported("GL_KHR_debug", GFX_CONT_AS_ARG))
	{
		GFX_REND_GET.intExt[GFX_INT_EXT_DEBUG_OUTPUT] = 1;

		GFX_REND_GET.DebugMessageCallback =
			(PFNGLDEBUGMESSAGECALLBACKPROC)_gfx_platform_get_proc_address("glDebugMessageCallback");
		GFX_REND_GET.DebugMessageControl =
			(PFNGLDEBUGMESSAGECONTROLPROC)_gfx_platform_get_proc_address("glDebugMessageControl");
	}

	else if(_gfx_gl_is_extension_supported("GL_ARB_debug_output", GFX_CONT_AS_ARG))
	{
		GFX_REND_GET.intExt[GFX_INT_EXT_DEBUG_OUTPUT] = 1;

		GFX_REND_GET.DebugMessageCallback =
			(PFNGLDEBUGMESSAGECALLBACKPROC)_gfx_platform_get_proc_address("glDebugMessageCallbackARB");
		GFX_REND_GET.DebugMessageControl =
			(PFNGLDEBUGMESSAGECONTROLPROC)_gfx_platform_get_proc_address("glDebugMessageControlARB");
	}

	/* GFX_INT_EXT_DIRECT_STATE_ACCESS */
	if(
		GFX_CONT_GET.version.major > 4 ||
		(GFX_CONT_GET.version.major == 4 && GFX_CONT_GET.version.minor > 4) ||
		_gfx_gl_is_extension_supported("GL_ARB_direct_state_access", GFX_CONT_AS_ARG))
	{
		GFX_REND_GET.intExt[GFX_INT_EXT_DIRECT_STATE_ACCESS] = 1;

		GFX_REND_GET.BindTextureUnit =
			(PFNGLBINDTEXTUREUNITPROC)_gfx_platform_get_proc_address("glBindTextureUnit");
		GFX_REND_GET.CopyNamedBufferSubData =
			(PFNGLCOPYNAMEDBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glCopyNamedBufferSubData");
		GFX_REND_GET.CreateBuffers =
			(PFNGLCREATEBUFFERSPROC)_gfx_platform_get_proc_address("glCreateBuffers");
		GFX_REND_GET.CreateFramebuffers =
			(PFNGLCREATEFRAMEBUFFERSPROC)_gfx_platform_get_proc_address("glCreateFramebuffers");
		GFX_REND_GET.CreateProgramPipelines =
			(PFNGLCREATEPROGRAMPIPELINESPROC)_gfx_platform_get_proc_address("glCreateProgramPipelines");
		GFX_REND_GET.CreateSamplers =
			(PFNGLCREATESAMPLERSPROC)_gfx_platform_get_proc_address("glCreateSamplers");
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
		GFX_REND_GET.NamedBufferStorage =
			(PFNGLNAMEDBUFFERSTORAGEPROC)_gfx_platform_get_proc_address("glNamedBufferStorage");
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
		GFX_REND_GET.TextureParameterf =
			(PFNGLTEXTUREPARAMETERFPROC)_gfx_platform_get_proc_address("glTextureParameterf");
		GFX_REND_GET.TextureParameteri =
			(PFNGLTEXTUREPARAMETERIPROC)_gfx_platform_get_proc_address("glTextureParameteri");
		GFX_REND_GET.TextureStorage2D =
			(PFNGLTEXTURESTORAGE2DPROC)_gfx_platform_get_proc_address("glTextureStorage2D");
		GFX_REND_GET.TextureStorage2DMultisample =
			(PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTextureStorage2DMultisample");
		GFX_REND_GET.TextureStorage3D =
			(PFNGLTEXTURESTORAGE3DPROC)_gfx_platform_get_proc_address("glTextureStorage3D");
		GFX_REND_GET.TextureStorage3DMultisample =
			(PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTextureStorage3DMultisample");
		GFX_REND_GET.TextureSubImage2D =
			(PFNGLTEXTURESUBIMAGE2DPROC)_gfx_platform_get_proc_address("glTextureSubImage2D");
		GFX_REND_GET.TextureSubImage3D =
			(PFNGLTEXTURESUBIMAGE3DPROC)_gfx_platform_get_proc_address("glTextureSubImage3D");
		GFX_REND_GET.UnmapNamedBuffer =
			(PFNGLUNMAPNAMEDBUFFERPROC)_gfx_platform_get_proc_address("glUnmapNamedBuffer");
		GFX_REND_GET.VertexArrayAttribBinding =
			(PFNGLVERTEXARRAYATTRIBBINDINGPROC)_gfx_platform_get_proc_address("glVertexArrayAttribBinding");
		GFX_REND_GET.VertexArrayAttribFormat =
			(PFNGLVERTEXARRAYATTRIBFORMATPROC)_gfx_platform_get_proc_address("glVertexArrayAttribFormat");
		GFX_REND_GET.VertexArrayAttribIFormat =
			(PFNGLVERTEXARRAYATTRIBIFORMATPROC)_gfx_platform_get_proc_address("glVertexArrayAttribIFormat");
		GFX_REND_GET.VertexArrayBindingDivisor =
			(PFNGLVERTEXARRAYBINDINGDIVISORPROC)_gfx_platform_get_proc_address("glVertexArrayBindingDivisor");
		GFX_REND_GET.VertexArrayElementBuffer =
			(PFNGLVERTEXARRAYELEMENTBUFFERPROC)_gfx_platform_get_proc_address("glVertexArrayElementBuffer");
		GFX_REND_GET.VertexArrayVertexBuffer =
			(PFNGLVERTEXARRAYVERTEXBUFFERPROC)_gfx_platform_get_proc_address("glVertexArrayVertexBuffer");
	}

	/* GFX_EXT_INSTANCED_ATTRIBUTES */
	if(
		GFX_CONT_GET.version.major > 3 ||
		(GFX_CONT_GET.version.major == 3 && GFX_CONT_GET.version.minor > 2))
	{
		GFX_CONT_GET.ext[GFX_EXT_INSTANCED_ATTRIBUTES] = 1;

		GFX_REND_GET.VertexAttribDivisor =
			(PFNGLVERTEXATTRIBDIVISORPROC)_gfx_platform_get_proc_address("glVertexAttribDivisor");
	}

	else if(_gfx_gl_is_extension_supported("GL_ARB_instanced_arrays", GFX_CONT_AS_ARG))
	{
		GFX_CONT_GET.ext[GFX_EXT_INSTANCED_ATTRIBUTES] = 1;

		GFX_REND_GET.VertexAttribDivisor =
			(PFNGLVERTEXATTRIBDIVISORPROC)_gfx_platform_get_proc_address("glVertexAttribDivisorARB");
	}

	/* GFX_EXT_INSTANCED_BASE_ATTRIBUTES */
	if(
		GFX_CONT_GET.version.major > 4 ||
		(GFX_CONT_GET.version.major == 4 && GFX_CONT_GET.version.minor > 1) ||
		_gfx_gl_is_extension_supported("GL_ARB_base_instance", GFX_CONT_AS_ARG))
	{
		GFX_CONT_GET.ext[GFX_EXT_INSTANCED_BASE_ATTRIBUTES] = 1;

		GFX_REND_GET.DrawArraysInstancedBaseInstance =
			(PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC)_gfx_platform_get_proc_address("glDrawArraysInstancedBaseInstance");
		GFX_REND_GET.DrawElementsInstancedBaseInstance =
			(PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC)_gfx_platform_get_proc_address("glDrawElementsInstancedBaseInstance");
		GFX_REND_GET.DrawElementsInstancedBaseVertexBaseInstance =
			(PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC)_gfx_platform_get_proc_address("glDrawElementsInstancedBaseVertexBaseInstance");
	}

	/* GFX_EXT_LAYERED_CUBEMAP */
	if(
		GFX_CONT_GET.version.major > 3 ||
		_gfx_gl_is_extension_supported("GL_ARB_texture_cube_map_array", GFX_CONT_AS_ARG))
	{
		GFX_CONT_GET.ext[GFX_EXT_LAYERED_CUBEMAP] = 1;
	}

	/* GFX_INT_EXT_MULTI_BIND */
	if(
		GFX_CONT_GET.version.major > 4 ||
		(GFX_CONT_GET.version.major == 4 && GFX_CONT_GET.version.minor > 3) ||
		_gfx_gl_is_extension_supported("GL_ARB_multi_bind", GFX_CONT_AS_ARG))
	{
		GFX_REND_GET.intExt[GFX_INT_EXT_MULTI_BIND] = 1;

		GFX_REND_GET.BindBuffersRange =
			(PFNGLBINDBUFFERSRANGEPROC)_gfx_platform_get_proc_address("glBindBuffersRange");
	}

	/* GFX_EXT_PROGRAM_BINARY */
	if(
		GFX_CONT_GET.version.major > 4 ||
		(GFX_CONT_GET.version.major == 4 && GFX_CONT_GET.version.minor > 0) ||
		_gfx_gl_is_extension_supported("GL_ARB_get_program_binary", GFX_CONT_AS_ARG))
	{
		GFX_CONT_GET.ext[GFX_EXT_PROGRAM_BINARY] = 1;

		GFX_REND_GET.GetProgramBinary =
			(PFNGLGETPROGRAMBINARYPROC)_gfx_platform_get_proc_address("glGetProgramBinary");
		GFX_REND_GET.ProgramBinary =
			(PFNGLPROGRAMBINARYPROC)_gfx_platform_get_proc_address("glProgramBinary");
		GFX_REND_GET.ProgramParameteri =
			(PFNGLPROGRAMPARAMETERIPROC)_gfx_platform_get_proc_address("glProgramParameteri");
	}

	/* GFX_EXT_PROGRAM_MAP */
	if(
		GFX_CONT_GET.version.major > 4 ||
		(GFX_CONT_GET.version.major == 4 && GFX_CONT_GET.version.minor > 0) ||
		_gfx_gl_is_extension_supported("GL_ARB_separate_shader_objects", GFX_CONT_AS_ARG))
	{
		GFX_CONT_GET.ext[GFX_EXT_PROGRAM_MAP] = 1;

		GFX_REND_GET.BindProgramPipeline =
			(PFNGLBINDPROGRAMPIPELINEPROC)_gfx_platform_get_proc_address("glBindProgramPipeline");
		GFX_REND_GET.DeleteProgramPipelines =
			(PFNGLDELETEPROGRAMPIPELINESPROC)_gfx_platform_get_proc_address("glDeleteProgramPipelines");
		GFX_REND_GET.GenProgramPipelines =
			(PFNGLGENPROGRAMPIPELINESPROC)_gfx_platform_get_proc_address("glGenProgramPipelines");
		GFX_REND_GET.ProgramParameteri =
			(PFNGLPROGRAMPARAMETERIPROC)_gfx_platform_get_proc_address("glProgramParameteri");
		GFX_REND_GET.ProgramUniform1fv =
			(GFX_PROGRAMUNIFORM1FVPROC)_gfx_platform_get_proc_address("glProgramUniform1fv");
		GFX_REND_GET.ProgramUniform1iv =
			(GFX_PROGRAMUNIFORM1IVPROC)_gfx_platform_get_proc_address("glProgramUniform1iv");
		GFX_REND_GET.ProgramUniform1uiv =
			(GFX_PROGRAMUNIFORM1UIVPROC)_gfx_platform_get_proc_address("glProgramUniform1uiv");
		GFX_REND_GET.ProgramUniform2fv =
			(GFX_PROGRAMUNIFORM2FVPROC)_gfx_platform_get_proc_address("glProgramUniform2fv");
		GFX_REND_GET.ProgramUniform2iv =
			(GFX_PROGRAMUNIFORM2IVPROC)_gfx_platform_get_proc_address("glProgramUniform2iv");
		GFX_REND_GET.ProgramUniform2uiv =
			(GFX_PROGRAMUNIFORM2UIVPROC)_gfx_platform_get_proc_address("glProgramUniform2uiv");
		GFX_REND_GET.ProgramUniform3fv =
			(GFX_PROGRAMUNIFORM3FVPROC)_gfx_platform_get_proc_address("glProgramUniform3fv");
		GFX_REND_GET.ProgramUniform3iv =
			(GFX_PROGRAMUNIFORM3IVPROC)_gfx_platform_get_proc_address("glProgramUniform3iv");
		GFX_REND_GET.ProgramUniform3uiv =
			(GFX_PROGRAMUNIFORM3UIVPROC)_gfx_platform_get_proc_address("glProgramUniform3uiv");
		GFX_REND_GET.ProgramUniform4fv =
			(GFX_PROGRAMUNIFORM4FVPROC)_gfx_platform_get_proc_address("glProgramUniform4fv");
		GFX_REND_GET.ProgramUniform4iv =
			(GFX_PROGRAMUNIFORM4IVPROC)_gfx_platform_get_proc_address("glProgramUniform4iv");
		GFX_REND_GET.ProgramUniform4uiv =
			(GFX_PROGRAMUNIFORM4UIVPROC)_gfx_platform_get_proc_address("glProgramUniform4uiv");
		GFX_REND_GET.ProgramUniformMatrix2fv =
			(GFX_PROGRAMUNIFORMMATRIX2FVPROC)_gfx_platform_get_proc_address("glProgramUniformMatrix2fv");
		GFX_REND_GET.ProgramUniformMatrix3fv =
			(GFX_PROGRAMUNIFORMMATRIX3FVPROC)_gfx_platform_get_proc_address("glProgramUniformMatrix3fv");
		GFX_REND_GET.ProgramUniformMatrix4fv =
			(GFX_PROGRAMUNIFORMMATRIX4FVPROC)_gfx_platform_get_proc_address("glProgramUniformMatrix4fv");
		GFX_REND_GET.UseProgramStages =
			(PFNGLUSEPROGRAMSTAGESPROC)_gfx_platform_get_proc_address("glUseProgramStages");
	}

	/* GFX_INT_EXT_SAMPLER_OBJECTS */
	if(
		GFX_CONT_GET.version.major > 3 ||
		(GFX_CONT_GET.version.major == 3 && GFX_CONT_GET.version.minor > 2) ||
		_gfx_gl_is_extension_supported("GL_ARB_sampler_objects", GFX_CONT_AS_ARG))
	{
		GFX_REND_GET.intExt[GFX_INT_EXT_SAMPLER_OBJECTS] = 1;

		GFX_REND_GET.BindSampler =
			(PFNGLBINDSAMPLERPROC)_gfx_platform_get_proc_address("glBindSampler");
		GFX_REND_GET.DeleteSamplers =
			(PFNGLDELETESAMPLERSPROC)_gfx_platform_get_proc_address("glDeleteSamplers");
		GFX_REND_GET.GenSamplers =
			(PFNGLGENSAMPLERSPROC)_gfx_platform_get_proc_address("glGenSamplers");
		GFX_REND_GET.SamplerParameterf =
			(PFNGLSAMPLERPARAMETERFPROC)_gfx_platform_get_proc_address("glSamplerParameterf");
		GFX_REND_GET.SamplerParameteri =
			(PFNGLSAMPLERPARAMETERIPROC)_gfx_platform_get_proc_address("glSamplerParameteri");
	}

	/* GFX_EXT_TESSELLATION_SHADER */
	if(
		GFX_CONT_GET.version.major > 3 ||
		_gfx_gl_is_extension_supported("GL_ARB_tessellation_shader", GFX_CONT_AS_ARG))
	{
		GFX_CONT_GET.ext[GFX_EXT_TESSELLATION_SHADER] = 1;

		glGetIntegerv(GL_MAX_PATCH_VERTICES, &limit),
			GFX_CONT_GET.lim[GFX_LIM_MAX_PATCH_VERTICES] = limit;

		GFX_REND_GET.PatchParameteri =
			(PFNGLPATCHPARAMETERIPROC)_gfx_platform_get_proc_address("glPatchParameteri");
	}

	/* GFX_INT_EXT_TEXTURE_STORAGE */
	if(
		GFX_CONT_GET.version.major > 4 ||
		(GFX_CONT_GET.version.major == 4 && GFX_CONT_GET.version.minor > 1) ||
		_gfx_gl_is_extension_supported("GL_ARB_texture_storage", GFX_CONT_AS_ARG))
	{
		GFX_REND_GET.intExt[GFX_INT_EXT_TEXTURE_STORAGE] = 1;

		GFX_REND_GET.TexStorage2D =
			(PFNGLTEXSTORAGE2DPROC)_gfx_platform_get_proc_address("glTexStorage2D");
		GFX_REND_GET.TexStorage3D =
			(PFNGLTEXSTORAGE3DPROC)_gfx_platform_get_proc_address("glTexStorage3D");
	}

	/* GFX_INT_EXT_TEXTURE_STORAGE_MULTISAMPLE */
	if(
		GFX_CONT_GET.version.major > 4 ||
		(GFX_CONT_GET.version.major == 4 && GFX_CONT_GET.version.minor > 2) ||
		_gfx_gl_is_extension_supported("GL_ARB_texture_storage_multisample", GFX_CONT_AS_ARG))
	{
		GFX_REND_GET.intExt[GFX_INT_EXT_TEXTURE_STORAGE_MULTISAMPLE] = 1;

		GFX_REND_GET.TexStorage2DMultisample =
			(PFNGLTEXSTORAGE2DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTexStorage2DMultisample");
		GFX_REND_GET.TexStorage3DMultisample =
			(PFNGLTEXSTORAGE3DMULTISAMPLEPROC)_gfx_platform_get_proc_address("glTexStorage3DMultisample");
	}

	/* GFX_INT_EXT_VERTEX_ATTRIB_BINDING */
	if(
		GFX_CONT_GET.version.major > 4 ||
		(GFX_CONT_GET.version.major == 4 && GFX_CONT_GET.version.minor > 2) ||
		_gfx_gl_is_extension_supported("GL_ARB_vertex_attrib_binding", GFX_CONT_AS_ARG))
	{
		GFX_REND_GET.intExt[GFX_INT_EXT_VERTEX_ATTRIB_BINDING] = 1;

		glGetIntegerv(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, &limit),
			GFX_CONT_GET.lim[GFX_LIM_MAX_VERTEX_ATTRIB_OFFSET] = limit;
		glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, &limit),
			GFX_CONT_GET.lim[GFX_LIM_MAX_VERTEX_BUFFERS] = limit;

		GFX_REND_GET.BindVertexBuffer =
			(PFNGLBINDVERTEXBUFFERPROC)_gfx_platform_get_proc_address("glBindVertexBuffer");
		GFX_REND_GET.VertexAttribBinding =
			(PFNGLVERTEXATTRIBBINDINGPROC)_gfx_platform_get_proc_address("glVertexAttribBinding");
		GFX_REND_GET.VertexAttribFormat =
			(PFNGLVERTEXATTRIBFORMATPROC)_gfx_platform_get_proc_address("glVertexAttribFormat");
		GFX_REND_GET.VertexAttribIFormat =
			(PFNGLVERTEXATTRIBIFORMATPROC)_gfx_platform_get_proc_address("glVertexAttribIFormat");
		GFX_REND_GET.VertexBindingDivisor =
			(PFNGLVERTEXBINDINGDIVISORPROC)_gfx_platform_get_proc_address("glVertexBindingDivisor");
	}

#endif
}

/******************************************************/
void _gfx_renderer_unload(

		GFX_CONT_ARG)
{
	/* Free binding points */
	free(GFX_REND_GET.uniformBuffers);
	free(GFX_REND_GET.textureUnits);

	GFX_REND_GET.uniformBuffers = NULL;
	GFX_REND_GET.textureUnits = NULL;
}
