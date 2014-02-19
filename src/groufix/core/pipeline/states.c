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

#include "groufix/core/pipeline/internal.h"

/* Compatibility defines */
#ifndef GL_POINT
	#define GL_POINT  0x1b00
#endif
#ifndef GL_LINE
	#define GL_LINE   0x1b01
#endif
#ifndef GL_FILL
	#define GL_FILL   0x1b02
#endif

/******************************************************/
static inline void _gfx_states_clear_buffers(GFXPipeState state, const GFX_Extensions* ext)
{
	GLbitfield mask = 0;
	mask |= (state & GFX_CLEAR_COLOR) ? GL_COLOR_BUFFER_BIT : 0;
	mask |= (state & GFX_CLEAR_DEPTH) ? GL_DEPTH_BUFFER_BIT : 0;
	mask |= (state & GFX_CLEAR_STENCIL) ? GL_STENCIL_BUFFER_BIT : 0;

	if(mask) ext->Clear(mask);
}

/******************************************************/
static inline void _gfx_state_set_polygon_mode(GFXPipeState state, const GFX_Extensions* ext)
{
	if(state & GFX_STATE_WIREFRAME) ext->PolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else if(state & GFX_STATE_POINTCLOUD) ext->PolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	else ext->PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

/******************************************************/
static inline void _gfx_state_set_rasterizer(GFXPipeState state, const GFX_Extensions* ext)
{
	if(state & GFX_STATE_NO_RASTERIZER) ext->Enable(GL_RASTERIZER_DISCARD);
	else ext->Disable(GL_RASTERIZER_DISCARD);
}

/******************************************************/
static inline void _gfx_state_set_depth_test(GFXPipeState state, const GFX_Extensions* ext)
{
	if(state & GFX_STATE_DEPTH_TEST) ext->Enable(GL_DEPTH_TEST);
	else ext->Disable(GL_DEPTH_TEST);
}

/******************************************************/
static inline void _gfx_state_set_cull_face(GFXPipeState state, const GFX_Extensions* ext)
{
	if(state & (GFX_STATE_CULL_FRONT | GFX_STATE_CULL_BACK))
	{
		ext->Enable(GL_CULL_FACE);
		ext->CullFace(state & GFX_STATE_CULL_FRONT ? GL_FRONT : GL_BACK);
	}
	else ext->Disable(GL_CULL_FACE);
}

/******************************************************/
static inline void _gfx_state_set_blend(GFXPipeState state, const GFX_Extensions* ext)
{
	if(state & GFX_STATE_BLEND) ext->Enable(GL_BLEND);
	else ext->Disable(GL_BLEND);
}

/******************************************************/
static inline void _gfx_state_set_stencil_test(GFXPipeState state, const GFX_Extensions* ext)
{
	if(state & GFX_STATE_STENCIL_TEST) ext->Enable(GL_STENCIL_TEST);
	else ext->Disable(GL_STENCIL_TEST);
}

/******************************************************/
void _gfx_states_set(GFXPipeState state, GFX_Extensions* ext)
{
	/* Clear buffers & check stripped state */
	_gfx_states_clear_buffers(state, ext);
	state &= ~GFX_CLEAR_ALL;

	GFXPipeState diff = state ^ ext->state;
	ext->state = state;

	if(diff)
	{
		/* Set all states */
		if(diff & (GFX_STATE_WIREFRAME | GFX_STATE_POINTCLOUD))
			_gfx_state_set_polygon_mode(state, ext);
		if(diff & GFX_STATE_NO_RASTERIZER)
			_gfx_state_set_rasterizer(state, ext);
		if(diff & GFX_STATE_DEPTH_WRITE)
			ext->DepthMask(state & GFX_STATE_DEPTH_WRITE ? GL_TRUE : GL_FALSE);
		if(diff & GFX_STATE_DEPTH_TEST)
			_gfx_state_set_depth_test(state, ext);
		if(diff & (GFX_STATE_CULL_FRONT | GFX_STATE_CULL_BACK))
			_gfx_state_set_cull_face(state, ext);
		if(diff & GFX_STATE_BLEND)
			_gfx_state_set_blend(state, ext);
		if(diff & GFX_STATE_STENCIL_TEST)
			_gfx_state_set_stencil_test(state, ext);
	}
}

/******************************************************/
void _gfx_states_force_set(GFXPipeState state, GFX_Extensions* ext)
{
	/* Clear buffers & strip state */
	_gfx_states_clear_buffers(state, ext);
	state &= ~GFX_CLEAR_ALL;

	ext->state = state;

	/* Set all states */
	_gfx_state_set_polygon_mode(state, ext);
	_gfx_state_set_rasterizer(state, ext);
	ext->DepthMask(state & GFX_STATE_DEPTH_WRITE ? GL_TRUE : GL_FALSE);
	_gfx_state_set_depth_test(state, ext);
	_gfx_state_set_cull_face(state, ext);
	_gfx_state_set_blend(state, ext);
	_gfx_state_set_stencil_test(state, ext);
}

/******************************************************/
void _gfx_states_set_viewport(unsigned int width, unsigned int height, GFX_Extensions* ext)
{
	if(ext->width != width || ext->height != height)
	{
		ext->Viewport(0, 0, width, height);
		ext->width = width;
		ext->height = height;
	}
}
