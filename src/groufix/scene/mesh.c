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
#include "groufix/scene/internal.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

/******************************************************/
/* Internal mesh */
typedef struct GFX_Mesh
{
	/* Super class */
	GFX_LodMap lodMap;

	/* Hidden data */
	GFXVector  layouts; /* Stores GFXVertexLayout* */
	GFXVector  buckets; /* Stores (GFX_Bucket + GFXBucketSource * lodMap size) */
	GFXVector  buffers; /* Stores GFXSharedBuffer */

} GFX_Mesh;


/* Internal source data */
typedef struct GFX_SourceData
{
	GFXVertexSource  source;
	unsigned int     layout; /* Index into layouts (layout ID - 1) */

} GFX_SourceData;


/* Internal bucket reference */
typedef struct GFX_Bucket
{
	GFXBucket*    bucket;
	unsigned int  ref;   /* Reference count */

} GFX_Bucket;


/******************************************************/
static inline GFXVertexLayout* _gfx_mesh_get_layout(

		GFX_Mesh*     mesh,
		unsigned int  index)
{
	return *(GFXVertexLayout**)gfx_vector_at(&mesh->layouts, index);
}

/******************************************************/
static inline size_t _gfx_mesh_bucket_size(

		GFXMesh* mesh)
{
	return
		sizeof(GFX_Bucket) +
		sizeof(GFXBucketSource) *
		gfx_lod_map_count((GFXLodMap*)mesh, UINT_MAX);
}

/******************************************************/
static GFX_Bucket* _gfx_mesh_find_bucket(

		GFX_Mesh*   mesh,
		GFXBucket*  bucket,
		size_t      size)
{
	GFX_Bucket* it;
	for(
		it = mesh->buckets.begin;
		it != mesh->buckets.end;
		it = gfx_vector_advance(&mesh->buckets, it, size))
	{
		if(it->bucket == bucket) break;
	}

	return it;
}

/******************************************************/
static void _gfx_mesh_erase_bucket(

		GFX_Mesh*    mesh,
		GFX_Bucket*  bucket,
		size_t       size)
{
	/* Remove all the sources */
	size_t ind = sizeof(GFX_Bucket);
	GFXBucketSource* src = (GFXBucketSource*)(bucket + 1);

	while(ind < size)
	{
		gfx_bucket_remove_source(bucket->bucket, *(src++));
		ind += sizeof(GFXBucketSource);
	}

	/* Erase from vector */
	gfx_vector_erase_range(&mesh->buckets, size, bucket);
}

/******************************************************/
static int _gfx_mesh_expand_buckets(

		GFX_Mesh* mesh)
{
	size_t size = _gfx_mesh_bucket_size((GFXMesh*)mesh);
	size_t total = gfx_vector_get_size(&mesh->buckets);

	/* First reserve some memory */
	/* Size cannot be zero, so no worries */
	if(!gfx_vector_reserve(
		&mesh->buckets,
		total + (total / size) * sizeof(GFXBucketSource)))
	{
		return 0;
	}

	/* Append an empty source to all buckets */
	GFXBucketSource insert = 0;

	while(total)
	{
		gfx_vector_insert_range_at(
			&mesh->buckets,
			sizeof(GFXBucketSource),
			&insert,
			total
		);

		total -= size;
	}

	return 1;
}

/******************************************************/
static void _gfx_mesh_shrink_buckets(

		GFX_Mesh* mesh)
{
	/* Size will be smaller than the actual stored size in buckets */
	/* As this is only allowed to be called when the lod map is smaller */
	size_t size = _gfx_mesh_bucket_size((GFXMesh*)mesh);
	size_t total = gfx_vector_get_size(&mesh->buckets);

	/* Remove the last source of all buckets */
	size_t ind = size;

	while(ind < total)
	{
		gfx_vector_erase_range_at(
			&mesh->buckets,
			sizeof(GFXBucketSource),
			ind
		);

		total -= sizeof(GFXBucketSource);
		ind += size;
	}
}

/******************************************************/
int _gfx_mesh_add_bucket(

		GFXMesh*    mesh,
		GFXBucket*  bucket)
{
	/* See if it already exists */
	GFX_Mesh* internal =
		(GFX_Mesh*)mesh;
	size_t size =
		_gfx_mesh_bucket_size(mesh);
	GFX_Bucket* buck =
		_gfx_mesh_find_bucket(internal, bucket, size);

	if(buck == internal->buckets.end)
	{
		/* Insert bucket */
		buck = gfx_vector_insert_range(
			&internal->buckets,
			size,
			NULL,
			internal->buckets.end
		);

		if(buck == internal->buckets.end)
			return 0;

		/* Initialize bucket, reference and source IDs */
		memset(buck, 0, size);

		buck->bucket = bucket;
		buck->ref = 1;
	}
	else
	{
		/* Increase reference counter */
		if(!(buck->ref + 1))
		{
			/* Overflow error */
			gfx_errors_push(
				GFX_ERROR_OVERFLOW,
				"Overflow occurred during Mesh bucket referencing."
			);
			return 0;
		}
		++buck->ref;
	}

	return 1;
}

/******************************************************/
GFXBucketSource _gfx_mesh_get_bucket_source(

		GFXMesh*      mesh,
		GFXBucket*    bucket,
		unsigned int  index)
{
	/* Find bucket */
	GFX_Mesh* internal =
		(GFX_Mesh*)mesh;
	size_t size =
		_gfx_mesh_bucket_size(mesh);
	GFX_Bucket* buck =
		_gfx_mesh_find_bucket(internal, bucket, size);

	/* Validate index */
	unsigned int num;
	GFX_SourceData* list = gfx_lod_map_get_all(
		(GFXLodMap*)mesh,
		&num);

	if(buck == internal->buckets.end || index >= num)
		return 0;

	/* Fetch the source */
	GFXBucketSource* src = ((GFXBucketSource*)(buck + 1)) + index;

	if(!(*src))
	{
		/* Add and set source of bucket if it doesn't exist yet */
		GFXVertexLayout* layout = _gfx_mesh_get_layout(
			internal,
			list[index].layout);

		*src = gfx_bucket_add_source(
			bucket,
			layout);

		gfx_bucket_set_source(
			bucket,
			*src,
			list[index].source);
	}

	return *src;
}

/******************************************************/
int _gfx_mesh_remove_bucket(

		GFXMesh*    mesh,
		GFXBucket*  bucket)
{
	/* Find the bucket */
	GFX_Mesh* internal =
		(GFX_Mesh*)mesh;
	size_t size =
		_gfx_mesh_bucket_size(mesh);
	GFX_Bucket* buck =
		_gfx_mesh_find_bucket(internal, bucket, size);

	if(buck == internal->buckets.end) return 0;

	/* Decrease reference counter */
	if(!(--buck->ref))
		_gfx_mesh_erase_bucket(internal, buck, size);

	return 1;
}

/******************************************************/
GFXMesh* gfx_mesh_create(void)
{
	/* Allocate mesh */
	GFX_Mesh* mesh = malloc(sizeof(GFX_Mesh));
	if(!mesh)
	{
		/* Out of memory error */
		gfx_errors_push(
			GFX_ERROR_OUT_OF_MEMORY,
			"Mesh could not be allocated."
		);
		return NULL;
	}

	/* Initialize */
	_gfx_lod_map_init(
		(GFX_LodMap*)mesh,
		0,
		sizeof(GFX_SourceData),
		sizeof(GFX_SourceData)
	);

	gfx_vector_init(&mesh->layouts, sizeof(GFXVertexLayout*));
	gfx_vector_init(&mesh->buckets, 1);
	gfx_vector_init(&mesh->buffers, sizeof(GFXSharedBuffer));

	return (GFXMesh*)mesh;
}

/******************************************************/
void gfx_mesh_free(

		GFXMesh* mesh)
{
	if(mesh)
	{
		GFX_Mesh* internal = (GFX_Mesh*)mesh;

		/* Remove all buckets */
		size_t size = _gfx_mesh_bucket_size(mesh);
		size_t total = gfx_vector_get_size(&internal->buckets);

		while(total)
		{
			total -= size;
			_gfx_mesh_erase_bucket(
				internal,
				gfx_vector_at(&internal->buckets, total),
				size
			);
		}

		/* Free all layouts */
		GFXVectorIterator it;
		for(
			it = internal->layouts.begin;
			it != internal->layouts.end;
			it = gfx_vector_next(&internal->layouts, it))
		{
			gfx_vertex_layout_free(*(GFXVertexLayout**)it);
		}

		/* Clear all shared buffers */
		for(
			it = internal->buffers.begin;
			it != internal->buffers.end;
			it = gfx_vector_next(&internal->buffers, it))
		{
			gfx_shared_buffer_clear((GFXSharedBuffer*)it);
		}

		/* Free everything */
		gfx_vector_clear(&internal->layouts);
		gfx_vector_clear(&internal->buckets);
		gfx_vector_clear(&internal->buffers);

		_gfx_lod_map_clear((GFX_LodMap*)internal);
		free(mesh);
	}
}

/******************************************************/
GFXMeshLayout gfx_mesh_add_layout(

		GFXMesh*       mesh,
		unsigned char  drawCalls)
{
	/* Overflow */
	GFX_Mesh* internal = (GFX_Mesh*)mesh;
	size_t max = gfx_vector_get_size(&internal->layouts);

	if(max == UINT_MAX)
	{
		gfx_errors_push(
			GFX_ERROR_OVERFLOW,
			"Overflow occurred during layout creation at a Mesh."
		);
		return 0;
	}

	/* Insert new vector element */
	GFXVertexLayout** it = gfx_vector_insert(
		&internal->layouts,
		NULL,
		internal->layouts.end
	);

	if(it == internal->layouts.end) return 0;

	/* Create new layout */
	*it = gfx_vertex_layout_create(drawCalls);
	if(!(*it))
	{
		gfx_vector_erase(&internal->layouts, it);
		return 0;
	}

	return max + 1;
}

/******************************************************/
GFXVertexLayout* gfx_mesh_get_layout(

		GFXMesh*       mesh,
		GFXMeshLayout  layout)
{
	return _gfx_mesh_get_layout((GFX_Mesh*)mesh, layout - 1);
}

/******************************************************/
GFXMeshBuffer gfx_mesh_add_buffer(

		GFXMesh*         mesh,
		GFXBufferTarget  target,
		size_t           size,
		const void*      data)
{
	/* Overflow */
	GFX_Mesh* internal = (GFX_Mesh*)mesh;
	size_t max = gfx_vector_get_size(&internal->buffers);

	if(max == UINT_MAX)
	{
		gfx_errors_push(
			GFX_ERROR_OVERFLOW,
			"Overflow occurred during buffer creation at a Mesh."
		);
		return 0;
	}

	/* Insert new vector element */
	GFXSharedBuffer* it = gfx_vector_insert(
		&internal->buffers,
		NULL,
		internal->buffers.end
	);

	if(it == internal->buffers.end) return 0;

	/* Create new shared buffer */
	if(!gfx_shared_buffer_init(it, target, size, data))
	{
		gfx_vector_erase(&internal->buffers, it);
		return 0;
	}

	return max + 1;
}

/******************************************************/
int gfx_mesh_set_vertex_buffer(

		GFXMesh*       mesh,
		GFXMeshLayout  layout,
		GFXMeshBuffer  buffer,
		unsigned int   index,
		size_t         offset,
		size_t         stride)
{
	GFX_Mesh* internal =
		(GFX_Mesh*)mesh;
	GFXVertexLayout* lay =
		_gfx_mesh_get_layout(internal, layout - 1);

	/* Pass buffer to vertex layout */
	GFXSharedBuffer* buff = (GFXSharedBuffer*)gfx_vector_at(
		&internal->buffers,
		buffer - 1);

	return gfx_vertex_layout_set_shared_vertex_buffer(
		lay,
		index,
		buff,
		offset,
		stride);
}

/******************************************************/
void gfx_mesh_set_index_buffer(

		GFXMesh*       mesh,
		GFXMeshLayout  layout,
		GFXMeshBuffer  buffer,
		size_t         offset)
{
	GFX_Mesh* internal =
		(GFX_Mesh*)mesh;
	GFXVertexLayout* lay =
		_gfx_mesh_get_layout(internal, layout - 1);

	/* Pass buffer to vertex layout */
	GFXSharedBuffer* buff = (GFXSharedBuffer*)gfx_vector_at(
		&internal->buffers,
		buffer - 1);

	gfx_vertex_layout_set_shared_index_buffer(
		lay,
		buff,
		offset);
}

/******************************************************/
int gfx_mesh_add(

		GFXMesh*         mesh,
		unsigned int     level,
		GFXVertexSource  source,
		GFXMeshLayout    layout)
{
	--layout;

	/* Check draw call boundaries */
	GFXVertexLayout* lay = _gfx_mesh_get_layout(
		(GFX_Mesh*)mesh,
		layout);

	if(source.startDraw + source.numDraw > lay->drawCalls)
		return 0;

	/* First extend the bucket vector */
	if(!_gfx_mesh_expand_buckets((GFX_Mesh*)mesh))
		return 0;

	/* Memset so memcmp will be correct */
	GFX_SourceData data;
	memset(&data, 0, sizeof(GFX_SourceData));

	data.source.startDraw     = source.startDraw;
	data.source.numDraw       = source.numDraw;
	data.source.startFeedback = source.startFeedback;
	data.source.numFeedback   = source.numFeedback;

	data.layout = layout;

	/* Add it to the LOD map */
	if(!gfx_lod_map_add((GFXLodMap*)mesh, level, &data))
	{
		_gfx_mesh_shrink_buckets((GFX_Mesh*)mesh);
		return 0;
	}

	return 1;
}

/******************************************************/
GFXVertexSourceList gfx_mesh_get(

		GFXMesh*       mesh,
		unsigned int   level,
		unsigned int*  num)
{
	return gfx_lod_map_get((GFXLodMap*)mesh, level, num);
}

/******************************************************/
GFXVertexSourceList gfx_mesh_get_all(

		GFXMesh*       mesh,
		unsigned int*  num)
{
	return gfx_lod_map_get_all((GFXLodMap*)mesh, num);
}

/******************************************************/
GFXMeshLayout gfx_vertex_source_list_layout_at(

		GFXVertexSourceList  list,
		unsigned int         index)
{
	return ((GFX_SourceData*)list)[index].layout + 1;
}

/******************************************************/
GFXVertexSource gfx_vertex_source_list_at(

		GFXVertexSourceList  list,
		unsigned int         index)
{
	return ((GFX_SourceData*)list)[index].source;
}
