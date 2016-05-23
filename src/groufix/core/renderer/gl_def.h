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

#ifndef GFX_CORE_RENDERER_GL_DEF_H
#define GFX_CORE_RENDERER_GL_DEF_H

#include "groufix/core/pipeline.h"


/* Correct OGL header */
#if defined(GFX_GL)
	#include <GL/glcorearb.h>
#elif defined(GFX_GLES)
	#include <GLES3/gl32.h>
#endif


/* Compatibility defines (for elsewhere than gl headers) */
#ifndef GL_CLIENT_STORAGE_BIT
	#define GL_CLIENT_STORAGE_BIT   0x0200
#endif
#ifndef GL_DYNAMIC_STORAGE_BIT
	#define GL_DYNAMIC_STORAGE_BIT  0x0100
#endif
#ifndef GL_MAP_PERSISTENT_BIT
	#define GL_MAP_PERSISTENT_BIT   0x0040
#endif


/* Correct context versions */
#if defined(GFX_GL)

	#define GFX_CONTEXT_MAJOR_MIN 3
	#define GFX_CONTEXT_MINOR_MIN 2

	#define GFX_CONTEXT_MAJOR_MAX 4
	#define GFX_CONTEXT_MINOR_MAX 5

	#define GFX_CONTEXT_ALL_MINORS_MAX 5

#elif defined(GFX_GLES)

	#define GFX_CONTEXT_MAJOR_MIN 3
	#define GFX_CONTEXT_MINOR_MIN 0

	#define GFX_CONTEXT_MAJOR_MAX 3
	#define GFX_CONTEXT_MINOR_MAX 2

	#define GFX_CONTEXT_ALL_MINORS_MAX 2

#endif


#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif


/********************************************************
 * OpenGL extensions & emulators
 *******************************************************/

/** Debug callback process */
typedef void (APIENTRYP GFX_DEBUGPROC)(
		GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, const void*);


/** Extension function pointers */
typedef void (APIENTRYP GFX_ACTIVETEXTUREPROC)(
		GLenum);
typedef void (APIENTRYP GFX_ATTACHSHADERPROC)(
		GLuint, GLuint);
typedef void (APIENTRYP GFX_BEGINTRANSFORMFEEDBACKPROC)(
		GLenum);
typedef void (APIENTRYP GFX_BINDATTRIBLOCATIONPROC)(
		GLuint, GLuint, const GLchar*);
typedef void (APIENTRYP GFX_BINDBUFFERPROC)(
		GLenum, GLuint);
typedef void (APIENTRYP GFX_BINDBUFFERBASEPROC)(
		GLenum, GLuint, GLuint);
typedef void (APIENTRYP GFX_BINDBUFFERRANGEPROC)(
		GLenum, GLuint, GLuint, GLintptr, GLsizeiptr);
typedef void (APIENTRYP GFX_BINDBUFFERSRANGEPROC)(
		GLenum, GLuint, GLsizei, const GLuint*, const GLintptr*, const GLsizeiptr*);
typedef void (APIENTRYP GFX_BINDFRAMEBUFFERPROC)(
		GLenum, GLuint);
typedef void (APIENTRYP GFX_BINDPROGRAMPIPELINEPROC)(
		GLuint);
typedef void (APIENTRYP GFX_BINDSAMPLERPROC)(
		GLuint, GLuint);
typedef void (APIENTRYP GFX_BINDTEXTUREPROC)(
		GLenum, GLuint);
typedef void (APIENTRYP GFX_BINDTEXTUREUNITPROC)(
		GLuint, GLuint);
typedef void (APIENTRYP GFX_BINDVERTEXARRAYPROC)(
		GLuint);
typedef void (APIENTRYP GFX_BINDVERTEXBUFFERPROC)(
		GLuint, GLuint, GLintptr, GLsizei);
typedef void (APIENTRYP GFX_BLENDEQUATIONSEPARATEPROC)(
		GLenum, GLenum);
typedef void (APIENTRYP GFX_BLENDFUNCSEPARATEPROC)(
		GLenum, GLenum, GLenum, GLenum);
typedef void (APIENTRYP GFX_BUFFERDATAPROC)(
		GLenum, GLsizeiptr, const GLvoid*, GLenum);
typedef void (APIENTRYP GFX_BUFFERSTORAGEPROC)(
		GLenum, GLsizeiptr, const GLvoid*, GLbitfield);
typedef void (APIENTRYP GFX_BUFFERSUBDATAPROC)(
		GLenum, GLintptr, GLsizeiptr, const GLvoid*);
typedef void (APIENTRYP GFX_CLEARPROC)(
		GLbitfield);
typedef void (APIENTRYP GFX_COMPILESHADERPROC)(
		GLuint);
typedef void (APIENTRYP GFX_COPYBUFFERSUBDATAPROC)(
		GLenum, GLenum, GLintptr, GLintptr, GLsizeiptr);
typedef void (APIENTRYP GFX_COPYNAMEDBUFFERSUBDATAPROC)(
		GLuint, GLuint, GLintptr, GLintptr, GLsizeiptr);
typedef void (APIENTRYP GFX_CREATEBUFFERSPROC)(
		GLsizei, GLuint*);
typedef void (APIENTRYP GFX_CREATEFRAMEBUFFERSPROC)(
		GLsizei, GLuint*);
typedef GLuint (APIENTRYP GFX_CREATEPROGRAMPROC)(
		void);
typedef void (APIENTRYP GFX_CREATEPROGRAMPIPELINESPROC)(
		GLsizei, GLuint*);
typedef void (APIENTRYP GFX_CREATESAMPLERSPROC)(
		GLsizei, GLuint*);
typedef GLuint (APIENTRYP GFX_CREATESHADERPROC)(
		GLenum);
typedef void (APIENTRYP GFX_CREATETEXTURESPROC)(
		GLenum, GLsizei, GLuint*);
typedef void (APIENTRYP GFX_CREATEVERTEXARRAYSPROC)(
		GLsizei, GLuint*);
typedef void (APIENTRYP GFX_CULLFACEPROC)(
		GLenum);
typedef void (APIENTRYP GFX_DEBUGMESSAGECALLBACKPROC)(
		GFX_DEBUGPROC, const GLvoid*);
typedef void (APIENTRYP GFX_DEBUGMESSAGECONTROLPROC)(
		GLenum, GLenum, GLenum, GLsizei, const GLuint*, GLboolean);
typedef void (APIENTRYP GFX_DELETEBUFFERSPROC)(
		GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_DELETEFRAMEBUFFERSPROC)(
		GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_DELETEPROGRAMPROC)(
		GLuint);
typedef void (APIENTRYP GFX_DELETEPROGRAMPIPELINESPROC)(
		GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_DELETESAMPLERSPROC)(
		GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_DELETESHADERPROC)(
		GLuint);
typedef void (APIENTRYP GFX_DELETETEXTURESPROC)(
		GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_DELETEVERTEXARRAYSPROC)(
		GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_DEPTHFUNCPROC)(
		GLenum);
typedef void (APIENTRYP GFX_DEPTHMASKPROC)(
		GLboolean);
typedef void (APIENTRYP GFX_DETACHSHADERPROC)(
		GLuint, GLuint);
typedef void (APIENTRYP GFX_DISABLEPROC)(
		GLenum);
typedef void (APIENTRYP GFX_DISABLEVERTEXARRAYATTRIBPROC)(
		GLuint, GLuint);
typedef void (APIENTRYP GFX_DISABLEVERTEXATTRIBARRAYPROC)(
		GLuint);
typedef void (APIENTRYP GFX_DRAWARRAYSPROC)(
		GLenum, GLint, GLsizei);
typedef void (APIENTRYP GFX_DRAWARRAYSINSTANCEDPROC)(
		GLenum, GLint, GLsizei, GLsizei);
typedef void (APIENTRYP GFX_DRAWARRAYSINSTANCEDBASEINSTANCEPROC)(
		GLenum, GLint, GLsizei, GLsizei, GLuint);
typedef void (APIENTRYP GFX_DRAWBUFFERSPROC)(
		GLsizei, const GLenum*);
typedef void (APIENTRYP GFX_DRAWELEMENTSPROC)(
		GLenum, GLsizei, GLenum, const GLvoid*);
typedef void (APIENTRYP GFX_DRAWELEMENTSBASEVERTEXPROC)(
		GLenum, GLsizei, GLenum, const GLvoid*, GLint);
typedef void (APIENTRYP GFX_DRAWELEMENTSINSTANCEDPROC)(
		GLenum, GLsizei, GLenum, const GLvoid*, GLsizei);
typedef void (APIENTRYP GFX_DRAWELEMENTSINSTANCEDBASEINSTANCEPROC)(
		GLenum, GLsizei, GLenum, const GLvoid*, GLsizei, GLuint);
typedef void (APIENTRYP GFX_DRAWELEMENTSINSTANCEDBASEVERTEXPROC)(
		GLenum, GLsizei, GLenum, const GLvoid*, GLsizei, GLint);
typedef void (APIENTRYP GFX_DRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC)(
		GLenum, GLsizei, GLenum, const GLvoid*, GLsizei, GLint, GLuint);
typedef void (APIENTRYP GFX_ENABLEPROC)(
		GLenum);
typedef void (APIENTRYP GFX_ENABLEVERTEXARRAYATTRIBPROC)(
		GLuint, GLuint);
typedef void (APIENTRYP GFX_ENABLEVERTEXATTRIBARRAYPROC)(
		GLuint);
typedef void (APIENTRYP GFX_ENDTRANSFORMFEEDBACKPROC)(
		void);
typedef void (APIENTRYP GFX_FLUSHPROC)(
		void);
typedef void (APIENTRYP GFX_FRAMEBUFFERTEXTUREPROC)(
		GLenum, GLenum, GLuint, GLint);
typedef void (APIENTRYP GFX_FRAMEBUFFERTEXTURE2DPROC)(
		GLenum, GLenum, GLenum, GLuint, GLint);
typedef void (APIENTRYP GFX_FRAMEBUFFERTEXTURELAYERPROC)(
		GLenum, GLenum, GLuint, GLint, GLint);
typedef void (APIENTRYP GFX_GENBUFFERSPROC)(
		GLsizei, GLuint*);
typedef void (APIENTRYP GFX_GENERATEMIPMAPPROC)(
		GLenum);
typedef void (APIENTRYP GFX_GENERATETEXTUREMIPMAPPROC)(
		GLuint);
typedef void (APIENTRYP GFX_GENFRAMEBUFFERSPROC)(
		GLsizei, GLuint*);
typedef void (APIENTRYP GFX_GENPROGRAMPIPELINESPROC)(
		GLsizei, GLuint*);
typedef void (APIENTRYP GFX_GENSAMPLERSPROC)(
		GLsizei, GLuint*);
typedef void (APIENTRYP GFX_GENTEXTURESPROC)(
		GLsizei, GLuint*);
typedef void (APIENTRYP GFX_GENVERTEXARRAYSPROC)(
		GLsizei, GLuint*);
typedef void (APIENTRYP GFX_GETACTIVEUNIFORMPROC)(
		GLuint, GLuint, GLsizei, GLsizei*, GLint*, GLenum*, GLchar*);
typedef void (APIENTRYP GFX_GETACTIVEUNIFORMBLOCKIVPROC)(
		GLuint, GLuint, GLenum, GLint*);
typedef void (APIENTRYP GFX_GETACTIVEUNIFORMSIVPROC)(
		GLuint, GLsizei, const GLuint*, GLenum, GLint*);
typedef void (APIENTRYP GFX_GETBUFFERSUBDATAPROC)(
		GLenum, GLintptr, GLsizeiptr, GLvoid*);
typedef void (APIENTRYP GFX_GETNAMEDBUFFERSUBDATAPROC)(
		GLuint, GLintptr, GLsizeiptr, GLvoid*);
typedef void (APIENTRYP GFX_GETPROGRAMBINARYPROC)(
		GLuint, GLsizei, GLsizei*, GLenum*, GLvoid*);
typedef void (APIENTRYP GFX_GETPROGRAMINFOLOGPROC)(
		GLuint, GLsizei, GLsizei*, GLchar*);
typedef void (APIENTRYP GFX_GETPROGRAMIVPROC)(
		GLuint, GLenum, GLint*);
typedef void (APIENTRYP GFX_GETSHADERINFOLOGPROC)(
		GLuint, GLsizei, GLsizei*, GLchar*);
typedef void (APIENTRYP GFX_GETSHADERIVPROC)(
		GLuint, GLenum, GLint*);
typedef void (APIENTRYP GFX_GETSHADERSOURCEPROC)(
		GLuint, GLsizei, GLsizei*, GLchar*);
typedef const GLubyte* (APIENTRYP GFX_GETSTRINGIPROC)(
		GLenum, GLuint);
typedef GLuint (APIENTRYP GFX_GETUNIFORMBLOCKINDEXPROC)(
		GLuint, const GLchar*);
typedef void (APIENTRYP GFX_GETUNIFORMINDICESPROC)(
		GLuint, GLsizei, const GLchar*const*, GLuint*);
typedef GLint (APIENTRYP GFX_GETUNIFORMLOCATIONPROC)(
		GLuint, const GLchar*);
typedef void (APIENTRYP GFX_INVALIDATEBUFFERSUBDATAPROC)(
		GLuint, GLintptr, GLsizeiptr);
typedef void (APIENTRYP GFX_LINKPROGRAMPROC)(
		GLuint);
typedef void* (APIENTRYP GFX_MAPBUFFERRANGEPROC)(
		GLenum, GLintptr, GLsizeiptr, GLbitfield);
typedef void* (APIENTRYP GFX_MAPNAMEDBUFFERRANGEPROC)(
		GLuint, GLintptr, GLsizeiptr, GLbitfield);
typedef void (APIENTRYP GFX_NAMEDBUFFERDATAPROC)(
		GLuint, GLsizeiptr, const GLvoid*, GLenum);
typedef void (APIENTRYP GFX_NAMEDBUFFERSTORAGEPROC)(
		GLuint, GLsizeiptr, const GLvoid*, GLbitfield);
typedef void (APIENTRYP GFX_NAMEDBUFFERSUBDATAPROC)(
		GLuint, GLintptr, GLsizeiptr, const GLvoid*);
typedef void (APIENTRYP GFX_NAMEDFRAMEBUFFERDRAWBUFFERSPROC)(
		GLuint, GLsizei, const GLenum*);
typedef void (APIENTRYP GFX_NAMEDFRAMEBUFFERTEXTUREPROC)(
		GLuint, GLenum, GLuint, GLint);
typedef void (APIENTRYP GFX_NAMEDFRAMEBUFFERTEXTURE2DPROC)(
		GLuint, GLenum, GLenum, GLuint, GLint);
typedef void (APIENTRYP GFX_NAMEDFRAMEBUFFERTEXTURELAYERPROC)(
		GLuint, GLenum, GLuint, GLint, GLint);
typedef void (APIENTRYP GFX_PATCHPARAMETERIPROC)(
		GLenum, GLint);
typedef void (APIENTRYP GFX_PIXELSTOREIPROC)(
		GLenum, GLint);
typedef void (APIENTRYP GFX_POLYGONMODEPROC)(
		GLenum, GLenum);
typedef void (APIENTRYP GFX_PROGRAMBINARYPROC)(
		GLuint, GLenum, const GLvoid*, GLsizei);
typedef void (APIENTRYP GFX_PROGRAMPARAMETERIPROC)(
		GLuint, GLenum, GLint);
typedef void (APIENTRYP GFX_PROGRAMUNIFORM1FVPROC)(
		GLuint, GLint, GLsizei, const GLfloat*);
typedef void (APIENTRYP GFX_PROGRAMUNIFORM1IVPROC)(
		GLuint, GLint, GLsizei, const GLint*);
typedef void (APIENTRYP GFX_PROGRAMUNIFORM1UIVPROC)(
		GLuint, GLint, GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_PROGRAMUNIFORM2FVPROC)(
		GLuint, GLint, GLsizei, const GLfloat*);
typedef void (APIENTRYP GFX_PROGRAMUNIFORM2IVPROC)(
		GLuint, GLint, GLsizei, const GLint*);
typedef void (APIENTRYP GFX_PROGRAMUNIFORM2UIVPROC)(
		GLuint, GLint, GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_PROGRAMUNIFORM3FVPROC)(
		GLuint, GLint, GLsizei, const GLfloat*);
typedef void (APIENTRYP GFX_PROGRAMUNIFORM3IVPROC)(
		GLuint, GLint, GLsizei, const GLint*);
typedef void (APIENTRYP GFX_PROGRAMUNIFORM3UIVPROC)(
		GLuint, GLint, GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_PROGRAMUNIFORM4FVPROC)(
		GLuint, GLint, GLsizei, const GLfloat*);
typedef void (APIENTRYP GFX_PROGRAMUNIFORM4IVPROC)(
		GLuint, GLint, GLsizei, const GLint*);
typedef void (APIENTRYP GFX_PROGRAMUNIFORM4UIVPROC)(
		GLuint, GLint, GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_PROGRAMUNIFORMMATRIX2FVPROC)(
		GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
typedef void (APIENTRYP GFX_PROGRAMUNIFORMMATRIX3FVPROC)(
		GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
typedef void (APIENTRYP GFX_PROGRAMUNIFORMMATRIX4FVPROC)(
		GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
typedef void (APIENTRYP GFX_SAMPLERPARAMETERFPROC)(
		GLuint, GLenum, GLfloat);
typedef void (APIENTRYP GFX_SAMPLERPARAMETERIPROC)(
		GLuint, GLenum, GLint);
typedef void (APIENTRYP GFX_SHADERSOURCEPROC)(
		GLuint, GLsizei, const GLchar*const*, const GLint*);
typedef void (APIENTRYP GFX_STENCILFUNCSEPARATEPROC)(
		GLenum, GLenum, GLint, GLuint);
typedef void (APIENTRYP GFX_STENCILOPSEPARATEPROC)(
		GLenum, GLenum, GLenum, GLenum);
typedef void (APIENTRYP GFX_TEXBUFFERPROC)(
		GLenum, GLenum, GLuint);
typedef void (APIENTRYP GFX_TEXIMAGE2DPROC)(
		GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*);
typedef void (APIENTRYP GFX_TEXIMAGE2DMULTISAMPLEPROC)(
		GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean);
typedef void (APIENTRYP GFX_TEXIMAGE3DPROC)(
		GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*);
typedef void (APIENTRYP GFX_TEXIMAGE3DMULTISAMPLEPROC)(
		GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean);
typedef void (APIENTRYP GFX_TEXPARAMETERFPROC)(
		GLenum, GLenum, GLfloat);
typedef void (APIENTRYP GFX_TEXPARAMETERIPROC)(
		GLenum, GLenum, GLint);
typedef void (APIENTRYP GFX_TEXSTORAGE2DPROC)(
		GLenum, GLsizei, GLenum, GLsizei, GLsizei);
typedef void (APIENTRYP GFX_TEXSTORAGE2DMULTISAMPLEPROC)(
		GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean);
typedef void (APIENTRYP GFX_TEXSTORAGE3DPROC)(
		GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei);
typedef void (APIENTRYP GFX_TEXSTORAGE3DMULTISAMPLEPROC)(
		GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean);
typedef void (APIENTRYP GFX_TEXSUBIMAGE2DPROC)(
		GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*);
typedef void (APIENTRYP GFX_TEXSUBIMAGE3DPROC)(
		GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*);
typedef void (APIENTRYP GFX_TEXTUREBUFFERPROC)(
		GLuint, GLenum, GLuint);
typedef void (APIENTRYP GFX_TEXTUREPARAMETERFPROC)(
		GLuint, GLenum, GLfloat);
typedef void (APIENTRYP GFX_TEXTUREPARAMETERIPROC)(
		GLuint, GLenum, GLint);
typedef void (APIENTRYP GFX_TEXTURESTORAGE2DPROC)(
		GLuint, GLsizei, GLenum, GLsizei, GLsizei);
typedef void (APIENTRYP GFX_TEXTURESTORAGE2DMULTISAMPLEPROC)(
		GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLboolean);
typedef void (APIENTRYP GFX_TEXTURESTORAGE3DPROC)(
		GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLsizei);
typedef void (APIENTRYP GFX_TEXTURESTORAGE3DMULTISAMPLEPROC)(
		GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean);
typedef void (APIENTRYP GFX_TEXTURESUBIMAGE2DPROC)(
		GLuint, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*);
typedef void (APIENTRYP GFX_TEXTURESUBIMAGE3DPROC)(
		GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*);
typedef void (APIENTRYP GFX_TRANSFORMFEEDBACKVARYINGSPROC)(
		GLuint, GLsizei, const GLchar*const*, GLenum);
typedef void (APIENTRYP GFX_UNIFORM1FVPROC)(
		GLint, GLsizei, const GLfloat*);
typedef void (APIENTRYP GFX_UNIFORM1IVPROC)(
		GLint, GLsizei, const GLint*);
typedef void (APIENTRYP GFX_UNIFORM1UIVPROC)(
		GLint, GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_UNIFORM2FVPROC)(
		GLint, GLsizei, const GLfloat*);
typedef void (APIENTRYP GFX_UNIFORM2IVPROC)(
		GLint, GLsizei, const GLint*);
typedef void (APIENTRYP GFX_UNIFORM2UIVPROC)(
		GLint, GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_UNIFORM3FVPROC)(
		GLint, GLsizei, const GLfloat*);
typedef void (APIENTRYP GFX_UNIFORM3IVPROC)(
		GLint, GLsizei, const GLint*);
typedef void (APIENTRYP GFX_UNIFORM3UIVPROC)(
		GLint, GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_UNIFORM4FVPROC)(
		GLint, GLsizei, const GLfloat*);
typedef void (APIENTRYP GFX_UNIFORM4IVPROC)(
		GLint, GLsizei, const GLint*);
typedef void (APIENTRYP GFX_UNIFORM4UIVPROC)(
		GLint, GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_UNIFORMBLOCKBINDINGPROC)(
		GLuint, GLuint, GLuint);
typedef void (APIENTRYP GFX_UNIFORMMATRIX2FVPROC)(
		GLint, GLsizei, GLboolean, const GLfloat*);
typedef void (APIENTRYP GFX_UNIFORMMATRIX3FVPROC)(
		GLint, GLsizei, GLboolean, const GLfloat*);
typedef void (APIENTRYP GFX_UNIFORMMATRIX4FVPROC)(
		GLint, GLsizei, GLboolean, const GLfloat*);
typedef GLboolean (APIENTRYP GFX_UNMAPBUFFERPROC)(
		GLenum);
typedef GLboolean (APIENTRYP GFX_UNMAPNAMEDBUFFERPROC)(
		GLuint);
typedef void (APIENTRYP GFX_USEPROGRAMPROC)(
		GLuint);
typedef void (APIENTRYP GFX_USEPROGRAMSTAGESPROC)(
		GLuint, GLbitfield, GLuint);
typedef void (APIENTRYP GFX_VERTEXARRAYATTRIBBINDINGPROC)(
		GLuint, GLuint, GLuint);
typedef void (APIENTRYP GFX_VERTEXARRAYATTRIBFORMATPROC)(
		GLuint, GLuint, GLint, GLenum, GLboolean, GLuint);
typedef void (APIENTRYP GFX_VERTEXARRAYATTRIBIFORMATPROC)(
		GLuint, GLuint, GLint, GLenum, GLuint);
typedef void (APIENTRYP GFX_VERTEXARRAYBINDINGDIVISORPROC)(
		GLuint, GLuint, GLuint);
typedef void (APIENTRYP GFX_VERTEXARRAYELEMENTBUFFERPROC)(
		GLuint, GLuint);
typedef void (APIENTRYP GFX_VERTEXARRAYVERTEXBUFFERPROC)(
		GLuint, GLuint, GLuint, GLintptr, GLsizei);
typedef void (APIENTRYP GFX_VERTEXATTRIBBINDINGPROC)(
		GLuint, GLuint);
typedef void (APIENTRYP GFX_VERTEXATTRIBDIVISORPROC)(
		GLuint, GLuint);
typedef void (APIENTRYP GFX_VERTEXATTRIBFORMATPROC)(
		GLuint, GLint, GLenum, GLboolean, GLuint);
typedef void (APIENTRYP GFX_VERTEXATTRIBIFORMATPROC)(
		GLuint, GLint, GLenum, GLuint);
typedef void (APIENTRYP GFX_VERTEXATTRIBIPOINTERPROC)(
		GLuint, GLint, GLenum, GLsizei, const GLvoid*);
typedef void (APIENTRYP GFX_VERTEXATTRIBPOINTERPROC)(
		GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*);
typedef void (APIENTRYP GFX_VERTEXBINDINGDIVISORPROC)(
		GLuint, GLuint);
typedef void (APIENTRYP GFX_VIEWPORTPROC)(
		GLint, GLint, GLsizei, GLsizei);


/** Emulators */
/* TODO: tabbed out functions to be ported to abstract renderer */
	void APIENTRY _gfx_gl_bind_buffers_range                                (GLenum, GLuint, GLsizei, const GLuint*, const GLintptr*, const GLsizeiptr*);
	void APIENTRY _gfx_gl_bind_program_pipeline                             (GLuint);
	void APIENTRY _gfx_gl_bind_texture_unit                                 (GLuint, GLuint);
	void APIENTRY _gfx_gl_bind_vertex_buffer                                (GLuint, GLuint, GLintptr, GLsizei);
void APIENTRY _gfx_gl_buffer_storage(
		GLenum, GLsizeiptr, const GLvoid*, GLbitfield);
void APIENTRY _gfx_gl_copy_named_buffer_sub_data(
		GLuint, GLuint, GLintptr, GLintptr, GLsizeiptr);
void APIENTRY _gfx_gl_create_buffers(
		GLsizei, GLuint*);
	void APIENTRY _gfx_gl_create_framebuffers                               (GLsizei, GLuint*);
	void APIENTRY _gfx_gl_create_program_pipelines                          (GLsizei, GLuint*);
	void APIENTRY _gfx_gl_create_samplers                                   (GLsizei, GLuint*);
	void APIENTRY _gfx_gl_create_textures                                   (GLenum, GLsizei, GLuint*);
void APIENTRY _gfx_gl_create_vertex_arrays(
		GLsizei, GLuint*);
void APIENTRY _gfx_gl_debug_message_callback(
		GFX_DEBUGPROC, const GLvoid*);
void APIENTRY _gfx_gl_debug_message_control(
		GLenum, GLenum, GLenum, GLsizei, const GLuint*, GLboolean);
	void APIENTRY _gfx_gl_delete_program_pipelines                          (GLsizei, const GLuint*);
	void APIENTRY _gfx_gl_disable_vertex_array_attrib                       (GLuint, GLuint);
	void APIENTRY _gfx_gl_draw_arrays_instanced_base_instance               (GLenum, GLint, GLsizei, GLsizei, GLuint);
	void APIENTRY _gfx_gl_draw_elements_instanced_base_instance             (GLenum, GLsizei, GLenum, const GLvoid*, GLsizei, GLuint);
	void APIENTRY _gfx_gl_draw_elements_instanced_base_vertex_base_instance (GLenum, GLsizei, GLenum, const GLvoid*, GLsizei, GLint, GLuint);
	void APIENTRY _gfx_gl_enable_vertex_array_attrib                        (GLuint, GLuint);
	void APIENTRY _gfx_gl_generate_texture_mipmap                           (GLuint);
	void APIENTRY _gfx_gl_gen_program_pipelines                             (GLsizei, GLuint*);
void APIENTRY _gfx_gl_get_named_buffer_sub_data(
		GLuint, GLintptr, GLsizeiptr, GLvoid*);
void APIENTRY _gfx_gl_invalidate_buffer_sub_data(
		GLuint, GLintptr, GLsizeiptr);
void* APIENTRY _gfx_gl_map_named_buffer_range(
		GLuint, GLintptr, GLsizeiptr, GLbitfield);
void APIENTRY _gfx_gl_named_buffer_data(
		GLuint, GLsizeiptr, const GLvoid*, GLenum);
void APIENTRY _gfx_gl_named_buffer_storage(
		GLuint, GLsizeiptr, const GLvoid*, GLbitfield);
void APIENTRY _gfx_gl_named_buffer_sub_data(
		GLuint, GLintptr, GLsizeiptr, const GLvoid*);
	void APIENTRY _gfx_gl_named_framebuffer_draw_buffers                    (GLuint, GLsizei, const GLenum*);
	void APIENTRY _gfx_gl_named_framebuffer_texture                         (GLuint, GLenum, GLuint, GLint);
	void APIENTRY _gfx_gl_named_framebuffer_texture_layer                   (GLuint, GLenum, GLuint, GLint, GLint);
void APIENTRY _gfx_gl_patch_parameter_i(
		GLenum, GLint);
	void APIENTRY _gfx_gl_program_uniform_1fv                               (GLuint, GLint, GLsizei, const GLfloat*);
	void APIENTRY _gfx_gl_program_uniform_1iv                               (GLuint, GLint, GLsizei, const GLint*);
	void APIENTRY _gfx_gl_program_uniform_1uiv                              (GLuint, GLint, GLsizei, const GLuint*);
	void APIENTRY _gfx_gl_program_uniform_2fv                               (GLuint, GLint, GLsizei, const GLfloat*);
	void APIENTRY _gfx_gl_program_uniform_2iv                               (GLuint, GLint, GLsizei, const GLint*);
	void APIENTRY _gfx_gl_program_uniform_2uiv                              (GLuint, GLint, GLsizei, const GLuint*);
	void APIENTRY _gfx_gl_program_uniform_3fv                               (GLuint, GLint, GLsizei, const GLfloat*);
	void APIENTRY _gfx_gl_program_uniform_3iv                               (GLuint, GLint, GLsizei, const GLint*);
	void APIENTRY _gfx_gl_program_uniform_3uiv                              (GLuint, GLint, GLsizei, const GLuint*);
	void APIENTRY _gfx_gl_program_uniform_4fv                               (GLuint, GLint, GLsizei, const GLfloat*);
	void APIENTRY _gfx_gl_program_uniform_4iv                               (GLuint, GLint, GLsizei, const GLint*);
	void APIENTRY _gfx_gl_program_uniform_4uiv                              (GLuint, GLint, GLsizei, const GLuint*);
	void APIENTRY _gfx_gl_program_uniform_matrix_2fv                        (GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
	void APIENTRY _gfx_gl_program_uniform_matrix_3fv                        (GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
	void APIENTRY _gfx_gl_program_uniform_matrix_4fv                        (GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
	void APIENTRY _gfx_gl_texture_buffer                                    (GLuint, GLenum, GLuint);
	void APIENTRY _gfx_gl_texture_parameter_f                               (GLuint, GLenum, GLfloat);
	void APIENTRY _gfx_gl_texture_parameter_i                               (GLuint, GLenum, GLint);
	void APIENTRY _gfx_gl_texture_storage_2d                                (GLuint, GLsizei, GLenum, GLsizei, GLsizei);
	void APIENTRY _gfx_gl_texture_storage_2d_multisample                    (GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLboolean);
	void APIENTRY _gfx_gl_texture_storage_3d                                (GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLsizei);
	void APIENTRY _gfx_gl_texture_storage_3d_multisample                    (GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean);
	void APIENTRY _gfx_gl_texture_sub_image_2d                              (GLuint, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*);
	void APIENTRY _gfx_gl_texture_sub_image_3d                              (GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*);
GLboolean APIENTRY _gfx_gl_unmap_named_buffer(
		GLuint);
	void APIENTRY _gfx_gl_use_program_stages                                (GLuint, GLbitfield, GLuint);
	void APIENTRY _gfx_gl_vertex_array_attrib_binding                       (GLuint, GLuint, GLuint);
	void APIENTRY _gfx_gl_vertex_array_attrib_format                        (GLuint, GLuint, GLint, GLenum, GLboolean, GLuint);
	void APIENTRY _gfx_gl_vertex_array_attrib_i_format                      (GLuint, GLuint, GLint, GLenum, GLuint);
	void APIENTRY _gfx_gl_vertex_array_binding_divisor                      (GLuint, GLuint, GLuint);
	void APIENTRY _gfx_gl_vertex_array_element_buffer                       (GLuint, GLuint);
	void APIENTRY _gfx_gl_vertex_array_vertex_buffer                        (GLuint, GLuint, GLuint, GLintptr, GLsizei);
	void APIENTRY _gfx_gl_vertex_attrib_binding                             (GLuint, GLuint);
	void APIENTRY _gfx_gl_vertex_attrib_format                              (GLuint, GLint, GLenum, GLboolean, GLuint);
	void APIENTRY _gfx_gl_vertex_attrib_i_format                            (GLuint, GLint, GLenum, GLuint);
	void APIENTRY _gfx_gl_vertex_binding_divisor                            (GLuint, GLuint);
	void APIENTRY _gfx_gles_draw_elements_base_vertex                       (GLenum, GLsizei, GLenum, const GLvoid*, GLint);
	void APIENTRY _gfx_gles_draw_elements_instanced_base_vertex             (GLenum, GLsizei, GLenum, const GLvoid*, GLsizei, GLint);
	void APIENTRY _gfx_gles_framebuffer_texture                             (GLenum, GLenum, GLuint, GLint);
void APIENTRY _gfx_gles_get_buffer_sub_data(
		GLenum, GLintptr, GLsizeiptr, GLvoid*);
	void APIENTRY _gfx_gles_named_framebuffer_texture_2d                    (GLuint, GLenum, GLenum, GLuint, GLint);
void APIENTRY _gfx_gles_polygon_mode(
		GLenum, GLenum);
	void APIENTRY _gfx_gles_tex_buffer                                      (GLenum, GLenum, GLuint);
	void APIENTRY _gfx_gles_tex_image_2d_multisample                        (GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean);
	void APIENTRY _gfx_gles_tex_image_3d_multisample                        (GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean);
	void APIENTRY _gfx_gles_tex_storage_2d_multisample                      (GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean);
	void APIENTRY _gfx_gles_tex_storage_3d_multisample                      (GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean);
	void APIENTRY _gfx_gl_bind_sampler                                      (GLuint, GLuint);
	void APIENTRY _gfx_gl_delete_samplers                                   (GLsizei, const GLuint*);
	void APIENTRY _gfx_gl_gen_samplers                                      (GLsizei, GLuint*);
	void APIENTRY _gfx_gl_get_program_binary                                (GLuint, GLsizei, GLsizei*, GLenum*, GLvoid*);
	void APIENTRY _gfx_gl_named_framebuffer_texture_2d                      (GLuint, GLenum, GLenum, GLuint, GLint);
	void APIENTRY _gfx_gl_program_binary                                    (GLuint, GLenum, const GLvoid*, GLsizei);
	void APIENTRY _gfx_gl_program_parameter_i                               (GLuint, GLenum, GLint);
	void APIENTRY _gfx_gl_sampler_parameter_f                               (GLuint, GLenum, GLfloat);
	void APIENTRY _gfx_gl_sampler_parameter_i                               (GLuint, GLenum, GLint);
	void APIENTRY _gfx_gl_tex_storage_2d                                    (GLenum, GLsizei, GLenum, GLsizei, GLsizei);
	void APIENTRY _gfx_gl_tex_storage_2d_multisample                        (GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean);
	void APIENTRY _gfx_gl_tex_storage_3d                                    (GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei);
	void APIENTRY _gfx_gl_tex_storage_3d_multisample                        (GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean);
	void APIENTRY _gfx_gl_vertex_attrib_divisor                             (GLuint, GLuint);


/********************************************************
 * OpenGL renderer & context
 *******************************************************/

/** Internal Extensions */
/* TODO: tabbed out extensions to be ported to abstract renderer */
enum GFX_Extension
{
	GFX_INT_EXT_BUFFER_INVALIDATION,
	GFX_INT_EXT_BUFFER_READ,
	GFX_INT_EXT_BUFFER_STORAGE,
	GFX_INT_EXT_DEBUG_OUTPUT,
	GFX_INT_EXT_DIRECT_STATE_ACCESS,
		GFX_INT_EXT_MULTI_BIND,
		GFX_INT_EXT_SAMPLER_OBJECTS,
		GFX_INT_EXT_TEXTURE_STORAGE,
		GFX_INT_EXT_TEXTURE_STORAGE_MULTISAMPLE,
		GFX_INT_EXT_VERTEX_ATTRIB_BINDING,

	GFX_INT_EXT_COUNT
};


/** OpenGL renderer & context */
struct GFX_Renderer
{
	/* Internal Extensions */
	unsigned char  intExt[GFX_INT_EXT_COUNT];

	/* Bound objects */
	GLuint         fbos[2];  /* Currently bound FBOs (0 = draw, 1 = read) */
	GLuint         program;  /* Currently used program or program pipeline */
	GLuint         vao;      /* Currently bound VAO */
	GLuint         post;     /* Layout for post processing */

	/* Viewport & state values */
	GFXViewport    viewport;
	GFXPipeState   state;
	unsigned char  packAlignment;
	unsigned char  unpackAlignment;
	unsigned int   patchVertices;

	/* Binding points */
	void*          uniformBuffers;
	void*          textureUnits;


	/* OpenGL Extensions */
	/* TODO: tabbed out functions to be ported to abstract renderer */
		GFX_ACTIVETEXTUREPROC                               ActiveTexture;
		GFX_ATTACHSHADERPROC                                AttachShader;
		GFX_BEGINTRANSFORMFEEDBACKPROC                      BeginTransformFeedback;
		GFX_BINDATTRIBLOCATIONPROC                          BindAttribLocation;
	GFX_BINDBUFFERPROC                                  BindBuffer;
		GFX_BINDBUFFERBASEPROC                              BindBufferBase;
		GFX_BINDBUFFERRANGEPROC                             BindBufferRange;
		GFX_BINDBUFFERSRANGEPROC                            BindBuffersRange;
		GFX_BINDFRAMEBUFFERPROC                             BindFramebuffer;
		GFX_BINDPROGRAMPIPELINEPROC                         BindProgramPipeline;                         /* GFX_EXT_PROGRAM_MAP, fallback to UseProgram */
		GFX_BINDSAMPLERPROC                                 BindSampler;                                 /* GFX_INT_EXT_SAMPLER_OBJECTS, fallback to no-op */
		GFX_BINDTEXTUREPROC                                 BindTexture;
		GFX_BINDTEXTUREUNITPROC                             BindTextureUnit;                             /* GFX_INT_EXT_DIRECT_STATE_ACCESS */
		GFX_BINDVERTEXARRAYPROC                             BindVertexArray;
		GFX_BINDVERTEXBUFFERPROC                            BindVertexBuffer;                            /* GFX_INT_EXT_VERTEX_ATTRIB_BINDING */
	GFX_BLENDEQUATIONSEPARATEPROC                       BlendEquationSeparate;
	GFX_BLENDFUNCSEPARATEPROC                           BlendFuncSeparate;
	GFX_BUFFERDATAPROC                                  BufferData;
	/* GFX_INT_EXT_BUFFER_STORAGE, fallback to no-op */
	GFX_BUFFERSTORAGEPROC                               BufferStorage;
	GFX_BUFFERSUBDATAPROC                               BufferSubData;
	GFX_CLEARPROC                                       Clear;
		GFX_COMPILESHADERPROC                               CompileShader;
	GFX_COPYBUFFERSUBDATAPROC                           CopyBufferSubData;
	/* GFX_INT_EXT_DIRECT_STATE_ACCESS, fallback to CopyBufferSubData */
	GFX_COPYNAMEDBUFFERSUBDATAPROC                      CopyNamedBufferSubData;
	/* GFX_INT_EXT_DIRECT_STATE_ACCESS, fallback to GenBuffers */
	GFX_CREATEBUFFERSPROC                               CreateBuffers;
		GFX_CREATEFRAMEBUFFERSPROC                          CreateFramebuffers;
		GFX_CREATEPROGRAMPROC                               CreateProgram;
		GFX_CREATEPROGRAMPIPELINESPROC                      CreateProgramPipelines;                      /* GFX_EXT_PROGRAM_MAP */
		GFX_CREATESAMPLERSPROC                              CreateSamplers;                              /* GFX_INT_EXT_SAMPLER_OBJECTS */
		GFX_CREATESHADERPROC                                CreateShader;
		GFX_CREATETEXTURESPROC                              CreateTextures;
	/* GFX_INT_EXT_DIRECT_STATE_ACCESS, fallback to GenVertexArrays */
	GFX_CREATEVERTEXARRAYSPROC                          CreateVertexArrays;
	GFX_CULLFACEPROC                                    CullFace;
	/* GFX_INT_EXT_DEBUG_OUTPUT, fallback to no-op */
	GFX_DEBUGMESSAGECALLBACKPROC                        DebugMessageCallback;
	/* GFX_INT_EXT_DEBUG_OUTPUT, fallback to no-op */
	GFX_DEBUGMESSAGECONTROLPROC                         DebugMessageControl;
	GFX_DELETEBUFFERSPROC                               DeleteBuffers;
		GFX_DELETEFRAMEBUFFERSPROC                          DeleteFramebuffers;
		GFX_DELETEPROGRAMPROC                               DeleteProgram;
		GFX_DELETEPROGRAMPIPELINESPROC                      DeleteProgramPipelines;                      /* GFX_EXT_PROGRAM_MAP */
		GFX_DELETESAMPLERSPROC                              DeleteSamplers;                              /* GFX_INT_EXT_SAMPLER_OBJECTS */
		GFX_DELETESHADERPROC                                DeleteShader;
		GFX_DELETETEXTURESPROC                              DeleteTextures;
	GFX_DELETEVERTEXARRAYSPROC                          DeleteVertexArrays;
	GFX_DEPTHFUNCPROC                                   DepthFunc;
	GFX_DEPTHMASKPROC                                   DepthMask;
		GFX_DETACHSHADERPROC                                DetachShader;
		GFX_DISABLEPROC                                     Disable;
		GFX_DISABLEVERTEXARRAYATTRIBPROC                    DisableVertexArrayAttrib;
		GFX_DISABLEVERTEXATTRIBARRAYPROC                    DisableVertexAttribArray;
		GFX_DRAWARRAYSPROC                                  DrawArrays;
		GFX_DRAWARRAYSINSTANCEDPROC                         DrawArraysInstanced;
		GFX_DRAWARRAYSINSTANCEDBASEINSTANCEPROC             DrawArraysInstancedBaseInstance;             /* GFX_EXT_INSTANCED_BASE_ATTRIBUTES */
		GFX_DRAWBUFFERSPROC                                 DrawBuffers;
		GFX_DRAWELEMENTSPROC                                DrawElements;
		GFX_DRAWELEMENTSBASEVERTEXPROC                      DrawElementsBaseVertex;                      /* GFX_EXT_VERTEX_BASE */
		GFX_DRAWELEMENTSINSTANCEDPROC                       DrawElementsInstanced;
		GFX_DRAWELEMENTSINSTANCEDBASEINSTANCEPROC           DrawElementsInstancedBaseInstance;           /* GFX_EXT_INSTANCED_BASE_ATTRIBUTES */
		GFX_DRAWELEMENTSINSTANCEDBASEVERTEXPROC             DrawElementsInstancedBaseVertex;             /* GFX_EXT_VERTEX_BASE */
		GFX_DRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC DrawElementsInstancedBaseVertexBaseInstance; /* GFX_EXT_VERTEX_BASE */
		GFX_ENABLEPROC                                      Enable;
		GFX_ENABLEVERTEXARRAYATTRIBPROC                     EnableVertexArrayAttrib;
		GFX_ENABLEVERTEXATTRIBARRAYPROC                     EnableVertexAttribArray;
		GFX_ENDTRANSFORMFEEDBACKPROC                        EndTransformFeedback;
		GFX_FLUSHPROC                                       Flush;
		GFX_FRAMEBUFFERTEXTUREPROC                          FramebufferTexture;                          /* GFX_EXT_BUFFER_TEXTURE */
		GFX_FRAMEBUFFERTEXTURE2DPROC                        FramebufferTexture2D;
		GFX_FRAMEBUFFERTEXTURELAYERPROC                     FramebufferTextureLayer;
	GFX_GENBUFFERSPROC                                  GenBuffers;
		GFX_GENERATEMIPMAPPROC                              GenerateMipmap;
		GFX_GENERATETEXTUREMIPMAPPROC                       GenerateTextureMipmap;                       /* GFX_INT_EXT_DIRECT_STATE_ACCESS */
		GFX_GENFRAMEBUFFERSPROC                             GenFramebuffers;
		GFX_GENPROGRAMPIPELINESPROC                         GenProgramPipelines;                         /* GFX_EXT_PROGRAM_MAP */
		GFX_GENSAMPLERSPROC                                 GenSamplers;                                 /* GFX_INT_EXT_SAMPLER_OBJECTS */
		GFX_GENTEXTURESPROC                                 GenTextures;
	GFX_GENVERTEXARRAYSPROC                             GenVertexArrays;
		GFX_GETACTIVEUNIFORMPROC                            GetActiveUniform;
		GFX_GETACTIVEUNIFORMBLOCKIVPROC                     GetActiveUniformBlockiv;
		GFX_GETACTIVEUNIFORMSIVPROC                         GetActiveUniformsiv;
	/* GFX_INT_EXT_BUFFER_READ, fallback to MapBufferRange */
	GFX_GETBUFFERSUBDATAPROC                            GetBufferSubData;
	/* GFX_INT_EXT_DIRECT_STATE_ACCESS, fallback to GetBufferSubData */
	GFX_GETNAMEDBUFFERSUBDATAPROC                       GetNamedBufferSubData;
		GFX_GETPROGRAMBINARYPROC                            GetProgramBinary;                            /* GFX_EXT_PROGRAM_BINARY */
		GFX_GETPROGRAMINFOLOGPROC                           GetProgramInfoLog;
		GFX_GETPROGRAMIVPROC                                GetProgramiv;
		GFX_GETSHADERINFOLOGPROC                            GetShaderInfoLog;
		GFX_GETSHADERIVPROC                                 GetShaderiv;
		GFX_GETSHADERSOURCEPROC                             GetShaderSource;
	GFX_GETSTRINGIPROC                                  GetStringi;
		GFX_GETUNIFORMBLOCKINDEXPROC                        GetUniformBlockIndex;
		GFX_GETUNIFORMINDICESPROC                           GetUniformIndices;
		GFX_GETUNIFORMLOCATIONPROC                          GetUniformLocation;
	/* GFX_INT_EXT_BUFFER_INVALIDATION, fallback to MapNamedBufferRange */
	GFX_INVALIDATEBUFFERSUBDATAPROC                     InvalidateBufferSubData;
		GFX_LINKPROGRAMPROC                                 LinkProgram;
	GFX_MAPBUFFERRANGEPROC                              MapBufferRange;
	/* GFX_INT_EXT_DIRECT_STATE_ACCESS, fallback to MapBufferRange */
	GFX_MAPNAMEDBUFFERRANGEPROC                         MapNamedBufferRange;
	/* GFX_INT_EXT_DIRECT_STATE_ACCESS, fallback to BufferData */
	GFX_NAMEDBUFFERDATAPROC                             NamedBufferData;
	/* GFX_INT_EXT_DIRECT_STATE_ACCESS, fallback to BufferStorage */
	GFX_NAMEDBUFFERSTORAGEPROC                          NamedBufferStorage;
	/* GFX_INT_EXT_DIRECT_STATE_ACCESS, fallback to BufferSubData */
	GFX_NAMEDBUFFERSUBDATAPROC                          NamedBufferSubData;
		GFX_NAMEDFRAMEBUFFERDRAWBUFFERSPROC                 NamedFramebufferDrawBuffers;
		GFX_NAMEDFRAMEBUFFERTEXTUREPROC                     NamedFramebufferTexture;                     /* GFX_EXT_BUFFER_TEXTURE */
		GFX_NAMEDFRAMEBUFFERTEXTURE2DPROC                   NamedFramebufferTexture2D;
		GFX_NAMEDFRAMEBUFFERTEXTURELAYERPROC                NamedFramebufferTextureLayer;
	/* GFX_EXT_TESSELLATION_SHADER, fallback to no-op */
	GFX_PATCHPARAMETERIPROC                             PatchParameteri;
	GFX_PIXELSTOREIPROC                                 PixelStorei;
	/* GFX_EXT_POLYGON_STATE, fallback to no-op */
	GFX_POLYGONMODEPROC                                 PolygonMode;
		GFX_PROGRAMBINARYPROC                               ProgramBinary;                               /* GFX_EXT_PROGRAM_BINARY */
		GFX_PROGRAMPARAMETERIPROC                           ProgramParameteri;                           /* GFX_EXT_SEPARABLE_PROGRAM */
		GFX_PROGRAMUNIFORM1FVPROC                           ProgramUniform1fv;                           /* GFX_EXT_PROGRAM_MAP, fallback to Uniform1fv */
		GFX_PROGRAMUNIFORM1IVPROC                           ProgramUniform1iv;                           /* GFX_EXT_PROGRAM_MAP, fallback to Uniform1iv */
		GFX_PROGRAMUNIFORM1UIVPROC                          ProgramUniform1uiv;                          /* GFX_EXT_PROGRAM_MAP, fallback to Uniform1uiv */
		GFX_PROGRAMUNIFORM2FVPROC                           ProgramUniform2fv;                           /* GFX_EXT_PROGRAM_MAP, fallback to Uniform2fv */
		GFX_PROGRAMUNIFORM2IVPROC                           ProgramUniform2iv;                           /* GFX_EXT_PROGRAM_MAP, fallback to Uniform2iv */
		GFX_PROGRAMUNIFORM2UIVPROC                          ProgramUniform2uiv;                          /* GFX_EXT_PROGRAM_MAP, fallback to Uniform2uiv */
		GFX_PROGRAMUNIFORM3FVPROC                           ProgramUniform3fv;                           /* GFX_EXT_PROGRAM_MAP, fallback to Uniform3fv */
		GFX_PROGRAMUNIFORM3IVPROC                           ProgramUniform3iv;                           /* GFX_EXT_PROGRAM_MAP, fallback to Uniform3iv */
		GFX_PROGRAMUNIFORM3UIVPROC                          ProgramUniform3uiv;                          /* GFX_EXT_PROGRAM_MAP, fallback to Uniform3uiv */
		GFX_PROGRAMUNIFORM4FVPROC                           ProgramUniform4fv;                           /* GFX_EXT_PROGRAM_MAP, fallback to Uniform4fv */
		GFX_PROGRAMUNIFORM4IVPROC                           ProgramUniform4iv;                           /* GFX_EXT_PROGRAM_MAP, fallback to Uniform4iv */
		GFX_PROGRAMUNIFORM4UIVPROC                          ProgramUniform4uiv;                          /* GFX_EXT_PROGRAM_MAP, fallback to Uniform4uiv */
		GFX_PROGRAMUNIFORMMATRIX2FVPROC                     ProgramUniformMatrix2fv;                     /* GFX_EXT_PROGRAM_MAP, fallback to UniformMatrix2fv */
		GFX_PROGRAMUNIFORMMATRIX3FVPROC                     ProgramUniformMatrix3fv;                     /* GFX_EXT_PROGRAM_MAP, fallback to UniformMatrix3fv */
		GFX_PROGRAMUNIFORMMATRIX4FVPROC                     ProgramUniformMatrix4fv;                     /* GFX_EXT_PROGRAM_MAP, fallback to UniformMatrix4fv */
		GFX_SAMPLERPARAMETERFPROC                           SamplerParameterf;                           /* GFX_INT_EXT_SAMPLER_OBJECTS */
		GFX_SAMPLERPARAMETERIPROC                           SamplerParameteri;                           /* GFX_INT_EXT_SAMPLER_OBJECTS */
		GFX_SHADERSOURCEPROC                                ShaderSource;
	GFX_STENCILFUNCSEPARATEPROC                         StencilFuncSeparate;
	GFX_STENCILOPSEPARATEPROC                           StencilOpSeparate;
		GFX_TEXBUFFERPROC                                   TexBuffer;                                   /* GFX_EXT_BUFFER_TEXTURE */
		GFX_TEXIMAGE2DPROC                                  TexImage2D;
		GFX_TEXIMAGE2DMULTISAMPLEPROC                       TexImage2DMultisample;                       /* GFX_EXT_MULTISAMPLE_TEXTURE */
		GFX_TEXIMAGE3DPROC                                  TexImage3D;
		GFX_TEXIMAGE3DMULTISAMPLEPROC                       TexImage3DMultisample;                       /* GFX_EXT_LAYERED_MULTISAMPLE_TEXTURE */
		GFX_TEXPARAMETERFPROC                               TexParameterf;
		GFX_TEXPARAMETERIPROC                               TexParameteri;
		GFX_TEXSTORAGE2DPROC                                TexStorage2D;
		GFX_TEXSTORAGE2DMULTISAMPLEPROC                     TexStorage2DMultisample;                     /* GFX_EXT_MULTISAMPLE_TEXTURE */
		GFX_TEXSTORAGE3DPROC                                TexStorage3D;
		GFX_TEXSTORAGE3DMULTISAMPLEPROC                     TexStorage3DMultisample;                     /* GFX_EXT_LAYERED_MULTISAMPLE_TEXTURE */
		GFX_TEXSUBIMAGE2DPROC                               TexSubImage2D;
		GFX_TEXSUBIMAGE3DPROC                               TexSubImage3D;
		GFX_TEXTUREBUFFERPROC                               TextureBuffer;                               /* GFX_INT_EXT_DIRECT_STATE_ACCESS */
		GFX_TEXTUREPARAMETERFPROC                           TextureParameterf;                           /* GFX_INT_EXT_DIRECT_STATE_ACCESS */
		GFX_TEXTUREPARAMETERIPROC                           TextureParameteri;                           /* GFX_INT_EXT_DIRECT_STATE_ACCESS */
		GFX_TEXTURESTORAGE2DPROC                            TextureStorage2D;                            /* GFX_INT_EXT_DIRECT_STATE_ACCESS */
		GFX_TEXTURESTORAGE2DMULTISAMPLEPROC                 TextureStorage2DMultisample;                 /* GFX_INT_EXT_DIRECT_STATE_ACCESS */
		GFX_TEXTURESTORAGE3DPROC                            TextureStorage3D;                            /* GFX_INT_EXT_DIRECT_STATE_ACCESS */
		GFX_TEXTURESTORAGE3DMULTISAMPLEPROC                 TextureStorage3DMultisample;                 /* GFX_INT_EXT_DIRECT_STATE_ACCESS */
		GFX_TEXTURESUBIMAGE2DPROC                           TextureSubImage2D;                           /* GFX_INT_EXT_DIRECT_STATE_ACCESS */
		GFX_TEXTURESUBIMAGE3DPROC                           TextureSubImage3D;                           /* GFX_INT_EXT_DIRECT_STATE_ACCESS */
		GFX_TRANSFORMFEEDBACKVARYINGSPROC                   TransformFeedbackVaryings;
		GFX_UNIFORM1FVPROC                                  Uniform1fv;
		GFX_UNIFORM1IVPROC                                  Uniform1iv;
		GFX_UNIFORM1UIVPROC                                 Uniform1uiv;
		GFX_UNIFORM2FVPROC                                  Uniform2fv;
		GFX_UNIFORM2IVPROC                                  Uniform2iv;
		GFX_UNIFORM2UIVPROC                                 Uniform2uiv;
		GFX_UNIFORM3FVPROC                                  Uniform3fv;
		GFX_UNIFORM3IVPROC                                  Uniform3iv;
		GFX_UNIFORM3UIVPROC                                 Uniform3uiv;
		GFX_UNIFORM4FVPROC                                  Uniform4fv;
		GFX_UNIFORM4IVPROC                                  Uniform4iv;
		GFX_UNIFORM4UIVPROC                                 Uniform4uiv;
		GFX_UNIFORMBLOCKBINDINGPROC                         UniformBlockBinding;
		GFX_UNIFORMMATRIX2FVPROC                            UniformMatrix2fv;
		GFX_UNIFORMMATRIX3FVPROC                            UniformMatrix3fv;
		GFX_UNIFORMMATRIX4FVPROC                            UniformMatrix4fv;
	GFX_UNMAPBUFFERPROC                                 UnmapBuffer;
	/* GFX_INT_EXT_DIRECT_STATE_ACCESS, fallback to UnmapBuffer */
	GFX_UNMAPNAMEDBUFFERPROC                            UnmapNamedBuffer;
		GFX_USEPROGRAMPROC                                  UseProgram;
		GFX_USEPROGRAMSTAGESPROC                            UseProgramStages;                            /* GFX_EXT_PROGRAM_MAP */
		GFX_VERTEXARRAYATTRIBBINDINGPROC                    VertexArrayAttribBinding;                    /* GFX_INT_EXT_VERTEX_ATTRIB_BINDING */
		GFX_VERTEXARRAYATTRIBFORMATPROC                     VertexArrayAttribFormat;                     /* GFX_INT_EXT_VERTEX_ATTRIB_BINDING */
		GFX_VERTEXARRAYATTRIBIFORMATPROC                    VertexArrayAttribIFormat;                    /* GFX_INT_EXT_VERTEX_ATTRIB_BINDING */
		GFX_VERTEXARRAYBINDINGDIVISORPROC                   VertexArrayBindingDivisor;                   /* GFX_INT_EXT_VERTEX_ATTRIB_BINDING */
		GFX_VERTEXARRAYELEMENTBUFFERPROC                    VertexArrayElementBuffer;
		GFX_VERTEXARRAYVERTEXBUFFERPROC                     VertexArrayVertexBuffer;                     /* GFX_INT_EXT_VERTEX_ATTRIB_BINDING */
		GFX_VERTEXATTRIBBINDINGPROC                         VertexAttribBinding;                         /* GFX_INT_EXT_VERTEX_ATTRIB_BINDING */
		GFX_VERTEXATTRIBDIVISORPROC                         VertexAttribDivisor;                         /* GFX_EXT_INSTANCED_ATTRIBUTES */
		GFX_VERTEXATTRIBFORMATPROC                          VertexAttribFormat;                          /* GFX_INT_EXT_VERTEX_ATTRIB_BINDING */
		GFX_VERTEXATTRIBIFORMATPROC                         VertexAttribIFormat;                         /* GFX_INT_EXT_VERTEX_ATTRIB_BINDING */
		GFX_VERTEXATTRIBIPOINTERPROC                        VertexAttribIPointer;
		GFX_VERTEXATTRIBPOINTERPROC                         VertexAttribPointer;
		GFX_VERTEXBINDINGDIVISORPROC                        VertexBindingDivisor;                        /* GFX_INT_EXT_VERTEX_ATTRIB_BINDING */
	GFX_VIEWPORTPROC                                    Viewport;
};


#endif // GFX_CORE_RENDERER_GL_DEF_H
