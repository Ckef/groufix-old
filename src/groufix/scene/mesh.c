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

#include "groufix/scene/internal.h"

/******************************************************/
/* Internal submesh data */
typedef struct GFX_SubData
{
	GFXSubMesh*    sub;      /* Super class */
	unsigned int   material; /* Property map index within material */
	unsigned char  source;   /* Source to use within submesh */

} GFX_SubData;


/******************************************************/
GFXMesh* gfx_mesh_create(void)
{
	/* Allocate */
	return (GFXMesh*)gfx_lod_map_create(
		0,
		sizeof(GFX_SubData),
		sizeof(GFXSubMesh*)
	);
}

/******************************************************/
void gfx_mesh_free(

		GFXMesh* mesh)
{
	if(mesh)
	{
		/* Free all submeshes */
		unsigned int num;
		GFX_SubData* subs = gfx_lod_map_get_all(
			(GFXLodMap*)mesh,
			&num
		);

		while(num--) _gfx_submesh_free(subs[num].sub);

		/* Free */
		gfx_lod_map_free((GFXLodMap*)mesh);
	}
}

/******************************************************/
GFXSubMesh* gfx_mesh_add(

		GFXMesh*       mesh,
		unsigned int   level,
		unsigned char  drawCalls,
		unsigned char  sources)
{
	/* Create new submesh */
	GFX_SubData data =
	{
		.material = 0,
		.source = 0
	};

	data.sub = _gfx_submesh_create(drawCalls, sources);
	if(!data.sub) return NULL;

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

		GFXMesh*      mesh,
		unsigned int  level,
		GFXSubMesh*   share)
{
	/* Reference the submesh */
	if(!_gfx_submesh_reference(share)) return 0;

	/* Add it to the LOD map */
	GFX_SubData data =
	{
		.sub      = share,
		.material = 0,
		.source   = 0
	};

	if(!gfx_lod_map_add((GFXLodMap*)mesh, level, &data))
	{
		_gfx_submesh_free(share);
		return 0;
	}

	return 1;
}

/******************************************************/
unsigned int gfx_mesh_set_material(

		GFXMesh*      mesh,
		unsigned int  level,
		GFXSubMesh*   sub,
		unsigned int  material)
{
	/* First find the submesh */
	unsigned int num;
	GFX_SubData* data = gfx_lod_map_get(
		(GFXLodMap*)mesh,
		level,
		&num
	);

	unsigned int count = 0;

	/* Set all materials */
	while(num--) if(data[num].sub == sub)
	{
		data[num].material = material;
		++count;
	}

	return count;
}

/******************************************************/
int gfx_mesh_set_material_at(

		GFXMesh*      mesh,
		unsigned int  level,
		unsigned int  index,
		unsigned int  material)
{
	/* First get the submesh */
	unsigned int num;
	GFX_SubData* data = gfx_lod_map_get(
		(GFXLodMap*)mesh,
		level,
		&num
	);

	if(index >= num) return 0;

	/* Set material */
	data[index].material = material;

	return 1;
}

/******************************************************/
unsigned int gfx_mesh_set_source(

		GFXMesh*       mesh,
		unsigned int   level,
		GFXSubMesh*    sub,
		unsigned char  source)
{
	/* Bound check */
	if(source >= sub->sources) return 0;

	/* First find the submesh */
	unsigned int num;
	GFX_SubData* data = gfx_lod_map_get(
		(GFXLodMap*)mesh,
		level,
		&num
	);

	unsigned int count = 0;

	/* Set all sources */
	while(num--) if(data[num].sub == sub)
	{
		data[num].source = source;
		++count;
	}

	return count;
}

/******************************************************/
int gfx_mesh_set_source_at(

		GFXMesh*       mesh,
		unsigned int   level,
		unsigned int   index,
		unsigned char  source)
{
	/* First get the submesh */
	unsigned int num;
	GFX_SubData* data = gfx_lod_map_get(
		(GFXLodMap*)mesh,
		level,
		&num
	);

	if(index >= num) return 0;

	/* Bound check */
	if(source >= data[index].sub->sources) return 0;

	/* Set the source */
	data[index].source = source;

	return 1;
}

/******************************************************/
GFXSubMeshList gfx_mesh_get(

		GFXMesh*       mesh,
		unsigned int   level,
		unsigned int*  num)
{
	return gfx_lod_map_get((GFXLodMap*)mesh, level, num);
}

/******************************************************/
GFXSubMeshList gfx_mesh_get_all(

		GFXMesh*       mesh,
		unsigned int*  num)
{
	return gfx_lod_map_get_all((GFXLodMap*)mesh, num);
}

/******************************************************/
unsigned int gfx_submesh_list_material_at(

		GFXSubMeshList  list,
		unsigned int    index)
{
	return ((GFX_SubData*)list)[index].material;
}

/******************************************************/
unsigned char gfx_submesh_list_source_at(

		GFXSubMeshList  list,
		unsigned int    index)
{
	return ((GFX_SubData*)list)[index].source;
}

/******************************************************/
GFXSubMesh* gfx_submesh_list_at(

		GFXSubMeshList  list,
		unsigned int    index)
{
	return ((GFX_SubData*)list)[index].sub;
}
