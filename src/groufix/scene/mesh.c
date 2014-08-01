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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define GFX_INT_INVALID_BUCKET_HANDLE  SIZE_MAX

/******************************************************/
/* Internal mesh */
struct GFX_Mesh
{
	/* Super class */
	GFX_LodMap map;

	/* Hidden data */
	GFXVector  batches; /* Stores GFX_Batch, index + 1 = mesh ID of batch */
	GFXVector  buckets; /* Stores GFX_Bucket + size_t * units */
};

/* Internal batch (material reference) */
struct GFX_Batch
{
	GFXMaterial*  material;   /* NULL when empty */
	size_t        materialID;
	GFXBatchLod   params;     /* Level of detail parameters */
	GFXBatchType  type;
	size_t        units;      /* Max number of units reserved at any bucket */
	size_t        upper;      /* Upper bound in buckets vector */
};

/* Internal bucket reference */
struct GFX_Bucket
{
	GFXPipe*  pipe;
	size_t    instances; /* Number of instances */
	size_t    visible;   /* Number of visible instances */
	size_t    units;     /* Number of stored units */
};

/* Internal submesh data */
struct GFX_SubData
{
	GFXSubMesh*    sub;      /* Super class */
	size_t         material; /* Property map index within material */
	unsigned char  source;   /* Source to use within submesh */
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
static size_t _gfx_mesh_find_pipe(

		struct GFX_Mesh*  mesh,
		size_t            begin,
		size_t            end,
		GFXPipe*          pipe)
{
	while(begin < end)
	{
		struct GFX_Bucket* it = gfx_vector_at(&mesh->buckets, begin);
		if(it->pipe == pipe) break;

		begin +=
			sizeof(struct GFX_Bucket) +
			sizeof(size_t) * it->units;
	}

	return begin;
}

/******************************************************/
static void _gfx_mesh_update_bucket_units(

		struct GFX_Mesh*   mesh,
		struct GFX_Batch*  batch)
{
	batch->units = 0;

	/* Get bounds */
	size_t begin;
	size_t end;
	_gfx_mesh_get_bucket_bounds(mesh, batch, &begin, &end);

	/* Iterate and increase */
	while(begin < end)
	{
		struct GFX_Bucket* it = gfx_vector_at(&mesh->buckets, begin);

		batch->units =
			(batch->units < it->units) ?
			it->units : batch->units;
		begin +=
			sizeof(struct GFX_Bucket) +
			sizeof(size_t) * it->units;
	}
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
		/* Find pipe within the batch and erase it */
		size_t begin;
		size_t end;

		_gfx_mesh_get_bucket_bounds(mesh, it, &begin, &end);
		size_t index = _gfx_mesh_find_pipe(mesh, begin, end, pipe);

		/* Don't worry about submeshes, as they are registered at the bucket as well */
		if(index < end)
		{
			struct GFX_Bucket* bucket = gfx_vector_at(
				&mesh->buckets, index);
			size_t size =
				sizeof(struct GFX_Bucket) +
				sizeof(size_t) * bucket->units;

			/* Also don't worry about destroying the units */
			/* The bucket destroys them anyway */
			gfx_vector_erase_range(&mesh->buckets, size, bucket);
			_gfx_mesh_increase_bucket_bounds(mesh, it, -(long int)size);
			_gfx_mesh_update_bucket_units(mesh, it);
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

	if(index >= num) return 0;

	/* Reference the bucket at the submesh */
	if(!_gfx_submesh_reference_bucket(data[index].sub, pipe))
		return 0;

	/* Register the mesh at the pipe */
	GFXPipeCallback call;
	call.key = GFX_SCENE_KEY_MESH;
	call.data = mesh;

	if(gfx_pipe_exists(pipe, call))
		return 1;
	if(gfx_pipe_register(pipe, call, _gfx_mesh_callback))
		return 1;

	/* Well nevermind then */
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
	/* Get submesh and dereference the bucket at it */
	size_t num;
	struct GFX_SubData* data = gfx_lod_map_get(
		(GFXLodMap*)mesh,
		level,
		&num
	);

	_gfx_submesh_dereference_bucket(data[index].sub, pipe);

	/* Unregister at the pipe if it is not present at any batch */
	size_t max = gfx_vector_get_size(&mesh->buckets);
	size_t ind = _gfx_mesh_find_pipe(mesh, 0, max, pipe);

	if(ind >= max)
	{
		GFXPipeCallback call;
		call.key = GFX_SCENE_KEY_MESH;
		call.data = mesh;

		gfx_pipe_unregister(pipe, call);
	}
}

/******************************************************/
static void _gfx_mesh_update_batch_maps(

		struct GFX_Mesh*  mesh,
		size_t            level,
		size_t            index)
{
	/* Iterate through all batches */
	size_t meshID = 1;
	struct GFX_Batch* batch;

	for(
		batch = mesh->batches.begin;
		batch != mesh->batches.end;
		batch = gfx_vector_next(&mesh->batches, batch), ++meshID)
	{
		if(batch->params.mesh == level && batch->params.index == index)
		{
			/* Tell the material to update the map for this batch */
			/* This will recalculate the map index */
			_gfx_material_set_batch(
				batch->material,
				batch->materialID,
				meshID
			);
		}
	}
}

/******************************************************/
static void _gfx_mesh_rebuild_batches(

		struct GFX_Mesh*  mesh,
		size_t            level,
		size_t            index)
{
	/* Iterate through all batches */
	size_t meshID = 1;
	struct GFX_Batch* batch;

	for(
		batch = mesh->batches.begin;
		batch != mesh->batches.end;
		batch = gfx_vector_next(&mesh->batches, batch), ++meshID)
	{
		if(batch->params.mesh == level && batch->params.index == index)
		{
			size_t begin;
			size_t end;
			_gfx_mesh_get_bucket_bounds(mesh, batch, &begin, &end);

			GFXBatch bat;
			bat.material   = batch->material;
			bat.materialID = batch->materialID;
			bat.mesh       = (GFXMesh*)mesh;
			bat.meshID     = meshID;

			/* Iterate through all buckets */
			while(begin < end)
			{
				struct GFX_Bucket* it =
					gfx_vector_at(&mesh->buckets, begin);
				size_t size =
					sizeof(struct GFX_Bucket) +
					sizeof(size_t) * it->units;

				/* Recreate, decrease end boundary */
				/* as the bucket is reinserted at the end */
				GFXPipe* pipe    = it->pipe;
				size_t instances = it->instances;
				size_t visible   = it->visible;

				gfx_batch_decrease(&bat, pipe, SIZE_MAX);
				gfx_batch_increase(&bat, pipe, instances);
				gfx_batch_set_visible(&bat, pipe, visible);

				end -= size;
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
	new.type       = GFX_BATCH_DEFAULT;
	new.units      = 0;

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
	struct GFX_Mesh* internal =
		(struct GFX_Mesh*)mesh;
	struct GFX_Batch* batch =
		_gfx_mesh_find_batch(internal, material, params);

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
		struct GFX_Batch* batch =
			gfx_vector_at(&internal->batches, meshID - 1);

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
		/* Get batch and bounds */
		struct GFX_Batch* batch =
			gfx_vector_at(&internal->batches, meshID - 1);

		size_t begin;
		size_t end;
		_gfx_mesh_get_bucket_bounds(
			internal,
			batch,
			&begin,
			&end
		);

		GFXBatch bat;
		bat.material   = batch->material;
		bat.materialID = batch->materialID;
		bat.mesh       = mesh;
		bat.meshID     = meshID;

		/* Iterate through all buckets */
		while(begin < end)
		{
			struct GFX_Bucket* it =
				gfx_vector_at(&internal->buckets, begin);
			size_t size =
				sizeof(struct GFX_Bucket) +
				sizeof(size_t) * it->units;

			/* Destroy units of the bucket */
			/* Which will in turn destroy the bucket handle */
			gfx_batch_decrease(&bat, it->pipe, SIZE_MAX);
			end -= size;
		}

		/* Mark as empty and remove trailing empty batches */
		/* Also set type to that of a non existent batch */
		batch->type = GFX_BATCH_DEFAULT;
		batch->material = NULL;

		size_t num;
		struct GFX_Batch* beg = internal->batches.end;

		for(num = 0; num < size; ++num)
		{
			struct GFX_Batch* prev =
				gfx_vector_previous(&internal->batches, beg);

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
	struct GFX_Batch* batch =
		gfx_vector_at(&internal->batches, meshID - 1);

	if(!batch->material) return 0;
	*params = batch->params;

	return 1;
}

/******************************************************/
GFXBatchType _gfx_mesh_get_batch_type(

		GFXMesh*  mesh,
		size_t    meshID)
{
	/* Bound check */
	struct GFX_Mesh* internal = (struct GFX_Mesh*)mesh;
	size_t max = gfx_vector_get_size(&internal->batches);

	if(!meshID || meshID > max) return GFX_BATCH_DEFAULT;

	/* Get batch */
	struct GFX_Batch* batch =
		gfx_vector_at(&internal->batches, meshID - 1);

	return batch->type;
}

/******************************************************/
void _gfx_mesh_set_batch_type(

		GFXMesh*      mesh,
		size_t        meshID,
		GFXBatchType  type)
{
	/* Bound check */
	struct GFX_Mesh* internal = (struct GFX_Mesh*)mesh;
	size_t max = gfx_vector_get_size(&internal->batches);

	if(meshID && meshID <= max)
	{
		/* Get batch and set */
		struct GFX_Batch* batch =
			gfx_vector_at(&internal->batches, meshID - 1);

		if(batch->material) batch->type = type;
	}
}

/******************************************************/
static size_t _gfx_mesh_search_bucket(

		struct GFX_Mesh*   mesh,
		struct GFX_Batch*  batch,
		GFXPipe*           pipe)
{
	if(!batch->material) return batch->upper;

	/* Get bound and find bucket */
	size_t begin;
	size_t end;

	_gfx_mesh_get_bucket_bounds(mesh, batch, &begin, &end);
	return _gfx_mesh_find_pipe(mesh, begin, end, pipe);
}

/******************************************************/
size_t _gfx_mesh_get_bucket(

		GFXMesh*  mesh,
		size_t    meshID,
		GFXPipe*  pipe)
{
	/* Bound check */
	struct GFX_Mesh* internal = (struct GFX_Mesh*)mesh;
	size_t max = gfx_vector_get_size(&internal->batches);

	if(!meshID || meshID > max)
		return GFX_INT_INVALID_BUCKET_HANDLE;

	/* Get batch and search */
	struct GFX_Batch* batch =
		gfx_vector_at(&internal->batches, meshID - 1);
	size_t bucket =
		_gfx_mesh_search_bucket(internal, batch, pipe);

	if(bucket < batch->upper)
		return bucket;

	/* Validate pipe type before creating a new bucket */
	if(gfx_pipe_get_type(pipe) != GFX_PIPE_BUCKET)
		return GFX_INT_INVALID_BUCKET_HANDLE;

	/* Create new bucket */
	/* Insert it at the end so we can rebuild */
	struct GFX_Bucket insert;
	insert.pipe      = pipe;
	insert.instances = 0;
	insert.visible   = 0;
	insert.units     = 0;

	struct GFX_Bucket* it = gfx_vector_insert_range_at(
		&internal->buckets,
		sizeof(struct GFX_Bucket),
		&insert,
		bucket
	);

	if(it != internal->buckets.end)
	{
		/* Reference the bucket */
		if(_gfx_mesh_reference_bucket(
			internal,
			pipe,
			batch->params.mesh,
			batch->params.index))
		{
			/* Increase bounds at last */
			_gfx_mesh_increase_bucket_bounds(
				internal, batch, sizeof(struct GFX_Bucket));

			return bucket;
		}

		/* Failed, erase bucket */
		gfx_vector_erase_range(
			&internal->buckets, sizeof(struct GFX_Bucket), it);
	}

	return GFX_INT_INVALID_BUCKET_HANDLE;
}

/******************************************************/
size_t _gfx_mesh_find_bucket(

		GFXMesh*  mesh,
		size_t    meshID,
		GFXPipe*  pipe)
{
	/* Bound check */
	struct GFX_Mesh* internal = (struct GFX_Mesh*)mesh;
	size_t max = gfx_vector_get_size(&internal->batches);

	if(!meshID || meshID > max)
		return GFX_INT_INVALID_BUCKET_HANDLE;

	/* Get batch and search for bucket */
	struct GFX_Batch* batch =
		gfx_vector_at(&internal->batches, meshID - 1);
	size_t bucket =
		_gfx_mesh_search_bucket(internal, batch, pipe);

	return (bucket < batch->upper) ? bucket :
		GFX_INT_INVALID_BUCKET_HANDLE;
}

/******************************************************/
void _gfx_mesh_remove_bucket(

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
		struct GFX_Batch* batch =
			gfx_vector_at(&internal->batches, meshID - 1);

		size_t begin;
		size_t end;
		_gfx_mesh_get_bucket_bounds(
			internal,
			batch,
			&begin,
			&end
		);

		/* Find bucket */
		size_t index =
			_gfx_mesh_find_pipe(internal, begin, end, pipe);

		if(index < end)
		{
			/* Erase it */
			struct GFX_Bucket* bucket = gfx_vector_at(
				&internal->buckets, index);
			size_t size =
				sizeof(struct GFX_Bucket) +
				sizeof(size_t) * bucket->units;

			/* Fix bounds, dereference bucket and update max units */
			gfx_vector_erase_range(
				&internal->buckets, size, bucket);
			_gfx_mesh_increase_bucket_bounds(
				internal, batch, -(long int)size);
			_gfx_mesh_update_bucket_units(
				internal, batch);

			_gfx_mesh_dereference_bucket(
				internal,
				pipe,
				batch->params.mesh,
				batch->params.index);
		}
	}
}

/******************************************************/
size_t* _gfx_mesh_reserve(

		GFXMesh*  mesh,
		size_t    meshID,
		size_t    bucket,
		size_t    units)
{
	/* Bound check */
	struct GFX_Mesh* internal = (struct GFX_Mesh*)mesh;
	size_t maxID = gfx_vector_get_size(&internal->batches);
	size_t max = gfx_vector_get_byte_size(&internal->buckets);

	if(!meshID || meshID > maxID || bucket >= max) return NULL;

	/* Get bucket, batch & difference */
	struct GFX_Batch* batch =
		gfx_vector_at(&internal->batches, meshID - 1);
	struct GFX_Bucket* it =
		gfx_vector_at(&internal->buckets, bucket);

	long int diff =
		(long int)units - (long int)it->units;
	long int diffBytes =
		(long int)sizeof(size_t) * diff;

	if(diff)
	{
		it->units = units;

		if(diff > 0)
		{
			/* Insert new units */
			if(gfx_vector_insert_range_at(
				&internal->buckets,
				diffBytes,
				NULL,
				batch->upper) == internal->buckets.end)
			{
				it->units -= diff;
				return NULL;
			}

			/* Set max to more units at batch if necessary */
			batch->units =
				(batch->units < units) ?
				units : batch->units;
		}
		else
		{
			/* Erase units */
			gfx_vector_erase_range_at(
				&internal->buckets,
				-diffBytes,
				batch->upper + diffBytes);

			/* Recompute the max units at the batch */
			_gfx_mesh_update_bucket_units(
				internal,
				batch);
		}

		/* Adjust bounds */
		_gfx_mesh_increase_bucket_bounds(
			internal,
			batch,
			diffBytes
		);

		it = gfx_vector_at(&internal->buckets, bucket);
	}

	return (size_t*)(it + 1);
}

/******************************************************/
size_t* _gfx_mesh_get_reserved(

		GFXMesh*  mesh,
		size_t    bucket,
		size_t*   units)
{
	/* Bound check */
	struct GFX_Mesh* internal = (struct GFX_Mesh*)mesh;
	size_t max = gfx_vector_get_byte_size(&internal->buckets);

	if(bucket >= max)
	{
		*units = 0;
		return NULL;
	}

	/* Get bucket */
	struct GFX_Bucket* it =
		gfx_vector_at(&internal->buckets, bucket);

	*units = it->units;
	return (size_t*)(it + 1);
}

/******************************************************/
int _gfx_mesh_increase(

		GFXMesh*  mesh,
		size_t    bucket,
		size_t    instances)
{
	/* Bound check */
	struct GFX_Mesh* internal =
		(struct GFX_Mesh*)mesh;
	size_t max =
		gfx_vector_get_byte_size(&internal->buckets);

	if(bucket >= max) return 0;

	/* Get bucket */
	struct GFX_Bucket* it =
		gfx_vector_at(&internal->buckets, bucket);

	/* Check for overflow */
	if(SIZE_MAX - instances < it->instances) return 0;
	it->instances += instances;

	return 1;
}

/******************************************************/
int _gfx_mesh_decrease(

		GFXMesh*  mesh,
		size_t    bucket,
		size_t    instances)
{
	/* Bound check */
	struct GFX_Mesh* internal =
		(struct GFX_Mesh*)mesh;
	size_t max =
		gfx_vector_get_byte_size(&internal->buckets);

	if(bucket >= max) return 0;

	/* Get bucket */
	struct GFX_Bucket* it =
		gfx_vector_at(&internal->buckets, bucket);

	/* Decrease */
	it->instances =
		(instances > it->instances) ? 0 :
		it->instances - instances;
	it->visible =
		(it->instances < it->visible) ?
		it->instances : it->visible;

	return (it->instances > 0);
}

/******************************************************/
size_t _gfx_mesh_get(

		GFXMesh*  mesh,
		size_t    bucket)
{
	/* Bound check */
	struct GFX_Mesh* internal =
		(struct GFX_Mesh*)mesh;
	size_t max =
		gfx_vector_get_byte_size(&internal->buckets);

	if(bucket >= max) return 0;

	return ((struct GFX_Bucket*)gfx_vector_at(
		&internal->buckets, bucket))->instances;
}

/******************************************************/
size_t _gfx_mesh_set_visible(

		GFXMesh*  mesh,
		size_t    bucket,
		size_t    instances)
{
	/* Bound check */
	struct GFX_Mesh* internal =
		(struct GFX_Mesh*)mesh;
	size_t max =
		gfx_vector_get_byte_size(&internal->buckets);

	if(bucket >= max) return 0;

	/* Get bucket */
	struct GFX_Bucket* it =
		gfx_vector_at(&internal->buckets, bucket);

	/* Decrease */
	return it->visible =
		(it->instances < instances) ?
		it->instances : instances;
}


/******************************************************/
size_t _gfx_mesh_get_visible(

		GFXMesh*  mesh,
		size_t    bucket)
{
	/* Bound check */
	struct GFX_Mesh* internal =
		(struct GFX_Mesh*)mesh;
	size_t max =
		gfx_vector_get_byte_size(&internal->buckets);

	if(bucket >= max) return 0;

	return ((struct GFX_Bucket*)gfx_vector_at(
		&internal->buckets, bucket))->visible;
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
	gfx_vector_init(&mesh->buckets, 1);

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

			if(it->material)
			{
				GFXMaterial* mat = it->material;
				size_t matID = it->materialID;

				_gfx_mesh_remove_batch(mesh, b);
				_gfx_material_remove_batch(mat, matID);
			}
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
	data.material = 0;
	data.source = 0;

	data.sub = _gfx_submesh_create(drawCalls, sources);
	if(!data.sub) return NULL;

	/* Add it to the LOD map */
	if(!gfx_lod_map_add((GFXLodMap*)mesh, level, &data))
	{
		_gfx_submesh_free(data.sub);
		return NULL;
	}

	/* Update all batches */
	size_t num;

	gfx_lod_map_get(
		(GFXLodMap*)mesh, level, &num);
	_gfx_mesh_update_batch_maps(
		(struct GFX_Mesh*)mesh, level, num - 1);

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
	data.sub      = share;
	data.material = 0;
	data.source   = 0;

	if(!gfx_lod_map_add((GFXLodMap*)mesh, level, &data))
	{
		_gfx_submesh_free(share);
		return 0;
	}

	/* Update all batches */
	size_t num;

	gfx_lod_map_get(
		(GFXLodMap*)mesh, level, &num);
	_gfx_mesh_update_batch_maps(
		(struct GFX_Mesh*)mesh, level, num - 1);

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
		/* Set material if found and update batches */
		if(data[num].material != material)
		{
			data[num].material = material;

			_gfx_mesh_update_batch_maps(
				(struct GFX_Mesh*)mesh, level, num);
			_gfx_mesh_rebuild_batches(
				(struct GFX_Mesh*)mesh, level, num);
		}
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

	/* Set the material and update batches */
	if(data[index].material != material)
	{
		data[index].material = material;

		_gfx_mesh_update_batch_maps(
			(struct GFX_Mesh*)mesh, level, index);
		_gfx_mesh_rebuild_batches(
			(struct GFX_Mesh*)mesh, level, index);
	}

	return 1;
}

/******************************************************/
size_t gfx_mesh_set_source(

		GFXMesh*       mesh,
		size_t         level,
		GFXSubMesh*    sub,
		unsigned char  source)
{
	/* Bound check */
	if(source >= sub->sources) return 0;

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
		if(data[num].source != source)
		{
			data[num].source = source;
			_gfx_mesh_rebuild_batches(
				(struct GFX_Mesh*)mesh,
				level,
				num
			);
		}
		++count;
	}

	return count;
}

/******************************************************/
int gfx_mesh_set_source_at(

		GFXMesh*       mesh,
		size_t         level,
		size_t         index,
		unsigned char  source)
{
	/* First get the submesh */
	size_t num;
	struct GFX_SubData* data = gfx_lod_map_get(
		(GFXLodMap*)mesh,
		level,
		&num
	);

	if(index >= num) return 0;

	/* Bound check */
	if(source >= data[index].sub->sources) return 0;

	/* Set the source */
	if(data[index].source != source)
	{
		data[index].source = source;
		_gfx_mesh_rebuild_batches(
			(struct GFX_Mesh*)mesh,
			level,
			index
		);
	}

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
unsigned char gfx_submesh_list_source_at(

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
