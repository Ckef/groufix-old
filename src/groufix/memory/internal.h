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

#ifndef GFX_MEMORY_INTERNAL_H
#define GFX_MEMORY_INTERNAL_H

#include "groufix/internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Window dependent vertex layouts
 *******************************************************/

/**
 * Creates the vertex layout for the current window, if necessary.
 *
 */
int _gfx_vertex_layout_window_create(void);

/**
 * Draws the window vertex layout.
 *
 */
void _gfx_vertex_layout_window_draw(void);


/********************************************************
 * Internal draw calls
 *******************************************************/

/**
 * Regular drawing using a given vertex layout.
 *
 */
void _gfx_vertex_layout_draw(const GFXVertexLayout* layout, unsigned char startIndex, unsigned char num);

/**
 * Indexed drawing using a given vertex layout.
 *
 */
void _gfx_vertex_layout_draw_indexed(const GFXVertexLayout* layout, unsigned char startIndex, unsigned char num);

/**
 * Instanced drawing using a given vertex layout.
 *
 */
void _gfx_vertex_layout_draw_instanced(const GFXVertexLayout* layout, unsigned char startIndex, unsigned char num, size_t inst);

/**
 * Indexed AND instanced drawing using a given vertex layout.
 *
 */
void _gfx_vertex_layout_draw_indexed_instanced(const GFXVertexLayout* layout, unsigned char startIndex, unsigned char num, size_t inst);


/********************************************************
 * Internal program usage
 *******************************************************/

/**
 * Sets the program handle as currently in use.
 *
 * Note: this is not context-safe, use _gfx_program_force_reuse to make sure it works cross-context.
 *
 */
void _gfx_program_use(GLuint handle, const GFX_Extensions* ext);

/**
 * Forces the next program to be bound no matter what.
 *
 * This method should be called when binding a program to a non-main context.
 *
 */
void _gfx_program_force_reuse(void);

/**
 * Forces to set the program handle as currently in use.
 *
 * This acts as if _gfx_program_force_reuse is called before using.
 *
 */
void _gfx_program_force_use(GLuint handle, const GFX_Extensions* ext);


/********************************************************
 * Datatype helpers
 *******************************************************/

/**
 * Returns 1 if packed data, 0 if unpacked.
 *
 */
int _gfx_is_data_type_packed(GFXDataType type);

/**
 * Returns the size of a data type in bytes.
 *
 */
unsigned char _gfx_sizeof_data_type(GFXDataType type);


/********************************************************
 * Format retrieval (of textures) & converters
 *******************************************************/

/**
 * Returns the internal format of a texture.
 *
 */
GLint _gfx_texture_get_internal_format(const GFXTexture* texture);

/**
 * Returns the internal target of a texture (a.k.a type).
 *
 */
GLenum _gfx_texture_get_internal_target(const GFXTexture* texture);

/**
 * Converts a texture format to a client pixel format.
 *
 * @return Negative on failure.
 *
 */
GLint _gfx_texture_format_to_pixel_format(GFXTextureFormat format);

/**
 * Converts a texture format to an internal format.
 *
 * @return Negative on failure.
 *
 */
GLint _gfx_texture_format_to_internal(GFXTextureFormat format);

/**
 * Converts an internal format to a texture format.
 *
 */
GFXTextureFormat _gfx_texture_format_from_internal(GLint format);


#ifdef __cplusplus
}
#endif

#endif // GFX_MEMORY_INTERNAL_H
