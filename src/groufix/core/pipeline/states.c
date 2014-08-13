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

#include <limits.h>

/* Compatibility defines */
#ifndef GL_FILL
	#define GL_FILL            0x1b02
#endif
#ifndef GL_LINE
	#define GL_LINE            0x1b01
#endif
#ifndef GL_PATCH_VERTICES
	#define GL_PATCH_VERTICES  0x8e72
#endif
#ifndef GL_POINT
	#define GL_POINT           0x1b00
#endif

/******************************************************/
static inline void _gfx_states_clear_buffers(

		GFXRenderState state)
{
	GLbitfield mask =
		(state & GFX_CLEAR_COLOR ? GL_COLOR_BUFFER_BIT : 0) |
		(state & GFX_CLEAR_DEPTH ? GL_DEPTH_BUFFER_BIT : 0) |
		(state & GFX_CLEAR_STENCIL ? GL_STENCIL_BUFFER_BIT : 0);

	if(mask) (GFX_EXT)->Clear(mask);
}

/******************************************************/
static inline void _gfx_state_set_polygon_mode(

		GFXRenderState state)
{
	if(state & GFX_STATE_WIREFRAME)
		(GFX_EXT)->PolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	else if(state & GFX_STATE_POINTCLOUD)
		(GFX_EXT)->PolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	else (GFX_EXT)->PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

/******************************************************/
static inline void _gfx_state_set_rasterizer(

		GFXRenderState state)
{
	if(state & GFX_STATE_NO_RASTERIZER) (GFX_EXT)->Enable(GL_RASTERIZER_DISCARD);
	else (GFX_EXT)->Disable(GL_RASTERIZER_DISCARD);
}

/******************************************************/
static inline void _gfx_state_set_depth_test(

		GFXRenderState state)
{
	if(state & GFX_STATE_DEPTH_TEST) (GFX_EXT)->Enable(GL_DEPTH_TEST);
	else (GFX_EXT)->Disable(GL_DEPTH_TEST);
}

/******************************************************/
static inline void _gfx_state_set_cull_face(

		GFXRenderState state)
{
	if(state & (GFX_STATE_CULL_FRONT | GFX_STATE_CULL_BACK))
	{
		(GFX_EXT)->Enable(GL_CULL_FACE);
		(GFX_EXT)->CullFace(state & GFX_STATE_CULL_FRONT ? GL_FRONT : GL_BACK);
	}
	else (GFX_EXT)->Disable(GL_CULL_FACE);
}

/******************************************************/
static inline void _gfx_state_set_blend(

		GFXRenderState state)
{
	if(state & GFX_STATE_BLEND) (GFX_EXT)->Enable(GL_BLEND);
	else (GFX_EXT)->Disable(GL_BLEND);
}

/******************************************************/
static inline void _gfx_state_set_stencil_test(

		GFXRenderState state)
{
	if(state & GFX_STATE_STENCIL_TEST) (GFX_EXT)->Enable(GL_STENCIL_TEST);
	else (GFX_EXT)->Disable(GL_STENCIL_TEST);
}

/******************************************************/
void _gfx_states_set_default(

		GFXPipeState* state)
{
	state->render.state        = GFX_STATE_DEFAULT;

	state->depth.test          = GFX_FRAG_LESS;

	state->blend.stateRGB      = GFX_BLEND_ADD;
	state->blend.stateA        = GFX_BLEND_ADD;
	state->blend.sourceRGB     = GFX_BLEND_ONE;
	state->blend.sourceA       = GFX_BLEND_ONE;
	state->blend.bufferRGB     = GFX_BLEND_ZERO;
	state->blend.bufferA       = GFX_BLEND_ZERO;

	state->stencil.testFront   = GFX_FRAG_ALWAYS;
	state->stencil.testBack    = GFX_FRAG_ALWAYS;

	state->stencil.frontFail   = GFX_STENCIL_KEEP;
	state->stencil.frontDepth  = GFX_STENCIL_KEEP;
	state->stencil.frontPass   = GFX_STENCIL_KEEP;
	state->stencil.frontRef    = 0;
	state->stencil.frontMask   = UINT_MAX;

	state->stencil.backFail    = GFX_STENCIL_KEEP;
	state->stencil.backDepth   = GFX_STENCIL_KEEP;
	state->stencil.backPass    = GFX_STENCIL_KEEP;
	state->stencil.backRef     = 0;
	state->stencil.backMask    = UINT_MAX;
}

/******************************************************/
void _gfx_states_set(

		GFXPipeState* state)
{
	/* Clear buffers & check stripped render state */
	_gfx_states_clear_buffers(state->render.state);
	GFXRenderState extState = state->render.state & ~GFX_CLEAR_ALL;

	GFXRenderState diff = extState ^ (GFX_EXT)->state.render.state;

	/* Set all boolean states */
	if(diff & (GFX_STATE_WIREFRAME | GFX_STATE_POINTCLOUD))
		_gfx_state_set_polygon_mode(extState);
	if(diff & GFX_STATE_NO_RASTERIZER)
		_gfx_state_set_rasterizer(extState);
	if(diff & GFX_STATE_DEPTH_WRITE)
		(GFX_EXT)->DepthMask(extState & GFX_STATE_DEPTH_WRITE ? GL_TRUE : GL_FALSE);
	if(diff & GFX_STATE_DEPTH_TEST)
		_gfx_state_set_depth_test(extState);
	if(diff & (GFX_STATE_CULL_FRONT | GFX_STATE_CULL_BACK))
		_gfx_state_set_cull_face(extState);
	if(diff & GFX_STATE_BLEND)
		_gfx_state_set_blend(extState);
	if(diff & GFX_STATE_STENCIL_TEST)
		_gfx_state_set_stencil_test(extState);

	int comp;

	/* Depth function */
	comp = (GFX_EXT)->state.depth.test != state->depth.test;
	if(comp) (GFX_EXT)->DepthFunc(state->depth.test);

	/* Blend equation */
	comp =
		((GFX_EXT)->state.blend.stateRGB != state->blend.stateRGB) |
		((GFX_EXT)->state.blend.stateA != state->blend.stateA);

	if(comp) (GFX_EXT)->BlendEquationSeparate(
		state->blend.stateRGB,
		state->blend.stateA);

	/* Blend functions */
	comp =
		((GFX_EXT)->state.blend.sourceRGB != state->blend.sourceRGB) |
		((GFX_EXT)->state.blend.bufferRGB != state->blend.bufferRGB) |
		((GFX_EXT)->state.blend.sourceA != state->blend.sourceA) |
		((GFX_EXT)->state.blend.bufferA != state->blend.bufferA);

	if(comp) (GFX_EXT)->BlendFuncSeparate(
		state->blend.sourceRGB,
		state->blend.bufferRGB,
		state->blend.sourceA,
		state->blend.bufferA);

	/* Stencil front face functions */
	comp =
		((GFX_EXT)->state.stencil.testFront != state->stencil.testFront) |
		((GFX_EXT)->state.stencil.frontRef != state->stencil.frontRef) |
		((GFX_EXT)->state.stencil.frontMask != state->stencil.frontMask);

	if(comp) (GFX_EXT)->StencilFuncSeparate(
		GL_FRONT,
		state->stencil.testFront,
		state->stencil.frontRef,
		state->stencil.frontMask);

	/* Stencil back face functions */
	comp =
		((GFX_EXT)->state.stencil.testBack != state->stencil.testBack) |
		((GFX_EXT)->state.stencil.backRef != state->stencil.backRef) |
		((GFX_EXT)->state.stencil.backMask != state->stencil.backMask);

	if(comp) (GFX_EXT)->StencilFuncSeparate(
		GL_BACK,
		state->stencil.testBack,
		state->stencil.backRef,
		state->stencil.backMask);

	/* Stencil front face operations */
	comp =
		((GFX_EXT)->state.stencil.frontFail != state->stencil.frontFail) |
		((GFX_EXT)->state.stencil.frontDepth != state->stencil.frontDepth) |
		((GFX_EXT)->state.stencil.frontPass != state->stencil.frontPass);

	if(comp) (GFX_EXT)->StencilOpSeparate(
		GL_FRONT,
		state->stencil.frontFail,
		state->stencil.frontDepth,
		state->stencil.frontPass);

	/* Stencil back face operations */
	comp =
		((GFX_EXT)->state.stencil.backFail != state->stencil.backFail) |
		((GFX_EXT)->state.stencil.backDepth != state->stencil.backDepth) |
		((GFX_EXT)->state.stencil.backPass != state->stencil.backPass);

	if(comp) (GFX_EXT)->StencilOpSeparate(
		GL_BACK,
		state->stencil.backFail,
		state->stencil.backDepth,
		state->stencil.backPass);

	/* Set all values */
	(GFX_EXT)->state = *state;
	(GFX_EXT)->state.render.state = extState;
}

/******************************************************/
void _gfx_states_force_set(

		GFXPipeState* state)
{
	/* Clear buffers & strip state */
	_gfx_states_clear_buffers(state->render.state);
	GFXRenderState extState = state->render.state & ~GFX_CLEAR_ALL;

	/* Set all boolean states */
	_gfx_state_set_polygon_mode(extState);
	_gfx_state_set_rasterizer(extState);
	(GFX_EXT)->DepthMask(extState & GFX_STATE_DEPTH_WRITE ? GL_TRUE : GL_FALSE);
	_gfx_state_set_depth_test(extState);
	_gfx_state_set_cull_face(extState);
	_gfx_state_set_blend(extState);
	_gfx_state_set_stencil_test(extState);

	/* Depth function */
	(GFX_EXT)->DepthFunc(state->depth.test);

	/* Blending */
	(GFX_EXT)->BlendEquationSeparate(
		state->blend.stateRGB,
		state->blend.stateA);

	(GFX_EXT)->BlendFuncSeparate(
		state->blend.sourceRGB,
		state->blend.bufferRGB,
		state->blend.sourceA,
		state->blend.bufferA);

	/* Stencil test */
	(GFX_EXT)->StencilFuncSeparate(
		GL_FRONT,
		state->stencil.testFront,
		state->stencil.frontRef,
		state->stencil.frontMask);

	(GFX_EXT)->StencilFuncSeparate(
		GL_BACK,
		state->stencil.testBack,
		state->stencil.backRef,
		state->stencil.backMask);

	(GFX_EXT)->StencilOpSeparate(
		GL_FRONT,
		state->stencil.frontFail,
		state->stencil.frontDepth,
		state->stencil.frontPass);

	(GFX_EXT)->StencilOpSeparate(
		GL_BACK,
		state->stencil.backFail,
		state->stencil.backDepth,
		state->stencil.backPass);

	/* Set all values */
	(GFX_EXT)->state = *state;
	(GFX_EXT)->state.render.state = extState;
}

/******************************************************/
void _gfx_states_set_viewport(

		int           x,
		int           y,
		unsigned int  width,
		unsigned int  height)
{
	if(
		(GFX_EXT)->x != x ||
		(GFX_EXT)->y != y ||
		(GFX_EXT)->width != width ||
		(GFX_EXT)->height != height)
	{
		(GFX_EXT)->Viewport(x, y, width, height);

		(GFX_EXT)->x      = x;
		(GFX_EXT)->y      = y;
		(GFX_EXT)->width  = width;
		(GFX_EXT)->height = height;
	}
}

/******************************************************/
void _gfx_states_set_pixel_pack_alignment(

		unsigned char align)
{
	if((GFX_EXT)->packAlignment != align)
	{
		(GFX_EXT)->PixelStorei(GL_PACK_ALIGNMENT, align);
		(GFX_EXT)->packAlignment = align;
	}
}

/******************************************************/
void _gfx_states_set_pixel_unpack_alignment(

		unsigned char align)
{
	if((GFX_EXT)->unpackAlignment != align)
	{
		(GFX_EXT)->PixelStorei(GL_UNPACK_ALIGNMENT, align);
		(GFX_EXT)->unpackAlignment = align;
	}
}

/******************************************************/
void _gfx_states_set_patch_vertices(

		unsigned int vertices)
{
	if((GFX_EXT)->patchVertices != vertices)
	{
		(GFX_EXT)->PatchParameteri(GL_PATCH_VERTICES, vertices);
		(GFX_EXT)->patchVertices = vertices;
	}
}
