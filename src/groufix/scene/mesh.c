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
#include "groufix/scene/internal.h"

#include <stdlib.h>

/******************************************************/
static inline GFXSubMesh** _gfx_mesh_get_submesh(

		GFXMesh*       mesh,
		unsigned char  index)
{
	return ((GFXSubMesh**)(mesh + 1)) + index;
}

/******************************************************/
static inline void _gfx_mesh_set_submesh(

		GFXMesh*       mesh,
		unsigned char  index,
		GFXSubMesh*    sub)
{
	_gfx_submesh_free(*_gfx_mesh_get_submesh(mesh, index));
	*_gfx_mesh_get_submesh(mesh, index) = sub;
}

/******************************************************/
GFXMesh* gfx_mesh_create(

		unsigned char subMeshes)
{
	if(!subMeshes) return NULL;

	/* Allocate mesh */
	size_t size = sizeof(GFXMesh) + subMeshes * sizeof(GFXSubMesh*);
	GFXMesh* mesh = calloc(1, size);

	if(!mesh)
	{
		/* Out of memory error */
		gfx_errors_push(
			GFX_ERROR_OUT_OF_MEMORY,
			"Mesh could not be allocated."
		);
		return NULL;
	}

	/* Initialize */
	mesh->subMeshes = subMeshes;

	return mesh;
}

/******************************************************/
void gfx_mesh_free(

		GFXMesh* mesh)
{
	if(mesh)
	{
		/* Free all submeshes */
		while(mesh->subMeshes--)
			_gfx_submesh_free(*_gfx_mesh_get_submesh(mesh, mesh->subMeshes));

		free(mesh);
	}
}

/******************************************************/
GFXSubMesh* gfx_mesh_add(

		GFXMesh*       mesh,
		unsigned char  index)
{
	/* Create new submesh */
	GFXSubMesh* sub = _gfx_submesh_create();
	if(!sub) return NULL;

	/* Free old submesh and replace it */
	_gfx_mesh_set_submesh(mesh, index, sub);

	return sub;
}

/******************************************************/
int gfx_mesh_add_share(

		GFXMesh*       mesh,
		unsigned char  index,
		GFXSubMesh*    share)
{
	if(share)
	{
		/* Reference the submesh */
		if(!_gfx_submesh_reference(share)) return 0;
	}

	/* Free old submesh and replace it */
	_gfx_mesh_set_submesh(mesh, index, share);

	return 1;
}

/******************************************************/
GFXSubMesh* gfx_mesh_get(

		GFXMesh*       mesh,
		unsigned char  index)
{
	return *_gfx_mesh_get_submesh(mesh, index);
}
