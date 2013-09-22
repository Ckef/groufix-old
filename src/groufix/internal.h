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

#ifndef GFX_INTERNAL_H
#define GFX_INTERNAL_H

#include "groufix/geometry.h"
#include "groufix/platform.h"

/* Correct OGL header */
#ifdef GFX_GLES
	#include <GLES3/gl3.h>
#else
	#define GL_GLEXT_PROTOTYPES
	#include <GL/glcorearb.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * OpenGL and Hardware Extensions
 *******************************************************/

/* Extension function pointers */
typedef void (*GFX_ATTACHSHADERPROC)             (GLuint, GLuint);
typedef GLenum (*GFX_CLIENTWAITSYNCPROC)         (GLsync, GLbitfield, GLuint64);
typedef void (*GFX_BINDATTRIBLOCATIONPROC)       (GLuint, GLuint, const GLchar*);
typedef void (*GFX_BINDBUFFERPROC)               (GLenum, GLuint);
typedef void (*GFX_BINDVERTEXARRAYPROC)          (GLuint);
typedef void (*GFX_BUFFERDATAPROC)               (GLenum, GLsizeiptr, const GLvoid*, GLenum);
typedef void (*GFX_BUFFERSUBDATAPROC)            (GLenum, GLintptr, GLsizeiptr, const GLvoid*);
typedef void (*GFX_COMPILESHADERPROC)            (GLuint);
typedef GLuint (*GFX_CREATEPROGRAMPROC)          (void);
typedef GLuint (*GFX_CREATESHADERPROC)           (GLenum);
typedef void (*GFX_DELETEBUFFERSPROC)            (GLsizei, const GLuint*);
typedef void (*GFX_DELETEPROGRAMPROC)            (GLuint);
typedef void (*GFX_DELETESHADERPROC)             (GLuint);
typedef void (*GFX_DELETESYNCPROC)               (GLsync);
typedef void (*GFX_DELETEVERTEXARRAYSPROC)       (GLsizei, const GLuint*);
typedef void (*GFX_DETACHSHADERPROC)             (GLuint, GLuint);
typedef void (*GFX_DISABLEVERTEXATTRIBARRAYPROC) (GLuint);
typedef void (*GFX_DRAWARRAYSINSTANCEDPROC)      (GLenum, GLint, GLsizei, GLsizei);
typedef void (*GFX_DRAWELEMENTSINSTANCEDPROC)    (GLenum, GLsizei, GLenum, const GLvoid*, GLsizei);
typedef void (*GFX_ENABLEVERTEXATTRIBARRAYPROC)  (GLuint);
typedef GLsync (*GFX_FENCESYNCPROC)              (GLenum, GLbitfield);
typedef void (*GFX_GENBUFFERSPROC)               (GLsizei, GLuint*);
typedef void (*GFX_GENVERTEXARRAYSPROC)          (GLsizei, GLuint*);
typedef void (*GFX_GETBUFFERSUBDATAPROC)         (GLenum, GLintptr, GLsizeiptr, GLvoid*);
typedef void (*GFX_GETPROGRAMBINARYPROC)         (GLuint, GLsizei, GLsizei*, GLenum*, void*);
typedef void (*GFX_GETPROGRAMINFOLOGPROC)        (GLuint, GLsizei, GLsizei*, GLchar*);
typedef void (*GFX_GETPROGRAMIVPROC)             (GLuint, GLenum, GLint*);
typedef void (*GFX_GETSHADERINFOLOGPROC)         (GLuint, GLsizei, GLsizei*, GLchar*);
typedef void (*GFX_GETSHADERIVPROC)              (GLuint, GLenum, GLint*);
typedef void (*GFX_GETSHADERSOURCEPROC)          (GLuint, GLsizei, GLsizei*, GLchar*);
typedef void (*GFX_LINKPROGRAMPROC)              (GLuint);
typedef void* (*GFX_MAPBUFFERRANGEPROC)          (GLenum, GLintptr, GLsizeiptr, GLbitfield);
typedef void (*GFX_PROGRAMBINARYPROC)            (GLuint, GLenum, const void*, GLsizei);
typedef void (*GFX_PROGRAMPARAMETERIPROC)        (GLuint, GLenum, GLint);
typedef void (*GFX_SHADERSOURCEPROC)             (GLuint, GLsizei, const GLchar**, const GLint*);
typedef GLboolean (*GFX_UNMAPBUFFERPROC)         (GLenum);
typedef void (*GFX_USEPROGRAMPROC)               (GLuint);
typedef void (*GFX_VERTEXATTRIBDIVISORPROC)      (GLuint, GLuint);
typedef void (*GFX_VERTEXATTRIBIPOINTERPROC)     (GLuint, GLint, GLenum, GLsizei, const GLvoid*);
typedef void (*GFX_VERTEXATTRIBPOINTERPROC)      (GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*);


/** OpenGL extensions, a.k.a HardwareExtensions */
typedef struct GFX_Extensions
{
	/* Hardware Extensions */
	unsigned char flags[GFX_EXT_COUNT];

	/* OpenGL Constants */
	GLint  MAX_VERTEX_ATTRIBS;

	/* OpenGL Extensions */
	GFX_ATTACHSHADERPROC              AttachShader;
	GFX_CLIENTWAITSYNCPROC            ClientWaitSync;
	GFX_BINDATTRIBLOCATIONPROC        BindAttribLocation;
	GFX_BINDBUFFERPROC                BindBuffer;
	GFX_BINDVERTEXARRAYPROC           BindVertexArray;
	GFX_BUFFERDATAPROC                BufferData;
	GFX_BUFFERSUBDATAPROC             BufferSubData;
	GFX_COMPILESHADERPROC             CompileShader;
	GFX_CREATEPROGRAMPROC             CreateProgram;
	GFX_CREATESHADERPROC              CreateShader;
	GFX_DELETEBUFFERSPROC             DeleteBuffers;
	GFX_DELETEPROGRAMPROC             DeleteProgram;
	GFX_DELETESHADERPROC              DeleteShader;
	GFX_DELETESYNCPROC                DeleteSync;
	GFX_DELETEVERTEXARRAYSPROC        DeleteVertexArrays;
	GFX_DETACHSHADERPROC              DetachShader;
	GFX_DISABLEVERTEXATTRIBARRAYPROC  DisableVertexAttribArray;
	GFX_DRAWARRAYSINSTANCEDPROC       DrawArraysInstanced;
	GFX_DRAWELEMENTSINSTANCEDPROC     DrawElementsInstanced;
	GFX_ENABLEVERTEXATTRIBARRAYPROC   EnableVertexAttribArray;
	GFX_FENCESYNCPROC                 FenceSync;
	GFX_GENBUFFERSPROC                GenBuffers;
	GFX_GENVERTEXARRAYSPROC           GenVertexArrays;
	GFX_GETBUFFERSUBDATAPROC          GetBufferSubData;
	GFX_GETPROGRAMBINARYPROC          GetProgramBinary;    /* GFX_EXT_PROGRAM_BINARY */
	GFX_GETPROGRAMINFOLOGPROC         GetProgramInfoLog;
	GFX_GETPROGRAMIVPROC              GetProgramiv;
	GFX_GETSHADERINFOLOGPROC          GetShaderInfoLog;
	GFX_GETSHADERIVPROC               GetShaderiv;
	GFX_GETSHADERSOURCEPROC           GetShaderSource;
	GFX_LINKPROGRAMPROC               LinkProgram;
	GFX_MAPBUFFERRANGEPROC            MapBufferRange;
	GFX_PROGRAMBINARYPROC             ProgramBinary;       /* GFX_EXT_PROGRAM_BINARY */
	GFX_PROGRAMPARAMETERIPROC         ProgramParameteri;   /* GFX_EXT_PROGRAM_BINARY */
	GFX_SHADERSOURCEPROC              ShaderSource;
	GFX_UNMAPBUFFERPROC               UnmapBuffer;
	GFX_USEPROGRAMPROC                UseProgram;
	GFX_VERTEXATTRIBDIVISORPROC       VertexAttribDivisor; /* GFX_EXT_INSTANCED_ATTRIBUTES */
	GFX_VERTEXATTRIBIPOINTERPROC      VertexAttribIPointer;
	GFX_VERTEXATTRIBPOINTERPROC       VertexAttribPointer;

} GFX_Extensions;


/********************************************************
 * Internal window data & methods
 *******************************************************/

/** Internal window */
typedef struct GFX_Internal_Window
{
	/* Super class */
	GFXWindow window;

	/* Hidden data */
	GFX_Platform_Window  handle;
	GFX_Extensions       extensions;

} GFX_Internal_Window;


/**
 * Returns the top level window associated with a platform window.
 *
 */
GFX_Internal_Window* _gfx_window_get_from_handle(GFX_Platform_Window handle);

/**
 * Sets the window as the current render target.
 *
 */
void _gfx_window_make_current(GFX_Internal_Window* window);

/**
 * Returns the current window.
 *
 * Returns NULL if no window is active.
 *
 */
GFX_Internal_Window* _gfx_window_get_current(void);

/**
 * Loads all extensions for the current window's context.
 *
 * Should not be called by the platform.
 *
 */
void _gfx_extensions_load(void);


/********************************************************
 * Generic hardware object reconstruction
 *******************************************************/

/** Generic hardware object */
typedef void* GFX_Hardware_Object;


/** Generic hardware object operator */
typedef void (*GFX_Hardware_Object_Func) (GFX_Hardware_Object, const GFX_Extensions*);


/** Hardware vtable, can all be NULL */
typedef struct GFX_Hardware_Funcs
{
	GFX_Hardware_Object_Func free;    /* GPU free request */
	GFX_Hardware_Object_Func save;    /* Prepare for context destruction */
	GFX_Hardware_Object_Func restore; /* Restore for new context */

} GFX_Hardware_Funcs;


/**
 * Registers a new generic hardware object.
 *
 * @return Non-zero on success.
 *
 * When an object is registered, it will be asked to free when all contexts are destroyed,
 * or reconstructed when the main context is destroyed.
 *
 */
int _gfx_hardware_object_register(GFX_Hardware_Object object, const GFX_Hardware_Funcs* funcs);

/**
 * Unregisters a generic hardware object.
 *
 */
void _gfx_hardware_object_unregister(GFX_Hardware_Object object);

/**
 * Issue free request of all hardware objects.
 *
 * This will issue the free request and unregister ALL objects.
 * Thus this callback is NOT allowed to unregister the object.
 *
 */
void _gfx_hardware_objects_free(const GFX_Extensions* ext);

/**
 * Issue save method of all hardware objects.
 *
 * During this operation, the current window and context are considered "deleted".
 * It is guaranteed another context is still active, this is only meant for objects which can't be shared.
 *
 */
void _gfx_hardware_objects_save(const GFX_Extensions* ext);

/**
 * Issue restore method of all hardware objects.
 *
 * During this operation, a new window and context is current.
 *
 */
void _gfx_hardware_objects_restore(const GFX_Extensions* ext);


/********************************************************
 * Internal hardware object access
 *******************************************************/

/**
 * Returns the current handle of a buffer.
 *
 */
GLuint _gfx_buffer_get_handle(const GFXBuffer* buffer);

/**
 * Returns the VAO of a layout.
 *
 */
GLuint _gfx_vertex_layout_get_handle(const GFXVertexLayout layout);


#ifdef __cplusplus
}
#endif

#endif // GFX_INTERNAL_H
