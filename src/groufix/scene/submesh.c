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
#include "groufix/containers/vector.h"

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
	GFXVector     buckets;    /* Stores (GFX_BucketRef + size_t * sources) */
	GFXVector     buffers;    /* Stores GFXSharedBuffer */
};

/* Internal bucket reference */
struct GFX_BucketRef
{
	GFXPipe*  pipe;
	size_t    ref; /* Reference count */
};

/******************************************************/
static inline size_t* _gfx_submesh_get_src(

		struct GFX_BucketRef*  bucket,
		unsigned char          index)
{
	return ((size_t*)(bucket + 1)) + index;
}

/******************************************************/
static struct GFX_BucketRef* _gfx_submesh_find_bucket(

		struct GFX_SubMesh*  mesh,
		GFXPipe*             pipe)
{
	GFXVectorIterator it;
	for(
		it = mesh->buckets.begin;
		it != mesh->buckets.end;
		it = gfx_vector_next(&mesh->buckets, it))
	{
		if(((struct GFX_BucketRef*)it)->pipe == pipe) break;
	}

	return (struct GFX_BucketRef*)it;
}

/******************************************************/
static void _gfx_submesh_erase_bucket(

		struct GFX_SubMesh*    mesh,
		struct GFX_BucketRef*  bucket)
{
	/* Remove all the sources */
	size_t srcInd;
	for(srcInd = 0; srcInd < mesh->submesh.sources; ++srcInd)
	{
		gfx_bucket_remove_source(
			bucket->pipe->bucket,
			*_gfx_submesh_get_src(bucket, srcInd)
		);
	}

	/* Unregister submesh at pipe */
	GFXPipeCallback call;
	call.key = GFX_SCENE_KEY_SUBMESH;
	call.data = mesh;

	gfx_pipe_unregister(bucket->pipe, call);

	/* Erase from vector */
	gfx_vector_erase(&mesh->buckets, bucket);
}

/******************************************************/
static void _gfx_submesh_callback(

		GFXPipe*          pipe,
		GFXPipeCallback*  callback)
{
	struct GFX_SubMesh* sub = callback->data;

	/* Find the pipe and erase it */
	struct GFX_BucketRef* ref = _gfx_submesh_find_bucket(sub, pipe);

	if(ref != sub->buckets.end)
		gfx_vector_erase(&sub->buckets, ref);
}

/******************************************************/
GFXSubMesh* _gfx_submesh_create(

		unsigned char  drawCalls,
		unsigned char  sources)
{
	if(!sources) return NULL;

	/* Allocate submesh */
	size_t size = sizeof(struct GFX_SubMesh) + sources * sizeof(GFXVertexSource);

	struct GFX_SubMesh* sub = calloc(1, size);
	if(!sub) return NULL;

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

	gfx_vector_init(&sub->buckets, sizeof(struct GFX_BucketRef) + sources * sizeof(size_t));
	gfx_vector_init(&sub->buffers, sizeof(GFXSharedBuffer));

	return (GFXSubMesh*)sub;
}

/******************************************************/
int _gfx_submesh_reference(

		GFXSubMesh* mesh)
{
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;

	if(!(internal->references + 1)) return 0;
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
			/* Erase all buckets */
			while(internal->buckets.begin != internal->buckets.end)
				_gfx_submesh_erase_bucket(
					internal,
					internal->buckets.begin
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
int _gfx_submesh_reference_bucket(

		GFXSubMesh*  mesh,
		GFXPipe*     pipe)
{
	/* Validate pipe type */
	if(gfx_pipe_get_type(pipe) != GFX_PIPE_BUCKET) return 0;

	/* See if it already exists */
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	struct GFX_BucketRef* bucket = _gfx_submesh_find_bucket(internal, pipe);

	if(bucket == internal->buckets.end)
	{
		/* Insert bucket */
		bucket = gfx_vector_insert(
			&internal->buckets,
			NULL,
			internal->buckets.end
		);

		if(bucket == internal->buckets.end) return 0;

		/* Register submesh at pipe */
		GFXPipeCallback call;
		call.key = GFX_SCENE_KEY_SUBMESH;
		call.data = internal;

		if(!gfx_pipe_register(pipe, call, _gfx_submesh_callback))
		{
			/* Failed to register, erase bucket */
			gfx_vector_erase(&internal->buckets, bucket);
			return 0;
		}

		/* Initialize bucket, reference and source IDs */
		memset(bucket + 1, 0, sizeof(size_t) * mesh->sources);

		bucket->pipe = pipe;
		bucket->ref = 1;
	}
	else
	{
		/* Increase reference counter */
		if(!(bucket->ref + 1)) return 0;
		++bucket->ref;
	}

	return 1;
}

/******************************************************/
void _gfx_submesh_remove_bucket(

		GFXSubMesh*  mesh,
		GFXPipe*     pipe)
{
	/* Find the bucket and remove it */
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	struct GFX_BucketRef* bucket = _gfx_submesh_find_bucket(internal, pipe);

	if(bucket != internal->buckets.end)
	{
		/* Decrease reference counter */
		if(!(--bucket->ref))
			_gfx_submesh_erase_bucket(internal, bucket);
	}
}

/******************************************************/
size_t _gfx_submesh_get_bucket_source(

		GFXSubMesh*    mesh,
		GFXPipe*       pipe,
		unsigned char  index)
{
	/* Find bucket and validate index */
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	struct GFX_BucketRef* bucket = _gfx_submesh_find_bucket(internal, pipe);

	if(bucket == internal->buckets.end || index >= mesh->sources) return 0;

	/* Get the source ID */
	size_t* srcPtr = _gfx_submesh_get_src(bucket, index);
	size_t src = *srcPtr;

	if(!src)
	{
		/* Add and set source of bucket if it doesn't exist yet */
		src = gfx_bucket_add_source(
			pipe->bucket,
			internal->submesh.layout);

		gfx_bucket_set_source(
			pipe->bucket,
			src,
			((GFXVertexSource*)(internal + 1))[index]);

		*srcPtr = src;
	}

	return src;
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
			((struct GFX_BucketRef*)it)->pipe->bucket,
			*_gfx_submesh_get_src(it, index),
			source);
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
size_t gfx_submesh_add_buffer(

		GFXSubMesh*      mesh,
		GFXBufferTarget  target,
		size_t           size,
		const void*      data)
{
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;

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

	return gfx_vector_get_size(&internal->buffers);
}

/******************************************************/
int gfx_submesh_set_attribute_buffer(

		GFXSubMesh*   mesh,
		unsigned int  index,
		size_t        buffer,
		size_t        offset)
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

		GFXSubMesh*  mesh,
		size_t       buffer,
		size_t       offset)
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
