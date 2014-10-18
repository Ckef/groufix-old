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
 * Internal program & property map usage
 *******************************************************/

/**
 * Sets the program handle as currently in use for the current context.
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
