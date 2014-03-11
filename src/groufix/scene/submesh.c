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

#include "groufix/scene/mesh.h"
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
	GFXVector     buckets;    /* Stores GFXPipe* */
	GFXVector     sources;    /* Stores size_t * sources for each bucket */
};

/******************************************************/
static inline GFXVectorIterator _gfx_submesh_get_src(struct GFX_SubMesh* mesh, size_t bucketIndex, unsigned char index)
{
	return gfx_vector_at(&mesh->sources, bucketIndex * mesh->submesh.sources + index);
}

/******************************************************/
static GFXVectorIterator _gfx_submesh_find_bucket(struct GFX_SubMesh* mesh, GFXPipe* pipe)
{
	GFXVectorIterator it;
	for(it = mesh->buckets.begin; it != mesh->buckets.end; it = gfx_vector_next(&mesh->buckets, it))
		if(*(GFXPipe**)it == pipe) break;

	return it;
}

/******************************************************/
static void _gfx_submesh_remove_bucket(struct GFX_SubMesh* mesh, size_t bucketIndex)
{
	GFXVectorIterator pipe = gfx_vector_at(&mesh->buckets, bucketIndex);
	GFXVectorIterator src = gfx_vector_at(&mesh->sources, bucketIndex * mesh->submesh.sources);

	/* Remove all the sources */
	size_t srcInd;
	for(srcInd = 0; srcInd < mesh->submesh.sources; ++srcInd)
	{
		gfx_bucket_remove_source(
			(*(GFXPipe**)pipe)->bucket,
			*(size_t*)gfx_vector_advance(&mesh->sources, src, srcInd)
		);
	}

	/* Erase from vectors */
	gfx_vector_erase(&mesh->buckets, pipe);
	gfx_vector_erase_range(&mesh->sources, mesh->submesh.sources, src);
}

/******************************************************/
GFXSubMesh* _gfx_submesh_create(unsigned char drawCalls, unsigned char sources)
{
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

	gfx_vector_init(&sub->buckets, sizeof(GFXPipe*));
	gfx_vector_init(&sub->sources, sizeof(size_t));

	return (GFXSubMesh*)sub;
}

/******************************************************/
int _gfx_submesh_reference(GFXSubMesh* mesh)
{
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;

	if(!(internal->references + 1)) return 0;
	++internal->references;

	return 1;
}

/******************************************************/
void _gfx_submesh_free(GFXSubMesh* mesh)
{
	if(mesh)
	{
		struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;

		/* Check references */
		if(!(--internal->references))
		{
			/* Remove all buckets (in backwards order to be memory friendly) */
			size_t bucketIndex = gfx_vector_get_size(&internal->buckets);
			while(bucketIndex) _gfx_submesh_remove_bucket(internal, --bucketIndex);

			gfx_vector_clear(&internal->buckets);
			gfx_vector_clear(&internal->sources);

			gfx_vertex_layout_free(mesh->layout);
			free(mesh);
		}
	}
}

/******************************************************/
void _gfx_submesh_add_to_bucket(GFXSubMesh* mesh, GFXPipe* pipe)
{
	/* Validate pipe type */
	if(gfx_pipe_get_type(pipe) == GFX_PIPE_BUCKET)
	{
		/* See if it already exists */
		struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
		GFXVectorIterator it = _gfx_submesh_find_bucket(internal, pipe);

		/* Insert it if not */
		if(it == internal->buckets.end)
		{
			it = gfx_vector_insert(&internal->buckets, &pipe, internal->buckets.end);

			if(it != internal->buckets.end)
			{
				/* Insert a source ID for each submesh source */
				GFXVectorIterator itSrc = gfx_vector_insert_range(
					&internal->sources,
					mesh->sources,
					NULL,
					internal->sources.end);

				if(itSrc == internal->sources.end) gfx_vector_erase(&internal->buckets, it);

				/* Initialize source IDs to 0 */
				else memset(itSrc, 0, sizeof(size_t) * mesh->sources);
			}
		}
	}
}

/******************************************************/
void _gfx_submesh_remove_from_bucket(GFXSubMesh* mesh, GFXPipe* pipe)
{
	/* Find the bucket and remove it */
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	GFXVectorIterator it = _gfx_submesh_find_bucket(internal, pipe);

	if(it != internal->buckets.end)
	{
		_gfx_submesh_remove_bucket(internal, gfx_vector_get_index(&internal->buckets, it));
	}
}

/******************************************************/
size_t _gfx_submesh_get_bucket_source(GFXSubMesh* mesh, GFXPipe* pipe, unsigned char index)
{
	/* Find bucket and validate index */
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	GFXVectorIterator it = _gfx_submesh_find_bucket(internal, pipe);

	if(it == internal->buckets.end || index >= mesh->sources) return 0;

	/* Get the source ID */
	GFXVectorIterator itSrc = _gfx_submesh_get_src(
		internal,
		gfx_vector_get_index(&internal->buckets, it),
		index
	);

	size_t src = *(size_t*)itSrc;

	if(!src)
	{
		/* Add and set source of bucket if it doesn't exist yet */
		src = gfx_bucket_add_source(pipe->bucket, internal->submesh.layout);
		gfx_bucket_set_source(pipe->bucket, src, ((GFXVertexSource*)(internal + 1))[index]);

		*(size_t*)itSrc = src;
	}

	return src;
}

/******************************************************/
int gfx_submesh_set_source(GFXSubMesh* mesh, unsigned char index, GFXVertexSource source)
{
	/* Validate index */
	if(index >= mesh->sources) return 0;

	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	((GFXVertexSource*)(internal + 1))[index] = source;

	/* Update all buckets */
	size_t bucketInd = 0;
	GFXVectorIterator it;

	for(it = internal->buckets.begin; it != internal->buckets.end; it = gfx_vector_next(&internal->buckets, it))
	{
		size_t src = *(size_t*)_gfx_submesh_get_src(internal, bucketInd, index);
		gfx_bucket_set_source((*(GFXPipe**)it)->bucket, src, source);

		++bucketInd;
	}

	return 1;
}

/******************************************************/
GFXVertexSource gfx_submesh_get_source(GFXSubMesh* mesh, unsigned char index)
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
