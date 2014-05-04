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
/* Internal submesh data */
struct GFX_SubData
{
	GFXSubMesh*    sub;      /* Super class */
	size_t         material; /* Property map index within material */
	GFXMeshSource  source;   /* Sources to use within sub */
};

/******************************************************/
GFXMesh* gfx_mesh_create(void)
{
	return (GFXMesh*)gfx_lod_map_create(
		0,
		sizeof(struct GFX_SubData),
		sizeof(GFXSubMesh*)
	);
}

/******************************************************/
void gfx_mesh_free(

		GFXMesh* mesh)
{
	if(mesh)
	{
		/* Free all submeshes */
		size_t num;
		struct GFX_SubData* subs = gfx_lod_map_get_all(
			(GFXLodMap*)mesh,
			&num
		);

		while(num--) _gfx_submesh_free(subs[num].sub);

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
	/* Create new submesh */
	struct GFX_SubData data;
	data.material           = 0;
	data.source.startSource = 0;
	data.source.numSource   = sources;

	data.sub = _gfx_submesh_create(drawCalls, sources);
	if(!data.sub) return NULL;

	/* Add it to the LOD map */
	if(!gfx_lod_map_add((GFXLodMap*)mesh, level, &data))
	{
		_gfx_submesh_free(data.sub);
		return NULL;
	}

	return data.sub;
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
	struct GFX_SubData data;
	data.sub                = share;
	data.material           = 0;
	data.source.startSource = 0;
	data.source.numSource   = share->sources;

	if(!gfx_lod_map_add((GFXLodMap*)mesh, level, &data))
	{
		_gfx_submesh_free(share);
		return 0;
	}

	return 1;
}

/******************************************************/
int gfx_mesh_set_material(

		GFXMesh*     mesh,
		size_t       level,
		GFXSubMesh*  sub,
		size_t       material)
{
	/* First find the submesh */
	size_t num;
	struct GFX_SubData* data = gfx_lod_map_get(
		(GFXLodMap*)mesh,
		level,
		&num
	);

	while(num--) if(data[num].sub == sub)
	{
		/* Set material if found */
		data[num].material = material;
		return 1;
	}

	return 0;
}

/******************************************************/
int gfx_mesh_set_material_at(

		GFXMesh*  mesh,
		size_t    level,
		size_t    index,
		size_t    material)
{
	/* First get the submesh */
	size_t num;
	struct GFX_SubData* data = gfx_lod_map_get(
		(GFXLodMap*)mesh,
		level,
		&num
	);

	if(index >= num) return 0;

	/* Set the material */
	data[index].material = material;

	return 1;
}

/******************************************************/
int gfx_mesh_set_source(

		GFXMesh*       mesh,
		size_t         level,
		GFXSubMesh*    sub,
		GFXMeshSource  source)
{
	/* First find the submesh */
	size_t num;
	struct GFX_SubData* data = gfx_lod_map_get(
		(GFXLodMap*)mesh,
		level,
		&num
	);

	while(num--) if(data[num].sub == sub)
	{
		/* Set source if found */
		data[num].source = source;
		return 1;
	}

	return 0;
}

/******************************************************/
int gfx_mesh_set_source_at(

		GFXMesh*       mesh,
		size_t         level,
		size_t         index,
		GFXMeshSource  source)
{
	/* First get the submesh */
	size_t num;
	struct GFX_SubData* data = gfx_lod_map_get(
		(GFXLodMap*)mesh,
		level,
		&num
	);

	if(index >= num) return 0;

	/* Set the source */
	data[index].source = source;

	return 1;
}

/******************************************************/
GFXSubMeshList gfx_mesh_get(

		GFXMesh*  mesh,
		size_t    level,
		size_t*   num)
{
	return gfx_lod_map_get((GFXLodMap*)mesh, level, num);
}

/******************************************************/
GFXSubMeshList gfx_mesh_get_all(

		GFXMesh*  mesh,
		size_t*   num)
{
	return gfx_lod_map_get_all((GFXLodMap*)mesh, num);
}

/******************************************************/
size_t gfx_submesh_list_material_at(

		GFXSubMeshList  list,
		size_t          index)
{
	return ((struct GFX_SubData*)list)[index].material;
}

/******************************************************/
GFXMeshSource gfx_submesh_list_source_at(

		GFXSubMeshList  list,
		size_t          index)
{
	return ((struct GFX_SubData*)list)[index].source;
}

/******************************************************/
GFXSubMesh* gfx_submesh_list_at(

		GFXSubMeshList  list,
		size_t          index)
{
	return ((struct GFX_SubData*)list)[index].sub;
}
