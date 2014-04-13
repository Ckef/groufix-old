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

/******************************************************/
GFXMesh* gfx_mesh_create(void)
{
	return (GFXMesh*)gfx_lod_map_create(sizeof(GFXSubMesh*));
}

/******************************************************/
void gfx_mesh_free(

		GFXMesh* mesh)
{
	if(mesh)
	{
		/* Iterate over all levels */
		size_t levels = mesh->lodMap.levels;

		while(levels)
		{
			/* Free all submeshes in it */
			size_t num;
			GFXSubMesh** subs = gfx_mesh_get(
				mesh,
				--levels,
				&num
			);

			while(num) _gfx_submesh_free(subs[--num]);
		}

		gfx_lod_map_free((GFXLodMap*)mesh);
	}
}

/******************************************************/
GFXSubMesh* gfx_mesh_add(

		GFXMesh*       mesh,
		size_t         level,
		unsigned char  drawCalls,
		unsigned char  sources)
{
	/* Create new submesh and add it to the LOD map */
	GFXSubMesh* sub = _gfx_submesh_create(drawCalls, sources);
	if(!sub) return NULL;

	if(!gfx_lod_map_add((GFXLodMap*)mesh, level, &sub))
	{
		_gfx_submesh_free(sub);
		return NULL;
	}

	return sub;
}

/******************************************************/
int gfx_mesh_add_share(

		GFXMesh*     mesh,
		size_t       level,
		GFXSubMesh*  share)
{
	/* Check if it's already mapped to avoid another reference */
	if(gfx_lod_map_has((GFXLodMap*)mesh, level, &share))
		return 1;

	/* Reference the submesh */
	if(!_gfx_submesh_reference(share))
		return 0;

	/* Add it to the LOD map */
	if(!gfx_lod_map_add((GFXLodMap*)mesh, level, &share))
	{
		_gfx_submesh_free(share);
		return 0;
	}

	return 1;
}

/******************************************************/
void gfx_mesh_remove(

		GFXMesh*     mesh,
		GFXSubMesh*  sub)
{
	/* Remove it from all levels */
	size_t levels = mesh->lodMap.levels;

	while(levels)
	{
		if(gfx_lod_map_remove((GFXLodMap*)mesh, --levels, &sub))
			_gfx_submesh_free(sub);
	}
}
