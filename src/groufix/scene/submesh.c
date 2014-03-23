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
	unsigned int  references;  /* Reference counter */
	GFXVector     buckets;     /* Stores GFXPipe* */
	GFXVector     sources;     /* Stores size_t * sources for each bucket */

	GFXVector     vertexBuffs; /* Stores GFXSharedBuffer */
	GFXVector     indexBuffs;  /* Stores GFXSharedBuffer */
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

	/* Unregister submesh at pipe */
	GFXPipeCallback call;
	call.key = GFX_SCENE_KEY_SUBMESH;
	call.data = mesh;

	gfx_pipe_unregister(*(GFXPipe**)pipe, call);

	/* Erase from vectors */
	gfx_vector_erase(&mesh->buckets, pipe);
	gfx_vector_erase_range(&mesh->sources, mesh->submesh.sources, src);
}

/******************************************************/
static void _gfx_submesh_callback(GFXPipe* pipe, GFXPipeCallback* callback)
{
	struct GFX_SubMesh* sub = callback->data;

	/* Find the pipe and source IDs */
	GFXVectorIterator it = _gfx_submesh_find_bucket(sub, pipe);
	if(it != sub->buckets.end)
	{
		size_t index = gfx_vector_get_index(&sub->buckets, it);

		GFXVectorIterator src = gfx_vector_at(&sub->sources, index * sub->submesh.sources);

		/* Erase them from vectors */
		gfx_vector_erase(&sub->buckets, it);
		gfx_vector_erase_range(&sub->sources, sub->submesh.sources, src);
	}
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
	gfx_vector_init(&sub->vertexBuffs, sizeof(GFXSharedBuffer));
	gfx_vector_init(&sub->indexBuffs, sizeof(GFXSharedBuffer));

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

			/* Clear all shared buffers */
			GFXVectorIterator it;
			for(
				it = internal->vertexBuffs.begin;
				it != internal->vertexBuffs.end;
				it = gfx_vector_next(&internal->vertexBuffs, it))
			{
				gfx_shared_buffer_clear((GFXSharedBuffer*)it);
			}
			for(
				it = internal->indexBuffs.begin;
				it != internal->indexBuffs.end;
				it = gfx_vector_next(&internal->indexBuffs, it))
			{
				gfx_shared_buffer_clear((GFXSharedBuffer*)it);
			}

			/* Free everything */
			gfx_vector_clear(&internal->buckets);
			gfx_vector_clear(&internal->sources);
			gfx_vector_clear(&internal->vertexBuffs);
			gfx_vector_clear(&internal->indexBuffs);

			gfx_vertex_layout_free(mesh->layout);
			free(mesh);
		}
	}
}

/******************************************************/
void _gfx_submesh_add_to_bucket(GFXSubMesh* mesh, GFXPipe* pipe)
{
	/* Validate pipe type */
	if(gfx_pipe_get_type(pipe) != GFX_PIPE_BUCKET) return;

	/* See if it already exists */
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	GFXVectorIterator it = _gfx_submesh_find_bucket(internal, pipe);

	/* Insert it if not */
	if(it == internal->buckets.end)
	{
		/* Register submesh at pipe */
		GFXPipeCallback call;
		call.key = GFX_SCENE_KEY_SUBMESH;
		call.data = internal;

		if(gfx_pipe_register(pipe, call, _gfx_submesh_callback))
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

				if(itSrc != internal->sources.end)
				{
					/* Initialize source IDs to 0 */
					memset(itSrc, 0, sizeof(size_t) * mesh->sources);

					/* Done */
					return;
				}

				/* Failure, erase bucket */
				gfx_vector_erase(&internal->buckets, it);
			}

			/* Failure, unregister */
			gfx_pipe_unregister(pipe, call);
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

/******************************************************/
static size_t _gfx_submesh_add_buffer(GFXVector* buffs, GFXBufferTarget target, size_t size, const void* data)
{
	/* Insert new vector element */
	GFXVectorIterator it = gfx_vector_insert(buffs, NULL, buffs->end);
	if(it == buffs->end) return 0;

	/* Create new shared buffer */
	GFXSharedBuffer* buff = (GFXSharedBuffer*)it;
	if(!gfx_shared_buffer_init(buff, target, size, data))
	{
		gfx_vector_erase(buffs, it);
		return 0;
	}

	return gfx_vector_get_size(buffs);
}

/******************************************************/
size_t gfx_submesh_add_attribute_buffer(GFXSubMesh* mesh, size_t size, const void* data)
{
	/* Create vertex buffer */
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	return _gfx_submesh_add_buffer(&internal->vertexBuffs, GFX_VERTEX_BUFFER, size, data);
}

/******************************************************/
size_t gfx_submesh_add_draw_call_buffer(GFXSubMesh* mesh, size_t size, const void* data)
{
	/* Create index buffer */
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	return _gfx_submesh_add_buffer(&internal->indexBuffs, GFX_INDEX_BUFFER, size, data);
}

/******************************************************/
int gfx_submesh_set_attribute_buffer(GFXSubMesh* mesh, unsigned int index, size_t buffer, size_t offset)
{
	/* Validate index */
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	if(buffer > gfx_vector_get_size(&internal->vertexBuffs))
	{
		/* Disable the attribute */
		gfx_vertex_layout_set_attribute_shared_buffer(mesh->layout, index, NULL, 0);
		return 0;
	}

	/* Pass buffer to vertex layout */
	GFXSharedBuffer* buff = (GFXSharedBuffer*)gfx_vector_at(&internal->vertexBuffs, buffer - 1);

	return gfx_vertex_layout_set_attribute_shared_buffer(mesh->layout, index, buff, offset);
}

/******************************************************/
int gfx_submesh_set_draw_call_buffer(GFXSubMesh* mesh, unsigned char index, size_t buffer, size_t offset)
{
	/* Validate index */
	struct GFX_SubMesh* internal = (struct GFX_SubMesh*)mesh;
	if(buffer > gfx_vector_get_size(&internal->indexBuffs))
	{
		/* Disable the attribute */
		gfx_vertex_layout_set_draw_call_shared_buffer(mesh->layout, index, NULL, 0);
		return 0;
	}

	/* Pass buffer to vertex layout */
	GFXSharedBuffer* buff = (GFXSharedBuffer*)gfx_vector_at(&internal->indexBuffs, buffer - 1);

	return gfx_vertex_layout_set_draw_call_shared_buffer(mesh->layout, index, buff, offset);
}
