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

#include "groufix/internal.h"
#include "groufix/errors.h"

#include <string.h>

#ifdef GFX_GLES

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

/******************************************************/
static void _gfx_gl_program_parameter_i(GLuint program, GLenum pname, GLint value)
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
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	/* Get OpenGL constants */
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &ext->MAX_VERTEX_ATTRIBS);

#ifdef GFX_GLES

	/* Default Extensions */
	ext->flags[GFX_EXT_GEOMETRY_SHADER]      = 0;
	ext->flags[GFX_EXT_INSTANCED_ATTRIBUTES] = 1;
	ext->flags[GFX_EXT_PROGRAM_BINARY]       = 1;
	ext->flags[GFX_EXT_TESSELLATION_SHADER]  = 0;

	/* GLES, assumes 3.0+ */
	ext->AttachShader             = glAttachShader;
	ext->ClientWaitSync           = glClientWaitSync;
	ext->BindAttribLocation       = glBindAttribLocation;
	ext->BindBuffer               = glBindBuffer;
	ext->BindVertexArray          = glBindVertexArray;
	ext->BufferData               = glBufferData;
	ext->BufferSubData            = glBufferSubData;
	ext->CompileShader            = glCompileShader;
	ext->CreateProgram            = glCreateProgram;
	ext->CreateShader             = glCreateShader;
	ext->DeleteBuffers            = glDeleteBuffers;
	ext->DeleteProgram            = glDeleteProgram;
	ext->DeleteShader             = glDeleteShader;
	ext->DeleteSync               = glDeleteSync;
	ext->DeleteVertexArrays       = glDeleteVertexArrays;
	ext->DetachShader             = glDetachShader;
	ext->DisableVertexAttribArray = glDisableVertexAttribArray;
	ext->DrawArraysInstanced      = glDrawArraysInstanced;
	ext->DrawElementsInstanced    = glDrawElementsInstanced;
	ext->EnableVertexAttribArray  = glEnableVertexAttribArray;
	ext->FenceSync                = glFenceSync;
	ext->GenBuffers               = glGenBuffers;
	ext->GenVertexArrays          = glGenVertexArrays;
	ext->GetBufferSubData         = _gfx_gles_get_buffer_sub_data;
	ext->GetProgramBinary         = glGetProgramBinary;
	ext->GetProgramInfoLog        = glGetProgramInfoLog;
	ext->GetProgramiv             = glGetProgramiv;
	ext->GetShaderInfoLog         = glGetShaderInfoLog;
	ext->GetShaderiv              = glGetShaderiv;
	ext->GetShaderSource          = glGetShaderSource;
	ext->LinkProgram              = glLinkProgram;
	ext->MapBufferRange           = glMapBufferRange;
	ext->ProgramBinary            = glProgramBinary;
	ext->ProgramParameteri        = glProgramParameteri;
	ext->ShaderSource             = glShaderSource;
	ext->UnmapBuffer              = glUnmapBuffer;
	ext->UseProgram               = glUseProgram;
	ext->VertexAttribDivisor      = glVertexAttribDivisor;
	ext->VertexAttribIPointer     = glVertexAttribIPointer;
	ext->VertexAttribPointer      = glVertexAttribPointer;

#else

	/* Default Extensions */
	ext->flags[GFX_EXT_GEOMETRY_SHADER] = 1;

	/* Core, assumes 3.2+ context */
	ext->AttachShader             = (PFNGLATTACHSHADERPROC)             _gfx_platform_get_proc_address("glAttachShader");
	ext->ClientWaitSync           = (PFNGLCLIENTWAITSYNCPROC)           _gfx_platform_get_proc_address("glClientWaitSync");
	ext->BindAttribLocation       = (PFNGLBINDATTRIBLOCATIONPROC)       _gfx_platform_get_proc_address("glBindAttribLocation");
	ext->BindBuffer               = (PFNGLBINDBUFFERPROC)               _gfx_platform_get_proc_address("glBindBuffer");
	ext->BindVertexArray          = (PFNGLBINDVERTEXARRAYPROC)          _gfx_platform_get_proc_address("glBindVertexArray");
	ext->BufferData               = (PFNGLBUFFERDATAPROC)               _gfx_platform_get_proc_address("glBufferData");
	ext->BufferSubData            = (PFNGLBUFFERSUBDATAPROC)            _gfx_platform_get_proc_address("glBufferSubData");
	ext->CompileShader            = (PFNGLCOMPILESHADERPROC)            _gfx_platform_get_proc_address("glCompileShader");
	ext->CreateProgram            = (PFNGLCREATEPROGRAMPROC)            _gfx_platform_get_proc_address("glCreateProgram");
	ext->CreateShader             = (PFNGLCREATESHADERPROC)             _gfx_platform_get_proc_address("glCreateShader");
	ext->DeleteBuffers            = (PFNGLDELETEBUFFERSPROC)            _gfx_platform_get_proc_address("glDeleteBuffers");
	ext->DeleteProgram            = (PFNGLDELETEPROGRAMPROC)            _gfx_platform_get_proc_address("glDeleteProgram");
	ext->DeleteShader             = (PFNGLDELETESHADERPROC)             _gfx_platform_get_proc_address("glDeleteShader");
	ext->DeleteSync               = (PFNGLDELETESYNCPROC)               _gfx_platform_get_proc_address("glDeleteSync");
	ext->DeleteVertexArrays       = (PFNGLDELETEVERTEXARRAYSPROC)       _gfx_platform_get_proc_address("glDeleteVertexArrays");
	ext->DetachShader             = (PFNGLDETACHSHADERPROC)             _gfx_platform_get_proc_address("glDetachShader");
	ext->DisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) _gfx_platform_get_proc_address("glDisableVertexAttribArray");
	ext->DrawArraysInstanced      = (PFNGLDRAWARRAYSINSTANCEDPROC)      _gfx_platform_get_proc_address("glDrawArraysInstanced");
	ext->DrawElementsInstanced    = (PFNGLDRAWELEMENTSINSTANCEDPROC)    _gfx_platform_get_proc_address("glDrawElementsInstanced");
	ext->EnableVertexAttribArray  = (PFNGLENABLEVERTEXATTRIBARRAYPROC)  _gfx_platform_get_proc_address("glEnableVertexAttribArray");
	ext->FenceSync                = (PFNGLFENCESYNCPROC)                _gfx_platform_get_proc_address("glFenceSync");
	ext->GenBuffers               = (PFNGLGENBUFFERSPROC)               _gfx_platform_get_proc_address("glGenBuffers");
	ext->GenVertexArrays          = (PFNGLGENVERTEXARRAYSPROC)          _gfx_platform_get_proc_address("glGenVertexArrays");
	ext->GetBufferSubData         = (PFNGLGETBUFFERSUBDATAPROC)         _gfx_platform_get_proc_address("glGetBufferSubData");
	ext->GetProgramInfoLog        = (PFNGLGETPROGRAMINFOLOGPROC)        _gfx_platform_get_proc_address("glGetProgramInfoLog");
	ext->GetProgramiv             = (PFNGLGETPROGRAMIVPROC)             _gfx_platform_get_proc_address("glGetProgramiv");
	ext->GetShaderInfoLog         = (PFNGLGETSHADERINFOLOGPROC)         _gfx_platform_get_proc_address("glGetShaderInfoLog");
	ext->GetShaderiv              = (PFNGLGETSHADERIVPROC)              _gfx_platform_get_proc_address("glGetShaderiv");
	ext->GetShaderSource          = (PFNGLGETSHADERSOURCEPROC)          _gfx_platform_get_proc_address("glGetShaderSource");
	ext->LinkProgram              = (PFNGLLINKPROGRAMPROC)              _gfx_platform_get_proc_address("glLinkProgram");
	ext->MapBufferRange           = (PFNGLMAPBUFFERRANGEPROC)           _gfx_platform_get_proc_address("glMapBufferRange");
	ext->ShaderSource             = (PFNGLSHADERSOURCEPROC)             _gfx_platform_get_proc_address("glShaderSource");
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

	/* GFX_EXT_PROGRAM_BINARY */
	if(major > 4 || (major == 4 && minor > 0))
	{
		ext->flags[GFX_EXT_PROGRAM_BINARY] = 1;
		ext->GetProgramBinary    = (PFNGLGETPROGRAMBINARYPROC)  _gfx_platform_get_proc_address("glGetProgramBinary");
		ext->ProgramBinary       = (PFNGLPROGRAMBINARYPROC)     _gfx_platform_get_proc_address("glProgramBinary");
		ext->ProgramParameteri   = (PFNGLPROGRAMPARAMETERIPROC) _gfx_platform_get_proc_address("glProgramParameteri");
	}
	else if(_gfx_platform_is_extension_supported(window->handle, "GL_ARB_get_program_binary"))
	{
		ext->flags[GFX_EXT_PROGRAM_BINARY] = 1;
		ext->GetProgramBinary    = (PFNGLGETPROGRAMBINARYPROC)  _gfx_platform_get_proc_address("GetProgramBinary");
		ext->ProgramBinary       = (PFNGLPROGRAMBINARYPROC)     _gfx_platform_get_proc_address("ProgramBinary");
		ext->ProgramParameteri   = (PFNGLPROGRAMPARAMETERIPROC) _gfx_platform_get_proc_address("ProgramParameteri");
	}
	else
	{
		ext->flags[GFX_EXT_PROGRAM_BINARY] = 0;
		ext->GetProgramBinary    = (PFNGLGETPROGRAMBINARYPROC)  _gfx_gl_get_program_binary;
		ext->ProgramBinary       = (PFNGLPROGRAMBINARYPROC)     _gfx_gl_program_binary;
		ext->ProgramParameteri   = (PFNGLPROGRAMPARAMETERIPROC) _gfx_gl_program_parameter_i;
	}

	/* GFX_EXT_TESSELLATION_SHADER */
	if(major > 3)
	{
		ext->flags[GFX_EXT_TESSELLATION_SHADER] = 1;
	}
	else if(_gfx_platform_is_extension_supported(window->handle, "GL_ARB_tessellation_shader"))
	{
		ext->flags[GFX_EXT_TESSELLATION_SHADER] = 1;
	}
	else
	{
		ext->flags[GFX_EXT_TESSELLATION_SHADER] = 0;
	}

#endif

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
		if((found == str || *(found - 1) == ' ') && (*end == ' ' || *end == '\0'))
			return 1;

		found = strstr(end, ext);
	}

	return 0;
}
