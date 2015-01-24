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
/* Internal submesh */
typedef struct GFX_SubMesh
{
	/* Super class */
	GFX_LodMap lodMap;

	/* Hidden data */
	unsigned int  references; /* Reference counter */
	GFXVector     layouts;    /* Stores GFXVertexLayout* */

	GFXVector     buckets;    /* Stores (GFX_Bucket + GFXBucketSource * lodMap size) */
	GFXVector     buffers;    /* Stores GFXSharedBuffer */

} GFX_SubMesh;


/* Internal source data */
typedef struct GFX_SourceData
{
	GFXVertexSource   source;
	unsigned int      layout; /* Index into layouts (layout ID - 1) */

} GFX_SourceData;


/* Internal bucket reference */
typedef struct GFX_Bucket
{
	GFXBucket*    bucket;
	unsigned int  ref;   /* Reference count */

} GFX_Bucket;


/******************************************************/
static inline GFXVertexLayout* _gfx_submesh_get_layout(

		GFX_SubMesh*  mesh,
		unsigned int  index)
{
	return *(GFXVertexLayout**)gfx_vector_at(&mesh->layouts, index);
}

/******************************************************/
static inline size_t _gfx_submesh_bucket_size(

		GFXSubMesh* mesh)
{
	return
		sizeof(GFX_Bucket) +
		sizeof(GFXBucketSource) *
		gfx_lod_map_count((GFXLodMap*)mesh, UINT_MAX);
}

/******************************************************/
static GFX_Bucket* _gfx_submesh_find_bucket(

		GFX_SubMesh*  mesh,
		GFXBucket*    bucket,
		size_t        size)
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
static void _gfx_submesh_erase_bucket(

		GFX_SubMesh*  mesh,
		GFX_Bucket*   bucket,
		size_t        size)
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
static int _gfx_submesh_expand_buckets(

		GFX_SubMesh* mesh)
{
	size_t size = _gfx_submesh_bucket_size((GFXSubMesh*)mesh);
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
static void _gfx_submesh_shrink_buckets(

		GFX_SubMesh* mesh)
{
	/* Size will be smaller than the actual stored size in buckets */
	/* As this is only allowed to be called when the lod map is smaller */
	size_t size = _gfx_submesh_bucket_size((GFXSubMesh*)mesh);
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
int _gfx_submesh_add_bucket(

		GFXSubMesh*  mesh,
		GFXBucket*   bucket)
{
	/* See if it already exists */
	GFX_SubMesh* internal =
		(GFX_SubMesh*)mesh;
	size_t size =
		_gfx_submesh_bucket_size(mesh);
	GFX_Bucket* buck =
		_gfx_submesh_find_bucket(internal, bucket, size);

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
				"Overflow occurred during SubMesh bucket referencing."
			);
			return 0;
		}
		++buck->ref;
	}

	return 1;
}

/******************************************************/
GFXBucketSource _gfx_submesh_get_bucket_source(

		GFXSubMesh*   mesh,
		GFXBucket*    bucket,
		unsigned int  index)
{
	/* Find bucket */
	GFX_SubMesh* internal =
		(GFX_SubMesh*)mesh;
	size_t size =
		_gfx_submesh_bucket_size(mesh);
	GFX_Bucket* buck =
		_gfx_submesh_find_bucket(internal, bucket, size);

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
		GFXVertexLayout* layout = _gfx_submesh_get_layout(
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
int _gfx_submesh_remove_bucket(

		GFXSubMesh*  mesh,
		GFXBucket*   bucket)
{
	/* Find the bucket */
	GFX_SubMesh* internal =
		(GFX_SubMesh*)mesh;
	size_t size =
		_gfx_submesh_bucket_size(mesh);
	GFX_Bucket* buck =
		_gfx_submesh_find_bucket(internal, bucket, size);

	if(buck == internal->buckets.end) return 0;

	/* Decrease reference counter */
	if(!(--buck->ref))
		_gfx_submesh_erase_bucket(internal, buck, size);

	return 1;
}

/******************************************************/
GFXSubMesh* _gfx_submesh_create(void)
{
	/* Allocate submesh */
	GFX_SubMesh* sub = malloc(sizeof(GFX_SubMesh));
	if(!sub)
	{
		/* Out of memory error */
		gfx_errors_push(
			GFX_ERROR_OUT_OF_MEMORY,
			"SubMesh could not be allocated."
		);
		return NULL;
	}

	/* Initialize */
	_gfx_lod_map_init(
		(GFX_LodMap*)sub,
		0,
		sizeof(GFX_SourceData),
		sizeof(GFX_SourceData)
	);

	sub->references = 1;

	gfx_vector_init(&sub->layouts, sizeof(GFXVertexLayout*));
	gfx_vector_init(&sub->buckets, 1);
	gfx_vector_init(&sub->buffers, sizeof(GFXSharedBuffer));

	return (GFXSubMesh*)sub;
}

/******************************************************/
int _gfx_submesh_reference(

		GFXSubMesh* mesh)
{
	GFX_SubMesh* internal = (GFX_SubMesh*)mesh;

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
	GFX_SubMesh* internal = (GFX_SubMesh*)mesh;

	/* Check references */
	if(mesh && !(--internal->references))
	{
		/* Remove all buckets */
		size_t size = _gfx_submesh_bucket_size(mesh);
		size_t total = gfx_vector_get_size(&internal->buckets);

		while(total)
		{
			total -= size;
			_gfx_submesh_erase_bucket(
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
GFXSubMeshLayout gfx_submesh_add_layout(

		GFXSubMesh*    mesh,
		unsigned char  drawCalls)
{
	/* Overflow */
	GFX_SubMesh* internal = (GFX_SubMesh*)mesh;
	size_t max = gfx_vector_get_size(&internal->layouts);

	if(max == UINT_MAX)
	{
		gfx_errors_push(
			GFX_ERROR_OVERFLOW,
			"Overflow occurred during layout creation at a SubMesh."
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
GFXVertexLayout* gfx_submesh_get_layout(

		GFXSubMesh*       mesh,
		GFXSubMeshLayout  layout)
{
	return _gfx_submesh_get_layout((GFX_SubMesh*)mesh, layout - 1);
}

/******************************************************/
GFXSubMeshBuffer gfx_submesh_add_buffer(

		GFXSubMesh*      mesh,
		GFXBufferTarget  target,
		size_t           size,
		const void*      data)
{
	/* Overflow */
	GFX_SubMesh* internal = (GFX_SubMesh*)mesh;
	size_t max = gfx_vector_get_size(&internal->buffers);

	if(max == UINT_MAX)
	{
		gfx_errors_push(
			GFX_ERROR_OVERFLOW,
			"Overflow occurred during buffer creation at a SubMesh."
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
int gfx_submesh_set_vertex_buffer(

		GFXSubMesh*       mesh,
		GFXSubMeshLayout  layout,
		GFXSubMeshBuffer  buffer,
		unsigned int      index,
		size_t            offset,
		size_t            stride)
{
	GFX_SubMesh* internal =
		(GFX_SubMesh*)mesh;
	GFXVertexLayout* lay =
		_gfx_submesh_get_layout(internal, layout - 1);

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
void gfx_submesh_set_index_buffer(

		GFXSubMesh*       mesh,
		GFXSubMeshLayout  layout,
		GFXSubMeshBuffer  buffer,
		size_t            offset)
{
	GFX_SubMesh* internal =
		(GFX_SubMesh*)mesh;
	GFXVertexLayout* lay =
		_gfx_submesh_get_layout(internal, layout - 1);

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
int gfx_submesh_add(

		GFXSubMesh*       mesh,
		unsigned int      level,
		GFXVertexSource   source,
		GFXSubMeshLayout  layout)
{
	--layout;

	/* Check draw call boundaries */
	GFXVertexLayout* lay = _gfx_submesh_get_layout(
		(GFX_SubMesh*)mesh,
		layout);

	if(source.startDraw + source.numDraw > lay->drawCalls)
		return 0;

	/* First extend the bucket vector */
	if(!_gfx_submesh_expand_buckets((GFX_SubMesh*)mesh))
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
		_gfx_submesh_shrink_buckets((GFX_SubMesh*)mesh);
		return 0;
	}

	return 1;
}

/******************************************************/
GFXVertexSourceList gfx_submesh_get(

		GFXSubMesh*    mesh,
		unsigned int   level,
		unsigned int*  num)
{
	return gfx_lod_map_get((GFXLodMap*)mesh, level, num);
}

/******************************************************/
GFXVertexSourceList gfx_submesh_get_all(

		GFXSubMesh*    mesh,
		unsigned int*  num)
{
	return gfx_lod_map_get_all((GFXLodMap*)mesh, num);
}

/******************************************************/
GFXSubMeshLayout gfx_vertex_source_list_layout_at(

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
