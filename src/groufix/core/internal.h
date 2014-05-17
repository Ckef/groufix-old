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

#ifndef GFX_CORE_INTERNAL_H
#define GFX_CORE_INTERNAL_H

#include "groufix/core/platform.h"
#include "groufix/core/pipeline.h"

/* Correct OGL header */
#ifdef GFX_GLES
	#include <GLES3/gl31.h>
#else
	#include <GL/glcorearb.h>
#endif

/* Correct context versions */
#ifdef GFX_GLES

	/* Minimal supported context */
	#define GFX_CONTEXT_MAJOR_MIN 3
	#define GFX_CONTEXT_MINOR_MIN 0

	/* Maximal supported context */
	#define GFX_CONTEXT_MAJOR_MAX 3
	#define GFX_CONTEXT_MINOR_MAX 1

	/* Greatest minor version possible */
	#define GFX_CONTEXT_ALL_MINORS_MAX 1

#else

	/* Minimal supported context */
	#define GFX_CONTEXT_MAJOR_MIN 3
	#define GFX_CONTEXT_MINOR_MIN 2

	/* Maximal supported context */
	#define GFX_CONTEXT_MAJOR_MAX 4
	#define GFX_CONTEXT_MINOR_MAX 4

	/* Greatest minor version possible */
	#define GFX_CONTEXT_ALL_MINORS_MAX 4

#endif

#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * OpenGL and Hardware Extensions
 *******************************************************/

/* Extension function pointers */
typedef void (APIENTRYP GFX_ACTIVETEXTUREPROC)                     (GLenum);
typedef void (APIENTRYP GFX_ATTACHSHADERPROC)                      (GLuint, GLuint);
typedef void (APIENTRYP GFX_BEGINTRANSFORMFEEDBACKPROC)            (GLenum);
typedef void (APIENTRYP GFX_BINDATTRIBLOCATIONPROC)                (GLuint, GLuint, const GLchar*);
typedef void (APIENTRYP GFX_BINDBUFFERPROC)                        (GLenum, GLuint);
typedef void (APIENTRYP GFX_BINDBUFFERRANGEPROC)                   (GLenum, GLuint, GLuint, GLintptr, GLsizeiptr);
typedef void (APIENTRYP GFX_BINDFRAMEBUFFERPROC)                   (GLenum, GLuint);
typedef void (APIENTRYP GFX_BINDTEXTUREPROC)                       (GLenum, GLuint);
typedef void (APIENTRYP GFX_BINDVERTEXARRAYPROC)                   (GLuint);
typedef void (APIENTRYP GFX_BLENDEQUATIONSEPARATEPROC)             (GLenum, GLenum);
typedef void (APIENTRYP GFX_BLENDFUNCSEPARATEPROC)                 (GLenum, GLenum, GLenum, GLenum);
typedef void (APIENTRYP GFX_BUFFERDATAPROC)                        (GLenum, GLsizeiptr, const GLvoid*, GLenum);
typedef void (APIENTRYP GFX_BUFFERSUBDATAPROC)                     (GLenum, GLintptr, GLsizeiptr, const GLvoid*);
typedef void (APIENTRYP GFX_CLEARPROC)                             (GLbitfield);
typedef GLenum (APIENTRYP GFX_CLIENTWAITSYNCPROC)                  (GLsync, GLbitfield, GLuint64);
typedef void (APIENTRYP GFX_COMPILESHADERPROC)                     (GLuint);
typedef void (APIENTRYP GFX_COPYBUFFERSUBDATAPROC)                 (GLenum, GLenum, GLintptr, GLintptr, GLsizeiptr);
typedef GLuint (APIENTRYP GFX_CREATEPROGRAMPROC)                   (void);
typedef GLuint (APIENTRYP GFX_CREATESHADERPROC)                    (GLenum);
typedef void (APIENTRYP GFX_CULLFACEPROC)                          (GLenum);
typedef void (APIENTRYP GFX_DELETEBUFFERSPROC)                     (GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_DELETEFRAMEBUFFERSPROC)                (GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_DELETEPROGRAMPROC)                     (GLuint);
typedef void (APIENTRYP GFX_DELETESHADERPROC)                      (GLuint);
typedef void (APIENTRYP GFX_DELETESYNCPROC)                        (GLsync);
typedef void (APIENTRYP GFX_DELETETEXTURESPROC)                    (GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_DELETEVERTEXARRAYSPROC)                (GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_DEPTHFUNCPROC)                         (GLenum);
typedef void (APIENTRYP GFX_DEPTHMASKPROC)                         (GLboolean);
typedef void (APIENTRYP GFX_DETACHSHADERPROC)                      (GLuint, GLuint);
typedef void (APIENTRYP GFX_DISABLEPROC)                           (GLenum);
typedef void (APIENTRYP GFX_DISABLEVERTEXATTRIBARRAYPROC)          (GLuint);
typedef void (APIENTRYP GFX_DRAWARRAYSPROC)                        (GLenum, GLint, GLsizei);
typedef void (APIENTRYP GFX_DRAWARRAYSINSTANCEDPROC)               (GLenum, GLint, GLsizei, GLsizei);
typedef void (APIENTRYP GFX_DRAWARRAYSINSTANCEDBASEINSTANCEPROC)   (GLenum, GLint, GLsizei, GLsizei, GLuint);
typedef void (APIENTRYP GFX_DRAWBUFFERSPROC)                       (GLsizei, const GLenum*);
typedef void (APIENTRYP GFX_DRAWELEMENTSPROC)                      (GLenum, GLsizei, GLenum, const GLvoid*);
typedef void (APIENTRYP GFX_DRAWELEMENTSINSTANCEDPROC)             (GLenum, GLsizei, GLenum, const GLvoid*, GLsizei);
typedef void (APIENTRYP GFX_DRAWELEMENTSINSTANCEDBASEINSTANCEPROC) (GLenum, GLsizei, GLenum, const GLvoid*, GLsizei, GLuint);
typedef void (APIENTRYP GFX_ENABLEPROC)                            (GLenum);
typedef void (APIENTRYP GFX_ENABLEVERTEXATTRIBARRAYPROC)           (GLuint);
typedef void (APIENTRYP GFX_ENDTRANSFORMFEEDBACKPROC)              (void);
typedef GLsync (APIENTRYP GFX_FENCESYNCPROC)                       (GLenum, GLbitfield);
typedef void (APIENTRYP GFX_FRAMEBUFFERTEXTURE1DPROC)              (GLenum, GLenum, GLenum, GLuint, GLint);
typedef void (APIENTRYP GFX_FRAMEBUFFERTEXTURE2DPROC)              (GLenum, GLenum, GLenum, GLuint, GLint);
typedef void (APIENTRYP GFX_FRAMEBUFFERTEXTURELAYERPROC)           (GLenum, GLenum, GLuint, GLint, GLint);
typedef void (APIENTRYP GFX_GENBUFFERSPROC)                        (GLsizei, GLuint*);
typedef void (APIENTRYP GFX_GENERATEMIPMAPPROC)                    (GLenum);
typedef void (APIENTRYP GFX_GENFRAMEBUFFERSPROC)                   (GLsizei, GLuint*);
typedef void (APIENTRYP GFX_GENTEXTURESPROC)                       (GLsizei, GLuint*);
typedef void (APIENTRYP GFX_GENVERTEXARRAYSPROC)                   (GLsizei, GLuint*);
typedef void (APIENTRYP GFX_GETACTIVEUNIFORMPROC)                  (GLuint, GLuint, GLsizei, GLsizei*, GLint*, GLenum*, GLchar*);
typedef void (APIENTRYP GFX_GETACTIVEUNIFORMBLOCKIVPROC)           (GLuint, GLuint, GLenum, GLint*);
typedef void (APIENTRYP GFX_GETACTIVEUNIFORMSIVPROC)               (GLuint, GLsizei, const GLuint*, GLenum, GLint*);
typedef void (APIENTRYP GFX_GETBUFFERSUBDATAPROC)                  (GLenum, GLintptr, GLsizeiptr, GLvoid*);
typedef GLenum (APIENTRYP GFX_GETERRORPROC)                        (void);
typedef void (APIENTRYP GFX_GETPROGRAMBINARYPROC)                  (GLuint, GLsizei, GLsizei*, GLenum*, void*);
typedef void (APIENTRYP GFX_GETPROGRAMINFOLOGPROC)                 (GLuint, GLsizei, GLsizei*, GLchar*);
typedef void (APIENTRYP GFX_GETPROGRAMIVPROC)                      (GLuint, GLenum, GLint*);
typedef void (APIENTRYP GFX_GETSHADERINFOLOGPROC)                  (GLuint, GLsizei, GLsizei*, GLchar*);
typedef void (APIENTRYP GFX_GETSHADERIVPROC)                       (GLuint, GLenum, GLint*);
typedef void (APIENTRYP GFX_GETSHADERSOURCEPROC)                   (GLuint, GLsizei, GLsizei*, GLchar*);
typedef GLuint (APIENTRYP GFX_GETUNIFORMBLOCKINDEXPROC)            (GLuint, const GLchar*);
typedef void (APIENTRYP GFX_GETUNIFORMINDICESPROC)                 (GLuint, GLsizei, const GLchar*const*, GLuint*);
typedef GLint (APIENTRYP GFX_GETUNIFORMLOCATIONPROC)               (GLuint, const GLchar*);
typedef GLboolean (APIENTRYP GFX_ISTEXTUREPROC)                    (GLuint);
typedef void (APIENTRYP GFX_LINKPROGRAMPROC)                       (GLuint);
typedef void* (APIENTRYP GFX_MAPBUFFERRANGEPROC)                   (GLenum, GLintptr, GLsizeiptr, GLbitfield);
typedef void (APIENTRYP GFX_PATCHPARAMETERIPROC)                   (GLenum, GLint);
typedef void (APIENTRYP GFX_PIXELSTOREIPROC)                       (GLenum, GLint);
typedef void (APIENTRYP GFX_POLYGONMODEPROC)                       (GLenum, GLenum);
typedef void (APIENTRYP GFX_PROGRAMBINARYPROC)                     (GLuint, GLenum, const void*, GLsizei);
typedef void (APIENTRYP GFX_PROGRAMPARAMETERIPROC)                 (GLuint, GLenum, GLint);
typedef void (APIENTRYP GFX_SHADERSOURCEPROC)                      (GLuint, GLsizei, const GLchar*const*, const GLint*);
typedef void (APIENTRYP GFX_STENCILFUNCSEPARATEPROC)               (GLenum, GLenum, GLint, GLuint);
typedef void (APIENTRYP GFX_STENCILOPSEPARATEPROC)                 (GLenum, GLenum, GLenum, GLenum);
typedef void (APIENTRYP GFX_TEXBUFFERPROC)                         (GLenum, GLenum, GLuint);
typedef void (APIENTRYP GFX_TEXIMAGE1DPROC)                        (GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const GLvoid*);
typedef void (APIENTRYP GFX_TEXIMAGE2DPROC)                        (GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*);
typedef void (APIENTRYP GFX_TEXIMAGE2DMULTISAMPLEPROC)             (GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean);
typedef void (APIENTRYP GFX_TEXIMAGE3DPROC)                        (GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*);
typedef void (APIENTRYP GFX_TEXIMAGE3DMULTISAMPLEPROC)             (GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean);
typedef void (APIENTRYP GFX_TEXPARAMETERIPROC)                     (GLenum, GLenum, GLint);
typedef void (APIENTRYP GFX_TEXSUBIMAGE1DPROC)                     (GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const GLvoid*);
typedef void (APIENTRYP GFX_TEXSUBIMAGE2DPROC)                     (GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*);
typedef void (APIENTRYP GFX_TEXSUBIMAGE3DPROC)                     (GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*);
typedef void (APIENTRYP GFX_TRANSFORMFEEDBACKVARYINGSPROC)         (GLuint, GLsizei, const GLchar*const*, GLenum);
typedef void (APIENTRYP GFX_UNIFORM1FVPROC)                        (GLint, GLsizei, const GLfloat*);
typedef void (APIENTRYP GFX_UNIFORM1IVPROC)                        (GLint, GLsizei, const GLint*);
typedef void (APIENTRYP GFX_UNIFORM1UIVPROC)                       (GLint, GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_UNIFORM2FVPROC)                        (GLint, GLsizei, const GLfloat*);
typedef void (APIENTRYP GFX_UNIFORM2IVPROC)                        (GLint, GLsizei, const GLint*);
typedef void (APIENTRYP GFX_UNIFORM2UIVPROC)                       (GLint, GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_UNIFORM3FVPROC)                        (GLint, GLsizei, const GLfloat*);
typedef void (APIENTRYP GFX_UNIFORM3IVPROC)                        (GLint, GLsizei, const GLint*);
typedef void (APIENTRYP GFX_UNIFORM3UIVPROC)                       (GLint, GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_UNIFORM4FVPROC)                        (GLint, GLsizei, const GLfloat*);
typedef void (APIENTRYP GFX_UNIFORM4IVPROC)                        (GLint, GLsizei, const GLint*);
typedef void (APIENTRYP GFX_UNIFORM4UIVPROC)                       (GLint, GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_UNIFORMBLOCKBINDINGPROC)               (GLuint, GLuint, GLuint);
typedef void (APIENTRYP GFX_UNIFORMMATRIX2FVPROC)                  (GLint, GLsizei, GLboolean, const GLfloat*);
typedef void (APIENTRYP GFX_UNIFORMMATRIX3FVPROC)                  (GLint, GLsizei, GLboolean, const GLfloat*);
typedef void (APIENTRYP GFX_UNIFORMMATRIX4FVPROC)                  (GLint, GLsizei, GLboolean, const GLfloat*);
typedef GLboolean (APIENTRYP GFX_UNMAPBUFFERPROC)                  (GLenum);
typedef void (APIENTRYP GFX_USEPROGRAMPROC)                        (GLuint);
typedef void (APIENTRYP GFX_VERTEXATTRIBDIVISORPROC)               (GLuint, GLuint);
typedef void (APIENTRYP GFX_VERTEXATTRIBIPOINTERPROC)              (GLuint, GLint, GLenum, GLsizei, const GLvoid*);
typedef void (APIENTRYP GFX_VERTEXATTRIBPOINTERPROC)               (GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*);
typedef void (APIENTRYP GFX_VIEWPORTPROC)                          (GLint, GLint, GLsizei, GLsizei);


/** OpenGL extensions & context states */
typedef struct GFX_Extensions
{
	/* Hardware Extensions & Limits */
	unsigned char  flags[GFX_EXT_COUNT];
	GLint          limits[GFX_LIM_COUNT];

	/* State & bound objects */
	GFXPipeState   state;
	GLuint         pipeline; /* Currently bound FBO */
	GLuint         layout;   /* Currently bound VAO */
	GLuint         program;  /* Currently used program */

	/* Viewport & state values */
	unsigned int   width;
	unsigned int   height;
	unsigned char  packAlignment;
	unsigned char  unpackAlignment;
	unsigned int   patchVertices;

	/* Binding points */
	void*          uniformBuffers;
	void*          textureUnits;


	/* OpenGL Extensions */
	GFX_ACTIVETEXTUREPROC                      ActiveTexture;
	GFX_ATTACHSHADERPROC                       AttachShader;
	GFX_BEGINTRANSFORMFEEDBACKPROC             BeginTransformFeedback;
	GFX_BINDATTRIBLOCATIONPROC                 BindAttribLocation;
	GFX_BINDBUFFERPROC                         BindBuffer;
	GFX_BINDBUFFERRANGEPROC                    BindBufferRange;
	GFX_BINDFRAMEBUFFERPROC                    BindFramebuffer;
	GFX_BINDTEXTUREPROC                        BindTexture;
	GFX_BINDVERTEXARRAYPROC                    BindVertexArray;
	GFX_BLENDEQUATIONSEPARATEPROC              BlendEquationSeparate;
	GFX_BLENDFUNCSEPARATEPROC                  BlendFuncSeparate;
	GFX_BUFFERDATAPROC                         BufferData;
	GFX_BUFFERSUBDATAPROC                      BufferSubData;
	GFX_CLEARPROC                              Clear;
	GFX_CLIENTWAITSYNCPROC                     ClientWaitSync;
	GFX_COMPILESHADERPROC                      CompileShader;
	GFX_COPYBUFFERSUBDATAPROC                  CopyBufferSubData;
	GFX_CREATEPROGRAMPROC                      CreateProgram;
	GFX_CREATESHADERPROC                       CreateShader;
	GFX_CULLFACEPROC                           CullFace;
	GFX_DELETEBUFFERSPROC                      DeleteBuffers;
	GFX_DELETEFRAMEBUFFERSPROC                 DeleteFramebuffers;
	GFX_DELETEPROGRAMPROC                      DeleteProgram;
	GFX_DELETESHADERPROC                       DeleteShader;
	GFX_DELETESYNCPROC                         DeleteSync;
	GFX_DELETETEXTURESPROC                     DeleteTextures;
	GFX_DELETEVERTEXARRAYSPROC                 DeleteVertexArrays;
	GFX_DEPTHFUNCPROC                          DepthFunc;
	GFX_DEPTHMASKPROC                          DepthMask;
	GFX_DETACHSHADERPROC                       DetachShader;
	GFX_DISABLEPROC                            Disable;
	GFX_DISABLEVERTEXATTRIBARRAYPROC           DisableVertexAttribArray;
	GFX_DRAWARRAYSPROC                         DrawArrays;
	GFX_DRAWARRAYSINSTANCEDPROC                DrawArraysInstanced;
	GFX_DRAWARRAYSINSTANCEDBASEINSTANCEPROC    DrawArraysInstancedBaseInstance;   /* GFX_EXT_INTSANCED_BASE_ATTRIBUTES */
	GFX_DRAWBUFFERSPROC                        DrawBuffers;
	GFX_DRAWELEMENTSPROC                       DrawElements;
	GFX_DRAWELEMENTSINSTANCEDPROC              DrawElementsInstanced;
	GFX_DRAWELEMENTSINSTANCEDBASEINSTANCEPROC  DrawElementsInstancedBaseInstance; /* GFX_EXT_INSTANCED_BASE_ATTRIBUTES */
	GFX_ENABLEPROC                             Enable;
	GFX_ENABLEVERTEXATTRIBARRAYPROC            EnableVertexAttribArray;
	GFX_ENDTRANSFORMFEEDBACKPROC               EndTransformFeedback;
	GFX_FENCESYNCPROC                          FenceSync;
	GFX_FRAMEBUFFERTEXTURE1DPROC               FramebufferTexture1D;              /* GFX_EXT_TEXTURE_1D or GFX_EXT_BUFFER_TEXTURE */
	GFX_FRAMEBUFFERTEXTURE2DPROC               FramebufferTexture2D;
	GFX_FRAMEBUFFERTEXTURELAYERPROC            FramebufferTextureLayer;
	GFX_GENBUFFERSPROC                         GenBuffers;
	GFX_GENERATEMIPMAPPROC                     GenerateMipmap;
	GFX_GENFRAMEBUFFERSPROC                    GenFramebuffers;
	GFX_GENTEXTURESPROC                        GenTextures;
	GFX_GENVERTEXARRAYSPROC                    GenVertexArrays;
	GFX_GETACTIVEUNIFORMPROC                   GetActiveUniform;
	GFX_GETACTIVEUNIFORMBLOCKIVPROC            GetActiveUniformBlockiv;
	GFX_GETACTIVEUNIFORMSIVPROC                GetActiveUniformsiv;
	GFX_GETBUFFERSUBDATAPROC                   GetBufferSubData;
	GFX_GETERRORPROC                           GetError;
	GFX_GETPROGRAMBINARYPROC                   GetProgramBinary;                  /* GFX_EXT_PROGRAM_BINARY */
	GFX_GETPROGRAMINFOLOGPROC                  GetProgramInfoLog;
	GFX_GETPROGRAMIVPROC                       GetProgramiv;
	GFX_GETSHADERINFOLOGPROC                   GetShaderInfoLog;
	GFX_GETSHADERIVPROC                        GetShaderiv;
	GFX_GETSHADERSOURCEPROC                    GetShaderSource;
	GFX_GETUNIFORMBLOCKINDEXPROC               GetUniformBlockIndex;
	GFX_GETUNIFORMINDICESPROC                  GetUniformIndices;
	GFX_GETUNIFORMLOCATIONPROC                 GetUniformLocation;
	GFX_ISTEXTUREPROC                          IsTexture;
	GFX_LINKPROGRAMPROC                        LinkProgram;
	GFX_MAPBUFFERRANGEPROC                     MapBufferRange;
	GFX_PATCHPARAMETERIPROC                    PatchParameteri;                   /* GFX_EXT_TESSELLATION_SHADER */
	GFX_PIXELSTOREIPROC                        PixelStorei;
	GFX_POLYGONMODEPROC                        PolygonMode;                       /* GFX_EXT_POLYGON_STATE */
	GFX_PROGRAMBINARYPROC                      ProgramBinary;                     /* GFX_EXT_PROGRAM_BINARY */
	GFX_PROGRAMPARAMETERIPROC                  ProgramParameteri;                 /* GFX_EXT_SEPARABLE_PROGRAM */
	GFX_SHADERSOURCEPROC                       ShaderSource;
	GFX_STENCILFUNCSEPARATEPROC                StencilFuncSeparate;
	GFX_STENCILOPSEPARATEPROC                  StencilOpSeparate;
	GFX_TEXBUFFERPROC                          TexBuffer;                         /* GFX_EXT_BUFFER_TEXTURE */
	GFX_TEXIMAGE1DPROC                         TexImage1D;                        /* GFX_EXT_TEXTURE_1D */
	GFX_TEXIMAGE2DPROC                         TexImage2D;
	GFX_TEXIMAGE2DMULTISAMPLEPROC              TexImage2DMultisample;             /* GFX_EXT_MULTISAMPLE_TEXTURE */
	GFX_TEXIMAGE3DPROC                         TexImage3D;
	GFX_TEXIMAGE3DMULTISAMPLEPROC              TexImage3DMultisample;             /* GFX_EXT_MULTISAMPLE_TEXTURE */
	GFX_TEXPARAMETERIPROC                      TexParameteri;
	GFX_TEXSUBIMAGE1DPROC                      TexSubImage1D;                     /* GFX_EXT_TEXTURE1D */
	GFX_TEXSUBIMAGE2DPROC                      TexSubImage2D;
	GFX_TEXSUBIMAGE3DPROC                      TexSubImage3D;
	GFX_TRANSFORMFEEDBACKVARYINGSPROC          TransformFeedbackVaryings;
	GFX_UNIFORM1FVPROC                         Uniform1fv;
	GFX_UNIFORM1IVPROC                         Uniform1iv;
	GFX_UNIFORM1UIVPROC                        Uniform1uiv;
	GFX_UNIFORM2FVPROC                         Uniform2fv;
	GFX_UNIFORM2IVPROC                         Uniform2iv;
	GFX_UNIFORM2UIVPROC                        Uniform2uiv;
	GFX_UNIFORM3FVPROC                         Uniform3fv;
	GFX_UNIFORM3IVPROC                         Uniform3iv;
	GFX_UNIFORM3UIVPROC                        Uniform3uiv;
	GFX_UNIFORM4FVPROC                         Uniform4fv;
	GFX_UNIFORM4IVPROC                         Uniform4iv;
	GFX_UNIFORM4UIVPROC                        Uniform4uiv;
	GFX_UNIFORMBLOCKBINDINGPROC                UniformBlockBinding;
	GFX_UNIFORMMATRIX2FVPROC                   UniformMatrix2fv;
	GFX_UNIFORMMATRIX3FVPROC                   UniformMatrix3fv;
	GFX_UNIFORMMATRIX4FVPROC                   UniformMatrix4fv;
	GFX_UNMAPBUFFERPROC                        UnmapBuffer;
	GFX_USEPROGRAMPROC                         UseProgram;
	GFX_VERTEXATTRIBDIVISORPROC                VertexAttribDivisor;               /* GFX_EXT_INSTANCED_ATTRIBUTES */
	GFX_VERTEXATTRIBIPOINTERPROC               VertexAttribIPointer;
	GFX_VERTEXATTRIBPOINTERPROC                VertexAttribPointer;
	GFX_VIEWPORTPROC                           Viewport;

} GFX_Extensions;


/********************************************************
 * Internal window data & methods
 *******************************************************/

/** Internal window */
typedef struct GFX_Window
{
	/* Super class */
	GFXWindow window;

	/* Hidden data */
	GFX_PlatformWindow  handle;

	GFXContext          context;    /* Context version */
	GFX_Extensions      extensions; /* Context extensions and state */
	GLuint              layout;     /* Layout for post processing */

} GFX_Window;


/**
 * Returns the top level window associated with a platform window.
 *
 */
GFX_Window* _gfx_window_get_from_handle(

		GFX_PlatformWindow handle);

/**
 * Destroys the server side window.
 *
 * Creates a zombie window, the window struct still exists, but is not registered.
 * Thus, it must still be freed.
 *
 */
void _gfx_window_destroy(

		GFX_Window* window);

/**
 * Sets the window as the current render target.
 *
 * It will ignore NULL as argument.
 * Note: This SHOULD NOT be called unless you know damn well what you're doing.
 *
 */
void _gfx_window_make_current(

		GFX_Window* window);

/**
 * Returns the current window.
 *
 * @return NULL if no window is active.
 *
 */
GFX_Window* _gfx_window_get_current(void);

/**
 * Swaps the internal buffers of the current window.
 *
 * Also polls errors of the context if error mode is debug.
 *
 */
void _gfx_window_swap_buffers(void);

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

/** Generic hardware object operator */
typedef void (*GFX_HardwareObjectFunc) (void* object, GFX_Extensions*);


/** Hardware vtable, can all be NULL */
typedef struct GFX_HardwareFuncs
{
	GFX_HardwareObjectFunc free;    /* GPU free request */
	GFX_HardwareObjectFunc save;    /* Prepare for context destruction */
	GFX_HardwareObjectFunc restore; /* Restore for new context */

} GFX_HardwareFuncs;


/**
 * Registers a new generic hardware object.
 *
 * @object Arbitrary data to identify with a number.
 * @funcs  Functions to associate with the object.
 * @return Identifier of the object, id > 1 (0 on failure).
 *
 * When an object is registered, it will be asked to free when all contexts are destroyed,
 * or reconstructed when the main context is destroyed.
 *
 */
size_t _gfx_hardware_object_register(

		void*                     object,
		const GFX_HardwareFuncs*  funcs);

/**
 * Unregisters a generic hardware object by identifier.
 *
 */
void _gfx_hardware_object_unregister(

		size_t id);

/**
 * Issue free request of all hardware objects, this happens when its parent context is destroyed.
 *
 * This will issue the free request and unregister ALL objects.
 * Thus this callback is NOT allowed to unregister the object.
 *
 */
void _gfx_hardware_objects_free(

		GFX_Extensions* ext);

/**
 * Issue save method of all hardware objects.
 *
 * During this operation, the current window and context are considered "deleted".
 * It is guaranteed another context is still active, this is only meant for objects which can't be shared.
 *
 */
void _gfx_hardware_objects_save(

		GFX_Extensions* ext);

/**
 * Issue restore method of all hardware objects.
 *
 * During this operation, a new window and context is current.
 *
 */
void _gfx_hardware_objects_restore(

		GFX_Extensions* ext);


/********************************************************
 * Internal hardware object access
 *******************************************************/

/**
 * Returns the current handle of a buffer.
 *
 */
GLuint _gfx_buffer_get_handle(

		const GFXBuffer* buffer);

/**
 * Returns the handle of the framebuffer associated with a pipeline.
 *
 */
GLuint _gfx_pipeline_get_handle(

		const GFXPipeline* pipeline);

/**
 * Returns the handle of a program.
 *
 */
GLuint _gfx_program_get_handle(

		const GFXProgram* program);

/**
 * Returns the handle of a shader.
 *
 */
GLuint _gfx_shader_get_handle(

		const GFXShader* shader);

/**
 * Returns the handle of a shared buffer.
 *
 */
GLuint _gfx_shared_buffer_get_handle(

		const GFXSharedBuffer* buffer);

/**
 * Returns the handle of a texture.
 *
 */
GLuint _gfx_texture_get_handle(

		const GFXTexture* texture);

/**
 * Returns the VAO of a layout.
 *
 */
GLuint _gfx_vertex_layout_get_handle(

		const GFXVertexLayout* layout);


#ifdef __cplusplus
}
#endif

#endif // GFX_CORE_INTERNAL_H
