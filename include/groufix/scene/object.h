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
	unsigned int   materialIndex; /* Index into the material level */
	unsigned int   meshIndex;     /* Index into the mesh level */

	unsigned int   copies;        /* Number of copies to reserve */
	unsigned char  levels;        /* Number of mesh/material level combinations */

} GFXBatch;


/**
 * Creates a new batch.
 *
 * @param bucket    Bucket all units are insert into.
 * @param matIndex  Index into all material levels.
 * @param meshIndex Index into all mesh levels.
 * @param copies    Number of copies (ergo units) to reserve per level.
 * @param levels    Number of mesh/material level combinations.
 * @return NULL on failure.
 *
 */
GFX_API GFXBatch* gfx_batch_create(

		GFXBucket*    bucket,
		GFXMaterial*  material,
		GFXMesh*      mesh,
		unsigned int  matIndex,
		unsigned int  meshIndex,
		unsigned int  copies,
		unsigned int  levels);

/**
 * Makes sure the batch is freed properly.
 *
 */
GFX_API void gfx_batch_free(

		GFXBatch* batch);


#ifdef __cplusplus
}
#endif

#endif // GFX_SCENE_OBJECT_H
