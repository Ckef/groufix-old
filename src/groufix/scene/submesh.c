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

#include "groufix/containers/vector.h"
#include "groufix/core/errors.h"
#include "groufix/scene/mesh.h"

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
};

/* Internal bucket reference */
struct GFX_Bucket
{
	GFXBucket*    bucket;
	unsigned int  ref;   /* Reference count */
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
		GFXBucket*           bucket)
{
	struct GFX_Bucket* it;
	for(
		it = mesh->buckets.begin;
		it != mesh->buckets.end;
		it = gfx_vector_next(&mesh->buckets, it))
	{
		if(it->bucket == bucket) break;
	}

	return it;
}

/******************************************************/
static void _gfx_submesh_remove_bucket(

		struct GFX_SubMesh*  mesh,
		struct GFX_Bucket*   bucket)
{
	/* Remove all the sources */
	unsigned char s;
	for(s = 0; s < mesh->submesh.sources; ++s)
	{
		gfx_bucket_remove_source(
			bucket->bucket,
			*_gfx_submesh_get_src(bucket, s)
		);
	}

	/* Erase from vector */
	gfx_vector_erase(&mesh->buckets, bucket);
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
			/* Remove all buckets */
			size_t num = gfx_vector_get_size(&internal->buckets);
			while(num) _gfx_submesh_remove_bucket(
				internal,
				gfx_vector_at(&internal->buckets, --num)
			);

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
			gfx_vector_clear(&internal->buckets);
			gfx_vector_clear(&internal->buffers);

			gfx_vertex_layout_free(mesh->layout);
			free(mesh);
		}
	}
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
	struct GFX_Bucket* it;
	for(
		it = internal->buckets.begin;
		it != internal->buckets.end;
		it = gfx_vector_next(&internal->buckets, it))
	{
		gfx_bucket_set_source(
			it->bucket,
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
int gfx_submesh_add_bucket(

		GFXSubMesh*  mesh,
		GFXBucket*   bucket)
{
	/* See if it already exists */
	struct GFX_SubMesh* internal =
		(struct GFX_SubMesh*)mesh;
	struct GFX_Bucket* buck =
		_gfx_submesh_find_bucket(internal, bucket);

	if(buck == internal->buckets.end)
	{
		/* Insert bucket */
		buck = gfx_vector_insert(
			&internal->buckets,
			NULL,
			internal->buckets.end
		);

		if(buck == internal->buckets.end)
			return 0;

		/* Initialize bucket, reference and source IDs */
		memset(buck + 1, 0, sizeof(GFXBucketSource) * mesh->sources);

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
		};
		++buck->ref;
	}

	return 1;
}

/******************************************************/
GFXBucketSource gfx_submesh_get_bucket_source(

		GFXSubMesh*    mesh,
		GFXBucket*     bucket,
		unsigned char  index)
{
	/* Find bucket and validate index */
	struct GFX_SubMesh* internal =
		(struct GFX_SubMesh*)mesh;
	struct GFX_Bucket* buck =
		_gfx_submesh_find_bucket(internal, bucket);

	if(buck == internal->buckets.end || index >= mesh->sources)
		return 0;

	/* Fetch the source */
	GFXBucketSource* src = _gfx_submesh_get_src(buck, index);

	if(!(*src))
	{
		/* Add and set source of bucket if it doesn't exist yet */
		*src = gfx_bucket_add_source(
			bucket,
			mesh->layout);

		gfx_bucket_set_source(
			bucket,
			*src,
			((GFXVertexSource*)(internal + 1))[index]);
	}

	return *src;
}

/******************************************************/
int gfx_submesh_remove_bucket(

		GFXSubMesh*  mesh,
		GFXBucket*   bucket)
{
	/* Find the bucket */
	struct GFX_SubMesh* internal =
		(struct GFX_SubMesh*)mesh;
	struct GFX_Bucket* buck =
		_gfx_submesh_find_bucket(internal, bucket);

	if(buck == internal->buckets.end) return 0;

	/* Decrease reference counter */
	if(!(--buck->ref))
		_gfx_submesh_remove_bucket(internal, buck);

	return 1;
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
int gfx_submesh_set_vertex_buffer(

		GFXSubMesh*       mesh,
		unsigned int      index,
		GFXSubMeshBuffer  buffer,
		size_t            offset,
		size_t            stride)
{
	/* Validate index */
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	if(buffer > gfx_vector_get_size(&internal->buffers))
	{
		/* Disable the attribute */
		gfx_vertex_layout_set_shared_vertex_buffer(
			mesh->layout,
			index,
			NULL,
			0,
			0
		);
		return 0;
	}

	/* Pass buffer to vertex layout */
	GFXSharedBuffer* buff = (GFXSharedBuffer*)gfx_vector_at(
		&internal->buffers,
		buffer - 1
	);

	return gfx_vertex_layout_set_shared_vertex_buffer(
		mesh->layout,
		index,
		buff,
		offset,
		stride
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
		gfx_vertex_layout_set_shared_index_buffer(
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

	gfx_vertex_layout_set_shared_index_buffer(
		mesh->layout,
		buff,
		offset
	);

	return 1;
}
