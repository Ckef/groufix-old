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

#ifndef GFX_SCENE_INTERNAL_H
#define GFX_SCENE_INTERNAL_H

#include "groufix/scene/mesh.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * SubMesh management
 *******************************************************/

/**
 * Creates a new submesh.
 *
 * @param drawCalls Fixed number of draw calls associated with the layout.
 * @param sources   Fixed number of sources to create.
 * @return NULL on failure.
 *
 */
GFXSubMesh* _gfx_submesh_create(

		unsigned char  drawCalls,
		unsigned char  sources);

/**
 * References a submesh to postpone its destruction.
 *
 * @return Zero on overflow.
 *
 */
int _gfx_submesh_reference(

		GFXSubMesh* mesh);

/**
 * Makes sure the submesh is freed properly.
 *
 */
void _gfx_submesh_free(

		GFXSubMesh* mesh);

/**
 * Adds a submesh to a bucket pipe.
 *
 */
void _gfx_submesh_add_to_bucket(

		GFXSubMesh*  mesh,
		GFXPipe*     pipe);

/**
 * Removes a submesh from a bucket pipe.
 *
 */
void _gfx_submesh_remove_from_bucket(

		GFXSubMesh*  mesh,
		GFXPipe*     pipe);

/**
 * Retrieves the bucket source ID from a submesh.
 *
 * @param index Index of the submesh source (must be < mesh->sources).
 * @return The ID of the source, 0 on failure.
 *
 */
size_t _gfx_submesh_get_bucket_source(

		GFXSubMesh*    mesh,
		GFXPipe*       pipe,
		unsigned char  index);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_INTERNAL_H
