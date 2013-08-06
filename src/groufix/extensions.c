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
#include "groufix/platform.h"
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
	int major, minor;
	_gfx_platform_context_get(&major, &minor);

#ifdef GFX_GLES

	/* Default Extensions */
	ext->extensions[GFX_EXT_GEOMETRY_SHADER]      = 0;
	ext->extensions[GFX_EXT_INSTANCED_ATTRIBUTES] = 1;
	ext->extensions[GFX_EXT_PROGRAM_BINARY]       = 1;
	ext->extensions[GFX_EXT_TESSELLATION_SHADER]  = 0;

	/* GLES, assumes 3.0+ */
	ext->AttachShader             = (GFX_ATTACHSHADERPROC)             glAttachShader;
	ext->BindAttribLocation       = (GFX_BINDATTRIBLOCATIONPROC)       glBindAttribLocation;
	ext->BindBuffer               = (GFX_BINDBUFFERPROC)               glBindBuffer;
	ext->BindVertexArray          = (GFX_BINDVERTEXARRAYPROC)          glBindVertexArray;
	ext->BufferData               = (GFX_BUFFERDATAPROC)               glBufferData;
	ext->BufferSubData            = (GFX_BUFFERSUBDATAPROC)            glBufferSubData;
	ext->CopyBufferSubData        = (GFX_COPYBUFFERSUBDATAPROC)        glCopyBufferSubData;
	ext->CompileShader            = (GFX_COMPILESHADERPROC)            glCompileShader;
	ext->CreateProgram            = (GFX_CREATEPROGRAMPROC)            glCreateProgram;
	ext->CreateShader             = (GFX_CREATESHADERPROC)             glCreateShader;
	ext->DeleteBuffers            = (GFX_DELETEBUFFERSPROC)            glDeleteBuffers;
	ext->DeleteProgram            = (GFX_DELETEPROGRAMPROC)            glDeleteProgram;
	ext->DeleteShader             = (GFX_DELETESHADERPROC)             glDeleteShader;
	ext->DeleteVertexArrays       = (GFX_DELETEVERTEXARRAYSPROC)       glDeleteVertexArrays;
	ext->DetachShader             = (GFX_DETACHSHADERPROC)             glDetachShader;
	ext->DisableVertexAttribArray = (GFX_DISABLEVERTEXATTRIBARRAYPROC) glDisableVertexAttribArray;
	ext->DrawArraysInstanced      = (GFX_DRAWARRAYSINSTANCEDPROC)      glDrawArraysInstanced;
	ext->DrawElementsInstanced    = (GFX_DRAWELEMENTSINSTANCEDPROC)    glDrawElementsInstanced;
	ext->EnableVertexAttribArray  = (GFX_ENABLEVERTEXATTRIBARRAYPROC)  glEnableVertexAttribArray;
	ext->GenBuffers               = (GFX_GENBUFFERSPROC)               glGenBuffers;
	ext->GenVertexArrays          = (GFX_GENVERTEXARRAYSPROC)          glGenVertexArrays;
	ext->GetAttribLocation        = (GFX_GETATTRIBLOCATIONPROC)        glGetAttribLocation;
	ext->GetBufferParameteriv     = (GFX_GETBUFFERPARAMETERIVPROC)     glGetBufferParameteriv;
	ext->GetBufferPointerv        = (GFX_GETBUFFERPOINTERVPROC)        glGetBufferPointerv;
	ext->GetBufferSubData         = (GFX_GETBUFFERSUBDATAPROC)         _gfx_gles_get_buffer_sub_data;
	ext->GetProgramBinary         = (GFX_GETPROGRAMBINARYPROC)         glGetProgramBinary;
	ext->GetProgramInfoLog        = (GFX_GETPROGRAMINFOLOGPROC)        glGetProgramInfoLog;
	ext->GetProgramiv             = (GFX_GETPROGRAMIVPROC)             glGetProgramiv;
	ext->GetShaderInfoLog         = (GFX_GETSHADERINFOLOGPROC)         glGetShaderInfoLog;
	ext->GetShaderiv              = (GFX_GETSHADERIVPROC)              glGetShaderiv;
	ext->GetShaderSource          = (GFX_GETSHADERSOURCEPROC)          glGetShaderSource;
	ext->GetVertexAttribIiv       = (GFX_GETVERTEXATTRIBIIVPROC)       glGetVertexAttribIiv;
	ext->GetVertexAttribIuiv      = (GFX_GETVERTEXATTRIBIUIVPROC)      glGetVertexAttribIuiv;
	ext->GetVertexAttribPointerv  = (GFX_GETVERTEXATTRIBPOINTERVPROC)  glGetVertexAttribPointerv;
	ext->LinkProgram              = (GFX_LINKPROGRAMPROC)              glLinkProgram;
	ext->MapBufferRange           = (GFX_MAPBUFFERRANGEPROC)           glMapBufferRange;
	ext->ProgramBinary            = (GFX_PROGRAMBINARYPROC)            glProgramBinary;
	ext->ProgramParameteri        = (GFX_PROGRAMPARAMETERIPROC)        glProgramParameteri;
	ext->ShaderSource             = (GFX_SHADERSOURCEPROC)             glShaderSource;
	ext->UnmapBuffer              = (GFX_UNMAPBUFFERPROC)              glUnmapBuffer;
	ext->UseProgram               = (GFX_USEPROGRAMPROC)               glUseProgram;
	ext->VertexAttribDivisor      = (GFX_VERTEXATTRIBDIVISORPROC)      glVertexAttribDivisor;
	ext->VertexAttribIPointer     = (GFX_VERTEXATTRIBIPOINTERPROC)     glVertexAttribIPointer;
	ext->VertexAttribPointer      = (GFX_VERTEXATTRIBPOINTERPROC)      glVertexAttribPointer;

#else

	/* Default Extensions */
	ext->extensions[GFX_EXT_GEOMETRY_SHADER] = 1;

	/* Core, assumes 3.2+ context */
	ext->AttachShader             = (GFX_ATTACHSHADERPROC)             _gfx_platform_get_proc_address("glAttachShader");
	ext->BindAttribLocation       = (GFX_BINDATTRIBLOCATIONPROC)       _gfx_platform_get_proc_address("glBindAttribLocation");
	ext->BindBuffer               = (GFX_BINDBUFFERPROC)               _gfx_platform_get_proc_address("glBindBuffer");
	ext->BindVertexArray          = (GFX_BINDVERTEXARRAYPROC)          _gfx_platform_get_proc_address("glBindVertexArray");
	ext->BufferData               = (GFX_BUFFERDATAPROC)               _gfx_platform_get_proc_address("glBufferData");
	ext->BufferSubData            = (GFX_BUFFERSUBDATAPROC)            _gfx_platform_get_proc_address("glBufferSubData");
	ext->CopyBufferSubData        = (GFX_COPYBUFFERSUBDATAPROC)        _gfx_platform_get_proc_address("glCopyBufferSubData");
	ext->CompileShader            = (GFX_COMPILESHADERPROC)            _gfx_platform_get_proc_address("glCompileShader");
	ext->CreateProgram            = (GFX_CREATEPROGRAMPROC)            _gfx_platform_get_proc_address("glCreateProgram");
	ext->CreateShader             = (GFX_CREATESHADERPROC)             _gfx_platform_get_proc_address("glCreateShader");
	ext->DeleteBuffers            = (GFX_DELETEBUFFERSPROC)            _gfx_platform_get_proc_address("glDeleteBuffers");
	ext->DeleteProgram            = (GFX_DELETEPROGRAMPROC)            _gfx_platform_get_proc_address("glDeleteProgram");
	ext->DeleteShader             = (GFX_DELETESHADERPROC)             _gfx_platform_get_proc_address("glDeleteShader");
	ext->DeleteVertexArrays       = (GFX_DELETEVERTEXARRAYSPROC)       _gfx_platform_get_proc_address("glDeleteVertexArrays");
	ext->DetachShader             = (GFX_DETACHSHADERPROC)             _gfx_platform_get_proc_address("glDetachShader");
	ext->DisableVertexAttribArray = (GFX_DISABLEVERTEXATTRIBARRAYPROC) _gfx_platform_get_proc_address("glDisableVertexAttribArrayProc");
	ext->DrawArraysInstanced      = (GFX_DRAWARRAYSINSTANCEDPROC)      _gfx_platform_get_proc_address("glDrawArraysInstanced");
	ext->DrawElementsInstanced    = (GFX_DRAWELEMENTSINSTANCEDPROC)    _gfx_platform_get_proc_address("glDrawElementsInstanced");
	ext->EnableVertexAttribArray  = (GFX_ENABLEVERTEXATTRIBARRAYPROC)  _gfx_platform_get_proc_address("glEnableVertexAttribArrayProc");
	ext->GenBuffers               = (GFX_GENBUFFERSPROC)               _gfx_platform_get_proc_address("glGenBuffers");
	ext->GenVertexArrays          = (GFX_GENVERTEXARRAYSPROC)          _gfx_platform_get_proc_address("glGenVertexArrays");
	ext->GetAttribLocation        = (GFX_GETATTRIBLOCATIONPROC)        _gfx_platform_get_proc_address("glGetAttribLocation");
	ext->GetBufferParameteriv     = (GFX_GETBUFFERPARAMETERIVPROC)     _gfx_platform_get_proc_address("glGetBufferParameteriv");
	ext->GetBufferPointerv        = (GFX_GETBUFFERPOINTERVPROC)        _gfx_platform_get_proc_address("glGetBufferPointerv");
	ext->GetBufferSubData         = (GFX_GETBUFFERSUBDATAPROC)         _gfx_platform_get_proc_address("glGetBufferSubData");
	ext->GetProgramInfoLog        = (GFX_GETPROGRAMINFOLOGPROC)        _gfx_platform_get_proc_address("glGetProgramInfoLog");
	ext->GetProgramiv             = (GFX_GETPROGRAMIVPROC)             _gfx_platform_get_proc_address("glGetProgramiv");
	ext->GetShaderInfoLog         = (GFX_GETSHADERINFOLOGPROC)         _gfx_platform_get_proc_address("glGetShaderInfoLog");
	ext->GetShaderiv              = (GFX_GETSHADERIVPROC)              _gfx_platform_get_proc_address("glGetShaderiv");
	ext->GetShaderSource          = (GFX_GETSHADERSOURCEPROC)          _gfx_platform_get_proc_address("glGetShaderSource");
	ext->GetVertexAttribIiv       = (GFX_GETVERTEXATTRIBIIVPROC)       _gfx_platform_get_proc_address("glGetVertexAttribIiv");
	ext->GetVertexAttribIuiv      = (GFX_GETVERTEXATTRIBIUIVPROC)      _gfx_platform_get_proc_address("glGetVertexAttribIuiv");
	ext->GetVertexAttribPointerv  = (GFX_GETVERTEXATTRIBPOINTERVPROC)  _gfx_platform_get_proc_address("glGetVertexAttribPointerv");
	ext->LinkProgram              = (GFX_LINKPROGRAMPROC)              _gfx_platform_get_proc_address("glLinkProgram");
	ext->MapBufferRange           = (GFX_MAPBUFFERRANGEPROC)           _gfx_platform_get_proc_address("glMapBufferRange");
	ext->ShaderSource             = (GFX_SHADERSOURCEPROC)             _gfx_platform_get_proc_address("glShaderSource");
	ext->UnmapBuffer              = (GFX_UNMAPBUFFERPROC)              _gfx_platform_get_proc_address("glUnmapBuffer");
	ext->UseProgram               = (GFX_USEPROGRAMPROC)               _gfx_platform_get_proc_address("glUseProgram");
	ext->VertexAttribIPointer     = (GFX_VERTEXATTRIBIPOINTERPROC)     _gfx_platform_get_proc_address("glVertexAttribIPointer");
	ext->VertexAttribPointer      = (GFX_VERTEXATTRIBPOINTERPROC)      _gfx_platform_get_proc_address("glVertexAttribPointer");

	/* GFX_EXT_INSTANCED_ATTRIBUTES */
	if(major > 3 || minor > 2)
	{
		ext->extensions[GFX_EXT_INSTANCED_ATTRIBUTES] = 1;
		ext->VertexAttribDivisor = (GFX_VERTEXATTRIBDIVISORPROC)      _gfx_platform_get_proc_address("glVertexAttribDivisor");
	}
	else if(_gfx_platform_is_extension_supported(window->handle, "GL_ARB_instanced_arrays"))
	{
		ext->extensions[GFX_EXT_INSTANCED_ATTRIBUTES] = 1;
		ext->VertexAttribDivisor = (GFX_VERTEXATTRIBDIVISORPROC)      _gfx_platform_get_proc_address("VertexAttribDivisorARB");
	}
	else
	{
		ext->extensions[GFX_EXT_INSTANCED_ATTRIBUTES] = 0;
		ext->VertexAttribDivisor = (GFX_VERTEXATTRIBDIVISORPROC)      _gfx_gl_vertex_attrib_divisor;
	}

	/* GFX_EXT_PROGRAM_BINARY */
	if(major > 4 || (major == 4 && minor > 0))
	{
		ext->extensions[GFX_EXT_PROGRAM_BINARY] = 1;
		ext->GetProgramBinary    = (GFX_GETPROGRAMBINARYPROC)         _gfx_platform_get_proc_address("glGetProgramBinary");
		ext->ProgramBinary       = (GFX_PROGRAMBINARYPROC)            _gfx_platform_get_proc_address("glProgramBinary");
		ext->ProgramParameteri   = (GFX_PROGRAMPARAMETERIPROC)        _gfx_platform_get_proc_address("glProgramParameteri");
	}
	else if(_gfx_platform_is_extension_supported(window->handle, "GL_ARB_get_program_binary"))
	{
		ext->extensions[GFX_EXT_PROGRAM_BINARY] = 1;
		ext->GetProgramBinary    = (GFX_GETPROGRAMBINARYPROC)         _gfx_platform_get_proc_address("GetProgramBinary");
		ext->ProgramBinary       = (GFX_PROGRAMBINARYPROC)            _gfx_platform_get_proc_address("ProgramBinary");
		ext->ProgramParameteri   = (GFX_PROGRAMPARAMETERIPROC)        _gfx_platform_get_proc_address("ProgramParameteri");
	}
	else
	{
		ext->extensions[GFX_EXT_PROGRAM_BINARY] = 0;
		ext->GetProgramBinary    = (GFX_GETPROGRAMBINARYPROC)         _gfx_gl_get_program_binary;
		ext->ProgramBinary       = (GFX_PROGRAMBINARYPROC)            _gfx_gl_program_binary;
		ext->ProgramParameteri   = (GFX_PROGRAMPARAMETERIPROC)        _gfx_gl_program_parameter_i;
	}

	/* GFX_EXT_TESSELLATION_SHADER */
	if(major > 3)
	{
		ext->extensions[GFX_EXT_TESSELLATION_SHADER] = 1;
	}
	else if(_gfx_platform_is_extension_supported(window->handle, "GL_ARB_tessellation_shader"))
	{
		ext->extensions[GFX_EXT_TESSELLATION_SHADER] = 1;
	}
	else
	{
		ext->extensions[GFX_EXT_TESSELLATION_SHADER] = 0;
	}

#endif

	/* Same everywhere */
	ext->DrawArrays   = (GFX_DRAWARRAYSPROC)   glDrawArrays;
	ext->DrawElements = (GFX_DRAWELEMENTSPROC) glDrawElements;
	ext->GetError     = (GFX_GETERRORPROC)     glGetError;
	ext->GetIntegerv  = (GFX_GETINTEGERVPROC)  glGetIntegerv;

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
