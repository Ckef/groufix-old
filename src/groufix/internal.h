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

#include "groufix/hardware.h"
#include "groufix/utils.h"
#include "groufix/window.h"

/* Get build target */
#if defined(_WIN32) || defined(__WIN32__)
	#define GFX_WIN32
#elif defined(__APPLE__) || defined(__MACH__)
	#define GFX_OSX
#elif defined(__unix) || defined(__unix__) || defined(__linux__)
	#define GFX_UNIX

/* Maybe GLES? */
#elif !defined(GFX_GLES)
	#error "Platform not supported"
#endif

/* Windows */
#ifdef GFX_WIN32

	/* Windows XP */
	#ifndef WINVER
	#define WINVER 0x0501
	#endif

	/* Nothing extra */
	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif

	#ifndef VC_EXTRALEAN
	#define VC_EXTRALEAN
	#endif
	
	/* To avoid redifinitions */
	#include <windows.h>

#endif

/* Correct OGL header */
#if defined(GFX_INTERNAL_GL_LEGACY)
	#include <GL/gl.h>
#elif defined(GFX_GLES)
	#include <GLES3/gl3.h>
#else
	#include <GL/glcorearb.h>
#endif

/* Extensions from void */
#define VOID_TO_EXT(x) ((const GFX_Extensions*)x)

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * OpenGL and Hardware Extensions
 *******************************************************/

/** \brief Proc Address */
typedef void (*GFXProcAddress)(void);


/* Extension function pointers */
typedef void (*GFX_ATTACHSHADERPROC)             (GLuint, GLuint);
typedef void (*GFX_BINDATTRIBLOCATIONPROC)       (GLuint, GLuint, const GLchar*);
typedef void (*GFX_BINDBUFFERPROC)               (GLenum, GLuint);
typedef void (*GFX_BINDVERTEXARRAYPROC)          (GLuint);
typedef void (*GFX_BUFFERDATAPROC)               (GLenum, GLsizeiptr, const GLvoid*, GLenum);
typedef void (*GFX_BUFFERSUBDATAPROC)            (GLenum, GLintptr, GLsizeiptr, const GLvoid*);
typedef void (*GFX_COMPILESHADERPROC)            (GLuint);
typedef void (*GFX_COPYBUFFERSUBDATAPROC)        (GLenum, GLenum, GLintptr, GLintptr, GLsizeiptr);
typedef GLuint (*GFX_CREATEPROGRAMPROC)          (void);
typedef GLuint (*GFX_CREATESHADERPROC)           (GLenum);
typedef void (*GFX_DELETEBUFFERSPROC)            (GLsizei, const GLuint*);
typedef void (*GFX_DELETEPROGRAMPROC)            (GLuint);
typedef void (*GFX_DELETESHADERPROC)             (GLuint);
typedef void (*GFX_DELETEVERTEXARRAYSPROC)       (GLsizei, const GLuint*);
typedef void (*GFX_DETACHSHADERPROC)             (GLuint, GLuint);
typedef void (*GFX_DISABLEVERTEXATTRIBARRAYPROC) (GLuint);
typedef void (*GFX_DRAWARRAYSPROC)               (GLenum, GLint, GLsizei);
typedef void (*GFX_DRAWARRAYSINSTANCEDPROC)      (GLenum, GLint, GLsizei, GLsizei);
typedef void (*GFX_DRAWELEMENTSPROC)             (GLenum, GLsizei, GLenum, const GLvoid*);
typedef void (*GFX_DRAWELEMENTSINSTANCEDPROC)    (GLenum, GLsizei, GLenum, const GLvoid*, GLsizei);
typedef void (*GFX_ENABLEVERTEXATTRIBARRAYPROC)  (GLuint);
typedef void (*GFX_GENBUFFERSPROC)               (GLsizei, GLuint*);
typedef void (*GFX_GENVERTEXARRAYSPROC)          (GLsizei, GLuint*);
typedef GLint (*GFX_GETATTRIBLOCATIONPROC)       (GLuint, const GLchar*);
typedef void (*GFX_GETBUFFERPARAMETERIVPROC)     (GLenum, GLenum, GLint*);
typedef void (*GFX_GETBUFFERPOINTERVPROC)        (GLenum, GLenum, GLvoid**);
typedef void (*GFX_GETBUFFERSUBDATAPROC)         (GLenum, GLintptr, GLsizeiptr, GLvoid*);
typedef GLenum (*GFX_GETERRORPROC)               (void);
typedef void (*GFX_GETINTEGERVPROC)              (GLenum, GLint*);
typedef void (*GFX_GETPROGRAMBINARYPROC)         (GLuint, GLsizei, GLsizei*, GLenum*, void*);
typedef void (*GFX_GETPROGRAMINFOLOGPROC)        (GLuint, GLsizei, GLsizei*, GLchar*);
typedef void (*GFX_GETPROGRAMIVPROC)             (GLuint, GLenum, GLint*);
typedef void (*GFX_GETSHADERINFOLOGPROC)         (GLuint, GLsizei, GLsizei*, GLchar*);
typedef void (*GFX_GETSHADERIVPROC)              (GLuint, GLenum, GLint*);
typedef void (*GFX_GETSHADERSOURCEPROC)          (GLuint, GLsizei, GLsizei*, GLchar*);
typedef void (*GFX_GETVERTEXATTRIBIIVPROC)       (GLuint, GLenum, GLint*);
typedef void (*GFX_GETVERTEXATTRIBIUIVPROC)      (GLuint, GLenum, GLuint*);
typedef void (*GFX_GETVERTEXATTRIBPOINTERVPROC)  (GLuint, GLenum, GLvoid**);
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


/** \brief OpenGL extensions, a.k.a HardwareContext */
typedef struct GFX_Extensions
{
	/* Hardware Extensions */
	unsigned char extensions[GFX_EXT_COUNT];

	/* OpenGL Extensions */
	GFX_ATTACHSHADERPROC              AttachShader;
	GFX_BINDATTRIBLOCATIONPROC        BindAttribLocation;
	GFX_BINDBUFFERPROC                BindBuffer;
	GFX_BINDVERTEXARRAYPROC           BindVertexArray;
	GFX_BUFFERDATAPROC                BufferData;
	GFX_BUFFERSUBDATAPROC             BufferSubData;
	GFX_COMPILESHADERPROC             CompileShader;
	GFX_COPYBUFFERSUBDATAPROC         CopyBufferSubData;
	GFX_CREATEPROGRAMPROC             CreateProgram;
	GFX_CREATESHADERPROC              CreateShader;
	GFX_DELETEBUFFERSPROC             DeleteBuffers;
	GFX_DELETEPROGRAMPROC             DeleteProgram;
	GFX_DELETESHADERPROC              DeleteShader;
	GFX_DELETEVERTEXARRAYSPROC        DeleteVertexArrays;
	GFX_DETACHSHADERPROC              DetachShader;
	GFX_DISABLEVERTEXATTRIBARRAYPROC  DisableVertexAttribArray;
	GFX_DRAWARRAYSPROC                DrawArrays;
	GFX_DRAWARRAYSINSTANCEDPROC       DrawArraysInstanced;
	GFX_DRAWELEMENTSPROC              DrawElements;
	GFX_DRAWELEMENTSINSTANCEDPROC     DrawElementsInstanced;
	GFX_ENABLEVERTEXATTRIBARRAYPROC   EnableVertexAttribArray;
	GFX_GENBUFFERSPROC                GenBuffers;
	GFX_GENVERTEXARRAYSPROC           GenVertexArrays;
	GFX_GETATTRIBLOCATIONPROC         GetAttribLocation;
	GFX_GETBUFFERPARAMETERIVPROC      GetBufferParameteriv;
	GFX_GETBUFFERPOINTERVPROC         GetBufferPointerv;
	GFX_GETBUFFERSUBDATAPROC          GetBufferSubData;
	GFX_GETERRORPROC                  GetError;
	GFX_GETINTEGERVPROC               GetIntegerv;
	GFX_GETPROGRAMBINARYPROC          GetProgramBinary;    /* GFX_EXT_PROGRAM_BINARY */
	GFX_GETPROGRAMINFOLOGPROC         GetProgramInfoLog;
	GFX_GETPROGRAMIVPROC              GetProgramiv;
	GFX_GETSHADERINFOLOGPROC          GetShaderInfoLog;
	GFX_GETSHADERIVPROC               GetShaderiv;
	GFX_GETSHADERSOURCEPROC           GetShaderSource;
	GFX_GETVERTEXATTRIBIIVPROC        GetVertexAttribIiv;
	GFX_GETVERTEXATTRIBIUIVPROC       GetVertexAttribIuiv;
	GFX_GETVERTEXATTRIBPOINTERVPROC   GetVertexAttribPointerv;
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
 * Platform definitions
 *******************************************************/

/** \brief A Screen */
typedef void* GFX_Platform_Screen;


/** \brief A Window */
typedef void* GFX_Platform_Window;


/** \brief Window initialization attributes */
typedef struct GFX_Platform_Attributes
{
	GFX_Platform_Screen  screen;
	const char*          name;

	unsigned int         width;
	unsigned int         height;
	int                  x;
	int                  y;

	GFXColorDepth        depth;

} GFX_Platform_Attributes;


/********************************************************
 * Internal window data & methods
 *******************************************************/

/** \brief Internal window */
typedef struct GFX_Internal_Window
{
	/* Super class */
	GFXWindow window;

	/* Hidden data */
	GFX_Platform_Window  handle;
	GFX_Extensions       extensions;

} GFX_Internal_Window;


/**
 * \brief Returns the top level window associated with a platform window.
 *
 */
GFX_Internal_Window* _gfx_window_get_from_handle(GFX_Platform_Window handle);

/**
 * \brief Sets the window as the current render target.
 *
 */
void _gfx_window_make_current(GFX_Internal_Window* window);

/**
 * \brief Returns the current window.
 *
 * Returns NULL if no window is active.
 *
 */
GFX_Internal_Window* _gfx_window_get_current(void);

/**
 * \brief Loads all extensions for the current window's context.
 *
 */
void _gfx_extensions_load(void);

/**
 * \brief Returns whether the OpenGL extension can be found in the space seperated string.
 *
 * This method is primarily used in the platform implementations.
 *
 */
int _gfx_extensions_is_in_string(const char* str, const char* ext);


/********************************************************
 * Event triggers (must be called manually by platform)
 *******************************************************/

/**
 * \brief Called when a window is requested to close.
 *
 * Used to intercept a user requested window termination.
 *
 */
void _gfx_event_window_close(GFX_Platform_Window handle);

/**
 * \brief Handles a key press event.
 *
 * \param key   What key is pressed.
 * \param state State of some special keys.
 *
 */
void _gfx_event_key_press(GFX_Platform_Window handle, GFXKey key, GFXKeyState state);

/**
 * \brief Handles a key release event.
 *
 * \param key   What key is released.
 * \param state State of some special keys.
 *
 */
void _gfx_event_key_release(GFX_Platform_Window handle, GFXKey key, GFXKeyState state);

/**
 * \brief Called when the cursor moves in a window.
 *
 * \param x     X coordinate of the cursor relative to the window.
 * \param y     Y coordinate of the cursor relative to the window.
 * \param state State of some special keys.
 *
 */
void _gfx_event_mouse_move(GFX_Platform_Window handle, int x, int y, GFXKeyState state);

/**
 * \brief Handles a mouse key press event.
 *
 * \param key   What key is pressed.
 * \param x     X coordinate of the cursor relative to the window.
 * \param y     Y coordinate of the cursor relative to the window.
 * \param state State of some special keys.
 *
 */
void _gfx_event_mouse_press(GFX_Platform_Window handle, GFXMouseKey key, int x, int y, GFXKeyState state);

/**
 * \brief Handles a mouse key release event.
 *
 * \param key   What key is pressed.
 * \param x     X coordinate of the cursor relative to the window.
 * \param y     Y coordinate of the cursor relative to the window.
 * \param state State of some special keys.
 *
 */
void _gfx_event_mouse_release(GFX_Platform_Window handle, GFXMouseKey key, int x, int y, GFXKeyState state);

/**
 * \brief Handles a mouse wheel event.
 *
 * \param xoffset Mouse wheel tilt (negative = left, positive = right).
 * \param yoffset Mouse wheel rotate (negative = down, positive = up).
 * \param x       X coordinate of the cursor relative to the window.
 * \param y       Y coordinate of the cursor relative to the window.
 * \param state   State of some special keys.
 *
 */
void _gfx_event_mouse_wheel(GFX_Platform_Window handle, int xoffset, int yoffset, int x, int y, GFXKeyState state);


#ifdef __cplusplus
}
#endif

#endif // GFX_INTERNAL_H
