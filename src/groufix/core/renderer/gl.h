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

#ifndef GFX_CORE_RENDERER_GL_H
#define GFX_CORE_RENDERER_GL_H

#include "groufix/core/renderer.h"


/********************************************************
 * State management
 *******************************************************/

/**
 * Sets the number of vertices per patch.
 *
 */
void _gfx_gl_states_set_patch_vertices(

		unsigned int vertices,
		GFX_CONT_ARG);

/**
 * Sets the pixel pack alignment of the current context.
 *
 */
void _gfx_gl_states_set_pixel_pack_alignment(

		unsigned char align,
		GFX_CONT_ARG);

/**
 * Sets the pixel unpack alignment of the current context.
 *
 */
void _gfx_gl_states_set_pixel_unpack_alignment(

		unsigned char align,
		GFX_CONT_ARG);

/**
 * Sets the viewport size of the current context.
 *
 */
void _gfx_gl_states_set_viewport(

		GFXViewport viewport,
		GFX_CONT_ARG);


/********************************************************
 * Format descriptor interpreters
 *******************************************************/

/**
 * Converts a format of an attribute to a vertex format as defined by GL.
 *
 * @param shaderType Returns the general shader type of the attribute.
 * @return Zero on an invalid format or failure otherwise (output may be written to).
 *
 * Note: shaderType is 0 on integer, 1 on float and 2 on double.
 *
 */
int _gfx_gl_format_to_vertex(

		const GFXVertexAttribute*  attribute,
		int*                       shaderType,
		GLint*                     size,
		GLenum*                    type,
		GLboolean*                 normalized,
		GFX_CONT_ARG);


/********************************************************
 * Internal GL object access
 *******************************************************/

/**
 * Returns the handle of the framebuffer associated with a pipeline.
 *
 */
/*GLuint _gfx_gl_pipeline_get_handle(

		const GFXPipeline* pipeline);*/

/**
 * Returns the handle of a program.
 *
 */
/*GLuint _gfx_gl_program_get_handle(

		const GFXProgram* program);*/

/**
 * Get the location of a property (a.k.a uniform).
 *
 * @return Negative on failure, the location otherwise.
 *
 */
/*GLint _gfx_gl_program_get_location(

		const GFXProgram*  program,
		unsigned short     index);*/

/**
 * Returns the handle of the program pipeline.
 *
 * Note: handle of a program if GFX_EXT_PROGRAM_MAP is unavailable.
 *
 */
/*GLuint _gfx_gl_program_map_get_handle(

		const GFXProgramMap* map);*/

/**
 * Returns the handle of a sampler.
 *
 * Note: 0 if GFX_EXT_SAMPLER_OBJECTS is unavailable.
 *
 */
/*GLuint _gfx_gl_sampler_get_handle(

		const GFXSampler* sampler);*/

/**
 * Returns the handle of a shader.
 *
 */
/*GLuint _gfx_gl_shader_get_handle(

		const GFXShader* shader);*/

/**
 * Returns the handle of a shared buffer.
 *
 */
/*GLuint _gfx_gl_shared_buffer_get_handle(

		const GFXSharedBuffer* buffer);*/

/**
 * Returns the handle of a texture.
 *
 */
/*GLuint _gfx_gl_texture_get_handle(

		const GFXTexture* texture);*/

/**
 * Returns the index buffer and the byte offset within it of a layout.
 *
 * @return Zero if no index buffer is used.
 *
 */
/*GLuint _gfx_gl_vertex_layout_get_index_buffer(

		const GFXVertexLayout*  layout,
		size_t*                 offset);*/


/********************************************************
 * Texture format handling
 *******************************************************/

/**
 * Returns a minification filter of a sampler.
 *
 */
/*GLint _gfx_gl_texture_min_filter_from_sampler(

		const GFXSampler* sampler);*/

/**
 * Sets the sampler properties of a texture itself.
 *
 * @param target Internal target of the texture.
 * @param values Sampler values to use for the texture's state.
 *
 * Note: this function assumes the texture is currently bound
 * to a unit and active if GFX_EXT_DIRECT_STATE_ACCESS is unavailable.
 *
 */
/*void _gfx_gl_texture_set_sampler(

		GLuint             texture,
		GLuint             target,
		const GFXSampler*  values,
		GFX_CONT_ARG);*/

/**
 * Returns the internal target of a texture (a.k.a type).
 *
 */
/*GLenum _gfx_gl_texture_get_internal_target(

		const GFXTexture* texture);*/

/**
 * Converts a texture format to a client pixel format.
 *
 * @return Negative on failure.
 *
 */
/*GLint _gfx_gl_texture_format_to_pixel_format(

		GFXTextureFormat format);*/

/**
 * Converts a texture format to an internal format.
 *
 * @return Negative on failure.
 *
 */
/*GLint _gfx_gl_texture_format_to_internal(

		GFXTextureFormat format);*/

/**
 * Converts an internal format to a texture format.
 *
 */
/*GFXTextureFormat _gfx_gl_texture_format_from_internal(

		GLint format);*/


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
/*size_t _gfx_gl_binder_bind_uniform_buffer(

		GLuint      buffer,
		GLintptr    offset,
		GLsizeiptr  size,
		int         prioritize,
		GFX_CONT_ARG);*/

/**
 * Makes sure a buffer is unbound from any uniform buffer index.
 *
 */
/*void _gfx_gl_binder_unbind_uniform_buffer(

		GLuint buffer,
		GFX_CONT_ARG);*/

/**
 * Binds a texture to the appropriate unit.
 *
 * @param target     Internal target of the texture.
 * @param prioritize Non-zero signifies this texture must stay bound as long as possible.
 * @return the texture unit it was bound to.
 *
 */
/*size_t _gfx_gl_binder_bind_texture(

		GLuint  texture,
		GLenum  target,
		int     prioritize,
		GFX_CONT_ARG);*/

/**
 * Binds a texture and sampler to the appropriate unit.
 *
 * @param prioritize Non-zero signifies this texture sampler pair must stay bound as long as possible.
 * @return the texture unit they were bound to.
 *
 */
/*size_t _gfx_gl_binder_bind_sampler(

		GLuint  sampler,
		GLuint  texture,
		GLenum  target,
		int     prioritize,
		GFX_CONT_ARG);*/

/**
 * Makes sure a texture is unbound from any unit.
 *
 */
/*void _gfx_gl_binder_unbind_texture(

		GLuint texture,
		GFX_CONT_ARG);*/

/**
 * Makes sure a sampler is unbound from any unit.
 *
 */
/*void _gfx_gl_binder_unbind_sampler(

		GLuint sampler,
		GFX_CONT_ARG);*/

/**
 * Sets the framebuffer handle associated with a pipeline as current for the current context.
 *
 */
/*void _gfx_gl_pipeline_bind(

		GLenum  target,
		GLuint  framebuffer,
		GFX_CONT_ARG);*/

/**
 * Sets the program pipeline or program handle as currently bound to the current context.
 *
 */
/*void _gfx_gl_program_map_bind(

		GLuint id,
		GFX_CONT_ARG);*/

/**
 * Sets the layout handle as currently bound to the current context.
 *
 */
/*void _gfx_gl_vertex_layout_bind(

		GLuint vao,
		GFX_CONT_ARG);*/


#endif // GFX_CORE_RENDERER_GL_H
