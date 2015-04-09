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

#ifndef GFX_SCENE_OBJECT_H
#define GFX_SCENE_OBJECT_H

#include "groufix/scene/material.h"
#include "groufix/scene/mesh.h"

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Batch (collection of bucket units)
 *******************************************************/

/** Batch */
typedef struct GFXBatch
{
	GFXBucket*     bucket;        /* Bucket all units are associated with */

	GFXMaterial*   material;
	GFXMesh*       mesh;
	unsigned int   materialIndex; /* Index into all material levels */
	unsigned int   meshIndex;     /* Index into all mesh levels */

	unsigned char  levels;        /* Number of mesh/material LOD combinations */
	unsigned int   units;         /* Number of units to reserve per level */

} GFXBatch;


/**
 * Creates a new batch.
 *
 * @param bucket    Bucket all units are insert into.
 * @param matIndex  Index into all material levels.
 * @param meshIndex Index into all mesh levels.
 * @param levels    Number of mesh/material LOD combinations.
 * @param units     Number of units to reserve per level.
 * @return NULL on failure.
 *
 */
GFX_API GFXBatch* gfx_batch_create(

		GFXBucket*     bucket,
		GFXMaterial*   material,
		GFXMesh*       mesh,
		unsigned int   matIndex,
		unsigned int   meshIndex,
		unsigned char  levels,
		unsigned int   units);

/**
 * Makes sure the batch is freed properly.
 *
 */
GFX_API void gfx_batch_free(

		GFXBatch* batch);

/**
 * Sets the properties of a level at a batch.
 *
 * @param level    Level to set properties of (must be < batch->levels).
 * @param material LOD of the material to use.
 * @param mesh     LOD of the mesh to use.
 * @param copies   Number of copies to reserve at the property map (must be >= 1).
 * @return Zero on failure.
 *
 * Note: this will reallocate copies at the property map, which is expensive!
 * If this level already has allocated units, the function will fail.
 *
 * If it already had a level and the function failed, the previous level is not
 * guaranteed to still be set.
 *
 */
GFX_API int gfx_batch_set_level(

		GFXBatch*      batch,
		unsigned char  level,
		unsigned int   material,
		unsigned int   mesh,
		unsigned int   copies);

/**
 * Sets the properties of a level at a batch and shares copies.
 *
 * @param src      Batch to share copies with.
 * @param srcLevel Level at the source batch to share with.
 * @return Zero on failure.
 *
 * Note: this will fail if the batches do not share the material and material index,
 * as it will attempt to share copies at a property map.
 *
 */
GFX_API int gfx_batch_set_level_share(

		GFXBatch*      batch,
		unsigned char  level,
		GFXBatch*      src,
		unsigned char  srcLevel,
		unsigned int   mesh);

/**
 * Fetches the properties of a level at a batch.
 *
 * @param copies    Returns the number of reserved copies.
 * @param instances Returns maximum number of instances per unit, 0 for infinite.
 * @param offset    Returns the starting of the reserved range.
 * @return Zero if the properties are not set yet, no parameter is written to.
 *
 */
GFX_API int gfx_batch_get_level(

		const GFXBatch*  batch,
		unsigned char    level,
		unsigned int*    copies,
		unsigned int*    instances,
		unsigned int*    offset);

/**
 * Sets the number of allocated units at a level at a batch.
 *
 * @param num     Number of units to allocate.
 * @param visible If new units are created, non-zero if visible, invisible otherwise.
 * @return Array of allocated bucket units (of num length), NULL on failure.
 *
 * If num is less than currently allocated units, units will be destroyed.
 *
 */
GFX_API GFXBucketUnit* gfx_batch_set(

		GFXBatch*      batch,
		unsigned char  level,
		unsigned int   num,
		int            visible);

/**
 * Fetches the allocated units at a level at a batch.
 *
 * @param num Returns the number of units in the returned array.
 * @return Array of allocated bucket units.
 *
 */
GFX_API GFXBucketUnit* gfx_batch_get(

		const GFXBatch*  batch,
		unsigned char    level,
		unsigned int*    num);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_OBJECT_H
