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

#ifndef GFX_CORE_RENDERER_GL_H
#define GFX_CORE_RENDERER_GL_H

#include "groufix/core/pipeline.h"

/* Correct OGL header */
#if defined(GFX_GL)
	#include <GL/glcorearb.h>
#elif defined(GFX_GLES)
	#include <GLES3/gl31.h>
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
	#define GFX_CONTEXT_MINOR_MAX 1

	#define GFX_CONTEXT_ALL_MINORS_MAX 1

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
 * OpenGL extensions & emulators
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
typedef void (APIENTRYP GFX_BINDTEXTUREUNITPROC)                   (GLuint, GLuint);
typedef void (APIENTRYP GFX_BINDVERTEXARRAYPROC)                   (GLuint);
typedef void (APIENTRYP GFX_BLENDEQUATIONSEPARATEPROC)             (GLenum, GLenum);
typedef void (APIENTRYP GFX_BLENDFUNCSEPARATEPROC)                 (GLenum, GLenum, GLenum, GLenum);
typedef void (APIENTRYP GFX_BUFFERDATAPROC)                        (GLenum, GLsizeiptr, const GLvoid*, GLenum);
typedef void (APIENTRYP GFX_BUFFERSUBDATAPROC)                     (GLenum, GLintptr, GLsizeiptr, const GLvoid*);
typedef void (APIENTRYP GFX_CLEARPROC)                             (GLbitfield);
typedef void (APIENTRYP GFX_COMPILESHADERPROC)                     (GLuint);
typedef void (APIENTRYP GFX_COPYBUFFERSUBDATAPROC)                 (GLenum, GLenum, GLintptr, GLintptr, GLsizeiptr);
typedef void (APIENTRYP GFX_COPYNAMEDBUFFERSUBDATAPROC)            (GLuint, GLuint, GLintptr, GLintptr, GLsizei);
typedef void (APIENTRYP GFX_CREATEBUFFERSPROC)                     (GLsizei, GLuint*);
typedef void (APIENTRYP GFX_CREATEFRAMEBUFFERSPROC)                (GLsizei, GLuint*);
typedef GLuint (APIENTRYP GFX_CREATEPROGRAMPROC)                   (void);
typedef GLuint (APIENTRYP GFX_CREATESHADERPROC)                    (GLenum);
typedef void (APIENTRYP GFX_CREATETEXTURESPROC)                    (GLenum, GLsizei, GLuint*);
typedef void (APIENTRYP GFX_CREATEVERTEXARRAYSPROC)                (GLsizei, GLuint*);
typedef void (APIENTRYP GFX_CULLFACEPROC)                          (GLenum);
typedef void (APIENTRYP GFX_DELETEBUFFERSPROC)                     (GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_DELETEFRAMEBUFFERSPROC)                (GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_DELETEPROGRAMPROC)                     (GLuint);
typedef void (APIENTRYP GFX_DELETESHADERPROC)                      (GLuint);
typedef void (APIENTRYP GFX_DELETETEXTURESPROC)                    (GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_DELETEVERTEXARRAYSPROC)                (GLsizei, const GLuint*);
typedef void (APIENTRYP GFX_DEPTHFUNCPROC)                         (GLenum);
typedef void (APIENTRYP GFX_DEPTHMASKPROC)                         (GLboolean);
typedef void (APIENTRYP GFX_DETACHSHADERPROC)                      (GLuint, GLuint);
typedef void (APIENTRYP GFX_DISABLEPROC)                           (GLenum);
typedef void (APIENTRYP GFX_DISABLEVERTEXARRAYATTRIBPROC)          (GLuint, GLuint);
typedef void (APIENTRYP GFX_DISABLEVERTEXATTRIBARRAYPROC)          (GLuint);
typedef void (APIENTRYP GFX_DRAWARRAYSPROC)                        (GLenum, GLint, GLsizei);
typedef void (APIENTRYP GFX_DRAWARRAYSINSTANCEDPROC)               (GLenum, GLint, GLsizei, GLsizei);
typedef void (APIENTRYP GFX_DRAWARRAYSINSTANCEDBASEINSTANCEPROC)   (GLenum, GLint, GLsizei, GLsizei, GLuint);
typedef void (APIENTRYP GFX_DRAWBUFFERSPROC)                       (GLsizei, const GLenum*);
typedef void (APIENTRYP GFX_DRAWELEMENTSPROC)                      (GLenum, GLsizei, GLenum, const GLvoid*);
typedef void (APIENTRYP GFX_DRAWELEMENTSINSTANCEDPROC)             (GLenum, GLsizei, GLenum, const GLvoid*, GLsizei);
typedef void (APIENTRYP GFX_DRAWELEMENTSINSTANCEDBASEINSTANCEPROC) (GLenum, GLsizei, GLenum, const GLvoid*, GLsizei, GLuint);
typedef void (APIENTRYP GFX_ENABLEPROC)                            (GLenum);
typedef void (APIENTRYP GFX_ENABLEVERTEXARRAYATTRIBPROC)           (GLuint, GLuint);
typedef void (APIENTRYP GFX_ENABLEVERTEXATTRIBARRAYPROC)           (GLuint);
typedef void (APIENTRYP GFX_ENDTRANSFORMFEEDBACKPROC)              (void);
typedef void (APIENTRYP GFX_FRAMEBUFFERTEXTUREPROC)                (GLenum, GLenum, GLuint, GLint);
typedef void (APIENTRYP GFX_FRAMEBUFFERTEXTURE1DPROC)              (GLenum, GLenum, GLenum, GLuint, GLint);
typedef void (APIENTRYP GFX_FRAMEBUFFERTEXTURE2DPROC)              (GLenum, GLenum, GLenum, GLuint, GLint);
typedef void (APIENTRYP GFX_FRAMEBUFFERTEXTURELAYERPROC)           (GLenum, GLenum, GLuint, GLint, GLint);
typedef void (APIENTRYP GFX_GENBUFFERSPROC)                        (GLsizei, GLuint*);
typedef void (APIENTRYP GFX_GENERATEMIPMAPPROC)                    (GLenum);
typedef void (APIENTRYP GFX_GENERATETEXTUREMIPMAPPROC)             (GLuint);
typedef void (APIENTRYP GFX_GENFRAMEBUFFERSPROC)                   (GLsizei, GLuint*);
typedef void (APIENTRYP GFX_GENTEXTURESPROC)                       (GLsizei, GLuint*);
typedef void (APIENTRYP GFX_GENVERTEXARRAYSPROC)                   (GLsizei, GLuint*);
typedef void (APIENTRYP GFX_GETACTIVEUNIFORMPROC)                  (GLuint, GLuint, GLsizei, GLsizei*, GLint*, GLenum*, GLchar*);
typedef void (APIENTRYP GFX_GETACTIVEUNIFORMBLOCKIVPROC)           (GLuint, GLuint, GLenum, GLint*);
typedef void (APIENTRYP GFX_GETACTIVEUNIFORMSIVPROC)               (GLuint, GLsizei, const GLuint*, GLenum, GLint*);
typedef void (APIENTRYP GFX_GETBUFFERSUBDATAPROC)                  (GLenum, GLintptr, GLsizeiptr, GLvoid*);
typedef GLenum (APIENTRYP GFX_GETERRORPROC)                        (void);
typedef void (APIENTRYP GFX_GETNAMEDBUFFERSUBDATAPROC)             (GLuint, GLintptr, GLsizei, void*);
typedef void (APIENTRYP GFX_GETPROGRAMBINARYPROC)                  (GLuint, GLsizei, GLsizei*, GLenum*, void*);
typedef void (APIENTRYP GFX_GETPROGRAMINFOLOGPROC)                 (GLuint, GLsizei, GLsizei*, GLchar*);
typedef void (APIENTRYP GFX_GETPROGRAMIVPROC)                      (GLuint, GLenum, GLint*);
typedef void (APIENTRYP GFX_GETSHADERINFOLOGPROC)                  (GLuint, GLsizei, GLsizei*, GLchar*);
typedef void (APIENTRYP GFX_GETSHADERIVPROC)                       (GLuint, GLenum, GLint*);
typedef void (APIENTRYP GFX_GETSHADERSOURCEPROC)                   (GLuint, GLsizei, GLsizei*, GLchar*);
typedef const GLubyte* (APIENTRYP GFX_GETSTRINGIPROC)              (GLenum, GLuint);
typedef GLuint (APIENTRYP GFX_GETUNIFORMBLOCKINDEXPROC)            (GLuint, const GLchar*);
typedef void (APIENTRYP GFX_GETUNIFORMINDICESPROC)                 (GLuint, GLsizei, const GLchar*const*, GLuint*);
typedef GLint (APIENTRYP GFX_GETUNIFORMLOCATIONPROC)               (GLuint, const GLchar*);
typedef void (APIENTRYP GFX_LINKPROGRAMPROC)                       (GLuint);
typedef void* (APIENTRYP GFX_MAPBUFFERRANGEPROC)                   (GLenum, GLintptr, GLsizeiptr, GLbitfield);
typedef void* (APIENTRYP GFX_MAPNAMEDBUFFERRANGEPROC)              (GLuint, GLintptr, GLsizei, GLbitfield);
typedef void (APIENTRYP GFX_NAMEDBUFFERDATAPROC)                   (GLuint, GLsizei, const void*, GLenum);
typedef void (APIENTRYP GFX_NAMEDBUFFERSUBDATAPROC)                (GLuint, GLintptr, GLsizei, const void*);
typedef void (APIENTRYP GFX_NAMEDFRAMEBUFFERDRAWBUFFERSPROC)       (GLuint, GLsizei, const GLenum*);
typedef void (APIENTRYP GFX_NAMEDFRAMEBUFFERTEXTUREPROC)           (GLuint, GLenum, GLuint, GLint);
typedef void (APIENTRYP GFX_NAMEDFRAMEBUFFERTEXTURE1DPROC)         (GLuint, GLenum, GLenum, GLuint, GLint);
typedef void (APIENTRYP GFX_NAMEDFRAMEBUFFERTEXTURE2DPROC)         (GLuint, GLenum, GLenum, GLuint, GLint);
typedef void (APIENTRYP GFX_NAMEDFRAMEBUFFERTEXTURELAYERPROC)      (GLuint, GLenum, GLuint, GLint, GLint);
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
typedef void (APIENTRYP GFX_TEXSTORAGE1DPROC)                      (GLenum, GLsizei, GLenum, GLsizei);
typedef void (APIENTRYP GFX_TEXSTORAGE2DPROC)                      (GLenum, GLsizei, GLenum, GLsizei, GLsizei);
typedef void (APIENTRYP GFX_TEXSTORAGE2DMULTISAMPLEPROC)           (GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean);
typedef void (APIENTRYP GFX_TEXSTORAGE3DPROC)                      (GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei);
typedef void (APIENTRYP GFX_TEXSTORAGE3DMULTISAMPLEPROC)           (GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean);
typedef void (APIENTRYP GFX_TEXSUBIMAGE1DPROC)                     (GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const GLvoid*);
typedef void (APIENTRYP GFX_TEXSUBIMAGE2DPROC)                     (GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*);
typedef void (APIENTRYP GFX_TEXSUBIMAGE3DPROC)                     (GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*);
typedef void (APIENTRYP GFX_TEXTUREBUFFERPROC)                     (GLuint, GLenum, GLuint);
typedef void (APIENTRYP GFX_TEXTUREPARAMETERIPROC)                 (GLuint, GLenum, GLint);
typedef void (APIENTRYP GFX_TEXTURESTORAGE1DPROC)                  (GLuint, GLsizei, GLenum, GLsizei);
typedef void (APIENTRYP GFX_TEXTURESTORAGE2DPROC)                  (GLuint, GLsizei, GLenum, GLsizei, GLsizei);
typedef void (APIENTRYP GFX_TEXTURESTORAGE2DMULTISAMPLEPROC)       (GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLboolean);
typedef void (APIENTRYP GFX_TEXTURESTORAGE3DPROC)                  (GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLsizei);
typedef void (APIENTRYP GFX_TEXTURESTORAGE3DMULTISAMPLEPROC)       (GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean);
typedef void (APIENTRYP GFX_TEXTURESUBIMAGE1DPROC)                 (GLuint, GLint, GLint, GLsizei, GLenum, GLenum, const void*);
typedef void (APIENTRYP GFX_TEXTURESUBIMAGE2DPROC)                 (GLuint, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void*);
typedef void (APIENTRYP GFX_TEXTURESUBIMAGE3DPROC)                 (GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void*);
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
typedef GLboolean (APIENTRYP GFX_UNMAPNAMEDBUFFERPROC)             (GLuint);
typedef void (APIENTRYP GFX_USEPROGRAMPROC)                        (GLuint);
typedef void (APIENTRYP GFX_VERTEXATTRIBDIVISORPROC)               (GLuint, GLuint);
typedef void (APIENTRYP GFX_VERTEXATTRIBIPOINTERPROC)              (GLuint, GLint, GLenum, GLsizei, const GLvoid*);
typedef void (APIENTRYP GFX_VERTEXATTRIBPOINTERPROC)               (GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*);
typedef void (APIENTRYP GFX_VIEWPORTPROC)                          (GLint, GLint, GLsizei, GLsizei);


/* Emulators */
void APIENTRY _gfx_gl_bind_texture_unit                     (GLuint, GLuint);
void APIENTRY _gfx_gl_copy_named_buffer_sub_data            (GLuint, GLuint, GLintptr, GLintptr, GLsizei);
void APIENTRY _gfx_gl_create_buffers                        (GLsizei, GLuint*);
void APIENTRY _gfx_gl_create_framebuffers                   (GLsizei, GLuint*);
void APIENTRY _gfx_gl_create_textures                       (GLenum, GLsizei, GLuint*);
void APIENTRY _gfx_gl_create_vertex_arrays                  (GLsizei, GLuint*);
void APIENTRY _gfx_gl_disable_vertex_array_attrib           (GLuint, GLuint);
void APIENTRY _gfx_gl_draw_arrays_instanced_base_instance   (GLenum, GLint, GLsizei, GLsizei, GLuint);
void APIENTRY _gfx_gl_draw_elements_instanced_base_instance (GLenum, GLsizei, GLenum, const GLvoid*, GLsizei, GLuint);
void APIENTRY _gfx_gl_enable_vertex_array_attrib            (GLuint, GLuint);
void APIENTRY _gfx_gl_generate_texture_mipmap               (GLuint);
void APIENTRY _gfx_gl_get_named_buffer_sub_data             (GLuint, GLintptr, GLsizei, void*);
void* APIENTRY _gfx_gl_map_named_buffer_range               (GLuint, GLintptr, GLsizei, GLbitfield);
void APIENTRY _gfx_gl_named_buffer_data                     (GLuint, GLsizei, const void*, GLenum);
void APIENTRY _gfx_gl_named_buffer_sub_data                 (GLuint, GLintptr, GLsizei, const void*);
void APIENTRY _gfx_gl_named_framebuffer_draw_buffers        (GLuint, GLsizei, const GLenum*);
void APIENTRY _gfx_gl_named_framebuffer_texture             (GLuint, GLenum, GLuint, GLint);
void APIENTRY _gfx_gl_named_framebuffer_texture_layer       (GLuint, GLenum, GLuint, GLint, GLint);
void APIENTRY _gfx_gl_patch_parameter_i                     (GLenum, GLint);
void APIENTRY _gfx_gl_texture_buffer                        (GLuint, GLenum, GLuint);
void APIENTRY _gfx_gl_texture_parameter_i                   (GLuint, GLenum, GLint);
void APIENTRY _gfx_gl_texture_storage_1d                    (GLuint, GLsizei, GLenum, GLsizei);
void APIENTRY _gfx_gl_texture_storage_2d                    (GLuint, GLsizei, GLenum, GLsizei, GLsizei);
void APIENTRY _gfx_gl_texture_storage_2d_multisample        (GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLboolean);
void APIENTRY _gfx_gl_texture_storage_3d                    (GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLsizei);
void APIENTRY _gfx_gl_texture_storage_3d_multisample        (GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean);
void APIENTRY _gfx_gl_texture_sub_image_1d                  (GLuint, GLint, GLint, GLsizei, GLenum, GLenum, const void*);
void APIENTRY _gfx_gl_texture_sub_image_2d                  (GLuint, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void*);
void APIENTRY _gfx_gl_texture_sub_image_3d                  (GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void*);
GLboolean APIENTRY _gfx_gl_unmap_named_buffer               (GLuint);
void APIENTRY _gfx_gles_framebuffer_texture                 (GLenum, GLenum, GLuint, GLint);
void APIENTRY _gfx_gles_framebuffer_texture_1d              (GLenum, GLenum, GLenum, GLuint, GLint);
void APIENTRY _gfx_gles_get_buffer_sub_data                 (GLenum, GLintptr, GLsizeiptr, GLvoid*);
void APIENTRY _gfx_gles_named_framebuffer_texture_1d        (GLuint, GLenum, GLenum, GLuint, GLint);
void APIENTRY _gfx_gles_named_framebuffer_texture_2d        (GLuint, GLenum, GLenum, GLuint, GLint);
void APIENTRY _gfx_gles_polygon_mode                        (GLenum, GLenum);
void APIENTRY _gfx_gles_tex_buffer                          (GLenum, GLenum, GLuint);
void APIENTRY _gfx_gles_tex_image_1d                        (GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const GLvoid*);
void APIENTRY _gfx_gles_tex_image_2d_multisample            (GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean);
void APIENTRY _gfx_gles_tex_image_3d_multisample            (GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean);
void APIENTRY _gfx_gles_tex_storage_1d                      (GLenum, GLsizei, GLenum, GLsizei);
void APIENTRY _gfx_gles_tex_storage_2d_multisample          (GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean);
void APIENTRY _gfx_gles_tex_storage_3d_multisample          (GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean);
void APIENTRY _gfx_gles_tex_sub_image_1d                    (GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const GLvoid*);
void APIENTRY _gfx_gl_named_framebuffer_texture_1d          (GLuint, GLenum, GLenum, GLuint, GLint);
void APIENTRY _gfx_gl_named_framebuffer_texture_2d          (GLuint, GLenum, GLenum, GLuint, GLint);
void APIENTRY _gfx_gl_get_program_binary                    (GLuint, GLsizei, GLsizei*, GLenum*, void*);
void APIENTRY _gfx_gl_program_binary                        (GLuint, GLenum, const void*, GLsizei);
void APIENTRY _gfx_gl_program_parameter_i                   (GLuint, GLenum, GLint);
void APIENTRY _gfx_gl_tex_storage_1d                        (GLenum, GLsizei, GLenum, GLsizei);
void APIENTRY _gfx_gl_tex_storage_2d                        (GLenum, GLsizei, GLenum, GLsizei, GLsizei);
void APIENTRY _gfx_gl_tex_storage_2d_multisample            (GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean);
void APIENTRY _gfx_gl_tex_storage_3d                        (GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei);
void APIENTRY _gfx_gl_tex_storage_3d_multisample            (GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean);
void APIENTRY _gfx_gl_vertex_attrib_divisor                 (GLuint, GLuint);


/********************************************************
 * OpenGL renderer & context
 *******************************************************/

/** OpenGL context */
typedef struct GFX_Renderer
{
	/* State & bound objects */
	GLuint         fbos[2];  /* Currently bound FBOs (0 = draw, 1 = read) */
	GLuint         vao;      /* Currently bound VAO */
	GLuint         program;  /* Currently used program */
	GLuint         post;     /* Layout for post processing */

	/* Viewport & state values */
	int            x;
	int            y;
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
	GFX_BINDTEXTUREUNITPROC                    BindTextureUnit;
	GFX_BINDVERTEXARRAYPROC                    BindVertexArray;
	GFX_BLENDEQUATIONSEPARATEPROC              BlendEquationSeparate;
	GFX_BLENDFUNCSEPARATEPROC                  BlendFuncSeparate;
	GFX_BUFFERDATAPROC                         BufferData;
	GFX_BUFFERSUBDATAPROC                      BufferSubData;
	GFX_CLEARPROC                              Clear;
	GFX_COMPILESHADERPROC                      CompileShader;
	GFX_COPYBUFFERSUBDATAPROC                  CopyBufferSubData;
	GFX_COPYNAMEDBUFFERSUBDATAPROC             CopyNamedBufferSubData;
	GFX_CREATEBUFFERSPROC                      CreateBuffers;
	GFX_CREATEFRAMEBUFFERSPROC                 CreateFramebuffers;
	GFX_CREATEPROGRAMPROC                      CreateProgram;
	GFX_CREATESHADERPROC                       CreateShader;
	GFX_CREATETEXTURESPROC                     CreateTextures;
	GFX_CREATEVERTEXARRAYSPROC                 CreateVertexArrays;
	GFX_CULLFACEPROC                           CullFace;
	GFX_DELETEBUFFERSPROC                      DeleteBuffers;
	GFX_DELETEFRAMEBUFFERSPROC                 DeleteFramebuffers;
	GFX_DELETEPROGRAMPROC                      DeleteProgram;
	GFX_DELETESHADERPROC                       DeleteShader;
	GFX_DELETETEXTURESPROC                     DeleteTextures;
	GFX_DELETEVERTEXARRAYSPROC                 DeleteVertexArrays;
	GFX_DEPTHFUNCPROC                          DepthFunc;
	GFX_DEPTHMASKPROC                          DepthMask;
	GFX_DETACHSHADERPROC                       DetachShader;
	GFX_DISABLEPROC                            Disable;
	GFX_DISABLEVERTEXARRAYATTRIBPROC           DisableVertexArrayAttrib;
	GFX_DISABLEVERTEXATTRIBARRAYPROC           DisableVertexAttribArray;
	GFX_DRAWARRAYSPROC                         DrawArrays;
	GFX_DRAWARRAYSINSTANCEDPROC                DrawArraysInstanced;
	GFX_DRAWARRAYSINSTANCEDBASEINSTANCEPROC    DrawArraysInstancedBaseInstance;   /* GFX_EXT_INSTANCED_BASE_ATTRIBUTES */
	GFX_DRAWBUFFERSPROC                        DrawBuffers;
	GFX_DRAWELEMENTSPROC                       DrawElements;
	GFX_DRAWELEMENTSINSTANCEDPROC              DrawElementsInstanced;
	GFX_DRAWELEMENTSINSTANCEDBASEINSTANCEPROC  DrawElementsInstancedBaseInstance; /* GFX_EXT_INSTANCED_BASE_ATTRIBUTES */
	GFX_ENABLEPROC                             Enable;
	GFX_ENABLEVERTEXARRAYATTRIBPROC            EnableVertexArrayAttrib;
	GFX_ENABLEVERTEXATTRIBARRAYPROC            EnableVertexAttribArray;
	GFX_ENDTRANSFORMFEEDBACKPROC               EndTransformFeedback;
	GFX_FRAMEBUFFERTEXTUREPROC                 FramebufferTexture;                /* GFX_EXT_BUFFER_TEXTURE */
	GFX_FRAMEBUFFERTEXTURE1DPROC               FramebufferTexture1D;              /* GFX_EXT_TEXTURE_1D */
	GFX_FRAMEBUFFERTEXTURE2DPROC               FramebufferTexture2D;
	GFX_FRAMEBUFFERTEXTURELAYERPROC            FramebufferTextureLayer;
	GFX_GENBUFFERSPROC                         GenBuffers;
	GFX_GENERATEMIPMAPPROC                     GenerateMipmap;
	GFX_GENERATETEXTUREMIPMAPPROC              GenerateTextureMipmap;             /* GFX_EXT_DIRECT_STATE_ACCESS */
	GFX_GENFRAMEBUFFERSPROC                    GenFramebuffers;
	GFX_GENTEXTURESPROC                        GenTextures;
	GFX_GENVERTEXARRAYSPROC                    GenVertexArrays;
	GFX_GETACTIVEUNIFORMPROC                   GetActiveUniform;
	GFX_GETACTIVEUNIFORMBLOCKIVPROC            GetActiveUniformBlockiv;
	GFX_GETACTIVEUNIFORMSIVPROC                GetActiveUniformsiv;
	GFX_GETBUFFERSUBDATAPROC                   GetBufferSubData;
	GFX_GETERRORPROC                           GetError;
	GFX_GETNAMEDBUFFERSUBDATAPROC              GetNamedBufferSubData;
	GFX_GETPROGRAMBINARYPROC                   GetProgramBinary;                  /* GFX_EXT_PROGRAM_BINARY */
	GFX_GETPROGRAMINFOLOGPROC                  GetProgramInfoLog;
	GFX_GETPROGRAMIVPROC                       GetProgramiv;
	GFX_GETSHADERINFOLOGPROC                   GetShaderInfoLog;
	GFX_GETSHADERIVPROC                        GetShaderiv;
	GFX_GETSHADERSOURCEPROC                    GetShaderSource;
	GFX_GETSTRINGIPROC                         GetStringi;
	GFX_GETUNIFORMBLOCKINDEXPROC               GetUniformBlockIndex;
	GFX_GETUNIFORMINDICESPROC                  GetUniformIndices;
	GFX_GETUNIFORMLOCATIONPROC                 GetUniformLocation;
	GFX_LINKPROGRAMPROC                        LinkProgram;
	GFX_MAPBUFFERRANGEPROC                     MapBufferRange;
	GFX_MAPNAMEDBUFFERRANGEPROC                MapNamedBufferRange;
	GFX_NAMEDBUFFERDATAPROC                    NamedBufferData;
	GFX_NAMEDBUFFERSUBDATAPROC                 NamedBufferSubData;
	GFX_NAMEDFRAMEBUFFERDRAWBUFFERSPROC        NamedFramebufferDrawBuffers;
	GFX_NAMEDFRAMEBUFFERTEXTUREPROC            NamedFramebufferTexture;           /* GFX_EXT_BUFFER_TEXTURE */
	GFX_NAMEDFRAMEBUFFERTEXTURE1DPROC          NamedFramebufferTexture1D;         /* GFX_EXT_TEXTURE_1D */
	GFX_NAMEDFRAMEBUFFERTEXTURE2DPROC          NamedFramebufferTexture2D;
	GFX_NAMEDFRAMEBUFFERTEXTURELAYERPROC       NamedFramebufferTextureLayer;
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
	GFX_TEXIMAGE2DMULTISAMPLEPROC              TexImage2DMultisample;             /* No GLES */
	GFX_TEXIMAGE3DPROC                         TexImage3D;
	GFX_TEXIMAGE3DMULTISAMPLEPROC              TexImage3DMultisample;             /* No GLES */
	GFX_TEXPARAMETERIPROC                      TexParameteri;
	GFX_TEXSTORAGE1DPROC                       TexStorage1D;                      /* GFX_EXT_TEXTURE_1D */
	GFX_TEXSTORAGE2DPROC                       TexStorage2D;
	GFX_TEXSTORAGE2DMULTISAMPLEPROC            TexStorage2DMultisample;           /* GFX_EXT_MULTISAMPLE_TEXTURE */
	GFX_TEXSTORAGE3DPROC                       TexStorage3D;
	GFX_TEXSTORAGE3DMULTISAMPLEPROC            TexStorage3DMultisample;           /* GFX_EXT_LAYERED_MULTISAMPLE_TEXTURE */
	GFX_TEXSUBIMAGE1DPROC                      TexSubImage1D;                     /* GFX_EXT_TEXTURE_1D */
	GFX_TEXSUBIMAGE2DPROC                      TexSubImage2D;
	GFX_TEXSUBIMAGE3DPROC                      TexSubImage3D;
	GFX_TEXTUREBUFFERPROC                      TextureBuffer;                     /* GFX_EXT_DIRECT_STATE_ACCESS */
	GFX_TEXTUREPARAMETERIPROC                  TextureParameteri;                 /* GFX_EXT_DIRECT_STATE_ACCESS */
	GFX_TEXTURESTORAGE1DPROC                   TextureStorage1D;                  /* GFX_EXT_DIRECT_STATE_ACCESS */
	GFX_TEXTURESTORAGE2DPROC                   TextureStorage2D;                  /* GFX_EXT_DIRECT_STATE_ACCESS */
	GFX_TEXTURESTORAGE2DMULTISAMPLEPROC        TextureStorage2DMultisample;       /* GFX_EXT_DIRECT_STATE_ACCESS */
	GFX_TEXTURESTORAGE3DPROC                   TextureStorage3D;                  /* GFX_EXT_DIRECT_STATE_ACCESS */
	GFX_TEXTURESTORAGE3DMULTISAMPLEPROC        TextureStorage3DMultisample;       /* GFX_EXT_DIRECT_STATE_ACCESS */
	GFX_TEXTURESUBIMAGE1DPROC                  TextureSubImage1D;                 /* GFX_EXT_DIRECT_STATE_ACCESS */
	GFX_TEXTURESUBIMAGE2DPROC                  TextureSubImage2D;                 /* GFX_EXT_DIRECT_STATE_ACCESS */
	GFX_TEXTURESUBIMAGE3DPROC                  TextureSubImage3D;                 /* GFX_EXT_DIRECT_STATE_ACCESS */
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
	GFX_UNMAPNAMEDBUFFERPROC                   UnmapNamedBuffer;
	GFX_USEPROGRAMPROC                         UseProgram;
	GFX_VERTEXATTRIBDIVISORPROC                VertexAttribDivisor;               /* GFX_EXT_INSTANCED_ATTRIBUTES */
	GFX_VERTEXATTRIBIPOINTERPROC               VertexAttribIPointer;
	GFX_VERTEXATTRIBPOINTERPROC                VertexAttribPointer;
	GFX_VIEWPORTPROC                           Viewport;

} GFX_Renderer;


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

#endif // GFX_CORE_RENDERER_GL_H