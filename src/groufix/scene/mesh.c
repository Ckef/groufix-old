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

#include "groufix/scene.h"
#include "groufix/scene/internal.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
/* Internal mesh */
struct GFX_Mesh
{
	/* Super class */
	GFX_LodMap map;

	/* Hidden data */
	GFXVector  batches; /* Stores GFX_Batch, index + 1 = mesh ID of batch */
	GFXVector  buckets; /* Stores GFX_Bucket */
};

/* Internal batch (material reference) */
struct GFX_Batch
{
	GFXMaterial*  material;  /* NULL when empty */
	size_t        materialID;
	GFXBatchLod   params;    /* Level of detail parameters */
	size_t        upper;     /* Upper bound in buckets vector */
};

/* Internal bucket reference */
struct GFX_Bucket
{
	GFXPipe*  pipe;
	size_t    unitsID; /* ID of unit group at material */
};

/* Internal submesh data */
struct GFX_SubData
{
	GFXSubMesh*    sub;      /* Super class */
	size_t         material; /* Property map index within material */
	GFXMeshSource  source;   /* Sources to use within sub */
};

/******************************************************/
static void _gfx_mesh_increase_bucket_bounds(

		struct GFX_Mesh*   mesh,
		struct GFX_Batch*  batch,
		long int           diff)
{
	while(batch != mesh->batches.end)
	{
		batch->upper += diff;
		batch = gfx_vector_next(&mesh->batches, batch);
	}
}

/******************************************************/
static void _gfx_mesh_get_bucket_bounds(

		struct GFX_Mesh*   mesh,
		struct GFX_Batch*  batch,
		size_t*            begin,
		size_t*            end)
{
	*end = batch->upper;

	if(batch != mesh->batches.begin)
	{
		batch = gfx_vector_previous(&mesh->batches, batch);
		*begin = batch->upper;
	}
	else *begin = 0;
}

/******************************************************/
static size_t _gfx_mesh_find_bucket(

		struct GFX_Mesh*  mesh,
		size_t            begin,
		size_t            end,
		GFXPipe*          pipe)
{
	while(begin < end)
	{
		struct GFX_Bucket* it = gfx_vector_at(&mesh->buckets, begin);
		if(it->pipe == pipe) break;

		++begin;
	}

	return begin;
}

/******************************************************/
static void _gfx_mesh_callback(

		GFXPipe*          pipe,
		GFXPipeCallback*  callback)
{
	struct GFX_Mesh* mesh = callback->data;

	/* Iterate through all batches */
	struct GFX_Batch* it;
	for(
		it = mesh->batches.begin;
		it != mesh->batches.end;
		it = gfx_vector_next(&mesh->batches, it))
	{
		size_t begin;
		size_t end;
		_gfx_mesh_get_bucket_bounds(mesh, it, &begin, &end);

		/* Iterate through buckets and erase them */
		/* Don't worry about submeshes, as they are registered at the bucket as well */
		while(begin < end)
		{
			struct GFX_Bucket* bucket = gfx_vector_at(
				&mesh->buckets, begin);

			if(bucket->pipe == pipe)
			{
				/* Also destory the units at the material */
				_gfx_material_remove_units(it->material, bucket->unitsID);
				gfx_vector_erase(&mesh->buckets, bucket);
				_gfx_mesh_increase_bucket_bounds(mesh, it, -1);
			}
			else ++begin;
		}
	}
}

/******************************************************/
static int _gfx_mesh_reference_bucket(

		struct GFX_Mesh*  mesh,
		GFXPipe*          pipe,
		size_t            level,
		size_t            index)
{
	/* Get submesh */
	size_t num;
	struct GFX_SubData* data = gfx_lod_map_get(
		(GFXLodMap*)mesh,
		level,
		&num
	);

	if(index < num)
	{
		/* Reference the bucket at the submesh */
		if(!_gfx_submesh_reference_bucket(data[index].sub, pipe))
			return 0;
	}

	/* Register the mesh at the pipe */
	GFXPipeCallback call;
	call.key = GFX_SCENE_KEY_MESH;
	call.data = mesh;

	if(gfx_pipe_exists(pipe, call))
		return 1;
	if(gfx_pipe_register(pipe, call, _gfx_mesh_callback))
		return 1;

	/* Well nevermind then */
	if(index < num)
		_gfx_submesh_dereference_bucket(data[index].sub, pipe);

	return 0;
}

/******************************************************/
static void _gfx_mesh_dereference_bucket(

		struct GFX_Mesh*  mesh,
		GFXPipe*          pipe,
		size_t            level,
		size_t            index)
{
	/* Get submesh */
	size_t num;
	struct GFX_SubData* data = gfx_lod_map_get(
		(GFXLodMap*)mesh,
		level,
		&num
	);

	/* Dereference the bucket at the submesh */
	if(index < num)
		_gfx_submesh_dereference_bucket(data[index].sub, pipe);

	/* Unregister at the pipe if it is not present at any batch */
	size_t max = gfx_vector_get_size(&mesh->buckets);
	size_t ind = _gfx_mesh_find_bucket(mesh, 0, max, pipe);

	if(ind >= max)
	{
		GFXPipeCallback call;
		call.key = GFX_SCENE_KEY_MESH;
		call.data = mesh;

		gfx_pipe_unregister(pipe, call);
	}
}

/******************************************************/
static int _gfx_mesh_reference_submesh(

		struct GFX_Mesh*  mesh,
		GFXSubMesh*       sub,
		size_t            level,
		size_t            index)
{
	/* Iterate through all batches */
	struct GFX_Batch* it;
	for(
		it = mesh->batches.begin;
		it != mesh->batches.end;
		it = gfx_vector_next(&mesh->batches, it))
	{
		if(it->params.mesh == level && it->params.index == index)
		{
			size_t begin;
			size_t end;
			_gfx_mesh_get_bucket_bounds(mesh, it, &begin, &end);

			/* Iterate through buckets and reference them */
			size_t i;
			for(i = begin; i < end; ++i)
			{
				struct GFX_Bucket* bucket = gfx_vector_at(
					&mesh->buckets, i);

				if(!_gfx_submesh_reference_bucket(sub, bucket->pipe))
					break;
			}

			/* Undo the current batch */
			if(i < end)
			{
				while(i > begin)
				{
					struct GFX_Bucket* bucket = gfx_vector_at(
						&mesh->buckets, --i);

					_gfx_submesh_dereference_bucket(sub, bucket->pipe);
				}

				break;
			}
		}
	}

	/* Undo all other buckets */
	if(it != mesh->batches.end)
	{
		while(it != mesh->batches.begin)
		{
			/* Iterate through buckets and dereference */
			it = gfx_vector_previous(&mesh->batches, it);

			size_t begin;
			size_t end;
			_gfx_mesh_get_bucket_bounds(mesh, it, &begin, &end);

			while(begin < end)
			{
				struct GFX_Bucket* bucket = gfx_vector_at(
					&mesh->buckets, begin++);

				_gfx_submesh_dereference_bucket(sub, bucket->pipe);
			}
		}

		return 0;
	}

	return 1;
}

/******************************************************/
static void _gfx_mesh_dereference_submesh(

		struct GFX_Mesh*  mesh,
		GFXSubMesh*       sub,
		size_t            level,
		size_t            index)
{
	/* Iterate through all batches */
	struct GFX_Batch* it;
	for(
		it = mesh->batches.begin;
		it != mesh->batches.end;
		it = gfx_vector_next(&mesh->batches, it))
	{
		if(it->params.mesh == level && it->params.index == index)
		{
			size_t begin;
			size_t end;
			_gfx_mesh_get_bucket_bounds(mesh, it, &begin, &end);

			/* Iterate through buckets and dereference them */
			while(begin < end)
			{
				struct GFX_Bucket* bucket = gfx_vector_at(
					&mesh->buckets, begin++);

				_gfx_submesh_dereference_bucket(sub, bucket->pipe);
			}
		}
	}
}

/******************************************************/
static struct GFX_Batch* _gfx_mesh_find_batch(

		struct GFX_Mesh*  mesh,
		GFXMaterial*      mat,
		GFXBatchLod       params)
{
	/* Try to find the material */
	struct GFX_Batch* empty = mesh->batches.end;
	struct GFX_Batch* it;

	for(
		it = mesh->batches.begin;
		it != mesh->batches.end;
		it = gfx_vector_next(&mesh->batches, it))
	{
		if(it->material == mat && !memcmp(&it->params, &params, sizeof(GFXBatchLod)))
			return it;

		else if(!it->material && empty == mesh->batches.end)
			empty = it;
	}

	/* Construct new ID */
	struct GFX_Batch new;
	new.material   = mat;
	new.materialID = 0;
	new.params     = params;

	/* Replace an empty ID */
	if(empty != mesh->batches.end)
	{
		new.upper = empty->upper;
		*empty = new;

		return empty;
	}

	/* Get upper bound and insert new ID */
	new.upper = gfx_vector_get_size(&mesh->buckets);

	return gfx_vector_insert(
		&mesh->batches,
		&new,
		empty
	);
}

/******************************************************/
size_t _gfx_mesh_get_batch(

		GFXMesh*      mesh,
		GFXMaterial*  material,
		GFXBatchLod   params,
		size_t*       materialID)
{
	struct GFX_Mesh* internal = (struct GFX_Mesh*)mesh;
	struct GFX_Batch* batch = _gfx_mesh_find_batch(internal, material, params);

	/* Return correct IDs */
	if(batch == internal->batches.end)
	{
		*materialID = 0;
		return 0;
	}

	*materialID = batch->materialID;
	return gfx_vector_get_index(&internal->batches, batch) + 1;
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
		/* Get batch and bounds, and fix them */
		struct GFX_Batch* batch = gfx_vector_at(
			&internal->batches,
			meshID - 1
		);

		size_t begin;
		size_t end;
		_gfx_mesh_get_bucket_bounds(internal, batch, &begin, &end);

		/* Iterate through all buckets */
		size_t i = end;
		while(i > begin)
		{
			struct GFX_Bucket* it = gfx_vector_at(&internal->buckets, --i);

			/* Destroy units */
			_gfx_material_remove_units(
				batch->material,
				it->unitsID);

			/* Dereference it */
			GFXPipe* pipe = it->pipe;
			it->pipe = NULL;

			_gfx_mesh_dereference_bucket(
				internal,
				pipe,
				batch->params.mesh,
				batch->params.index);
		}

		/* Remove all buckets and fix bounds */
		gfx_vector_erase_range_at(
			&internal->buckets,
			end - begin,
			begin);

		_gfx_mesh_increase_bucket_bounds(
			internal,
			batch,
			(long int)begin - (long int)end);

		/* Mark as empty and remove trailing empty batches */
		batch->material = NULL;

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
int _gfx_mesh_get_batch_lod(

		GFXMesh*      mesh,
		size_t        meshID,
		GFXBatchLod*  params)
{
	/* Bound check */
	struct GFX_Mesh* internal = (struct GFX_Mesh*)mesh;
	size_t max = gfx_vector_get_size(&internal->batches);

	if(!meshID || meshID > max) return 0;

	/* Get batch */
	struct GFX_Batch* batch = gfx_vector_at(
		&internal->batches,
		meshID - 1
	);

	*params = batch->params;
	return 1;
}

/******************************************************/
size_t _gfx_mesh_get_units(

		GFXMesh*  mesh,
		size_t    meshID,
		GFXPipe*  pipe)
{
	/* Validate pipe type & bound check */
	struct GFX_Mesh* internal = (struct GFX_Mesh*)mesh;
	size_t max = gfx_vector_get_size(&internal->batches);

	if(gfx_pipe_get_type(pipe) != GFX_PIPE_BUCKET ||
		!meshID ||
		meshID > max) return 0;

	/* Get batch and bounds */
	struct GFX_Batch* batch = gfx_vector_at(
		&internal->batches,
		meshID - 1
	);

	size_t begin;
	size_t end;
	_gfx_mesh_get_bucket_bounds(internal, batch, &begin, &end);

	/* Find bucket */
	size_t index = _gfx_mesh_find_bucket(internal, begin, end, pipe);
	if(index < end)
	{
		struct GFX_Bucket* it = gfx_vector_at(&internal->buckets, index);
		return it->unitsID;
	}

	/* Create new unit group */
	struct GFX_Bucket bucket;
	bucket.pipe = pipe;

	bucket.unitsID = _gfx_material_insert_units(
		batch->material,
		batch->materialID
	);

	if(bucket.unitsID)
	{
		/* Insert the bucket */
		struct GFX_Bucket* it = gfx_vector_insert_at(
			&internal->buckets,
			&bucket,
			end
		);

		if(it != internal->buckets.end)
		{
			if(_gfx_mesh_reference_bucket(
				internal,
				pipe,
				batch->params.mesh,
				batch->params.index))
			{
				/* Increase bounds at last */
				_gfx_mesh_increase_bucket_bounds(internal, batch, 1);
				return bucket.unitsID;
			}

			/* Failed, erase bucket */
			gfx_vector_erase(&internal->buckets, it);
		}

		/* Destroy unit group */
		_gfx_material_remove_units(batch->material, bucket.unitsID);
	}

	return 0;
}

/******************************************************/
void _gfx_mesh_remove_units(

		GFXMesh*  mesh,
		size_t    meshID,
		GFXPipe*  pipe)
{
	/* Bound check */
	struct GFX_Mesh* internal = (struct GFX_Mesh*)mesh;
	size_t max = gfx_vector_get_size(&internal->batches);

	if(meshID && meshID <= max)
	{
		/* Get batch and bounds */
		struct GFX_Batch* batch = gfx_vector_at(
			&internal->batches,
			meshID - 1
		);

		size_t begin;
		size_t end;
		_gfx_mesh_get_bucket_bounds(internal, batch, &begin, &end);

		/* Find bucket */
		size_t index = _gfx_mesh_find_bucket(internal, begin, end, pipe);
		if(index < end)
		{
			struct GFX_Bucket* it = gfx_vector_at(&internal->buckets, index);

			/* Destroy unit group & remove the bucket */
			_gfx_material_remove_units(batch->material, it->unitsID);
			gfx_vector_erase(&internal->buckets, it);
			_gfx_mesh_increase_bucket_bounds(internal, batch, -1);

			/* Dereference bucket */
			_gfx_mesh_dereference_bucket(
				internal,
				pipe,
				batch->params.mesh,
				batch->params.index
			);
		}
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
	gfx_vector_init(&mesh->buckets, sizeof(struct GFX_Bucket));

	return (GFXMesh*)mesh;
}

/******************************************************/
void gfx_mesh_free(

		GFXMesh* mesh)
{
	if(mesh)
	{
		struct GFX_Mesh* internal = (struct GFX_Mesh*)mesh;

		/* Remove all batches */
		size_t b;
		for(b = gfx_vector_get_size(&internal->batches); b > 0; --b)
		{
			struct GFX_Batch* it = gfx_vector_at(
				&internal->batches,
				b - 1);

			if(it->material) _gfx_material_remove_batch(
				it->material,
				it->materialID);

			_gfx_mesh_remove_batch(mesh, b);
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
		gfx_vector_clear(&internal->buckets);
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

	/* Reference all related buckets */
	size_t num;
	gfx_lod_map_get((GFXLodMap*)mesh, level, &num);

	if(!_gfx_mesh_reference_submesh(
		(struct GFX_Mesh*)mesh,
		data.sub,
		level,
		num))
	{
		_gfx_submesh_free(data.sub);
		return NULL;
	}

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

	/* Reference all related buckets */
	size_t num;
	gfx_lod_map_get((GFXLodMap*)mesh, level, &num);

	if(!_gfx_mesh_reference_submesh(
		(struct GFX_Mesh*)mesh,
		share,
		level,
		num))
	{
		_gfx_submesh_free(share);
		return 0;
	}

	/* Add it to the LOD map */
	struct GFX_SubData data;
	data.sub                = share;
	data.material           = 0;
	data.source.startSource = 0;
	data.source.numSource   = share->sources;

	if(!gfx_lod_map_add((GFXLodMap*)mesh, level, &data))
	{
		_gfx_mesh_dereference_submesh(
			(struct GFX_Mesh*)mesh,
			share,
			level,
			num
		);
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
