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

#ifndef GFX_PIPELINE_INTERNAL_H
#define GFX_PIPELINE_INTERNAL_H

#include "groufix/internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * OpenGL State management
 *******************************************************/

/**
 * Sets the state of a context.
 *
 * Note: this assumes the context is current.
 *
 */
void _gfx_states_set(GFXPipeState state, GFX_Extensions* ext);

/**
 * Forces all state fields of a context.
 *
 * This will reset all state fields, regardless of previous states.
 * Note: this assumes the context is current.
 *
 */
void _gfx_states_force_set(GFXPipeState state, GFX_Extensions* ext);


/********************************************************
 * Internal bucket creation & processing
 *******************************************************/

/**
 * Creates a new bucket.
 *
 * @param bits Number of manual bits to consider when sorting (LSB = 1st bit, 0 for all bits).
 * @return NULL on failure.
 *
 */
GFXBucket* _gfx_bucket_create(unsigned char bits, GFXBucketFlags flags);

/**
 * Makes sure the bucket is freed properly.
 *
 */
void _gfx_bucket_free(GFXBucket* bucket);

/**
 * Processes the bucket, calling all batch processes.
 *
 */
void _gfx_bucket_process(GFXBucket* bucket, const GFX_Extensions* ext);


/********************************************************
 * Internal draw calls and program usage
 *******************************************************/

/**
 * Sets the program handle as currently in use.
 *
 */
void _gfx_program_use(GLuint handle, const GFX_Extensions* ext);

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


#ifdef __cplusplus
}
#endif

#endif // GFX_PIPELINE_INTERNAL_H
