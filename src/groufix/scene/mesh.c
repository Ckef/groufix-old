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
	size_t        index;     /* Submesh index for each LOD */
	size_t        upper;     /* Upper bound in buckets vector */
};

/* Internal bucket reference */
struct GFX_Bucket
{
	GFXPipe*  pipe;
	size_t    instances;
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
				&mesh->buckets, begin++);

			if(bucket->pipe == pipe)
			{
				gfx_vector_erase(&mesh->buckets, bucket);
				_gfx_mesh_increase_bucket_bounds(mesh, it, -1);
			}
		}
	}
}

/******************************************************/
static int _gfx_mesh_reference_bucket(

		struct GFX_Mesh*  mesh,
		GFXPipe*          pipe,
		size_t            index)
{
	/* Register the mesh at the pipe */
	GFXPipeCallback call;
	call.key = GFX_SCENE_KEY_MESH;
	call.data = mesh;

	/* Only register if not yet registered */
	int exists = gfx_pipe_exists(pipe, call);

	if(!exists)
	{
		if(!gfx_pipe_register(pipe, call, _gfx_mesh_callback))
			return 0;
	}

	/* Reference the bucket at all associated submeshes */
	size_t level;
	for(level = 0; level < mesh->map.map.levels; ++level)
	{
		size_t num;
		struct GFX_SubData* data = gfx_lod_map_get(
			(GFXLodMap*)mesh,
			level,
			&num
		);

		/* Skip if index does not exist */
		if(index >= num)
			continue;
		if(!_gfx_submesh_reference_bucket(data[index].sub, pipe))
			break;
	}

	/* Undo all */
	if(level < mesh->map.map.levels)
	{
		while(level--)
		{
			size_t num;
			struct GFX_SubData* data = gfx_lod_map_get(
				(GFXLodMap*)mesh,
				level,
				&num
			);

			if(index >= num) continue;
			_gfx_submesh_dereference_bucket(data[index].sub, pipe);
		}

		/* Also unregister if necessary */
		if(!exists) gfx_pipe_unregister(pipe, call);

		return 0;
	}

	return 1;
}

/******************************************************/
static void _gfx_mesh_dereference_bucket(

		struct GFX_Mesh*  mesh,
		GFXPipe*          pipe,
		size_t            index)
{
	/* Dereference the bucket at all associated submeshes */
	size_t level;
	for(level = 0; level < mesh->map.map.levels; ++level)
	{
		size_t num;
		struct GFX_SubData* data = gfx_lod_map_get(
			(GFXLodMap*)mesh,
			level,
			&num
		);

		/* Skip if index does not exist */
		if(index >= num) continue;
		_gfx_submesh_dereference_bucket(data[index].sub, pipe);
	}

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
		size_t            index)
{
	/* Iterate through all batches */
	struct GFX_Batch* it;
	for(
		it = mesh->batches.begin;
		it != mesh->batches.end;
		it = gfx_vector_next(&mesh->batches, it))
	{
		if(it->index == index)
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
		size_t            index)
{
	/* Iterate through all batches */
	struct GFX_Batch* it;
	for(
		it = mesh->batches.begin;
		it != mesh->batches.end;
		it = gfx_vector_next(&mesh->batches, it))
	{
		if(it->index == index)
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
static struct GFX_Batch* _gfx_mesh_find_material(

		struct GFX_Mesh*  mesh,
		GFXMaterial*      mat,
		size_t            index)
{
	/* Try to find the material */
	struct GFX_Batch* empty = mesh->batches.end;
	struct GFX_Batch* it;

	for(
		it = mesh->batches.begin;
		it != mesh->batches.end;
		it = gfx_vector_next(&mesh->batches, it))
	{
		if(it->material == mat && it->index == index)
			return it;

		else if(!it->material && empty == mesh->batches.end)
			empty = it;
	}

	/* Construct new ID */
	struct GFX_Batch new;
	new.material   = mat;
	new.materialID = 0;
	new.index      = index;

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
		size_t        index,
		size_t*       materialID)
{
	struct GFX_Mesh* internal = (struct GFX_Mesh*)mesh;
	struct GFX_Batch* batch = _gfx_mesh_find_material(internal, material, index);

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
		/* Get batch and bounds */
		struct GFX_Batch* batch = gfx_vector_at(
			&internal->batches,
			meshID - 1);

		size_t begin;
		size_t end;
		_gfx_mesh_get_bucket_bounds(
			internal,
			batch,
			&begin,
			&end);

		/* Dereference and remove all buckets */
		size_t i = end;
		while(i > begin)
		{
			struct GFX_Bucket* it = gfx_vector_at(&internal->buckets, --i);
			GFXPipe* pipe = it->pipe;

			gfx_vector_erase(&internal->buckets, it);
			_gfx_mesh_dereference_bucket(internal, pipe, batch->index);
		}

		/* Fix bounds and mark as empty */
		_gfx_mesh_increase_bucket_bounds(
			internal,
			batch,
			(long int)begin - (long int)end
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
int _gfx_mesh_increase_instances(

		GFXMesh*  mesh,
		size_t    meshID,
		GFXPipe*  pipe,
		size_t    instances)
{
	/* Validate pipe type */
	if(gfx_pipe_get_type(pipe) != GFX_PIPE_BUCKET || !instances) return 0;

	/* Bound check */
	struct GFX_Mesh* internal = (struct GFX_Mesh*)mesh;
	size_t max = gfx_vector_get_size(&internal->batches);

	if(!meshID || meshID > max) return 0;

	/* Get batch and bounds */
	struct GFX_Batch* batch = gfx_vector_at(
		&internal->batches,
		meshID - 1);

	size_t begin;
	size_t end;
	_gfx_mesh_get_bucket_bounds(
		internal,
		batch,
		&begin,
		&end);

	/* Find bucket */
	size_t index = _gfx_mesh_find_bucket(
		internal,
		begin,
		end,
		pipe);

	/* Insert new bucket */
	if(index == end)
	{
		struct GFX_Bucket bucket;
		bucket.pipe = pipe;
		bucket.instances = instances;

		struct GFX_Bucket* it = gfx_vector_insert_at(
			&internal->buckets,
			&bucket,
			end);

		if(it == internal->buckets.end) return 0;

		/* Reference the bucket */
		if(!_gfx_mesh_reference_bucket(internal, pipe, batch->index))
		{
			gfx_vector_erase(&internal->buckets, it);
			return 0;
		}

		/* Finally increase bounds */
		_gfx_mesh_increase_bucket_bounds(internal, batch, 1);
	}
	else
	{
		/* Increase instances, first check for overflow */
		struct GFX_Bucket* it = gfx_vector_at(&internal->buckets, index);

		if(SIZE_MAX - instances < it->instances) return 0;
		it->instances += instances;
	}

	return 1;
}

/******************************************************/
void _gfx_mesh_decrease_instances(

		GFXMesh*  mesh,
		size_t    meshID,
		GFXPipe*  pipe,
		size_t    instances)
{
	/* Bound check */
	struct GFX_Mesh* internal = (struct GFX_Mesh*)mesh;
	size_t max = gfx_vector_get_size(&internal->batches);

	if(meshID && meshID <= max)
	{
		/* Get batch and bounds */
		struct GFX_Batch* batch = gfx_vector_at(
			&internal->batches,
			meshID - 1);

		size_t begin;
		size_t end;
		_gfx_mesh_get_bucket_bounds(
			internal,
			batch,
			&begin,
			&end);

		/* Find bucket */
		size_t index = _gfx_mesh_find_bucket(
			internal,
			begin,
			end,
			pipe);

		if(index != end)
		{
			struct GFX_Bucket* it = gfx_vector_at(&internal->buckets, index);
			if(it->instances <= instances)
			{
				/* Remove the bucket */
				gfx_vector_erase(&internal->buckets, it);
				_gfx_mesh_increase_bucket_bounds(internal, batch, -1);

				/* Dereference bucket */
				_gfx_mesh_dereference_bucket(internal, pipe, batch->index);
			}

			/* Decrease instances */
			else it->instances -= instances;
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

		/* Remove all batches */
		size_t b = gfx_vector_get_size(&internal->batches);
		while(b) _gfx_mesh_remove_batch(mesh, b--);

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

	if(!_gfx_mesh_reference_submesh((struct GFX_Mesh*)mesh, data.sub, num))
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

	if(!_gfx_mesh_reference_submesh((struct GFX_Mesh*)mesh, share, num))
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
		_gfx_mesh_dereference_submesh((struct GFX_Mesh*)mesh, share, num);
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
