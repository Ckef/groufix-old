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

#include "groufix/scene/batch.h"
#include "groufix/scene/internal.h"

/******************************************************/
int gfx_batch_get(

		GFXBatch*     batch,
		GFXMaterial*  material,
		GFXMesh*      mesh,
		size_t        index)
{
	/* Get batch at mesh */
	size_t matID;
	size_t meshID = _gfx_mesh_get_batch(
		mesh,
		material,
		index,
		&matID
	);

	if(!matID)
	{
		/* Insert batch at material if it doesn't exist yet */
		/* Also set the references to each other */
		matID = _gfx_material_insert_batch(material, mesh);
		_gfx_material_set_batch(material, matID, meshID);
		_gfx_mesh_set_batch(mesh, meshID, matID);
	}

	/* Remove on failure */
	if(!matID || !meshID)
	{
		_gfx_material_remove_batch(material, matID);
		_gfx_mesh_remove_batch(mesh, meshID);

		return 0;
	}

	/* Initialize batch struct */
	batch->material   = material;
	batch->materialID = matID;
	batch->mesh       = mesh;
	batch->meshID     = meshID;

	return 1;
}

/******************************************************/
int gfx_batch_increase(

		GFXBatch*  batch,
		GFXPipe*   bucket,
		size_t     instances)
{
	return _gfx_mesh_increase_instances(
		batch->mesh,
		batch->meshID,
		bucket,
		instances);
}

/******************************************************/
void gfx_batch_decrease(

		GFXBatch*  batch,
		GFXPipe*   bucket,
		size_t     instances)
{
	_gfx_mesh_decrease_instances(
		batch->mesh,
		batch->meshID,
		bucket,
		instances);
}
