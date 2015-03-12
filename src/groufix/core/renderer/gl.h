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

#ifndef GFX_CORE_RENDERER_GL_H
#define GFX_CORE_RENDERER_GL_H

#include "groufix/core/renderer.h"

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Internal GL object access
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
 * Get the location of a property (a.k.a uniform).
 *
 * @return Negative on failure, the location otherwise.
 *
 */
GLint _gfx_program_get_location(

		const GFXProgram*  program,
		unsigned short     index);

/**
 * Returns the handle of the program pipeline.
 *
 * Note: handle of a program if GFX_EXT_PROGRAM_MAP is unavailable.
 *
 */
GLuint _gfx_program_map_get_handle(

		const GFXProgramMap* map);

/**
 * Returns the handle of a sampler.
 *
 * Note: 0 if GFX_EXT_SAMPLER_OBJECTS is unavailable.
 *
 */
GLuint _gfx_sampler_get_handle(

		const GFXSampler* sampler);

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


/********************************************************
 * Texture format retrievers
 *******************************************************/

/**
 * Returns a minification filter of a sampler.
 *
 */
GLint _gfx_texture_min_filter_from_sampler(

		const GFXSampler* sampler);

/**
 * Returns the internal target of a texture (a.k.a type).
 *
 */
GLenum _gfx_texture_get_internal_target(

		const GFXTexture* texture);

/**
 * Converts a texture format to a client pixel format.
 *
 * @return Negative on failure.
 *
 */
GLint _gfx_texture_format_to_pixel_format(

		GFXTextureFormat format);

/**
 * Converts a texture format to an internal format.
 *
 * @return Negative on failure.
 *
 */
GLint _gfx_texture_format_to_internal(

		GFXTextureFormat format);

/**
 * Converts an internal format to a texture format.
 *
 */
GFXTextureFormat _gfx_texture_format_from_internal(

		GLint format);


/********************************************************
 * Unit binding & object binding
 *******************************************************/

/**
 * Binds a buffer to the appropriate uniform buffer index.
 *
 * @param prioritize Non-zero signifies this buffer must stay bound as long as possible.
 * @return the uniform buffer index it was bound to.
 *
 */
size_t _gfx_binder_bind_uniform_buffer(

		GLuint      buffer,
		GLintptr    offset,
		GLsizeiptr  size,
		int         prioritize,
		GFX_WIND_ARG);

/**
 * Makes sure a buffer is unbound from any uniform buffer index.
 *
 */
void _gfx_binder_unbind_uniform_buffer(

		GLuint buffer,
		GFX_WIND_ARG);

/**
 * Binds a texture to the appropriate unit.
 *
 * @param target     Internal target of the texture.
 * @param prioritize Non-zero signifies this texture must stay bound as long as possible.
 * @return the texture unit it was bound to.
 *
 */
size_t _gfx_binder_bind_texture(

		GLuint  texture,
		GLenum  target,
		int     prioritize,
		GFX_WIND_ARG);

/**
 * Binds a texture and sampler to the appropriate unit.
 *
 * @param prioritize Non-zero signifies this texture sampler pair must stay bound as long as possible.
 * @return the texture unit they were bound to.
 *
 */
size_t _gfx_binder_bind_sampler(

		GLuint  texture,
		GLuint  sampler,
		GLenum  target,
		int     prioritize,
		GFX_WIND_ARG);

/**
 * Makes sure a texture is unbound from any unit.
 *
 */
void _gfx_binder_unbind_texture(

		GLuint texture,
		GFX_WIND_ARG);

/**
 * Makes sure a sampler is unbound from any unit.
 *
 */
void _gfx_binder_unbind_sampler(

		GLuint sampler,
		GFX_WIND_ARG);

/**
 * Sets the framebuffer handle associated with a pipeline as current for the current context.
 *
 */
void _gfx_pipeline_bind(

		GLenum  target,
		GLuint  framebuffer,
		GFX_WIND_ARG);

/**
 * Sets the program pipeline or program handle as currently bound to the current context.
 *
 */
void _gfx_program_map_bind(

		GLuint id,
		GFX_WIND_ARG);

/**
 * Sets the layout handle as currently bound to the current context.
 *
 */
void _gfx_vertex_layout_bind(

		GLuint vao,
		GFX_WIND_ARG);


#ifdef __cplusplus
}
#endif

#endif // GFX_CORE_RENDERER_GL_H
