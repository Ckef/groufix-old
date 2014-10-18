/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#include "groufix/core/errors.h"
#include "groufix/scene/protocol.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

/******************************************************/
/* Internal submesh */
struct GFX_SubMesh
{
	/* Super class */
	GFXSubMesh submesh;

	/* Hidden data */
	unsigned int  references; /* Reference counter */
	GFXVector     buckets;    /* Stores (GFX_Bucket + GFXBucketSource * sources) */
	GFXVector     buffers;    /* Stores GFXSharedBuffer */

	GFXVector     batches;    /* Stores GFX_Batch, index + 1 = submesh ID of batch */
	GFXVector     units;      /* Stores (GFX_Units + GFXBucketUnit * units) */
};

/* Internal bucket reference */
struct GFX_Bucket
{
	GFXPipe*      pipe;
	unsigned int  ref; /* Reference count */
};

/* Internal batch (material reference) */
struct GFX_Batch
{
	GFXMaterial*   material;   /* NULL when empty */
	unsigned int   materialID;
	GFX_BatchData  data;
	unsigned int   units;      /* Max number of units reserved at any bucket */
	unsigned int   upper;      /* Upper bound in units vector */
};

/* Internal unit pool */
struct GFX_Units
{
	GFXPipe*      pipe;
	unsigned int  instances; /* Number of instances */
	unsigned int  visible;   /* Number of visible instances */
	unsigned int  units;     /* Number of stored units */
};

/******************************************************/
static inline GFXBucketSource* _gfx_submesh_get_src(

		struct GFX_Bucket*  bucket,
		unsigned char       index)
{
	return ((GFXBucketSource*)(bucket + 1)) + index;
}

/******************************************************/
static struct GFX_Bucket* _gfx_submesh_find_bucket(

		struct GFX_SubMesh*  mesh,
		GFXPipe*             pipe)
{
	GFXVectorIterator it;
	for(
		it = mesh->buckets.begin;
		it != mesh->buckets.end;
		it = gfx_vector_next(&mesh->buckets, it))
	{
		if(((struct GFX_Bucket*)it)->pipe == pipe) break;
	}

	return (struct GFX_Bucket*)it;
}

/******************************************************/
static void _gfx_submesh_increase_batch_units_bounds(

		struct GFX_SubMesh*  mesh,
		struct GFX_Batch*    batch,
		long int             diff)
{
	while(batch != mesh->batches.end)
	{
		batch->upper += diff;
		batch = gfx_vector_next(&mesh->batches, batch);
	}
}

/******************************************************/
static void _gfx_submesh_get_batch_units_bounds(

		struct GFX_SubMesh*  mesh,
		struct GFX_Batch*    batch,
		unsigned int*        begin,
		unsigned int*        end)
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
static unsigned int _gfx_submesh_find_batch_units(

		struct GFX_SubMesh*  mesh,
		unsigned int         begin,
		unsigned int         end,
		GFXPipe*             pipe)
{
	while(begin < end)
	{
		struct GFX_Units* it = gfx_vector_at(&mesh->units, begin);
		if(it->pipe == pipe) break;

		begin +=
			sizeof(struct GFX_Units) +
			sizeof(GFXBucketUnit) * it->units;
	}

	return begin;
}

/******************************************************/
static void _gfx_submesh_update_batch_units(

		struct GFX_SubMesh*  mesh,
		struct GFX_Batch*    batch)
{
	batch->units = 0;

	/* Get bounds */
	unsigned int begin;
	unsigned int end;
	_gfx_submesh_get_batch_units_bounds(mesh, batch, &begin, &end);

	/* Iterate and increase */
	while(begin < end)
	{
		struct GFX_Units* it = gfx_vector_at(&mesh->units, begin);

		batch->units =
			(batch->units < it->units) ?
			it->units : batch->units;
		begin +=
			sizeof(struct GFX_Units) +
			sizeof(GFXBucketUnit) * it->units;
	}
}

/******************************************************/
static int _gfx_submesh_reference_bucket(

		struct GFX_SubMesh*  mesh,
		GFXPipe*             pipe)
{
	/* Validate pipe type */
	if(gfx_pipe_get_type(pipe) != GFX_PIPE_BUCKET) return 0;

	/* See if it already exists */
	struct GFX_Bucket* bucket =
		_gfx_submesh_find_bucket(mesh, pipe);

	if(bucket == mesh->buckets.end)
	{
		/* Insert bucket */
		bucket = gfx_vector_insert(
			&mesh->buckets,
			NULL,
			mesh->buckets.end
		);

		if(bucket == mesh->buckets.end)
			return 0;

		/* Initialize bucket, reference and source IDs */
		memset(bucket + 1, 0, sizeof(GFXBucketSource) * mesh->submesh.sources);

		bucket->pipe = pipe;
		bucket->ref = 1;
	}
	else
	{
		/* Increase reference counter */
		if(!(bucket->ref + 1))
		{
			/* Overflow error */
			gfx_errors_push(
				GFX_ERROR_OVERFLOW,
				"Overflow occurred during SubMesh bucket referencing."
			);
			return 0;
		};
		++bucket->ref;
	}

	return 1;
}

/******************************************************/
static void _gfx_submesh_dereference_bucket(

		struct GFX_SubMesh*  mesh,
		GFXPipe*             pipe)
{
	/* Find the bucket */
	struct GFX_Bucket* bucket =
		_gfx_submesh_find_bucket(mesh, pipe);

	/* Decrease reference counter */
	if(bucket != mesh->buckets.end)
		if(!(--bucket->ref))
		{
			/* Remove all the sources */
			unsigned char s;
			for(s = 0; s < mesh->submesh.sources; ++s)
			{
				gfx_bucket_remove_source(
					bucket->pipe->bucket,
					*_gfx_submesh_get_src(bucket, s)
				);
			}

			/* Erase from vector */
			gfx_vector_erase(&mesh->buckets, bucket);
		}
}

/******************************************************/
static struct GFX_Batch* _gfx_submesh_find_batch(

		struct GFX_SubMesh*   mesh,
		GFXMaterial*          mat,
		const GFX_BatchData*  data)
{
	/* Try to find the material */
	struct GFX_Batch* empty = mesh->batches.end;
	struct GFX_Batch* it;

	for(
		it = mesh->batches.begin;
		it != mesh->batches.end;
		it = gfx_vector_next(&mesh->batches, it))
	{
		if(
			it->material == mat &&
			!memcmp(&it->data.params, &data->params, sizeof(GFXBatchParams)))
		{
			return it;
		}

		else if(!it->material && empty == mesh->batches.end)
			empty = it;
	}

	/* Construct new ID */
	struct GFX_Batch new;
	new.material   = mat;
	new.materialID = 0;
	new.data       = *data;
	new.units      = 0;

	/* Replace an empty ID */
	if(empty != mesh->batches.end)
	{
		new.upper = empty->upper;
		*empty = new;

		return empty;
	}

	/* Overflow */
	if(gfx_vector_get_size(&mesh->batches) == UINT_MAX)
		return mesh->batches.end;

	/* Get upper bound and insert new ID */
	new.upper = gfx_vector_get_size(&mesh->units);

	return gfx_vector_insert(
		&mesh->batches,
		&new,
		empty
	);
}

/******************************************************/
unsigned int _gfx_submesh_get_batch(

		GFXSubMesh*           mesh,
		GFXMaterial*          material,
		const GFX_BatchData*  data,
		unsigned int*         materialID)
{
	struct GFX_SubMesh* internal =
		(struct GFX_SubMesh*)mesh;
	struct GFX_Batch* batch =
		_gfx_submesh_find_batch(internal, material, data);

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
void _gfx_submesh_set_batch(

		GFXSubMesh*   mesh,
		unsigned int  submeshID,
		unsigned int  materialID)
{
	/* Bound check */
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	size_t max = gfx_vector_get_size(&internal->batches);

	if(submeshID && submeshID <= max)
	{
		/* Get batch and set */
		struct GFX_Batch* batch =
			gfx_vector_at(&internal->batches, submeshID - 1);

		batch->materialID = materialID;
	}
}

/******************************************************/
void _gfx_submesh_remove_batch(

		GFXSubMesh*   mesh,
		unsigned int  submeshID)
{
	/* Bound check */
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	size_t size = gfx_vector_get_size(&internal->batches);

	if(submeshID && submeshID <= size)
	{
		/* Get batch and bounds */
		struct GFX_Batch* batch =
			gfx_vector_at(&internal->batches, submeshID - 1);

		unsigned int begin;
		unsigned int end;
		_gfx_submesh_get_batch_units_bounds(
			internal,
			batch,
			&begin,
			&end
		);

		GFXBatch bat;
		bat.material   = batch->material;
		bat.materialID = batch->materialID;
		bat.submesh    = mesh;
		bat.submeshID  = submeshID;

		/* Iterate through all buckets */
		while(begin < end)
		{
			struct GFX_Units* it =
				gfx_vector_at(&internal->units, begin);
			size_t size =
				sizeof(struct GFX_Units) +
				sizeof(GFXBucketUnit) * it->units;

			/* Destroy units of the bucket */
			/* Which will in turn destroy the unit handle */
			gfx_batch_set_instances(&bat, it->pipe, 0);
			end -= size;
		}

		/* Mark as empty and remove trailing empty batches */
		memset(batch, 0, sizeof(struct GFX_Batch));

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
unsigned int _gfx_submesh_get_batch_units(

		GFXSubMesh*   mesh,
		unsigned int  submeshID)
{
	/* Bound check */
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	size_t max = gfx_vector_get_size(&internal->batches);

	if(!submeshID || submeshID > max) return 0;

	/* Get batch */
	struct GFX_Batch* batch =
		gfx_vector_at(&internal->batches, submeshID - 1);

	return batch->units;
}

/******************************************************/
const GFX_BatchData* _gfx_submesh_get_batch_data(

		GFXSubMesh*   mesh,
		unsigned int  submeshID)
{
	/* Bound check */
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	size_t max = gfx_vector_get_size(&internal->batches);

	if(!submeshID || submeshID > max) return NULL;

	/* Get batch */
	struct GFX_Batch* batch =
		gfx_vector_at(&internal->batches, submeshID - 1);

	return batch->material ? &batch->data : NULL;
}

/******************************************************/
void _gfx_submesh_set_batch_data(

		GFXSubMesh*    mesh,
		unsigned int   submeshID,
		GFXBatchFlags  flags,
		GFXBatchState  base,
		GFXBatchState  variant,
		unsigned int   copy)
{
	/* Bound check */
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	size_t max = gfx_vector_get_size(&internal->batches);

	if(submeshID && submeshID <= max)
	{
		/* Get batch */
		struct GFX_Batch* batch =
			gfx_vector_at(&internal->batches, submeshID - 1);

		if(batch->material)
		{
			/* Get difference and apply as appropriate */
			GFXBatchFlags diff =
				flags ^ batch->data.flags ||
				batch->data.base != base ||
				batch->data.variant != variant;

			batch->data.flags   = flags;
			batch->data.base    = base;
			batch->data.variant = variant;

			if(diff) _gfx_submesh_set_unit_data(mesh, submeshID, copy);
		}
	}
}

/******************************************************/
void _gfx_submesh_set_unit_data(

		GFXSubMesh*   mesh,
		unsigned int  submeshID,
		unsigned int  copy)
{
	/* Get batch and bounds */
	struct GFX_SubMesh* internal =
		(struct GFX_SubMesh*)mesh;
	struct GFX_Batch* batch =
		gfx_vector_at(&internal->batches, submeshID - 1);

	unsigned int begin;
	unsigned int end;
	_gfx_submesh_get_batch_units_bounds(internal, batch, &begin, &end);

	/* Iterate through all buckets */
	while(begin < end)
	{
		struct GFX_Units* it = gfx_vector_at(&internal->units, begin);

		/* Set the data of all units */
		_gfx_batch_set_unit_data(
			it->pipe->bucket,
			(GFXBucketUnit*)(it + 1),
			&batch->data,
			it->units,
			copy,
			0);

		begin +=
			sizeof(struct GFX_Units) +
			sizeof(GFXBucketUnit) * it->units;
	}
}

/******************************************************/
int _gfx_submesh_find_units(

		GFXSubMesh*    mesh,
		unsigned int   submeshID,
		GFXPipe*       pipe,
		unsigned int*  handle)
{
	/* Bound check */
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	size_t max = gfx_vector_get_size(&internal->batches);

	if(!submeshID || submeshID > max)
		return 0;

	/* Get batch and search */
	unsigned int begin;
	unsigned int end;

	struct GFX_Batch* batch = gfx_vector_at(
		&internal->batches, submeshID - 1);
	_gfx_submesh_get_batch_units_bounds(
		internal, batch, &begin, &end);
	begin = _gfx_submesh_find_batch_units(
		internal, begin, end, pipe);

	if(begin < end)
	{
		*handle = begin;
		return 1;
	}

	return 0;
}

/******************************************************/
int _gfx_submesh_insert_units(

		GFXSubMesh*    mesh,
		unsigned int   submeshID,
		GFXPipe*       pipe,
		unsigned int*  handle)
{
	/* Bound check and validate pipe type */
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	size_t max = gfx_vector_get_size(&internal->batches);

	if(!submeshID || submeshID > max || gfx_pipe_get_type(pipe) != GFX_PIPE_BUCKET)
		return 0;

	/* Get batch */
	struct GFX_Batch* batch =
		gfx_vector_at(&internal->batches, submeshID - 1);

	if(!batch->material)
		return 0;

	/* Create new bucket */
	/* Insert it at the end so we can rebuild */
	struct GFX_Units insert;
	insert.pipe      = pipe;
	insert.instances = 0;
	insert.visible   = 0;
	insert.units     = 0;

	struct GFX_Units* it = gfx_vector_insert_range_at(
		&internal->units,
		sizeof(struct GFX_Units),
		&insert,
		batch->upper
	);

	if(it != internal->units.end)
	{
		/* Reference the bucket */
		if(_gfx_submesh_reference_bucket(internal, pipe))
		{
			/* Increase bounds at last */
			*handle = batch->upper;
			_gfx_submesh_increase_batch_units_bounds(
				internal, batch, sizeof(struct GFX_Units));

			return 1;
		}

		/* Failed, erase bucket */
		gfx_vector_erase_range(
			&internal->units, sizeof(struct GFX_Units), it);
	}

	return 0;
}

/******************************************************/
void _gfx_submesh_remove_units(

		GFXSubMesh*   mesh,
		unsigned int  submeshID,
		GFXPipe*      pipe)
{
	/* Find it */
	unsigned int handle;
	if(_gfx_submesh_find_units(mesh, submeshID, pipe, &handle))
	{
		struct GFX_SubMesh* internal =
			(struct GFX_SubMesh*)mesh;
		struct GFX_Batch* batch =
			gfx_vector_at(&internal->batches, submeshID - 1);
		struct GFX_Units* it =
			gfx_vector_at(&internal->units, handle);

		size_t size =
			sizeof(struct GFX_Units) +
			sizeof(GFXBucketUnit) * it->units;

		/* Fix bounds, dereference bucket and update max units */
		gfx_vector_erase_range(
			&internal->units, size, it);
		_gfx_submesh_increase_batch_units_bounds(
			internal, batch, -(long int)size);
		_gfx_submesh_update_batch_units(
			internal, batch);
		_gfx_submesh_dereference_bucket(
			internal, pipe);
	}
}

/******************************************************/
GFXBucketUnit* _gfx_submesh_reserve(

		GFXSubMesh*   mesh,
		unsigned int  submeshID,
		unsigned int  handle,
		unsigned int  units)
{
	/* Get bucket, batch & difference */
	struct GFX_SubMesh* internal =
		(struct GFX_SubMesh*)mesh;
	struct GFX_Batch* batch =
		gfx_vector_at(&internal->batches, submeshID - 1);
	struct GFX_Units* it =
		gfx_vector_at(&internal->units, handle);

	long int diff =
		(long int)units - (long int)it->units;
	long int diffBytes =
		(long int)sizeof(GFXBucketUnit) * diff;

	if(diff)
	{
		it->units = units;

		if(diff > 0)
		{
			/* Insert new units */
			if(gfx_vector_insert_range_at(
				&internal->units,
				diffBytes,
				NULL,
				batch->upper) == internal->units.end)
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
				&internal->units,
				-diffBytes,
				batch->upper + diffBytes);

			/* Recompute the max units at the batch */
			_gfx_submesh_update_batch_units(
				internal,
				batch);
		}

		/* Adjust bounds */
		_gfx_submesh_increase_batch_units_bounds(
			internal,
			batch,
			diffBytes
		);

		it = gfx_vector_at(&internal->units, handle);
	}

	return (GFXBucketUnit*)(it + 1);
}

/******************************************************/
GFXBucketUnit* _gfx_submesh_get_reserved(

		GFXSubMesh*    mesh,
		unsigned int   handle,
		unsigned int*  units)
{
	/* Get bucket */
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	struct GFX_Units* it = gfx_vector_at(&internal->units, handle);

	*units = it->units;
	return (GFXBucketUnit*)(it + 1);
}

/******************************************************/
void _gfx_submesh_set_instances(

		GFXSubMesh*   mesh,
		unsigned int  handle,
		unsigned int  instances)
{
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	struct GFX_Units* it = gfx_vector_at(&internal->units, handle);

	it->instances = instances;
	it->visible = (instances < it->visible) ? instances : it->visible;
}

/******************************************************/
unsigned int _gfx_submesh_get_instances(

		GFXSubMesh*   mesh,
		unsigned int  handle)
{
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;

	return ((struct GFX_Units*)
		gfx_vector_at(&internal->units, handle))->instances;
}

/******************************************************/
unsigned int _gfx_submesh_set_visible(

		GFXSubMesh*   mesh,
		unsigned int  handle,
		unsigned int  instances)
{
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	struct GFX_Units* it = gfx_vector_at(&internal->units, handle);

	return it->visible =
		(it->instances < instances) ?
		it->instances : instances;
}


/******************************************************/
unsigned int _gfx_submesh_get_visible(

		GFXSubMesh*   mesh,
		unsigned int  handle)
{
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;

	return ((struct GFX_Units*)
		gfx_vector_at(&internal->units, handle))->visible;
}

/******************************************************/
GFXSubMesh* _gfx_submesh_create(

		unsigned char  drawCalls,
		unsigned char  sources)
{
	if(!sources) return NULL;

	/* Allocate submesh */
	size_t size = sizeof(struct GFX_SubMesh) + sources * sizeof(GFXVertexSource);
	struct GFX_SubMesh* sub = malloc(size);

	if(!sub)
	{
		/* Out of memory error */
		gfx_errors_push(
			GFX_ERROR_OUT_OF_MEMORY,
			"SubMesh could not be allocated."
		);
		return NULL;
	}

	/* Create layout */
	sub->submesh.layout = gfx_vertex_layout_create(drawCalls);
	if(!sub->submesh.layout)
	{
		free(sub);
		return NULL;
	}

	/* Initialize */
	sub->references = 1;
	sub->submesh.sources = sources;

	gfx_vector_init(
		&sub->batches, sizeof(struct GFX_Batch));
	gfx_vector_init(
		&sub->units, 1);
	gfx_vector_init(
		&sub->buckets,
		sizeof(struct GFX_Bucket) + sources * sizeof(GFXBucketSource));
	gfx_vector_init(
		&sub->buffers, sizeof(GFXSharedBuffer));

	return (GFXSubMesh*)sub;
}

/******************************************************/
int _gfx_submesh_reference(

		GFXSubMesh* mesh)
{
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;

	if(!(internal->references + 1))
	{
		/* Overflow error */
		gfx_errors_push(
			GFX_ERROR_OVERFLOW,
			"Overflow occurred during SubMesh referencing."
		);
		return 0;
	}

	++internal->references;
	return 1;
}

/******************************************************/
void _gfx_submesh_free(

		GFXSubMesh* mesh)
{
	if(mesh)
	{
		struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;

		/* Check references */
		if(!(--internal->references))
		{
			/* Remove all batches */
			unsigned int b;
			for(b = gfx_vector_get_size(&internal->batches); b > 0; --b)
			{
				struct GFX_Batch* it = gfx_vector_at(
					&internal->batches,
					b - 1);

				if(it->material)
				{
					GFXMaterial* mat = it->material;
					unsigned int matID = it->materialID;

					_gfx_submesh_remove_batch(mesh, b);
					_gfx_material_remove_batch(mat, matID);
				}
			}

			/* Clear all shared buffers */
			GFXVectorIterator it;
			for(
				it = internal->buffers.begin;
				it != internal->buffers.end;
				it = gfx_vector_next(&internal->buffers, it))
			{
				gfx_shared_buffer_clear((GFXSharedBuffer*)it);
			}

			/* Free everything */
			gfx_vector_clear(&internal->batches);
			gfx_vector_clear(&internal->units);
			gfx_vector_clear(&internal->buckets);
			gfx_vector_clear(&internal->buffers);

			gfx_vertex_layout_free(mesh->layout);
			free(mesh);
		}
	}
}

/******************************************************/
GFXBucketSource _gfx_submesh_get_bucket_source(

		GFXSubMesh*    mesh,
		GFXPipe*       pipe,
		unsigned char  index)
{
	/* Find bucket and validate index */
	struct GFX_SubMesh* internal =
		(struct GFX_SubMesh*)mesh;
	struct GFX_Bucket* bucket =
		_gfx_submesh_find_bucket(internal, pipe);

	if(bucket == internal->buckets.end || index >= mesh->sources)
		return 0;

	/* Fetch the source */
	GFXBucketSource* src = _gfx_submesh_get_src(bucket, index);

	if(!(*src))
	{
		/* Add and set source of bucket if it doesn't exist yet */
		*src = gfx_bucket_add_source(
			pipe->bucket,
			mesh->layout);

		gfx_bucket_set_source(
			pipe->bucket,
			*src,
			((GFXVertexSource*)(internal + 1))[index]);
	}

	return *src;
}

/******************************************************/
int gfx_submesh_set_source(

		GFXSubMesh*      mesh,
		unsigned char    index,
		GFXVertexSource  source)
{
	/* Validate index */
	if(index >= mesh->sources) return 0;

	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	((GFXVertexSource*)(internal + 1))[index] = source;

	/* Update all buckets */
	GFXVectorIterator it;
	for(
		it = internal->buckets.begin;
		it != internal->buckets.end;
		it = gfx_vector_next(&internal->buckets, it))
	{
		gfx_bucket_set_source(
			((struct GFX_Bucket*)it)->pipe->bucket,
			*_gfx_submesh_get_src(it, index),
			source
		);
	}

	return 1;
}

/******************************************************/
GFXVertexSource gfx_submesh_get_source(

		GFXSubMesh*    mesh,
		unsigned char  index)
{
	/* Validate index */
	if(index >= mesh->sources)
	{
		GFXVertexSource ret;
		memset(&ret, 0, sizeof(GFXVertexSource));

		return ret;
	}

	/* Fetch source */
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	return ((GFXVertexSource*)(internal + 1))[index];
}

/******************************************************/
GFXSubMeshBuffer gfx_submesh_add_buffer(

		GFXSubMesh*      mesh,
		GFXBufferTarget  target,
		size_t           size,
		const void*      data)
{
	/* Overflow */
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	size_t max = gfx_vector_get_size(&internal->buffers);

	if(max == UINT_MAX) return 0;

	/* Insert new vector element */
	GFXVectorIterator it = gfx_vector_insert(
		&internal->buffers,
		NULL,
		internal->buffers.end
	);

	if(it == internal->buffers.end) return 0;

	/* Create new shared buffer */
	GFXSharedBuffer* buff = (GFXSharedBuffer*)it;
	if(!gfx_shared_buffer_init(buff, target, size, data))
	{
		gfx_vector_erase(&internal->buffers, it);
		return 0;
	}

	return max + 1;
}

/******************************************************/
int gfx_submesh_set_attribute_buffer(

		GFXSubMesh*       mesh,
		unsigned int      index,
		GFXSubMeshBuffer  buffer,
		size_t            offset)
{
	/* Validate index */
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	if(buffer > gfx_vector_get_size(&internal->buffers))
	{
		/* Disable the attribute */
		gfx_vertex_layout_set_attribute_shared_buffer(
			mesh->layout,
			index,
			NULL,
			0
		);
		return 0;
	}

	/* Pass buffer to vertex layout */
	GFXSharedBuffer* buff = (GFXSharedBuffer*)gfx_vector_at(
		&internal->buffers,
		buffer - 1
	);

	return gfx_vertex_layout_set_attribute_shared_buffer(
		mesh->layout,
		index,
		buff,
		offset
	);
}

/******************************************************/
int gfx_submesh_set_index_buffer(

		GFXSubMesh*       mesh,
		GFXSubMeshBuffer  buffer,
		size_t            offset)
{
	/* Validate index */
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	if(buffer > gfx_vector_get_size(&internal->buffers))
	{
		/* Disable the attribute */
		gfx_vertex_layout_set_index_shared_buffer(
			mesh->layout,
			NULL,
			0
		);
		return 0;
	}

	/* Pass buffer to vertex layout */
	GFXSharedBuffer* buff = (GFXSharedBuffer*)gfx_vector_at(
		&internal->buffers,
		buffer - 1
	);

	gfx_vertex_layout_set_index_shared_buffer(
		mesh->layout,
		buff,
		offset
	);

	return 1;
}
