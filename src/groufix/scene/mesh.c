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

/******************************************************/
/* Internal mesh */
struct GFX_Mesh
{
	GFX_LodMap  map;     /* Super class */
	GFXVector   batches; /* Stores GFX_Batch, index + 1 = mesh ID of batch */
};

/* Internal batch (material reference) */
struct GFX_Batch
{
	GFXMaterial*  material;  /* NULL when empty */
	size_t        materialID;
};

/* Internal submesh data */
struct GFX_SubData
{
	GFXSubMesh*    sub;      /* Super class */
	size_t         material; /* Property map index within material */
	GFXMeshSource  source;   /* Sources to use within sub */
};

/******************************************************/
static struct GFX_Batch* _gfx_mesh_find_material(

		struct GFX_Mesh*  mesh,
		GFXMaterial*      mat)
{
	/* Try to find the mesh */
	struct GFX_Batch* empty = mesh->batches.end;
	struct GFX_Batch* it;

	for(
		it = mesh->batches.begin;
		it != mesh->batches.end;
		it = gfx_vector_next(&mesh->batches, it))
	{
		if(it->material == mat)
			return it;

		else if(!it->material && empty == mesh->batches.end)
			empty = it;
	}

	/* Construct new ID */
	struct GFX_Batch new;
	new.material = mat;
	new.materialID = 0;

	/* Replace an empty ID */
	if(empty != mesh->batches.end)
	{
		*empty = new;
		return empty;
	}

	/* Insert new ID */
	return gfx_vector_insert(
		&mesh->batches,
		&new,
		empty
	);
}

/******************************************************/
size_t _gfx_mesh_get_batch(

		GFXMesh*      mesh,
		GFXMaterial*  material)
{
	struct GFX_Mesh* internal = (struct GFX_Mesh*)mesh;
	struct GFX_Batch* batch = _gfx_mesh_find_material(internal, material);

	/* Return correct ID */
	return (batch == internal->batches.end) ? 0 :
		gfx_vector_get_index(&internal->batches, batch) + 1;
}

/******************************************************/
void _gfx_mesh_set_batch(

		GFXMesh*  mesh,
		size_t    meshID,
		size_t    materialID)
{
	/* Bound check */
	struct GFX_Mesh* internal = (struct GFX_Mesh*)mesh;
	size_t max = gfx_vector_get_size(&internal->batches);

	if(meshID && meshID <= max)
	{
		/* Get batch and set */
		struct GFX_Batch* batch = gfx_vector_at(
			&internal->batches,
			meshID - 1
		);

		batch->materialID = materialID;
	}
}

/******************************************************/
void _gfx_mesh_remove_batch(

		GFXMesh*  mesh,
		size_t    meshID)
{
	/* Bound check */
	struct GFX_Mesh* internal = (struct GFX_Mesh*)mesh;
	size_t size = gfx_vector_get_size(&internal->batches);

	if(meshID && meshID <= size)
	{
		/* Get batch and mark as empty */
		struct GFX_Batch* batch = gfx_vector_at(
			&internal->batches,
			meshID - 1
		);

		batch->material = NULL;

		/* Remove trailing empty batches */
		size_t num;
		struct GFX_Batch* beg = internal->batches.end;

		for(num = 0; num < size; ++num)
		{
			struct GFX_Batch* prev = gfx_vector_previous(&internal->batches, beg);
			if(prev->material) break;

			beg = prev;
		}
		gfx_vector_erase_range(&internal->batches, num, beg);
	}
}

/******************************************************/
GFXMesh* gfx_mesh_create(void)
{
	/* Allocate mesh */
	struct GFX_Mesh* mesh = malloc(sizeof(struct GFX_Mesh));
	if(!mesh) return NULL;

	/* Initialize */
	_gfx_lod_map_init(
		(GFX_LodMap*)mesh,
		0,
		sizeof(struct GFX_SubData),
		sizeof(GFXSubMesh*)
	);

	gfx_vector_init(&mesh->batches, sizeof(struct GFX_Batch));

	return (GFXMesh*)mesh;
}

/******************************************************/
void gfx_mesh_free(

		GFXMesh* mesh)
{
	if(mesh)
	{
		struct GFX_Mesh* internal = (struct GFX_Mesh*)mesh;

		/* Remove all batches at the materials */
		struct GFX_Batch* it;
		for(
			it = internal->batches.begin;
			it != internal->batches.end;
			it = gfx_vector_next(&internal->batches, it))
		{
			if(it->material) _gfx_material_remove_batch(
				it->material,
				it->materialID
			);
		}

		/* Free all submeshes */
		size_t num;
		struct GFX_SubData* subs = gfx_lod_map_get_all(
			(GFXLodMap*)mesh,
			&num
		);

		while(num--) _gfx_submesh_free(subs[num].sub);

		/* Clear and free */
		gfx_vector_clear(&internal->batches);
		_gfx_lod_map_clear((GFX_LodMap*)internal);

		free(mesh);
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
	/* Reference the submesh */
	if(!_gfx_submesh_reference(share)) return 0;

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
size_t gfx_mesh_set_material(

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

	size_t count = 0;

	while(num--) if(data[num].sub == sub)
	{
		/* Set material if found */
		data[num].material = material;
		++count;
	}

	return count;
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
size_t gfx_mesh_set_source(

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

	size_t count = 0;

	while(num--) if(data[num].sub == sub)
	{
		/* Set source if found */
		data[num].source = source;
		++count;
	}

	return count;
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
