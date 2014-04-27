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

#include <stdlib.h>

/******************************************************/
/* Internal mesh */
struct GFX_Mesh
{
	GFX_LodMap  map;     /* Super class */
	GFXVector   buckets; /* Stores GFX_BucketRef */
};

/* Internal submesh data */
struct GFX_SubData
{
	GFXSubMesh*    sub;    /* Super class */
	GFXMeshSource  source; /* Sources to use within sub */
};

/* Internal bucket reference */
struct GFX_BucketRef
{
	GFXPipe*  pipe;
	size_t    ref; /* Reference count */
};

/******************************************************/
static int _gfx_mesh_buckets_reference_submesh(

		struct GFX_Mesh*  mesh,
		GFXSubMesh*       sub)
{
	/* Reference all buckets at the submesh */
	GFXVectorIterator it;
	for(
		it = mesh->buckets.begin;
		it != mesh->buckets.end;
		it = gfx_vector_next(&mesh->buckets, it))
	{
		if(!_gfx_submesh_reference_bucket(
			sub,
			((struct GFX_BucketRef*)it)->pipe,
			((struct GFX_BucketRef*)it)->ref)) break;
	}

	/* Remove the references on failure */
	if(it != mesh->buckets.end)
	{
		while(it != mesh->buckets.begin)
		{
			it = gfx_vector_previous(&mesh->buckets, it);
			_gfx_submesh_remove_bucket(
				sub,
				((struct GFX_BucketRef*)it)->pipe,
				((struct GFX_BucketRef*)it)->ref
			);
		}
		return 0;
	}

	return 1;
}

/******************************************************/
static void _gfx_mesh_buckets_remove_submesh(

		struct GFX_Mesh*  mesh,
		GFXSubMesh*       sub)
{
	/* Remove all buckets from the submesh */
	GFXVectorIterator it;
	for(
		it = mesh->buckets.begin;
		it != mesh->buckets.end;
		it = gfx_vector_next(&mesh->buckets, it))
	{
		_gfx_submesh_remove_bucket(
			sub,
			((struct GFX_BucketRef*)it)->pipe,
			((struct GFX_BucketRef*)it)->ref
		);
	}
}

/******************************************************/
static struct GFX_BucketRef* _gfx_mesh_find_bucket(

		struct GFX_Mesh*  mesh,
		GFXPipe*          pipe)
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
static void _gfx_mesh_erase_bucket(

		struct GFX_Mesh*       mesh,
		struct GFX_BucketRef*  bucket,
		size_t                 ref)
{
	/* Remove the bucket at all submeshes */
	size_t num;
	struct GFX_SubData* subs = gfx_lod_map_get_all(
		(GFXLodMap*)mesh,
		&num);

	while(num) _gfx_submesh_remove_bucket(
		subs[--num].sub,
		bucket->pipe,
		ref);

	/* And decrease reference */
	if(!(bucket->ref = (bucket->ref <= ref) ? 0 : bucket->ref - ref))
	{
		/* Unregister mesh at pipe */
		GFXPipeCallback call;
		call.key = GFX_SCENE_KEY_MESH;
		call.data = mesh;

		gfx_pipe_unregister(bucket->pipe, call);

		/* Erase it */
		gfx_vector_erase(&mesh->buckets, bucket);
	}
}

/******************************************************/
static void _gfx_mesh_callback(

		GFXPipe*          pipe,
		GFXPipeCallback*  callback)
{
	/* Simply remove the bucket from the vector */
	/* We do not need to worry about submeshes as they are registered at the pipe as well */
	struct GFX_Mesh* mesh = callback->data;
	struct GFX_BucketRef* bucket = _gfx_mesh_find_bucket(mesh, pipe);

	if(bucket != mesh->buckets.end) gfx_vector_erase(
		&mesh->buckets,
		bucket
	);
}

/******************************************************/
static int _gfx_mesh_increase_references(

		struct GFX_Mesh*       mesh,
		struct GFX_BucketRef*  bucket)
{
	/* Check for overflow */
	if(!(bucket->ref + 1)) return 0;
	++bucket->ref;

	/* Reference the bucket at all submeshes */
	size_t num;
	struct GFX_SubData* subs = gfx_lod_map_get_all(
		(GFXLodMap*)mesh,
		&num
	);

	size_t n;
	for(n = 0; n < num; ++n) if(!_gfx_submesh_reference_bucket(
		subs[n].sub,
		bucket->pipe, 1)) break;

	/* Remove the references on failure */
	if(n < num)
	{
		while(n) _gfx_submesh_remove_bucket(
			subs[--n].sub,
			bucket->pipe,
			1
		);
		--bucket->ref;

		return 0;
	}

	return 1;
}

/******************************************************/
int _gfx_mesh_reference_bucket(

		GFXMesh*  mesh,
		GFXPipe*  pipe)
{
	/* Validate pipe type */
	if(gfx_pipe_get_type(pipe) != GFX_PIPE_BUCKET) return 0;

	/* Find the bucket first */
	struct GFX_Mesh* internal = (struct GFX_Mesh*)mesh;
	struct GFX_BucketRef* bucket = _gfx_mesh_find_bucket(
		internal,
		pipe
	);

	if(bucket == internal->buckets.end)
	{
		/* Insert a new bucket */
		struct GFX_BucketRef insert;
		insert.pipe = pipe;
		insert.ref = 0;

		bucket = gfx_vector_insert(
			&internal->buckets,
			&insert,
			internal->buckets.end
		);

		if(bucket == internal->buckets.end) return 0;

		/* Increase references */
		if(_gfx_mesh_increase_references(internal, bucket))
		{
			/* Register mesh at pipe */
			GFXPipeCallback call;
			call.key = GFX_SCENE_KEY_MESH;
			call.data = internal;

			if(gfx_pipe_register(pipe, call, _gfx_mesh_callback))
				return 1;
		}

		/* Erase bucket on failure */
		gfx_vector_erase(&internal->buckets, bucket);

		return 0;
	}

	/* Increase references */
	return _gfx_mesh_increase_references(internal, bucket);
}

/******************************************************/
void _gfx_mesh_remove_bucket(

		GFXMesh*  mesh,
		GFXPipe*  pipe)
{
	/* Find the bucket first */
	struct GFX_Mesh* internal = (struct GFX_Mesh*)mesh;
	struct GFX_BucketRef* bucket = _gfx_mesh_find_bucket(internal, pipe);

	if(bucket != internal->buckets.end) _gfx_mesh_erase_bucket(
		internal,
		bucket,
		1
	);
}

/******************************************************/
GFXMesh* gfx_mesh_create(void)
{
	/* Allocate mesh */
	struct GFX_Mesh* mesh = calloc(1, sizeof(struct GFX_Mesh));
	if(!mesh) return NULL;

	/* Initialize */
	_gfx_lod_map_init(
		(GFX_LodMap*)mesh,
		sizeof(struct GFX_SubData),
		sizeof(GFXSubMesh*)
	);

	gfx_vector_init(&mesh->buckets, sizeof(struct GFX_BucketRef));

	return (GFXMesh*)mesh;
}

/******************************************************/
void gfx_mesh_free(

		GFXMesh* mesh)
{
	if(mesh)
	{
		struct GFX_Mesh* internal = (struct GFX_Mesh*)mesh;

		/* Erase all buckets */
		while(internal->buckets.begin != internal->buckets.end)
			_gfx_mesh_erase_bucket(
				internal,
				internal->buckets.begin,
				((struct GFX_BucketRef*)internal->buckets.begin)->ref
			);

		/* Free all submeshes */
		size_t num;
		struct GFX_SubData* subs = gfx_lod_map_get_all(
			(GFXLodMap*)mesh,
			&num
		);

		while(num--) _gfx_submesh_free(subs[num].sub);

		/* Clear and free */
		gfx_vector_clear(&internal->buckets);
		_gfx_lod_map_clear((GFX_LodMap*)mesh);

		free(mesh);
	}
}

/******************************************************/
GFXSubMesh* gfx_mesh_add(

		GFXMesh*       mesh,
		size_t         level,
		unsigned char  drawCalls,
		unsigned char  sources)
{
	/* Create new submesh */
	struct GFX_SubData data;
	data.source.startSource = 0;
	data.source.numSource = sources;

	data.sub = _gfx_submesh_create(drawCalls, sources);
	if(!data.sub) return NULL;

	/* Reference the buckets at submesh */
	if(!_gfx_mesh_buckets_reference_submesh((struct GFX_Mesh*)mesh, data.sub))
	{
		_gfx_submesh_free(data.sub);
		return NULL;
	}

	/* Add it to the LOD map */
	if(!gfx_lod_map_add((GFXLodMap*)mesh, level, &data))
	{
		_gfx_submesh_free(data.sub);
		return NULL;
	}

	return data.sub;
}

/******************************************************/
int gfx_mesh_add_share(

		GFXMesh*     mesh,
		size_t       level,
		GFXSubMesh*  share)
{
	/* Check if it's already mapped to avoid another reference */
	if(gfx_lod_map_has((GFXLodMap*)mesh, level, &share))
		return 1;

	/* Reference the submesh */
	if(!_gfx_submesh_reference(share))
		return 0;

	/* Reference the buckets at submesh */
	if(!_gfx_mesh_buckets_reference_submesh((struct GFX_Mesh*)mesh, share))
	{
		_gfx_submesh_free(share);
		return 0;
	}

	/* Add it to the LOD map */
	struct GFX_SubData data;
	data.sub = share;
	data.source.startSource = 0;
	data.source.numSource = share->sources;

	if(!gfx_lod_map_add((GFXLodMap*)mesh, level, &data))
	{
		_gfx_mesh_buckets_remove_submesh((struct GFX_Mesh*)mesh, share);
		_gfx_submesh_free(share);

		return 0;
	}

	return 1;
}

/******************************************************/
int gfx_mesh_set_source(

		GFXMesh*       mesh,
		size_t         level,
		GFXSubMesh*    sub,
		GFXMeshSource  source)
{
	/* First find the submesh */
	size_t num;
	struct GFX_SubData* data = gfx_lod_map_get(
		(GFXLodMap*)mesh,
		level,
		&num
	);

	while(num--) if(data[num].sub == sub)
	{
		/* Set source if found */
		data[num].source = source;
		return 1;
	}

	return 0;
}

/******************************************************/
int gfx_mesh_set_source_at(

		GFXMesh*       mesh,
		size_t         level,
		size_t         index,
		GFXMeshSource  source)
{
	/* First get the submesh */
	size_t num;
	struct GFX_SubData* data = gfx_lod_map_get(
		(GFXLodMap*)mesh,
		level,
		&num
	);

	if(index >= num) return 0;

	/* Set the source */
	data[index].source = source;

	return 1;
}

/******************************************************/
int gfx_mesh_remove(

		GFXMesh*     mesh,
		size_t       level,
		GFXSubMesh*  sub)
{
	/* Try to remove it */
	if(gfx_lod_map_remove((GFXLodMap*)mesh, level, &sub))
	{
		_gfx_mesh_buckets_remove_submesh(
			(struct GFX_Mesh*)mesh,
			sub
		);
		_gfx_submesh_free(sub);

		return 1;
	}
	return 0;
}

/******************************************************/
int gfx_mesh_remove_at(

		GFXMesh*  mesh,
		size_t    level,
		size_t    index)
{
	/* First get the submesh */
	size_t num;
	struct GFX_SubData* data = gfx_lod_map_get(
		(GFXLodMap*)mesh,
		level,
		&num
	);

	if(index >= num) return 0;

	/* Try to remove it */
	GFXSubMesh* submesh = data[index].sub;

	if(gfx_lod_map_remove_at((GFXLodMap*)mesh, level, index))
	{
		_gfx_mesh_buckets_remove_submesh(
			(struct GFX_Mesh*)mesh,
			submesh
		);
		_gfx_submesh_free(submesh);

		return 1;
	}
	return 0;
}

/******************************************************/
GFXSubMeshList gfx_mesh_get(

		GFXMesh*  mesh,
		size_t    level,
		size_t*   num)
{
	return gfx_lod_map_get((GFXLodMap*)mesh, level, num);
}

/******************************************************/
GFXSubMeshList gfx_mesh_get_all(

		GFXMesh*  mesh,
		size_t*   num)
{
	return gfx_lod_map_get_all((GFXLodMap*)mesh, num);
}

/******************************************************/
GFXMeshSource gfx_submesh_list_source_at(

		GFXSubMeshList  list,
		size_t          index)
{
	return ((struct GFX_SubData*)list)[index].source;
}

/******************************************************/
GFXSubMesh* gfx_submesh_list_at(

		GFXSubMeshList  list,
		size_t          index)
{
	return ((struct GFX_SubData*)list)[index].sub;
}
