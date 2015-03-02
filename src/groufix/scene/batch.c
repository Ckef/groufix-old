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
#include "groufix/scene/object.h"
#include "groufix/scene/internal.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
/* Internal level pair */
typedef struct GFX_Level
{
	unsigned int offset;   /* Copy offset */
	unsigned int material; /* Material LOD */
	unsigned int mesh;     /* Mesh LOD */

} GFX_Level;


/******************************************************/
static inline GFX_Level* _gfx_batch_get_level(

		GFXBatch*     batch,
		unsigned int  level)
{
	return ((GFX_Level*)(batch + 1)) + level;
}

/******************************************************/
static inline GFXBucketUnit* _gfx_batch_get_unit(

		GFXBatch*     batch,
		unsigned int  level,
		unsigned int  copy)
{
	return ((GFXBucketUnit*)(((GFX_Level*)(batch + 1)) + batch->levels)) +
		(batch->levels * copy + level);
}

/******************************************************/
static inline int _gfx_batch_has_level(

		GFXBatch*     batch,
		unsigned int  level)
{
	return *_gfx_batch_get_unit(batch, level, 0) != 0;
}

/******************************************************/
GFXBatch* gfx_batch_create(

		GFXBucket*    bucket,
		GFXMaterial*  material,
		GFXMesh*      mesh,
		unsigned int  matIndex,
		unsigned int  meshIndex,
		unsigned int  copies,
		unsigned int  levels)
{
	/* ffs.. */
	if(!copies || !levels) return NULL;

	/* Create new batch, append level pairs and units to the end of struct */
	size_t size =
		sizeof(GFXBatch) +
		sizeof(GFX_Level) * levels +
		sizeof(GFXBucketUnit) * levels * copies;

	GFXBatch* batch = malloc(size);
	if(!batch)
	{
		/* Out of memory error */
		gfx_errors_push(
			GFX_ERROR_OUT_OF_MEMORY,
			"Batch could not be allocated."
		);
		return NULL;
	}

	/* Reference bucket at mesh */
	if(!_gfx_mesh_add_bucket(mesh, bucket))
	{
		free(batch);
		return NULL;
	}

	/* Initialize */
	batch->bucket        = bucket;
	batch->material      = material;
	batch->mesh          = mesh;
	batch->materialIndex = matIndex;
	batch->meshIndex     = meshIndex;
	batch->copies        = copies;
	batch->levels        = levels;

	/* Set first units of all levels to 0 to indicate unallocated */
	memset(
		_gfx_batch_get_unit(batch, 0, 0),
		0,
		levels * sizeof(GFXBucketUnit)
	);

	return batch;
}

/******************************************************/
void gfx_batch_free(

		GFXBatch* batch)
{
	if(batch)
	{
		/* Dereference bucket at mesh */
		_gfx_mesh_remove_bucket(batch->mesh, batch->bucket);

		free(batch);
	}
}
