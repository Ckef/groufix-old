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

#include "groufix/scene/internal.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
static int _gfx_mesh_alloc_submesh(GFXMesh* mesh)
{
	/* Allocate the pointer */
	GFXSubMesh** ptr = realloc(mesh->subMeshes, sizeof(GFXSubMesh*) * (mesh->num + 1));
	if(!ptr) return 0;

	mesh->subMeshes = ptr;
	++mesh->num;

	return 1;
}

/******************************************************/
static inline void _gfx_mesh_free_submesh(GFXMesh* mesh)
{
	mesh->subMeshes = realloc(mesh->subMeshes, sizeof(GFXSubMesh*) * (--mesh->num));
}

/******************************************************/
static void _gfx_mesh_remove_submesh(GFXMesh* mesh, size_t index)
{
	/* Move memory and free the last submesh */
	memmove(
		mesh->subMeshes + index,
		mesh->subMeshes + index + 1,
		sizeof(GFXSubMesh*) * (mesh->num - index - 1));

	_gfx_mesh_free_submesh(mesh);
}

/******************************************************/
void gfx_mesh_init(GFXMesh* mesh)
{
	memset(mesh, 0, sizeof(GFXMesh));
}

/******************************************************/
void gfx_mesh_init_copy(GFXMesh* mesh, GFXMesh* src)
{
	gfx_mesh_init(mesh);

	/* Share all the submeshes */
	size_t s;
	for(s = 0; s < src->num; ++s)
		gfx_mesh_push_share(mesh, src->subMeshes[s]);
}

/******************************************************/
void gfx_mesh_clear(GFXMesh* mesh)
{
	/* Free all submeshes */
	size_t s;
	for(s = 0; s < mesh->num; ++s)
		_gfx_submesh_free(mesh->subMeshes[s]);

	free(mesh->subMeshes);
	gfx_mesh_init(mesh);
}

/******************************************************/
GFXSubMesh* gfx_mesh_push(GFXMesh* mesh, unsigned char drawCalls, unsigned char sources)
{
	if(!_gfx_mesh_alloc_submesh(mesh)) return NULL;

	/* Create new submesh */
	GFXSubMesh* sub = _gfx_submesh_create(drawCalls, sources);
	if(!sub)
	{
		_gfx_mesh_free_submesh(mesh);
		return NULL;
	}
	mesh->subMeshes[mesh->num - 1] = sub;

	return sub;
}

/******************************************************/
int gfx_mesh_push_share(GFXMesh* mesh, GFXSubMesh* share)
{
	if(!_gfx_mesh_alloc_submesh(mesh)) return 0;

	/* Reference the submesh */
	if(!_gfx_submesh_reference(share))
	{
		_gfx_mesh_free_submesh(mesh);
		return 0;
	}
	mesh->subMeshes[mesh->num - 1] = share;

	return 1;
}

/******************************************************/
void gfx_mesh_remove(GFXMesh* mesh, size_t index)
{
	if(index < mesh->num)
	{
		/* Free the submesh and remove it */
		_gfx_submesh_free(mesh->subMeshes[index]);
		_gfx_mesh_remove_submesh(mesh, index);
	}
}
