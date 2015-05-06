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

#include "groufix/core/internal.h"

#include <stdlib.h>

/* Internal program stages */
#define GFX_INT_STAGE_VERTEX        0x00
#define GFX_INT_STAGE_TESS_CONTROL  0x01
#define GFX_INT_STAGE_TESS_EVAL     0x02
#define GFX_INT_STAGE_GEOMETRY      0x03
#define GFX_INT_STAGE_FRAGMENT      0x04

#define GFX_INT_NUM_STAGES          0x05

/******************************************************/
/* Internal program map */
typedef struct GFX_Map
{
	/* Super Class */
	GFXProgramMap map;

	/* Hidden data */
	GFX_RenderObjectID  id;
	GLuint              handle;                     /* OpenGL program or program pipeline handle */
	GFXProgram*         stages[GFX_INT_NUM_STAGES]; /* All stages with their associated program */
	unsigned int        blocks;                     /* Number of times blocked */

} GFX_Map;


/******************************************************/
static inline unsigned char _gfx_program_map_get_stage(

		GFXShaderStage stage)
{
	switch(stage)
	{
		case GFX_VERTEX_SHADER       : return GFX_INT_STAGE_VERTEX;
		case GFX_TESS_CONTROL_SHADER : return GFX_INT_STAGE_TESS_CONTROL;
		case GFX_TESS_EVAL_SHADER    : return GFX_INT_STAGE_TESS_EVAL;
		case GFX_GEOMETRY_SHADER     : return GFX_INT_STAGE_GEOMETRY;
		case GFX_FRAGMENT_SHADER     : return GFX_INT_STAGE_FRAGMENT;

		/* All of them, return something out of bounds */
		default : return GFX_INT_NUM_STAGES;
	}
}

/******************************************************/
static inline GLbitfield _gfx_program_map_get_bitfield(

		unsigned char stage)
{
	switch(stage)
	{
		case GFX_INT_STAGE_VERTEX       : return GL_VERTEX_SHADER_BIT;
		case GFX_INT_STAGE_TESS_CONTROL : return GL_TESS_CONTROL_SHADER_BIT;
		case GFX_INT_STAGE_TESS_EVAL    : return GL_TESS_EVALUATION_SHADER_BIT;
		case GFX_INT_STAGE_GEOMETRY     : return GL_GEOMETRY_SHADER_BIT;
		case GFX_INT_STAGE_FRAGMENT     : return GL_FRAGMENT_SHADER_BIT;

		/* Replace everything because why not :D! */
		default : return GL_ALL_SHADER_BITS;
	}
}

/******************************************************/
static int _gfx_program_map_set_stages(

		GFX_Map*        map,
		GFXShaderStage  stage,
		GFXProgram*     program)
{
	GFX_WIND_INIT(0);

	unsigned char ext = GFX_WIND_GET.ext[GFX_EXT_PROGRAM_MAP];

	/* Get program handle */
	GLuint handle = 0;
	if(program) handle = _gfx_program_get_handle(program);

	if(!ext || stage == GFX_ALL_SHADERS)
	{
		if(!program)
			map->map.instances = 0;

		else
		{
			/* Reference program for all stages */
			_gfx_program_reference(
				program,
				GFX_INT_NUM_STAGES - 1
			);

			/* Take program's number of instances */
			map->map.instances = program->instances;
		}

		/* Set all stages */
		unsigned int index;
		for(index = 0; index < GFX_INT_NUM_STAGES; ++index)
		{
			/* Free previous program */
			if(map->stages[index])
				gfx_program_free(map->stages[index]);

			map->stages[index] = program;
		}
	}
	else
	{
		/* Map to the requested stage */
		unsigned int index = _gfx_program_map_get_stage(stage);
		if(index >= GFX_INT_NUM_STAGES) return 0;

		/* Free previous program */
		if(map->stages[index])
			gfx_program_free(map->stages[index]);

		map->stages[index] = program;

		/* Loop over all stages to get minimum of instances */
		map->map.instances = 0;
		for(index = 0; index < GFX_INT_NUM_STAGES; ++index)
			if(map->stages[index])
			{
				size_t inst =
					map->stages[index]->instances;
				map->map.instances =
					!map->map.instances ? inst :
					!inst ? map->map.instances :
					(map->map.instances > inst) ? inst : map->map.instances;
			}
	}

	/* Force the program to be bound if no pipeline is available */
	if(!ext) map->handle = handle;

	return 1;
}

/******************************************************/
static void _gfx_program_map_obj_free(

		void*               object,
		GFX_RenderObjectID  id)
{
	GFX_Map* map = (GFX_Map*)object;

	/* If it was already freed, free memory */
	map->id = id;
	map->handle = 0;
}

/******************************************************/
static void _gfx_program_map_obj_save(

		void*               object,
		GFX_RenderObjectID  id)
{
	GFX_WIND_INIT_UNSAFE;

	GFX_Map* map = (GFX_Map*)object;

	map->id = id;
	GFX_REND_GET.DeleteProgramPipelines(1, &map->handle);
	map->handle = 0;
}

/******************************************************/
static void _gfx_program_map_obj_restore(

		void*               object,
		GFX_RenderObjectID  id)
{
	GFX_WIND_INIT_UNSAFE;

	GFX_Map* map = (GFX_Map*)object;

	/* Create program pipeline */
	map->id = id;
	GFX_REND_GET.CreateProgramPipelines(1, &map->handle);

	/* Use all programs */
	if(map->blocks)
	{
		unsigned char stage;
		for(stage = 0; stage < GFX_INT_NUM_STAGES; ++stage)
			if(map->stages[stage]) GFX_REND_GET.UseProgramStages(
				map->handle,
				_gfx_program_map_get_bitfield(stage),
				_gfx_program_get_handle(map->stages[stage])
			);
	}
}

/******************************************************/
/* vtable for render object part of the program map */
static GFX_RenderObjectFuncs _gfx_program_map_obj_funcs =
{
	_gfx_program_map_obj_free,
	_gfx_program_map_obj_save,
	_gfx_program_map_obj_restore
};

/******************************************************/
GLuint _gfx_program_map_get_handle(

		const GFXProgramMap* map)
{
	return ((const GFX_Map*)map)->handle;
}

/******************************************************/
int _gfx_program_map_block(

		GFXProgramMap* map)
{
	GFX_WIND_INIT(0);

	GFX_Map* internal = (GFX_Map*)map;

	/* Check if all programs are linked */
	unsigned char stage;
	for(stage = 0; stage < GFX_INT_NUM_STAGES; ++stage)
		if(internal->stages[stage])
		{
			if(!internal->stages[stage]->linked)
				return 0;
		}

	/* Increase block counter */
	if(!(internal->blocks + 1))
	{
		/* Overflow error */
		gfx_errors_push(
			GFX_ERROR_OVERFLOW,
			"Overflow occurred during Program Map usage."
		);
		return 0;
	}

	/* Use all programs */
	if(GFX_WIND_GET.ext[GFX_EXT_PROGRAM_MAP] && !internal->blocks)
	{
		for(stage = 0; stage < GFX_INT_NUM_STAGES; ++stage)
			if(internal->stages[stage]) GFX_REND_GET.UseProgramStages(
				internal->handle,
				_gfx_program_map_get_bitfield(stage),
				_gfx_program_get_handle(internal->stages[stage])
			);
	}

	++internal->blocks;

	return 1;
}

/******************************************************/
void _gfx_program_map_unblock(

		GFXProgramMap* map)
{
	GFX_Map* internal =
		(GFX_Map*)map;
	internal->blocks =
		internal->blocks ? internal->blocks - 1 : 0;
}

/******************************************************/
void _gfx_program_map_save(

		GFXProgramMap* map)
{
	GFX_WIND_INIT();

	GFX_Map* internal = (GFX_Map*)map;

	if(GFX_WIND_GET.ext[GFX_EXT_PROGRAM_MAP])
	{
		GFX_RenderObjectID id =
		{
			.objects = internal->id.objects,
			.id = 0
		};

		/* Unregister as object */
		_gfx_render_object_unregister(internal->id);
		_gfx_program_map_obj_save(map, id);
	}
}

/******************************************************/
void _gfx_program_map_restore(

		GFXProgramMap*      map,
		GFX_RenderObjects*  cont)
{
	GFX_WIND_INIT();

	if(GFX_WIND_GET.ext[GFX_EXT_PROGRAM_MAP])
	{
		/* Register as object */
		GFX_RenderObjectID id = _gfx_render_object_register(
			cont,
			map,
			&_gfx_program_map_obj_funcs
		);

		if(id.id) _gfx_program_map_obj_restore(map, id);
	}
}

/******************************************************/
GFXProgramMap* gfx_program_map_create(void)
{
	GFX_WIND_INIT(NULL);

	/* Create new program map */
	GFX_Map* map = calloc(1, sizeof(GFX_Map));
	if(!map)
	{
		/* Out of memory error */
		gfx_errors_push(
			GFX_ERROR_OUT_OF_MEMORY,
			"Program Map could not be allocated."
		);
		return NULL;
	}

	if(GFX_WIND_GET.ext[GFX_EXT_PROGRAM_MAP])
	{
		/* Register as object */
		map->id = _gfx_render_object_register(
			&GFX_WIND_GET.objects,
			map,
			&_gfx_program_map_obj_funcs
		);

		if(!map->id.id)
		{
			free(map);
			return NULL;
		}

		/* Create OpenGL resources */
		GFX_REND_GET.CreateProgramPipelines(1, &map->handle);
	}

	return (GFXProgramMap*)map;
}

/******************************************************/
void gfx_program_map_free(

		GFXProgramMap* map)
{
	if(map)
	{
		GFX_WIND_INIT_UNSAFE;

		GFX_Map* internal = (GFX_Map*)map;

		/* Unregister as object */
		_gfx_render_object_unregister(internal->id);

		if(!GFX_WIND_EQ(NULL))
		{
			/* Delete program pipeline */
			if(GFX_WIND_GET.ext[GFX_EXT_PROGRAM_MAP])
				GFX_REND_GET.DeleteProgramPipelines(
					1,
					&internal->handle
				);
		}

		/* Free all programs */
		unsigned char stage;
		for(stage = 0; stage < GFX_INT_NUM_STAGES; ++stage)
		{
			if(internal->stages[stage])
				gfx_program_free(internal->stages[stage]);
		}

		free(map);
	}
}

/******************************************************/
GFXProgram* gfx_program_map_add(

		GFXProgramMap*  map,
		GFXShaderStage  stage,
		size_t          instances)
{
	/* Check if blocked */
	GFX_Map* internal = (GFX_Map*)map;
	if(internal->blocks) return NULL;

	/* Create the program */
	GFXProgram* program = gfx_program_create(instances);
	if(!program) return NULL;

	/* Attempt to map it to the given stage(s) */
	if(!_gfx_program_map_set_stages(internal, stage, program))
	{
		gfx_program_free(program);
		return NULL;
	}

	return program;
}

/******************************************************/
int gfx_program_map_add_share(

		GFXProgramMap*  map,
		GFXShaderStage  stage,
		GFXProgram*     share)
{
	/* Check if blocked */
	GFX_Map* internal = (GFX_Map*)map;
	if(internal->blocks) return 0;

	if(share)
	{
		/* Reference the program */
		if(!_gfx_program_reference(share, 1)) return 0;
	}

	/* Attempt to map it to the given stage(s) */
	if(!_gfx_program_map_set_stages(internal, stage, share))
	{
		gfx_program_free(share);
		return 0;
	}

	return 1;
}

/******************************************************/
GFXProgram* gfx_program_map_get(

		const GFXProgramMap*  map,
		GFXShaderStage        stage)
{
	unsigned char index = _gfx_program_map_get_stage(stage);
	if(index >= GFX_INT_NUM_STAGES) return NULL;

	return ((const GFX_Map*)map)->stages[index];
}
