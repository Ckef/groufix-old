/**
 * Groufix  :  Graphics Engine produced by Ckef Worx
 * www      :  http://www.ejb.ckef-worx.com
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "groufix/pipeline/internal.h"

#include <stdlib.h>

/******************************************************/
/* Internal Pipe Process */
struct GFX_Internal_Process
{
	/* Super class */
	GFXPipeProcess        process;
	GFX_Internal_Window*  target;
};

/******************************************************/
GFXPipeProcess* _gfx_pipe_process_create(size_t dataSize)
{
	return (GFXPipeProcess*)calloc(1, sizeof(struct GFX_Internal_Process) + dataSize);
}

/******************************************************/
void _gfx_pipe_process_free(GFXPipeProcess* process)
{
	free(process);
}

/******************************************************/
void* gfx_pipe_process_get_data(GFXPipeProcess* process)
{
	return (void*)(((struct GFX_Internal_Process*)process) + 1);
}

/******************************************************/
void _gfx_pipe_process_execute(GFXPipeProcess* process, GFXPipeline* pipeline, GFX_Internal_Window* fallback)
{
	struct GFX_Internal_Process* internal = (struct GFX_Internal_Process*)process;

	/* Execute custom pre-process */
	void* data = (void*)(internal + 1);
	if(process->preprocess) process->preprocess(pipeline, data);

	/* Perform post-processing */
	_gfx_window_make_current(internal->target);
	_gfx_window_make_current(fallback);

	/* Execute custom post-process */
	if(process->postprocess) process->postprocess(pipeline, data);
}
