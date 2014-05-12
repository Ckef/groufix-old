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
#include "groufix/containers/vector.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/******************************************************/
/* Internal batch */
struct GFX_Batch
{
	GFXBatch   batch;      /* Super class */
	GFXVector  buckets;    /* Stores (GFX_BucketRef + size_t (instances) * (numIndices * submesh->sources) */
	size_t     numIndices; /* Number of material indices used */
};

/* Internal bucket reference */
struct GFX_BucketRef
{
	GFXPipe*  pipe;
	size_t    ref; /* Reference count */
};

/******************************************************/
static inline size_t _gfx_batch_get_bucket_size(

		struct GFX_Batch* batch)
{
	return
		sizeof(struct GFX_BucketRef) + sizeof(size_t) *
		(batch->numIndices * batch->batch.submesh->sources);
}

/******************************************************/
static inline size_t* _gfx_batch_get_instances(

		struct GFX_Batch*       batch,
		struct GFX_BucketRef*   bucket,
		size_t                  index,
		unsigned char           source)
{
	return ((size_t*)(bucket + 1)) +
		(index * batch->batch.submesh->sources + source);
}

/******************************************************/
static size_t _gfx_batch_get_instances_per_unit(

		GFXMaterial*  material,
		size_t        level,
		size_t        index)
{
	/* Check index */
	size_t num;
	GFXPropertyMapList list = gfx_material_get(
		material,
		level,
		&num);

	if(index >= num) return 0;

	return gfx_property_map_list_instances_at(list, index);
}

/******************************************************/
static size_t _gfx_batch_get_units(

		GFXMaterial*  material,
		size_t        level,
		size_t        index,
		size_t        instances)
{
	if(!instances) return 0;

	/* Check index */
	size_t num;
	GFXPropertyMapList list = gfx_material_get(
		material,
		level,
		&num);

	if(index >= num) return 0;

	/* Get instances per unit */
	size_t inst = gfx_property_map_list_instances_at(list, index);

	/* If infinite, use 1 unit, round up otherwise */
	return !inst ? 1 : (instances - 1) / inst + 1;
}

/******************************************************/
static struct GFX_Batch* _gfx_batch_create(

		GFXMaterial*  material,
		GFXSubMesh*   submesh)
{
	/* Allocate batch */
	struct GFX_Batch* batch = malloc(sizeof(struct GFX_Batch));
	if(!batch) return NULL;

	/* Initialize */
	batch->numIndices = 0;
	batch->batch.material = material;
	batch->batch.submesh = submesh;

	gfx_vector_init(&batch->buckets, 1);

	return batch;
}

/******************************************************/
static void _gfx_batch_free(

		struct GFX_Batch* batch)
{
	if(batch)
	{
		gfx_vector_clear(&batch->buckets);
		free(batch);
	}
}

/******************************************************/
static void _gfx_batch_init_units(

		GFXMaterial*  material,
		GFXPipe*      bucket,
		size_t        level,
		size_t        index,
		size_t        src,
		size_t        instances)
{
	/* Refind all the units as adding/removing invalidates previous pointers */
	/* Get instances per unit */
	size_t perUnit = _gfx_batch_get_instances_per_unit(
		material,
		level,
		index);

	/* Get all units associated with the property map */
	size_t num;
	void* data = _gfx_material_get_bucket_units(
		material,
		level,
		index,
		bucket,
		&num);

	size_t ind = _gfx_material_find_bucket_units(
		data,
		num,
		src,
		&num);

	/* Iterate over all units and initialize */
	size_t i;
	for(i = 0; i < num; ++i)
	{
		size_t id = _gfx_material_bucket_units_at(data, ind + i);

		/* Increasing copies */
		gfx_bucket_set_copy(bucket->bucket, id, i);

		/* Distribute instances across units */
		gfx_bucket_set_instances(bucket->bucket, id,
			(instances < perUnit) ? instances : perUnit);

		instances = (perUnit < instances) ? instances - perUnit : 0;
	}
}

/******************************************************/
static int _gfx_batch_insert_units(

		GFXMaterial*  material,
		GFXPipe*      bucket,
		size_t        index,
		size_t        src,
		size_t        instances)
{
	/* Iterate through all material LODs */
	size_t l;
	for(l = 0; l < material->lodMap.levels; ++l)
	{
		/* Get number units needed */
		size_t units = _gfx_batch_get_units(
			material,
			l,
			index,
			instances);

		if(units)
		{
			/* Get all units associated with the property map */
			size_t num;
			void* data = _gfx_material_get_bucket_units(
				material,
				l,
				index,
				bucket,
				&num);

			_gfx_material_find_bucket_units(
				data,
				num,
				src,
				&num);

			/* Insert new units */
			units = (num > units) ? 0 : units - num;
			data = _gfx_material_add_bucket_units(
				material,
				l,
				index,
				bucket,
				units,
				src, 0, 1);

			if(!data) return 0;

			/* Initialize units */
			_gfx_batch_init_units(
				material,
				bucket,
				l,
				index,
				src,
				instances);
		}
	}

	return 1;
}

/******************************************************/
static void _gfx_batch_erase_units(

		GFXMaterial*  material,
		GFXPipe*      bucket,
		size_t        index,
		size_t        src,
		size_t        instances)
{
	/* Iterate through all material LODs */
	size_t l;
	for(l = 0; l < material->lodMap.levels; ++l)
	{
		/* Get number of units to keep */
		size_t keep = _gfx_batch_get_units(
			material,
			l,
			index,
			instances);

		/* Get all units associated with the property map */
		size_t num;
		void* data = _gfx_material_get_bucket_units(
			material,
			l,
			index,
			bucket,
			&num);

		size_t ind = _gfx_material_find_bucket_units(
			data,
			num,
			src,
			&num);

		/* And remove associated units */
		keep = (keep > num) ? num : keep;
		_gfx_material_remove_bucket_units(
			material,
			l,
			index,
			bucket,
			ind + keep,
			num - keep);

		/* Initialize units */
		if(keep) _gfx_batch_init_units(
			material,
			bucket,
			l,
			index,
			src,
			instances);
	}
}

/******************************************************/
static struct GFX_Batch* _gfx_batch_find(

		GFXMaterial*  material,
		GFXSubMesh*   submesh)
{
	/* Iterate over all buckets the submesh is registered at */
	size_t buckets = _gfx_submesh_num_buckets(submesh);
	while(buckets)
	{
		GFXPipe* bucket = _gfx_submesh_get_bucket(
			submesh,
			--buckets);

		/* Find all batches */
		size_t num;
		GFXPipeCallbackList list = gfx_pipe_find(
			bucket,
			GFX_SCENE_KEY_BATCH,
			&num);

		while(num)
		{
			/* Find the one with the given material/submesh pair */
			GFXBatch* batch = gfx_pipe_at(list, --num)->data;

			if(batch->material == material && batch->submesh == submesh)
				return (struct GFX_Batch*)batch;
		}
	}

	return NULL;
}

/******************************************************/
static struct GFX_BucketRef* _gfx_batch_find_bucket(

		struct GFX_Batch*  batch,
		GFXPipe*           pipe)
{
	/* Iterate and find */
	size_t bucketSize = _gfx_batch_get_bucket_size(batch);
	struct GFX_BucketRef* it;

	for(
		it = batch->buckets.begin;
		it != batch->buckets.end;
		it = gfx_vector_advance(&batch->buckets, it, bucketSize))
	{
		if(it->pipe == pipe) break;
	}

	return it;
}

/******************************************************/
static void _gfx_batch_callback(

		GFXPipe*          pipe,
		GFXPipeCallback*  callback)
{
	struct GFX_Batch* batch = callback->data;

	/* Find the pipe and erase it */
	/* Don't worry about the submesh or material as they're registered at the pipe as well */
	struct GFX_BucketRef* ref = _gfx_batch_find_bucket(batch, pipe);

	if(ref != batch->buckets.end) gfx_vector_erase_range(
		&batch->buckets,
		_gfx_batch_get_bucket_size(batch),
		ref
	);

	/* Free the batch if nothing references it anymore */
	if(batch->buckets.begin == batch->buckets.end)
		_gfx_batch_free(batch);
}

/******************************************************/
GFXBatch* gfx_batch_reference(

		GFXPipe*      bucket,
		GFXMaterial*  material,
		GFXSubMesh*   submesh)
{
	/* Find the batch */
	struct GFX_Batch* batch = _gfx_batch_find(material, submesh);

	if(!batch)
	{
		/* Create it if not found */
		batch = _gfx_batch_create(material, submesh);
		if(!batch) return NULL;

		/* Try to reference it */
		if(!gfx_batch_reference_direct((GFXBatch*)batch, bucket))
		{
			_gfx_batch_free(batch);
			return NULL;
		}
	}
	else
	{
		/* Try to reference the found batch */
		if(!gfx_batch_reference_direct((GFXBatch*)batch, bucket))
			return NULL;
	}

	return (GFXBatch*)batch;
}

/******************************************************/
int gfx_batch_reference_direct(

		GFXBatch*  batch,
		GFXPipe*   bucket)
{
	/* Try to find the bucket */
	struct GFX_Batch* internal = (struct GFX_Batch*)batch;
	struct GFX_BucketRef* found = _gfx_batch_find_bucket(internal, bucket);

	if(found == internal->buckets.end)
	{
		/* Try to insert the bucket */
		size_t size = _gfx_batch_get_bucket_size(internal);
		found = gfx_vector_insert_range(
			&internal->buckets,
			size,
			NULL,
			internal->buckets.end
		);

		if(found == internal->buckets.end)
			return 0;

		/* Reference the bucket at the submesh */
		if(!_gfx_submesh_reference_bucket(batch->submesh, bucket))
		{
			gfx_vector_erase_range(&internal->buckets, size, found);
			return 0;
		}

		/* Register the batch at the pipe */
		GFXPipeCallback call;
		call.key = GFX_SCENE_KEY_BATCH;
		call.data = batch;

		if(!gfx_pipe_register(bucket, call, _gfx_batch_callback))
		{
			gfx_vector_erase_range(&internal->buckets, size, found);
			_gfx_submesh_dereference_bucket(batch->submesh, bucket);

			return 0;
		}

		/* Initialize the bucket */
		memset(found, 0, size);

		found->pipe = bucket;
		found->ref = 1;
	}
	else
	{
		/* Increase reference */
		if(!(found->ref + 1)) return 0;
		++found->ref;
	}

	return 1;
}

/******************************************************/
void gfx_batch_dereference(

		GFXBatch*  batch,
		GFXPipe*   bucket)
{
	/* Try to find the bucket */
	struct GFX_Batch* internal = (struct GFX_Batch*)batch;
	struct GFX_BucketRef* found = _gfx_batch_find_bucket(internal, bucket);

	/* Decrease references */
	if(found != internal->buckets.end)
		if(!(--found->ref))
		{
			/* Get all bucket sources */
			size_t* sources = _gfx_submesh_get_bucket_sources(
				batch->submesh,
				bucket,
				0,
				batch->submesh->sources
			);

			size_t i;
			size_t s;

			/* Iterate through indices and sources and remove units */
			for(i = 0; i < internal->numIndices; ++i)
				for(s = 0; s < batch->submesh->sources; ++s)
				{
					_gfx_batch_erase_units(
						batch->material,
						bucket,
						i,
						sources[s],
						0
					);
				}

			/* Unregister the batch at the pipe and dereference at submesh */
			GFXPipeCallback call;
			call.key = GFX_SCENE_KEY_BATCH;
			call.data = batch;

			gfx_pipe_unregister(bucket, call);
			_gfx_submesh_dereference_bucket(batch->submesh, bucket);

			/* Erase the bucket */
			gfx_vector_erase_range(
				&internal->buckets,
				_gfx_batch_get_bucket_size(internal),
				found
			);

			/* Free batch if nothing references it anymore */
			if(internal->buckets.begin == internal->buckets.end)
				_gfx_batch_free(internal);
		}
}

/******************************************************/
int gfx_batch_increase(

		GFXBatch*      batch,
		GFXPipe*       bucket,
		size_t         index,
		unsigned char  source,
		size_t         instances)
{
	/* Validate source */
	if(source >= batch->submesh->sources) return 0;

	/* Insert indices */
	struct GFX_Batch* internal = (struct GFX_Batch*)batch;
	if(internal->numIndices <= index)
	{
		/* Calculate new size and try to reserve it */
		size_t diff =
			index - internal->numIndices + 1;
		size_t sizeDiff =
			diff * (sizeof(size_t) * batch->submesh->sources);

		size_t bucketSize =
			_gfx_batch_get_bucket_size(internal);
		size_t bucketIndex =
			gfx_vector_get_byte_size(&internal->buckets);
		size_t size =
			bucketIndex + (bucketIndex / bucketSize) * sizeDiff;

		if(!gfx_vector_reserve(&internal->buckets, size))
			return 0;

		/* Iterate over all buckets and insert it */
		while(bucketIndex)
		{
			GFXVectorIterator it = gfx_vector_at(
				&internal->buckets,
				bucketIndex
			);

			bucketIndex -= bucketSize;
			it = gfx_vector_insert_range(
				&internal->buckets,
				sizeDiff,
				NULL,
				it
			);

			/* Initialize to 0 instances */
			memset(it, 0, sizeDiff);
		}

		internal->numIndices += diff;
	}

	/* Try to find the bucket */
	struct GFX_BucketRef* found = _gfx_batch_find_bucket(internal, bucket);
	if(found == internal->buckets.end) return 0;

	/* Get the source */
	size_t src = *_gfx_submesh_get_bucket_sources(
		batch->submesh,
		bucket,
		source,
		1);

	/* Calculate number of instances, check for overflow! */
	size_t* inst = _gfx_batch_get_instances(
		internal,
		found,
		index,
		source);

	if(!src || SIZE_MAX - instances < *inst) return 0;
	size_t new = *inst + instances;

	/* Insert the units in the material */
	if(!_gfx_batch_insert_units(
		batch->material,
		bucket,
		index,
		src,
		new)) return 0;

	/* Wooo! */
	*inst = new;

	return 1;
}

/******************************************************/
size_t gfx_batch_get(

		GFXBatch*      batch,
		GFXPipe*       bucket,
		size_t         index,
		unsigned char  source)
{
	/* Validate index and source */
	struct GFX_Batch* internal = (struct GFX_Batch*)batch;
	if(index >= internal->numIndices || source >= batch->submesh->sources)
		return 0;

	/* Try to find the bucket */
	struct GFX_BucketRef* found = _gfx_batch_find_bucket(internal, bucket);
	if(found == internal->buckets.end)
		return 0;

	/* Retrieve instances */
	return *_gfx_batch_get_instances(
		internal,
		found,
		index,
		source
	);
}

/******************************************************/
int gfx_batch_decrease(

		GFXBatch*      batch,
		GFXPipe*       bucket,
		size_t         index,
		unsigned char  source,
		size_t         instances)
{
	/* Validate index and source */
	struct GFX_Batch* internal = (struct GFX_Batch*)batch;
	if(index >= internal->numIndices || source >= batch->submesh->sources)
		return 0;

	/* Get the source */
	size_t src = *_gfx_submesh_get_bucket_sources(
		batch->submesh,
		bucket,
		source,
		1);

	/* Try to find the bucket */
	struct GFX_BucketRef* found = _gfx_batch_find_bucket(internal, bucket);
	if(!src || found == internal->buckets.end)
		return 0;

	/* Calculate number of instances */
	size_t* inst = _gfx_batch_get_instances(
		internal,
		found,
		index,
		source);

	*inst = (instances > *inst) ? 0 : *inst - instances;

	/* Erase the units from the material */
	_gfx_batch_erase_units(
		batch->material,
		bucket,
		index,
		src,
		*inst
	);

	return 1;
}

/******************************************************/
int gfx_batch_set_visible(

		GFXBatch*      batch,
		GFXPipe*       bucket,
		size_t         level,
		size_t         index,
		unsigned char  source,
		size_t         visible)
{
	return 0;
}
