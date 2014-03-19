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

#ifndef GFX_CORE_SHADING_INTERNAL_H
#define GFX_CORE_SHADING_INTERNAL_H

#include "groufix/core/internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Internal program & property map usage
 *******************************************************/

/**
 * Sets the program handle as currently in use for the given context.
 *
 */
void _gfx_property_map_use(GFXPropertyMap* map, GFX_Extensions* ext);

/**
 * Get the location of a property (a.k.a uniform).
 *
 * @return Negative on failure, the location otherwise.
 *
 */
GLint _gfx_program_get_location(GFXProgram* program, unsigned short index);

/**
 * Signals a program it is going to be used with a property map.
 *
 * @return Zero if the property map is already in use with this program.
 *
 * Note: this should only be called from within the property map.
 *
 */
int _gfx_program_target(GFXProgram* program, GFXPropertyMap* map);

/**
 * Makes sure the program is not targeted by the given property map.
 *
 * Note: this should only be called from within the property map.
 *
 */
void _gfx_program_untarget(GFXProgram* program, GFXPropertyMap* map);


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
size_t _gfx_binder_bind_uniform_buffer(GLuint buffer, GLintptr offset, GLsizeiptr size, int prioritize, int* old, GFX_Extensions* ext);

/**
 * Makes sure a buffer is unbound from any uniform buffer index.
 *
 */
void _gfx_binder_unbind_uniform_buffer(GLuint buffer, GFX_Extensions* ext);

/**
 * Binds a texture to the appropriate unit.
 *
 * @param target     Internal target of the texture.
 * @param prioritize Non-zero signifies this texture must stay bound as long as possible.
 * @param old        If the texture was already bound to the returned value, old will be non-zero.
 * @return the texture unit it was bound to.
 *
 */
size_t _gfx_binder_bind_texture(GLuint texture, GLenum target, int prioritize, int* old, GFX_Extensions* ext);

/**
 * Makes sure a texture is unbound from any unit.
 *
 */
void _gfx_binder_unbind_texture(GLuint texture, GFX_Extensions* ext);


#ifdef __cplusplus
}
#endif

#endif // GFX_CORE_SHADING_INTERNAL_H
