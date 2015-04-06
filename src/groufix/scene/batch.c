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
	unsigned int material;  /* Material LOD */
	unsigned int mesh;      /* Mesh LOD */
	unsigned int num;

	unsigned int copies;    /* Number of reserved copies (0 if uninitialized level) */
	unsigned int instances; /* Maximum number of instances per unit, 0 for infinite */
	unsigned int offset;    /* Copy offset */

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
		unsigned int  unit)
{
	return ((GFXBucketUnit*)(((GFX_Level*)(batch + 1)) + batch->levels)) +
		(batch->units * level + unit);
}

/******************************************************/
GFXBatch* gfx_batch_create(

		GFXBucket*     bucket,
		GFXMaterial*   material,
		GFXMesh*       mesh,
		unsigned int   matIndex,
		unsigned int   meshIndex,
		unsigned char  levels,
		unsigned int   units)
{
	/* ffs.. */
	if(!levels || !units) return NULL;

	/* Create new batch, append level pairs and units to the end of struct */
	size_t size =
		sizeof(GFXBatch) +
		sizeof(GFX_Level) * levels +
		sizeof(GFXBucketUnit) * levels * units;

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
	batch->levels        = levels;
	batch->units         = units;

	/* Initialize levels to 0 to indicate unallocated */
	memset(batch + 1, 0, sizeof(GFX_Level) * levels);

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

/******************************************************/
int gfx_batch_set_level(

		GFXBatch*      batch,
		unsigned char  level,
		unsigned int   material,
		unsigned int   mesh,
		unsigned int   copies)
{
	return 0;
}

/******************************************************/
int gfx_batch_set_level_share(

		GFXBatch*      batch,
		unsigned char  level,
		GFXBatch*      src,
		unsigned char  srcLevel,
		unsigned int   mesh)
{
	return 0;
}

/******************************************************/
int gfx_batch_get_level(

		GFXBatch*      batch,
		unsigned char  level,
		unsigned int*  copies,
		unsigned int*  instances,
		unsigned int*  offset)
{
	return 0;
}

/******************************************************/
GFXBucketUnit* gfx_batch_set(

		GFXBatch*      batch,
		unsigned char  level,
		unsigned int   num)
{
	return NULL;
}

/******************************************************/
GFXBucketUnit* gfx_batch_get(

		const GFXBatch*  batch,
		unsigned char    level,
		unsigned int*    num)
{
	return NULL;
}
