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
/* Internal batcher */
struct GFX_Batcher
{
	GFXBatcher  batcher;    /* Super class */
	GFXVector   buckets;    /* Stores (GFX_BucketRef + size_t (instances) * (numIndices * submesh->sources) */
	size_t      numIndices; /* Number of material indices used */
};

/* Internal bucket reference */
struct GFX_BucketRef
{
	GFXPipe*  pipe;
	size_t    ref; /* Reference count */
};

/******************************************************/
static inline size_t _gfx_batcher_get_bucket_size(

		struct GFX_Batcher* batcher)
{
	return
		sizeof(struct GFX_BucketRef) + sizeof(size_t) *
		(batcher->numIndices * batcher->batcher.submesh->sources);
}

/******************************************************/
static inline size_t* _gfx_batcher_get_instances(

		struct GFX_Batcher*     batcher,
		struct GFX_BucketRef*   bucket,
		size_t                  index,
		unsigned char           source)
{
	return ((size_t*)(bucket + 1)) +
		(index * batcher->batcher.submesh->sources + source);
}

/******************************************************/
static size_t _gfx_batcher_get_units(

		GFXMaterial*  material,
		size_t        level,
		size_t        index,
		size_t        instances,
		size_t*       perUnit)
{
	if(!instances)
	{
		*perUnit = 0;
		return 0;
	}

	/* Check index */
	size_t num;
	GFXPropertyMapList list = gfx_material_get(
		material,
		level,
		&num);

	if(index >= num)
	{
		*perUnit = 0;
		return 0;
	}

	/* Get instances per unit */
	size_t inst = gfx_property_map_list_instances_at(list, index);

	/* If infinite, use 1 unit, round up otherwise */
	size_t ret = !inst ? 1 : (instances - 1) / inst + 1;
	*perUnit = inst;

	return ret;
}

/******************************************************/
static struct GFX_Batcher* _gfx_batcher_create(

		GFXMaterial*  material,
		GFXSubMesh*   submesh)
{
	/* Allocate batcher */
	struct GFX_Batcher* batcher = malloc(sizeof(struct GFX_Batcher));
	if(!batcher) return NULL;

	/* Initialize */
	batcher->numIndices = 0;
	batcher->batcher.material = material;
	batcher->batcher.submesh = submesh;

	gfx_vector_init(&batcher->buckets, 1);

	return batcher;
}

/******************************************************/
static void _gfx_batcher_free(

		struct GFX_Batcher* batcher)
{
	if(batcher)
	{
		gfx_vector_clear(&batcher->buckets);
		free(batcher);
	}
}

/******************************************************/
static int _gfx_batcher_insert_units(

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
		size_t need = _gfx_batcher_get_units(
			material,
			l,
			index,
			instances,
			&instances);

		if(need)
		{
			/* Get all units associated with the property map */
			size_t num;
			void* data = _gfx_material_get_bucket_units(
				material,
				l,
				index,
				bucket,
				&num
			);
			_gfx_material_find_bucket_units(data, num, src, &num);

			/* Insert new units */
			need = (num > need) ? 0 : need - num;
			data = _gfx_material_add_bucket_units(
				material,
				l,
				index,
				bucket,
				need,
				src,
				0,
				0);

			if(!data) return 0;

			/* Initializes used copies for new units */
			while(need--)
			{
				size_t id = _gfx_material_bucket_units_at(
					data,
					need);

				gfx_bucket_set_copy(
					bucket->bucket,
					id,
					num + need);
			}
		}
	}

	return 1;
}

/******************************************************/
static void _gfx_batcher_erase_units(

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
		size_t perUnit;
		size_t keep = _gfx_batcher_get_units(
			material,
			l,
			index,
			instances,
			&perUnit);

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

		if(num)
		{
			/* Set number of instances of last unit to keep */
			/* This would be (number of wanted instances - instances of previous units) */
			keep = (keep > num) ? num : keep;

			if(keep) gfx_bucket_set_instances(
				bucket->bucket,
				_gfx_material_bucket_units_at(data, ind + keep - 1),
				instances - (keep - 1) * perUnit
			);

			/* And remove associated units */
			_gfx_material_remove_bucket_units(
				material,
				l,
				index,
				bucket,
				ind + keep,
				num - keep);
		}
	}
}

/******************************************************/
static struct GFX_Batcher* _gfx_batcher_find(

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
			GFX_SCENE_KEY_BATCHER,
			&num);

		while(num)
		{
			/* Find the one with the given material/submesh pair */
			GFXBatcher* batcher = gfx_pipe_at(list, --num)->data;

			if(batcher->material == material && batcher->submesh == submesh)
				return (struct GFX_Batcher*)batcher;
		}
	}

	return NULL;
}

/******************************************************/
static struct GFX_BucketRef* _gfx_batcher_find_bucket(

		struct GFX_Batcher*  batcher,
		GFXPipe*             pipe)
{
	/* Iterate and find */
	size_t bucketSize = _gfx_batcher_get_bucket_size(batcher);
	struct GFX_BucketRef* it;

	for(
		it = batcher->buckets.begin;
		it != batcher->buckets.end;
		it = gfx_vector_advance(&batcher->buckets, it, bucketSize))
	{
		if(it->pipe == pipe) break;
	}

	return it;
}

/******************************************************/
static void _gfx_batcher_callback(

		GFXPipe*          pipe,
		GFXPipeCallback*  callback)
{
	struct GFX_Batcher* batcher = callback->data;

	/* Find the pipe and erase it */
	/* Don't worry about the submesh or material as they're registered at the pipe as well */
	struct GFX_BucketRef* ref = _gfx_batcher_find_bucket(batcher, pipe);

	if(ref != batcher->buckets.end) gfx_vector_erase_range(
		&batcher->buckets,
		_gfx_batcher_get_bucket_size(batcher),
		ref
	);

	/* Free the batcher if nothing references it anymore */
	if(batcher->buckets.begin == batcher->buckets.end)
		_gfx_batcher_free(batcher);
}

/******************************************************/
GFXBatcher* gfx_batcher_reference(

		GFXMaterial*  material,
		GFXSubMesh*   submesh,
		GFXPipe*      bucket)
{
	/* Find the batcher */
	struct GFX_Batcher* batcher = _gfx_batcher_find(material, submesh);

	if(!batcher)
	{
		/* Create it if not found */
		batcher = _gfx_batcher_create(material, submesh);
		if(!batcher) return NULL;

		/* Try to reference it */
		if(!gfx_batcher_reference_direct((GFXBatcher*)batcher, bucket))
		{
			_gfx_batcher_free(batcher);
			return NULL;
		}
	}
	else
	{
		/* Try to reference the found batcher */
		if(!gfx_batcher_reference_direct((GFXBatcher*)batcher, bucket))
			return NULL;
	}

	return (GFXBatcher*)batcher;
}

/******************************************************/
int gfx_batcher_reference_direct(

		GFXBatcher*  batcher,
		GFXPipe*     bucket)
{
	/* Try to find the bucket */
	struct GFX_Batcher* internal = (struct GFX_Batcher*)batcher;
	struct GFX_BucketRef* found = _gfx_batcher_find_bucket(internal, bucket);

	if(found == internal->buckets.end)
	{
		/* Try to insert the bucket */
		size_t size = _gfx_batcher_get_bucket_size(internal);
		found = gfx_vector_insert_range(
			&internal->buckets,
			size,
			NULL,
			internal->buckets.end
		);

		if(found == internal->buckets.end)
			return 0;

		/* Reference the bucket at the submesh */
		if(!_gfx_submesh_reference_bucket(batcher->submesh, bucket))
		{
			gfx_vector_erase_range(&internal->buckets, size, found);
			return 0;
		}

		/* Register the batcher at the pipe */
		GFXPipeCallback call;
		call.key = GFX_SCENE_KEY_BATCHER;
		call.data = batcher;

		if(!gfx_pipe_register(bucket, call, _gfx_batcher_callback))
		{
			gfx_vector_erase_range(&internal->buckets, size, found);
			_gfx_submesh_dereference_bucket(batcher->submesh, bucket);

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
void gfx_batcher_dereference(

		GFXBatcher*  batcher,
		GFXPipe*     bucket)
{
	/* Try to find the bucket */
	struct GFX_Batcher* internal = (struct GFX_Batcher*)batcher;
	struct GFX_BucketRef* found = _gfx_batcher_find_bucket(internal, bucket);

	/* Decrease references */
	if(found != internal->buckets.end)
		if(!(--found->ref))
		{
			/* Get all bucket sources */
			size_t* sources = _gfx_submesh_get_bucket_sources(
				batcher->submesh,
				bucket,
				0,
				batcher->submesh->sources
			);

			size_t i;
			size_t s;

			/* Iterate through indices and sources and remove units */
			for(i = 0; i < internal->numIndices; ++i)
				for(s = 0; s < batcher->submesh->sources; ++s)
				{
					_gfx_batcher_erase_units(
						batcher->material,
						bucket,
						i,
						sources[s],
						0
					);
				}

			/* Unregister the batcher at the pipe and dereference at submesh */
			GFXPipeCallback call;
			call.key = GFX_SCENE_KEY_BATCHER;
			call.data = batcher;

			gfx_pipe_unregister(bucket, call);
			_gfx_submesh_dereference_bucket(batcher->submesh, bucket);

			/* Erase the bucket */
			gfx_vector_erase_range(
				&internal->buckets,
				_gfx_batcher_get_bucket_size(internal),
				found
			);

			/* Free batcher if nothing references it anymore */
			if(internal->buckets.begin == internal->buckets.end)
				_gfx_batcher_free(internal);
		}
}

/******************************************************/
int gfx_batcher_increase(

		GFXBatcher*    batcher,
		GFXPipe*       bucket,
		size_t         index,
		unsigned char  source,
		size_t         instances)
{
	/* Validate source */
	if(source >= batcher->submesh->sources) return 0;

	/* Insert indices */
	struct GFX_Batcher* internal = (struct GFX_Batcher*)batcher;
	if(internal->numIndices <= index)
	{
		/* Calculate new size and try to reserve it */
		size_t diff =
			index - internal->numIndices + 1;
		size_t sizeDiff =
			diff * (sizeof(size_t) * batcher->submesh->sources);

		size_t bucketSize =
			_gfx_batcher_get_bucket_size(internal);
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
	struct GFX_BucketRef* found = _gfx_batcher_find_bucket(internal, bucket);
	if(found == internal->buckets.end) return 0;

	/* Get the source */
	size_t src = *_gfx_submesh_get_bucket_sources(
		batcher->submesh,
		bucket,
		source,
		1);

	/* Calculate number of instances, check for overflow! */
	size_t* inst = _gfx_batcher_get_instances(
		internal,
		found,
		index,
		source);

	if(!src || SIZE_MAX - instances < *inst) return 0;
	size_t new = *inst + instances;

	/* Insert the units in the material */
	if(!_gfx_batcher_insert_units(
		batcher->material,
		bucket,
		index,
		src,
		new)) return 0;

	/* Wooo! */
	*inst = new;

	return 1;
}

/******************************************************/
size_t gfx_batcher_get(

		GFXBatcher*    batcher,
		GFXPipe*       bucket,
		size_t         index,
		unsigned char  source)
{
	/* Validate index and source */
	struct GFX_Batcher* internal = (struct GFX_Batcher*)batcher;
	if(index >= internal->numIndices || source >= batcher->submesh->sources)
		return 0;

	/* Try to find the bucket */
	struct GFX_BucketRef* found = _gfx_batcher_find_bucket(internal, bucket);
	if(found == internal->buckets.end)
		return 0;

	/* Retrieve instances */
	return *_gfx_batcher_get_instances(
		internal,
		found,
		index,
		source
	);
}

/******************************************************/
int gfx_batcher_decrease(

		GFXBatcher*    batcher,
		GFXPipe*       bucket,
		size_t         index,
		unsigned char  source,
		size_t         instances)
{
	/* Validate index and source */
	struct GFX_Batcher* internal = (struct GFX_Batcher*)batcher;
	if(index >= internal->numIndices || source >= batcher->submesh->sources)
		return 0;

	/* Get the source */
	size_t src = *_gfx_submesh_get_bucket_sources(
		batcher->submesh,
		bucket,
		source,
		1);

	/* Try to find the bucket */
	struct GFX_BucketRef* found = _gfx_batcher_find_bucket(internal, bucket);
	if(!src || found == internal->buckets.end)
		return 0;

	/* Calculate number of instances */
	size_t* inst = _gfx_batcher_get_instances(
		internal,
		found,
		index,
		source);

	*inst = (instances > *inst) ? 0 : *inst - instances;

	/* Erase the units from the material */
	_gfx_batcher_erase_units(
		batcher->material,
		bucket,
		index,
		src,
		*inst
	);

	return 1;
}

/******************************************************/
int gfx_batcher_set_visible(

		GFXBatcher*    batcher,
		GFXPipe*       bucket,
		size_t         level,
		size_t         index,
		unsigned char  source,
		size_t         visible)
{
	/* Validate index and source */
	struct GFX_Batcher* internal = (struct GFX_Batcher*)batcher;
	if(index >= internal->numIndices || source >= batcher->submesh->sources)
		return 0;

	/* Get the source */
	size_t src = *_gfx_submesh_get_bucket_sources(
		batcher->submesh,
		bucket,
		source,
		1);

	/* Get all units associated with the property map */
	size_t num;
	void* data = _gfx_material_get_bucket_units(
		batcher->material,
		level,
		index,
		bucket,
		&num);

	size_t ind = _gfx_material_find_bucket_units(data, num, src, &num);
	if(!num) return 0;

	/* Get number of instances per unit */
	size_t perUnit;
	_gfx_batcher_get_units(
		batcher->material,
		level,
		index,
		visible,
		&perUnit);

	/* Iterate over all units */
	size_t i;
	for(i = 0; i < num; ++i)
	{
		/* Set visibility */
		size_t id = _gfx_material_bucket_units_at(data, ind + i);
		gfx_bucket_set_visible(bucket->bucket, id, visible);

		/* Distribute instances across units */
		gfx_bucket_set_instances(bucket->bucket, id,
			(visible < perUnit) ? visible : perUnit);

		visible = (perUnit < visible) ? visible - perUnit : 0;
	}

	return 1;
}
