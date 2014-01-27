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

#ifndef GFX_MEMORY_INTERNAL_H
#define GFX_MEMORY_INTERNAL_H

#include "groufix/internal.h"

/* Compatibility defines */
#ifndef GL_TEXTURE_1D
	#define GL_TEXTURE_1D                    0x0de0
#endif
#ifndef GL_TEXTURE_1D_ARRAY
	#define GL_TEXTURE_1D_ARRAY              0x8c18
#endif
#ifndef GL_TEXTURE_2D_MULTISAMPLE
	#define GL_TEXTURE_2D_MULTISAMPLE        0x9100
#endif
#ifndef GL_TEXTURE_2D_MULTISAMPLE_ARRAY
	#define GL_TEXTURE_2D_MULTISAMPLE_ARRAY  0x9102
#endif
#ifndef GL_TEXTURE_BUFFER
	#define GL_TEXTURE_BUFFER                0x8c2a
#endif
#ifndef GL_TEXTURE_CUBE_MAP_ARRAY
	#define GL_TEXTURE_CUBE_MAP_ARRAY        0x9009
#endif

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Internal binding & draw calls
 *******************************************************/

/**
 * Sets the layout handle as currently bound for the given context.
 *
 */
void _gfx_layout_bind(GLuint handle, GFX_Extensions* ext);

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
