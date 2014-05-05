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

#include <stdlib.h>

/******************************************************/
/* Internal batch */
struct GFX_Batch
{
	GFXBatch   batch;   /* Super class */
	GFXVector  buckets; /* Stores GFX_BucketRef */
};

/* Internal bucket reference */
struct GFX_BucketRef
{
	GFXPipe*  pipe;
	size_t    ref; /* Reference count */
};

/******************************************************/
static struct GFX_Batch* _gfx_batch_create(

		GFXMaterial*  material,
		GFXSubMesh*   submesh)
{
	/* Allocate batch */
	struct GFX_Batch* batch = malloc(sizeof(struct GFX_Batch));
	if(!batch) return NULL;

	/* Initialize */
	batch->batch.material = material;
	batch->batch.submesh = submesh;

	gfx_vector_init(&batch->buckets, sizeof(struct GFX_BucketRef));

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
	struct GFX_BucketRef* it;

	for(
		it = batch->buckets.begin;
		it != batch->buckets.end;
		it = gfx_vector_next(&batch->buckets, it))
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

	if(ref != batch->buckets.end)
		gfx_vector_erase(&batch->buckets, ref);

	/* Free the batch if nothing references it anymore */
	if(batch->buckets.begin == batch->buckets.end)
		_gfx_batch_free(batch);
}

/******************************************************/
static void _gfx_batch_erase_units(

		struct GFX_Batch*  batch,
		GFXPipe*           bucket)
{
	/* Get all bucket sources */
	size_t* sources = _gfx_submesh_get_bucket_sources(
		batch->batch.submesh,
		bucket,
		0,
		batch->batch.submesh->sources
	);

	/* Iterate through all material LODs */
	size_t l;
	for(l = 0; l < batch->batch.material->lodMap.levels; ++l)
	{
		/* Iterate through all property maps */
		size_t num;
		gfx_material_get(batch->batch.material, l, &num);

		while(num--)
		{
			/* Get all units associated with the property map */
			size_t units;
			void* data = _gfx_material_get_bucket_units(
				batch->batch.material,
				l,
				num,
				bucket,
				&units);

			/* Iterate through all sources */
			size_t s;
			for(s = 0; s < batch->batch.submesh->sources; ++s)
			{
				size_t found;
				size_t index = _gfx_material_find_bucket_units(
					data,
					units,
					sources[s],
					&found);

				/* And remove associated units */
				_gfx_material_remove_bucket_units(
					batch->batch.material,
					l,
					num,
					bucket,
					index,
					found);
			}
		}
	}
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
		struct GFX_BucketRef ref;
		ref.pipe = bucket;
		ref.ref = 1;

		found = gfx_vector_insert(
			&internal->buckets,
			&ref,
			internal->buckets.end
		);

		if(found == internal->buckets.end)
			return 0;

		/* Reference the bucket at the submesh */
		if(!_gfx_submesh_reference_bucket(batch->submesh, bucket))
		{
			gfx_vector_erase(&internal->buckets, found);
			return 0;
		}

		/* Register the batch at the pipe */
		GFXPipeCallback call;
		call.key = GFX_SCENE_KEY_BATCH;
		call.data = batch;

		if(!gfx_pipe_register(bucket, call, _gfx_batch_callback))
		{
			gfx_vector_erase(&internal->buckets, found);
			_gfx_submesh_dereference_bucket(batch->submesh, bucket);

			return 0;
		}
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
			/* Remove all units at the material and dereference at the submesh */
			_gfx_batch_erase_units(internal, bucket);
			_gfx_submesh_dereference_bucket(batch->submesh, bucket);

			/* Unregister the batch at the pipe */
			GFXPipeCallback call;
			call.key = GFX_SCENE_KEY_BATCH;
			call.data = batch;

			gfx_pipe_unregister(bucket, call);

			/* Erase the bucket and free batch if nothing references it anymore */
			gfx_vector_erase(&internal->buckets, found);
			if(internal->buckets.begin == internal->buckets.end)
				_gfx_batch_free(internal);
		}
}
