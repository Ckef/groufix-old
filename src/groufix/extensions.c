/**
 * Groufix  :  Graphics Engine produced by Ckef Worx
 * www      :  http://www.ejb.ckef-worx.com
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#define GL_GLEXT_PROTOTYPES
#include "groufix/pipeline/internal.h"
#include "groufix/errors.h"

#include <string.h>

#ifdef GFX_GLES

/******************************************************/
static void _gfx_gles_framebuffer_texture_1d(GLenum target, GLenum attach, GLenum textarget, GLint texture, GLint level)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_1D_TEXTURE and GFX_EXT_BUFFER_TEXTURE are incompatible with this context."
	);
}

/******************************************************/
static void _gfx_gles_get_buffer_sub_data(GLenum target, GLintptr offset, GLsizeiptr size, GLvoid* data)
{
	void* map = glMapBufferRange(target, offset, size, GL_MAP_READ_BIT);

	if(map)
	{
		memcpy(data, map, size);

		if(!glUnmapBuffer(target)) gfx_errors_push(
			GFX_ERROR_MEMORY_CORRUPTION,
			"Reading from a buffer might have corrupted its memory."
		);
	}
}

/******************************************************/
static void _gfx_gles_tex_buffer(GLenum target, GLenum internalFormat, GLuint buffer)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_BUFFER_TEXTURE is incompatible with this context."
	);
}

/******************************************************/
static void _gfx_gles_tex_1d_error(void)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_TEXTURE_1D is incompatible with this context."
	);
}

/******************************************************/
static void _gfx_gles_tex_image_1d(GLenum target, GLint level, GLint internalFormat, GLsizei w, GLint b, GLenum format, GLenum type, const GLvoid* data)
{
	_gfx_gles_tex_1d_error();
}

/******************************************************/
static void _gfx_gles_tex_sub_image_1d(GLenum target, GLint level, GLint xoff, GLsizei w, GLenum format, GLenum type, const GLvoid* data)
{
	_gfx_gles_tex_1d_error();
}

/******************************************************/
static void _gfx_gles_multisample_tex_error(void)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_MULTISAMPLE_TEXTURE is incompatible with this context."
	);
}

/******************************************************/
static void _gfx_gles_tex_image_2d_multisample(GLenum target, GLsizei samples, GLint internalformat, GLsizei w, GLsizei h, GLboolean f)
{
	_gfx_gles_multisample_tex_error();
}

/******************************************************/
static void _gfx_gles_tex_image_3d_multisample(GLenum target, GLsizei samples, GLint internalformat, GLsizei w, GLsizei h, GLsizei d, GLboolean f)
{
	_gfx_gles_multisample_tex_error();
}

#else

/******************************************************/
static void _gfx_gl_vertex_attrib_divisor(GLuint index, GLuint divisor)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_INSTANCED_ATTRIBUTES is incompatible with this context."
	);
}

/******************************************************/
static void _gfx_gl_program_binary_error(void)
{
	gfx_errors_push(
		GFX_ERROR_INCOMPATIBLE_CONTEXT,
		"GFX_EXT_PROGRAM_BINARY is incompatible with this context."
	);
}

/******************************************************/
static void _gfx_gl_get_program_binary(GLuint program, GLsizei bufsize, GLsizei* length, GLenum *binaryFormat, void* binary)
{
	if(length) *length = 0;

	_gfx_gl_program_binary_error();
}

/******************************************************/
static void _gfx_gl_program_binary(GLuint program, GLenum binaryFormat, const void* binary, GLsizei length)
{
	_gfx_gl_program_binary_error();
}

#endif

/******************************************************/
void _gfx_extensions_load(void)
{
	/* Get current window and context */
	GFX_Internal_Window* window = _gfx_window_get_current();
	if(!window) return;

	GFX_Extensions* ext = &window->extensions;

	/* Get OpenGL version */
	int major, minor;
	_gfx_platform_context_get(&major, &minor);

	/* Get OpenGL constants (a.k.a hardware limits) */
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, ext->limits + GFX_LIM_MAX_ACTIVE_TEXTURES);
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS,            ext->limits + GFX_LIM_MAX_COLOR_ATTACHMENTS);
	glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE,        ext->limits + GFX_LIM_MAX_CUBEMAP_SIZE);
	glGetIntegerv(GL_MAX_SAMPLES,                      ext->limits + GFX_LIM_MAX_SAMPLES);
	glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE,              ext->limits + GFX_LIM_MAX_TEXTURE_3D_SIZE);
	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS,         ext->limits + GFX_LIM_MAX_TEXTURE_LAYERS);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE,                 ext->limits + GFX_LIM_MAX_TEXTURE_SIZE);
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS,               ext->limits + GFX_LIM_MAX_VERTEX_ATTRIBS);

#ifdef GFX_GLES

	/* Get OpenGL constants (a.k.a hardware limits) */
	ext->limits[GFX_LIM_MAX_BUFFER_TEXTURE_SIZE] = 0;

	/* Default Extensions */
	ext->flags[GFX_EXT_BUFFER_TEXTURE]       = 0;
	ext->flags[GFX_EXT_GEOMETRY_SHADER]      = 0;
	ext->flags[GFX_EXT_INSTANCED_ATTRIBUTES] = 1;
	ext->flags[GFX_EXT_LAYERED_CUBEMAP]      = 0;
	ext->flags[GFX_EXT_MULTISAMPLE_TEXTURE]  = 0;
	ext->flags[GFX_EXT_PROGRAM_BINARY]       = 1;
	ext->flags[GFX_EXT_SEAMLESS_CUBEMAP]     = 0;
	ext->flags[GFX_EXT_TESSELLATION_SHADER]  = 0;
	ext->flags[GFX_EXT_TEXTURE_1D]           = 0;

	/* GLES, assumes 3.0+ */
	ext->ActiveTexture            = glActiveTexture;
	ext->AttachShader             = glAttachShader;
	ext->BindAttribLocation       = glBindAttribLocation;
	ext->BindBuffer               = glBindBuffer;
	ext->BindFramebuffer          = glBindFramebuffer;
	ext->BindTexture              = glBindTexture;
	ext->BindVertexArray          = glBindVertexArray;
	ext->BufferData               = glBufferData;
	ext->BufferSubData            = glBufferSubData;
	ext->Clear                    = glClear;
	ext->ClientWaitSync           = glClientWaitSync;
	ext->CompileShader            = glCompileShader;
	ext->CopyBufferSubData        = glCopyBufferSubData;
	ext->CreateProgram            = glCreateProgram;
	ext->CreateShader             = glCreateShader;
	ext->CullFace                 = glCullFace;
	ext->DeleteBuffers            = glDeleteBuffers;
	ext->DeleteFramebuffers       = glDeleteFramebuffers;
	ext->DeleteProgram            = glDeleteProgram;
	ext->DeleteShader             = glDeleteShader;
	ext->DeleteSync               = glDeleteSync;
	ext->DeleteTextures           = glDeleteTextures;
	ext->DeleteVertexArrays       = glDeleteVertexArrays;
	ext->DepthMask                = glDepthMask;
	ext->DetachShader             = glDetachShader;
	ext->Disable                  = glDisable;
	ext->DisableVertexAttribArray = glDisableVertexAttribArray;
	ext->DrawArrays               = glDrawArrays;
	ext->DrawArraysInstanced      = glDrawArraysInstanced;
	ext->DrawElements             = glDrawElements;
	ext->DrawElementsInstanced    = glDrawElementsInstanced;
	ext->Enable                   = glEnable;
	ext->EnableVertexAttribArray  = glEnableVertexAttribArray;
	ext->FenceSync                = glFenceSync;
	ext->FramebufferTexture1D     = _gfx_gles_framebuffer_texture_1d;
	ext->FramebufferTexture2D     = glFramebufferTexture2D;
	ext->FramebufferTextureLayer  = glFramebufferTextureLayer;
	ext->GenBuffers               = glGenBuffers;
	ext->GenerateMipmap           = glGenerateMipmap;
	ext->GenFramebuffers          = glGenFramebuffers;
	ext->GenTextures              = glGenTextures;
	ext->GenVertexArrays          = glGenVertexArrays;
	ext->GetBufferSubData         = _gfx_gles_get_buffer_sub_data;
	ext->GetError                 = glGetError;
	ext->GetProgramBinary         = glGetProgramBinary;
	ext->GetProgramInfoLog        = glGetProgramInfoLog;
	ext->GetProgramiv             = glGetProgramiv;
	ext->GetShaderInfoLog         = glGetShaderInfoLog;
	ext->GetShaderiv              = glGetShaderiv;
	ext->GetShaderSource          = glGetShaderSource;
	ext->LinkProgram              = glLinkProgram;
	ext->MapBufferRange           = glMapBufferRange;
	ext->PixelStorei              = glPixelStorei;
	ext->ProgramBinary            = glProgramBinary;
	ext->ShaderSource             = glShaderSource;
	ext->TexBuffer                = _gfx_gles_tex_buffer;
	ext->TexImage1D               = _gfx_gles_tex_image_1d;
	ext->TexImage2D               = glTexImage2D;
	ext->TexImage2DMultisample    = _gfx_gles_tex_image_2d_multisample;
	ext->TexImage3D               = glTexImage3D;
	ext->TexImage3DMultisample    = _gfx_gles_tex_image_3d_multisample;
	ext->TexParameteri            = glTexParameteri;
	ext->TexSubImage1D            = _gfx_gles_tex_sub_image_1d;
	ext->TexSubImage2D            = glTexSubImage2D;
	ext->TexSubImage3D            = glTexSubImage3D;
	ext->UnmapBuffer              = glUnmapBuffer;
	ext->UseProgram               = glUseProgram;
	ext->VertexAttribDivisor      = glVertexAttribDivisor;
	ext->VertexAttribIPointer     = glVertexAttribIPointer;
	ext->VertexAttribPointer      = glVertexAttribPointer;

#else

	/* Settings */
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	/* Get OpenGL constants (a.k.a hardware limits) */
	glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, ext->limits + GFX_LIM_MAX_BUFFER_TEXTURE_SIZE);

	/* Default Extensions */
	ext->flags[GFX_EXT_BUFFER_TEXTURE]      = 1;
	ext->flags[GFX_EXT_GEOMETRY_SHADER]     = 1;
	ext->flags[GFX_EXT_MULTISAMPLE_TEXTURE] = 1;
	ext->flags[GFX_EXT_SEAMLESS_CUBEMAP]    = 1;
	ext->flags[GFX_EXT_TEXTURE_1D]          = 1;

	/* Core, assumes 3.2+ context */
	ext->ActiveTexture            = (PFNGLACTIVETEXTUREPROC)            _gfx_platform_get_proc_address("glActiveTexture");
	ext->AttachShader             = (PFNGLATTACHSHADERPROC)             _gfx_platform_get_proc_address("glAttachShader");
	ext->BindAttribLocation       = (PFNGLBINDATTRIBLOCATIONPROC)       _gfx_platform_get_proc_address("glBindAttribLocation");
	ext->BindBuffer               = (PFNGLBINDBUFFERPROC)               _gfx_platform_get_proc_address("glBindBuffer");
	ext->BindFramebuffer          = (PFNGLBINDFRAMEBUFFERPROC)          _gfx_platform_get_proc_address("glBindFramebuffer");
	ext->BindTexture              = (PFNGLBINDTEXTUREPROC)              glBindTexture;
	ext->BindVertexArray          = (PFNGLBINDVERTEXARRAYPROC)          _gfx_platform_get_proc_address("glBindVertexArray");
	ext->BufferData               = (PFNGLBUFFERDATAPROC)               _gfx_platform_get_proc_address("glBufferData");
	ext->BufferSubData            = (PFNGLBUFFERSUBDATAPROC)            _gfx_platform_get_proc_address("glBufferSubData");
	ext->Clear                    = (PFNGLCLEARPROC)                    glClear;
	ext->ClientWaitSync           = (PFNGLCLIENTWAITSYNCPROC)           _gfx_platform_get_proc_address("glClientWaitSync");
	ext->CompileShader            = (PFNGLCOMPILESHADERPROC)            _gfx_platform_get_proc_address("glCompileShader");
	ext->CopyBufferSubData        = (PFNGLCOPYBUFFERSUBDATAPROC)        _gfx_platform_get_proc_address("glCopyBufferSubData");
	ext->CreateProgram            = (PFNGLCREATEPROGRAMPROC)            _gfx_platform_get_proc_address("glCreateProgram");
	ext->CreateShader             = (PFNGLCREATESHADERPROC)             _gfx_platform_get_proc_address("glCreateShader");
	ext->CullFace                 = (PFNGLCULLFACEPROC)                 glCullFace;
	ext->DeleteBuffers            = (PFNGLDELETEBUFFERSPROC)            _gfx_platform_get_proc_address("glDeleteBuffers");
	ext->DeleteFramebuffers       = (PFNGLDELETEFRAMEBUFFERSPROC)       _gfx_platform_get_proc_address("glDeleteFramebuffers");
	ext->DeleteProgram            = (PFNGLDELETEPROGRAMPROC)            _gfx_platform_get_proc_address("glDeleteProgram");
	ext->DeleteShader             = (PFNGLDELETESHADERPROC)             _gfx_platform_get_proc_address("glDeleteShader");
	ext->DeleteSync               = (PFNGLDELETESYNCPROC)               _gfx_platform_get_proc_address("glDeleteSync");
	ext->DeleteTextures           = (PFNGLDELETETEXTURESPROC)           glDeleteTextures;
	ext->DeleteVertexArrays       = (PFNGLDELETEVERTEXARRAYSPROC)       _gfx_platform_get_proc_address("glDeleteVertexArrays");
	ext->DepthMask                = (PFNGLDEPTHMASKPROC)                glDepthMask;
	ext->DetachShader             = (PFNGLDETACHSHADERPROC)             _gfx_platform_get_proc_address("glDetachShader");
	ext->Disable                  = (PFNGLDISABLEPROC)                  glDisable;
	ext->DisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) _gfx_platform_get_proc_address("glDisableVertexAttribArray");
	ext->DrawArrays               = (PFNGLDRAWARRAYSPROC)               glDrawArrays;
	ext->DrawArraysInstanced      = (PFNGLDRAWARRAYSINSTANCEDPROC)      _gfx_platform_get_proc_address("glDrawArraysInstanced");
	ext->DrawElements             = (PFNGLDRAWELEMENTSPROC)             glDrawElements;
	ext->DrawElementsInstanced    = (PFNGLDRAWELEMENTSINSTANCEDPROC)    _gfx_platform_get_proc_address("glDrawElementsInstanced");
	ext->Enable                   = (PFNGLENABLEPROC)                   glEnable;
	ext->EnableVertexAttribArray  = (PFNGLENABLEVERTEXATTRIBARRAYPROC)  _gfx_platform_get_proc_address("glEnableVertexAttribArray");
	ext->FenceSync                = (PFNGLFENCESYNCPROC)                _gfx_platform_get_proc_address("glFenceSync");
	ext->FramebufferTexture1D     = (PFNGLFRAMEBUFFERTEXTURE1DPROC)     _gfx_platform_get_proc_address("glFramebufferTexture1D");
	ext->FramebufferTexture2D     = (PFNGLFRAMEBUFFERTEXTURE2DPROC)     _gfx_platform_get_proc_address("glFramebufferTexture2D");
	ext->FramebufferTextureLayer  = (PFNGLFRAMEBUFFERTEXTURELAYERPROC)  _gfx_platform_get_proc_address("glFramebufferTextureLayer");
	ext->GenBuffers               = (PFNGLGENBUFFERSPROC)               _gfx_platform_get_proc_address("glGenBuffers");
	ext->GenerateMipmap           = (PFNGLGENERATEMIPMAPPROC)           _gfx_platform_get_proc_address("glGenerateMipmap");
	ext->GenFramebuffers          = (PFNGLGENFRAMEBUFFERSPROC)          _gfx_platform_get_proc_address("glGenFramebuffers");
	ext->GenTextures              = (PFNGLGENTEXTURESPROC)              glGenTextures;
	ext->GenVertexArrays          = (PFNGLGENVERTEXARRAYSPROC)          _gfx_platform_get_proc_address("glGenVertexArrays");
	ext->GetBufferSubData         = (PFNGLGETBUFFERSUBDATAPROC)         _gfx_platform_get_proc_address("glGetBufferSubData");
	ext->GetError                 = (PFNGLGETERRORPROC)                 glGetError;
	ext->GetProgramInfoLog        = (PFNGLGETPROGRAMINFOLOGPROC)        _gfx_platform_get_proc_address("glGetProgramInfoLog");
	ext->GetProgramiv             = (PFNGLGETPROGRAMIVPROC)             _gfx_platform_get_proc_address("glGetProgramiv");
	ext->GetShaderInfoLog         = (PFNGLGETSHADERINFOLOGPROC)         _gfx_platform_get_proc_address("glGetShaderInfoLog");
	ext->GetShaderiv              = (PFNGLGETSHADERIVPROC)              _gfx_platform_get_proc_address("glGetShaderiv");
	ext->GetShaderSource          = (PFNGLGETSHADERSOURCEPROC)          _gfx_platform_get_proc_address("glGetShaderSource");
	ext->LinkProgram              = (PFNGLLINKPROGRAMPROC)              _gfx_platform_get_proc_address("glLinkProgram");
	ext->MapBufferRange           = (PFNGLMAPBUFFERRANGEPROC)           _gfx_platform_get_proc_address("glMapBufferRange");
	ext->PixelStorei              = (PFNGLPIXELSTOREIPROC)              glPixelStorei;
	ext->ShaderSource             = (PFNGLSHADERSOURCEPROC)             _gfx_platform_get_proc_address("glShaderSource");
	ext->TexBuffer                = (PFNGLTEXBUFFERPROC)                _gfx_platform_get_proc_address("glTexBuffer");
	ext->TexImage1D               = (PFNGLTEXIMAGE1DPROC)               glTexImage1D;
	ext->TexImage2D               = (PFNGLTEXIMAGE2DPROC)               glTexImage2D;
	ext->TexImage2DMultisample    = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)    _gfx_platform_get_proc_address("glTexImage2DMultisample");
	ext->TexImage3D               = (PFNGLTEXIMAGE3DPROC)               _gfx_platform_get_proc_address("glTexImage3D");
	ext->TexImage3DMultisample    = (PFNGLTEXIMAGE3DMULTISAMPLEPROC)    _gfx_platform_get_proc_address("glTexImage3DMultisample");
	ext->TexParameteri            = (PFNGLTEXPARAMETERIPROC)            glTexParameteri;
	ext->TexSubImage1D            = (PFNGLTEXSUBIMAGE1DPROC)            glTexSubImage1D;
	ext->TexSubImage2D            = (PFNGLTEXSUBIMAGE2DPROC)            glTexSubImage2D;
	ext->TexSubImage3D            = (PFNGLTEXSUBIMAGE3DPROC)            _gfx_platform_get_proc_address("glTexSubImage3D");
	ext->UnmapBuffer              = (PFNGLUNMAPBUFFERPROC)              _gfx_platform_get_proc_address("glUnmapBuffer");
	ext->UseProgram               = (PFNGLUSEPROGRAMPROC)               _gfx_platform_get_proc_address("glUseProgram");
	ext->VertexAttribIPointer     = (PFNGLVERTEXATTRIBIPOINTERPROC)     _gfx_platform_get_proc_address("glVertexAttribIPointer");
	ext->VertexAttribPointer      = (PFNGLVERTEXATTRIBPOINTERPROC)      _gfx_platform_get_proc_address("glVertexAttribPointer");

	/* GFX_EXT_INSTANCED_ATTRIBUTES */
	if(major > 3 || minor > 2)
	{
		ext->flags[GFX_EXT_INSTANCED_ATTRIBUTES] = 1;
		ext->VertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC) _gfx_platform_get_proc_address("glVertexAttribDivisor");
	}
	else if(_gfx_platform_is_extension_supported(window->handle, "GL_ARB_instanced_arrays"))
	{
		ext->flags[GFX_EXT_INSTANCED_ATTRIBUTES] = 1;
		ext->VertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC) _gfx_platform_get_proc_address("VertexAttribDivisorARB");
	}
	else
	{
		ext->flags[GFX_EXT_INSTANCED_ATTRIBUTES] = 0;
		ext->VertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC) _gfx_gl_vertex_attrib_divisor;
	}

	/* GFX_EXT_LAYERED_CUBEMAP */
	if(major > 4 || _gfx_platform_is_extension_supported(window->handle, "GL_ARB_texture_cube_map_array"))
	{
		ext->flags[GFX_EXT_LAYERED_CUBEMAP] = 1;
	}
	else
	{
		ext->flags[GFX_EXT_LAYERED_CUBEMAP] = 0;
	}

	/* GFX_EXT_PROGRAM_BINARY */
	if(major > 4 || (major == 4 && minor > 0))
	{
		ext->flags[GFX_EXT_PROGRAM_BINARY] = 1;
		ext->GetProgramBinary    = (PFNGLGETPROGRAMBINARYPROC)  _gfx_platform_get_proc_address("glGetProgramBinary");
		ext->ProgramBinary       = (PFNGLPROGRAMBINARYPROC)     _gfx_platform_get_proc_address("glProgramBinary");
	}
	else if(_gfx_platform_is_extension_supported(window->handle, "GL_ARB_get_program_binary"))
	{
		ext->flags[GFX_EXT_PROGRAM_BINARY] = 1;
		ext->GetProgramBinary    = (PFNGLGETPROGRAMBINARYPROC)  _gfx_platform_get_proc_address("GetProgramBinary");
		ext->ProgramBinary       = (PFNGLPROGRAMBINARYPROC)     _gfx_platform_get_proc_address("ProgramBinary");
	}
	else
	{
		ext->flags[GFX_EXT_PROGRAM_BINARY] = 0;
		ext->GetProgramBinary    = (PFNGLGETPROGRAMBINARYPROC)  _gfx_gl_get_program_binary;
		ext->ProgramBinary       = (PFNGLPROGRAMBINARYPROC)     _gfx_gl_program_binary;
	}

	/* GFX_EXT_TESSELLATION_SHADER */
	if(major > 3 || _gfx_platform_is_extension_supported(window->handle, "GL_ARB_tessellation_shader"))
	{
		ext->flags[GFX_EXT_TESSELLATION_SHADER] = 1;
	}
	else
	{
		ext->flags[GFX_EXT_TESSELLATION_SHADER] = 0;
	}

#endif

	/* Set default state */
	_gfx_states_force_set(GFX_STATE_DEFAULT, ext);
}

/******************************************************/
int _gfx_extensions_is_in_string(const char* str, const char* ext)
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
