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

#ifndef GFX_CORE_SHADING_INTERNAL_H
#define GFX_CORE_SHADING_INTERNAL_H

#include "groufix/core/renderer.h"

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Program management
 *******************************************************/

/**
 * Creates a new program.
 *
 * @param instances Number of instances that can be drawn in a single draw call, 0 for infinite.
 * @return NULL on failure.
 *
 */
GFXProgram* _gfx_program_create(

		size_t instances);

/**
 * References a program to postpone its destruction.
 *
 * @param references Number of times to reference it.
 * @return Zero on overflow.
 *
 */
int _gfx_program_reference(

		GFXProgram*   program,
		unsigned int  references);

/**
 * Makes sure the program is freed properly.
 *
 * Decrease the reference counter before freeing,
 * only freeing if the counter hits 0.
 *
 */
void _gfx_program_free(

		GFXProgram* program);

/**
 * Get the location of a property (a.k.a uniform).
 *
 * @return Negative on failure, the location otherwise.
 *
 */
GLint _gfx_program_get_location(

		GFXProgram*     program,
		unsigned short  index);


/********************************************************
 * Internal program map & property map usage
 *******************************************************/

/**
 * Blocks the program map from adding anymore programs.
 *
 * @return Zero on failure.
 *
 * This so it can link the programs.
 *
 */
int _gfx_program_map_block(

		GFXProgramMap* map);

/**
 * Unblocks the program map from adding anymore programs.
 *
 * Must be called equally many times as _gfx_program_map_block has been called.
 *
 */
void _gfx_program_map_unblock(

		GFXProgramMap* map);

/**
 * Acts as if _gfx_render_objects_save was called for only the program map.
 *
 */
void _gfx_program_map_save(

		GFXProgramMap*      map,
		GFX_RenderObjects*  cont);

/**
 * Acts as if _gfx_render_objects_restore was called for only the program map.
 *
 * The source render object container is implied from _gfx_program_map_save.
 *
 */
void _gfx_program_map_restore(

		GFXProgramMap*      map,
		GFX_RenderObjects*  cont);

/**
 * Sets the program pipeline as currently in use.
 *
 */
void _gfx_program_map_use(

		GFXProgramMap* map,
		GFX_WIND_ARG);

/**
 * Calls _gfx_program_map_use and uploads appropriate data.
 *
 * @param copy Index of the copy of the map to use.
 * @param base Base instance to use.
 *
 */
void _gfx_property_map_use(

		GFXPropertyMap*  map,
		unsigned int     copy,
		unsigned int     base,
		GFX_WIND_ARG);


/********************************************************
 * Internal unit binding
 *******************************************************/

/**
 * Binds a buffer to the appropriate uniform buffer index.
 *
 * @param prioritize Non-zero signifies this buffer must stay bound as long as possible.
 * @param old        If the uniform buffer was already bound to the returned value, old will be non-zero.
 * @return the uniform buffer index it was bound to.
 *
 */
size_t _gfx_binder_bind_uniform_buffer(

		GLuint      buffer,
		GLintptr    offset,
		GLsizeiptr  size,
		int         prioritize,
		int*        old,
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
 * @param old        If the texture was already bound to the returned value, old will be non-zero.
 * @return the texture unit it was bound to.
 *
 */
size_t _gfx_binder_bind_texture(

		GLuint  texture,
		GLenum  target,
		int     prioritize,
		int*    old,
		GFX_WIND_ARG);

/**
 * Makes sure a texture is unbound from any unit.
 *
 */
void _gfx_binder_unbind_texture(

		GLuint texture,
		GFX_WIND_ARG);


#ifdef __cplusplus
}
#endif

#endif // GFX_CORE_SHADING_INTERNAL_H
