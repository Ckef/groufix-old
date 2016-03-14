/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel.
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

		const GFXBatch  *batch,
		unsigned char    level)
{
	return ((GFX_Level*)(batch + 1)) + level;
}

/******************************************************/
static inline GFXBucketUnit* _gfx_batch_get_unit(

		const GFXBatch  *batch,
		unsigned char    level,
		unsigned int     unit)
{
	return ((GFXBucketUnit*)(((GFX_Level*)(batch + 1)) + batch->levels)) +
		(batch->units * level + unit);
}

/******************************************************/
static void _gfx_batch_erase_copies(

		GFXBatch   *batch,
		GFX_Level  *level)
{
	if(level->copies)
	{
		/* Get property map and erase copies */
		unsigned int num;
		GFXPropertyMapList list = gfx_material_get(
			batch->material,
			level->material,
			&num);

		_gfx_property_map_list_erase_copies_at(
			list,
			batch->materialIndex,
			level->offset);

		/* Mark as uninitialized */
		level->copies = 0;
	}
}

/******************************************************/
GFXBatch *gfx_batch_create(

		GFXBucket     *bucket,
		GFXMaterial   *material,
		GFXMesh       *mesh,
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

	GFXBatch *batch = malloc(size);
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

		GFXBatch *batch)
{
	if(batch)
	{
		/* Erase all units and copies */
		unsigned char level;
		unsigned int unit;

		for(level = 0; level < batch->levels; ++level)
		{
			GFX_Level *lev = _gfx_batch_get_level(
				batch, level);
			_gfx_batch_erase_copies(
				batch, lev);

			for(unit = 0; unit < lev->num; ++unit) gfx_bucket_erase(
				batch->bucket,
				*_gfx_batch_get_unit(batch, level, unit)
			);
		}

		/* Dereference bucket at mesh */
		_gfx_mesh_remove_bucket(batch->mesh, batch->bucket);

		free(batch);
	}
}

/******************************************************/
int gfx_batch_set_level(

		GFXBatch      *batch,
		unsigned char  level,
		unsigned int   material,
		unsigned int   mesh,
		unsigned int   copies)
{
	/* Check units */
	GFX_Level *lev = _gfx_batch_get_level(batch, level);
	if(lev->num) return 0;

	/* Validate material and mesh indices */
	unsigned int matNum;
	unsigned int mesNum;

	GFXPropertyMapList list = gfx_material_get(
		batch->material, material, &matNum);
	gfx_mesh_get(
		batch->mesh, mesh, &mesNum);

	if(
		batch->materialIndex >= matNum ||
		batch->meshIndex >= mesNum)
	{
		return 0;
	}

	/* Erase previous copies */
	_gfx_batch_erase_copies(batch, lev);

	/* Insert copies at material */
	unsigned int offset;
	if(!_gfx_property_map_list_insert_copies_at(
		list,
		batch->materialIndex,
		copies,
		&offset))
	{
		return 0;
	}

	lev->material = material;
	lev->mesh     = mesh;
	lev->copies   = copies;
	lev->offset   = offset;

	/* Fetch instances from property map */
	lev->instances = gfx_property_map_list_at(
		list,
		batch->materialIndex)->programMap->instances;

	return 1;
}

/******************************************************/
int gfx_batch_set_level_share(

		GFXBatch      *batch,
		unsigned char  level,
		GFXBatch      *src,
		unsigned char  srcLevel,
		unsigned int   mesh)
{
	GFX_Level *lev = _gfx_batch_get_level(batch, level);
	GFX_Level *srcLev = _gfx_batch_get_level(src, srcLevel);

	/* Herpderp */
	if(
		lev->num ||
		lev == srcLev ||
		batch->material != src->material ||
		batch->materialIndex != src->materialIndex)
	{
		return 0;
	}

	/* Validate material and mesh indices */
	unsigned int matNum;
	unsigned int mesNum;

	GFXPropertyMapList list = gfx_material_get(
		batch->material, srcLev->material, &matNum);
	gfx_mesh_get(
		batch->mesh, mesh, &mesNum);

	if(
		batch->materialIndex >= matNum ||
		batch->meshIndex >= mesNum)
	{
		return 0;
	}

	/* Erase previous copies */
	_gfx_batch_erase_copies(batch, lev);

	/* Reference copies at material */
	if(!_gfx_property_map_list_reference_copies_at(
		list,
		batch->materialIndex,
		srcLev->offset))
	{
		return 0;
	}

	lev->material = srcLev->material;
	lev->mesh     = mesh;
	lev->copies   = srcLev->copies;
	lev->offset   = srcLev->offset;

	/* Fetch instances from property map */
	lev->instances = gfx_property_map_list_at(
		list,
		batch->materialIndex)->programMap->instances;

	return 1;
}

/******************************************************/
int gfx_batch_get_level(

		const GFXBatch  *batch,
		unsigned char    level,
		unsigned int    *copies,
		unsigned int    *instances,
		unsigned int    *offset)
{
	/* Check if properties are set */
	GFX_Level *lev = _gfx_batch_get_level(batch, level);
	if(!lev->copies) return 0;

	*copies = lev->copies;
	*instances = lev->instances;
	*offset = lev->offset;

	return 1;
}

/******************************************************/
GFXBucketUnit *gfx_batch_set(

		GFXBatch      *batch,
		unsigned char  level,
		unsigned int   num,
		int            visible)
{
	/* Check if properties are set */
	GFX_Level *lev = _gfx_batch_get_level(batch, level);
	if(!lev->copies) return NULL;

	if(num > lev->num)
	{
		/* Get bucket source */
		unsigned int index =
			gfx_lod_map_count((GFXLodMap*)batch->mesh, lev->mesh) +
			batch->meshIndex;

		GFXBucketSource src = _gfx_mesh_get_bucket_source(
			batch->mesh,
			batch->bucket,
			index);

		/* Get property map */
		GFXPropertyMap *map = gfx_property_map_list_at(
			gfx_material_get(batch->material, lev->material, &index),
			batch->materialIndex);

		/* Insert more units */
		unsigned int unit;
		for(unit = lev->num; unit < num; ++unit)
		{
			GFXBucketUnit new = gfx_bucket_insert(
				batch->bucket, src, map, lev->offset, visible);
			*_gfx_batch_get_unit(
				batch, level, unit) = new;

			/* Nevermind erase them all again */
			if(!new)
			{
				while(unit > lev->num) gfx_bucket_erase(
					batch->bucket,
					*_gfx_batch_get_unit(batch, level, unit--)
				);

				return NULL;
			}
		}
	}
	else
	{
		/* Destroy excess units */
		unsigned int unit;
		for(unit = num; unit < lev->num; ++unit) gfx_bucket_erase(
			batch->bucket,
			*_gfx_batch_get_unit(batch, level, unit)
		);
	}

	/* Update */
	lev->num = num;
	return _gfx_batch_get_unit(batch, level, 0);
}

/******************************************************/
GFXBucketUnit *gfx_batch_get(

		const GFXBatch  *batch,
		unsigned char    level,
		unsigned int    *num)
{
	*num = _gfx_batch_get_level(batch, level)->num;
	return _gfx_batch_get_unit(batch, level, 0);
}
