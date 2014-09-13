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

	if(mask) (GFX_RND).Clear(mask);
}

/******************************************************/
static inline void _gfx_state_set_polygon_mode(

		GFXRenderState state)
{
	if(state & GFX_STATE_WIREFRAME)
		(GFX_RND).PolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	else if(state & GFX_STATE_POINTCLOUD)
		(GFX_RND).PolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	else (GFX_RND).PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

/******************************************************/
static inline void _gfx_state_set_rasterizer(

		GFXRenderState state)
{
	if(state & GFX_STATE_NO_RASTERIZER) (GFX_RND).Enable(GL_RASTERIZER_DISCARD);
	else (GFX_RND).Disable(GL_RASTERIZER_DISCARD);
}

/******************************************************/
static inline void _gfx_state_set_depth_test(

		GFXRenderState state)
{
	if(state & GFX_STATE_DEPTH_TEST) (GFX_RND).Enable(GL_DEPTH_TEST);
	else (GFX_RND).Disable(GL_DEPTH_TEST);
}

/******************************************************/
static inline void _gfx_state_set_cull_face(

		GFXRenderState state)
{
	if(state & (GFX_STATE_CULL_FRONT | GFX_STATE_CULL_BACK))
	{
		(GFX_RND).Enable(GL_CULL_FACE);
		(GFX_RND).CullFace(state & GFX_STATE_CULL_FRONT ? GL_FRONT : GL_BACK);
	}
	else (GFX_RND).Disable(GL_CULL_FACE);
}

/******************************************************/
static inline void _gfx_state_set_blend(

		GFXRenderState state)
{
	if(state & GFX_STATE_BLEND) (GFX_RND).Enable(GL_BLEND);
	else (GFX_RND).Disable(GL_BLEND);
}

/******************************************************/
static inline void _gfx_state_set_stencil_test(

		GFXRenderState state)
{
	if(state & GFX_STATE_STENCIL_TEST) (GFX_RND).Enable(GL_STENCIL_TEST);
	else (GFX_RND).Disable(GL_STENCIL_TEST);
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

	GFXRenderState diff = extState ^ (GFX_WND)->state.render.state;

	/* Set all boolean states */
	if(diff & (GFX_STATE_WIREFRAME | GFX_STATE_POINTCLOUD))
		_gfx_state_set_polygon_mode(extState);
	if(diff & GFX_STATE_NO_RASTERIZER)
		_gfx_state_set_rasterizer(extState);
	if(diff & GFX_STATE_DEPTH_WRITE)
		(GFX_RND).DepthMask(extState & GFX_STATE_DEPTH_WRITE ? GL_TRUE : GL_FALSE);
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
	comp = (GFX_WND)->state.depth.test != state->depth.test;
	if(comp) (GFX_RND).DepthFunc(state->depth.test);

	/* Blend equation */
	comp =
		((GFX_WND)->state.blend.stateRGB != state->blend.stateRGB) |
		((GFX_WND)->state.blend.stateA != state->blend.stateA);

	if(comp) (GFX_RND).BlendEquationSeparate(
		state->blend.stateRGB,
		state->blend.stateA);

	/* Blend functions */
	comp =
		((GFX_WND)->state.blend.sourceRGB != state->blend.sourceRGB) |
		((GFX_WND)->state.blend.bufferRGB != state->blend.bufferRGB) |
		((GFX_WND)->state.blend.sourceA != state->blend.sourceA) |
		((GFX_WND)->state.blend.bufferA != state->blend.bufferA);

	if(comp) (GFX_RND).BlendFuncSeparate(
		state->blend.sourceRGB,
		state->blend.bufferRGB,
		state->blend.sourceA,
		state->blend.bufferA);

	/* Stencil front face functions */
	comp =
		((GFX_WND)->state.stencil.testFront != state->stencil.testFront) |
		((GFX_WND)->state.stencil.frontRef != state->stencil.frontRef) |
		((GFX_WND)->state.stencil.frontMask != state->stencil.frontMask);

	if(comp) (GFX_RND).StencilFuncSeparate(
		GL_FRONT,
		state->stencil.testFront,
		state->stencil.frontRef,
		state->stencil.frontMask);

	/* Stencil back face functions */
	comp =
		((GFX_WND)->state.stencil.testBack != state->stencil.testBack) |
		((GFX_WND)->state.stencil.backRef != state->stencil.backRef) |
		((GFX_WND)->state.stencil.backMask != state->stencil.backMask);

	if(comp) (GFX_RND).StencilFuncSeparate(
		GL_BACK,
		state->stencil.testBack,
		state->stencil.backRef,
		state->stencil.backMask);

	/* Stencil front face operations */
	comp =
		((GFX_WND)->state.stencil.frontFail != state->stencil.frontFail) |
		((GFX_WND)->state.stencil.frontDepth != state->stencil.frontDepth) |
		((GFX_WND)->state.stencil.frontPass != state->stencil.frontPass);

	if(comp) (GFX_RND).StencilOpSeparate(
		GL_FRONT,
		state->stencil.frontFail,
		state->stencil.frontDepth,
		state->stencil.frontPass);

	/* Stencil back face operations */
	comp =
		((GFX_WND)->state.stencil.backFail != state->stencil.backFail) |
		((GFX_WND)->state.stencil.backDepth != state->stencil.backDepth) |
		((GFX_WND)->state.stencil.backPass != state->stencil.backPass);

	if(comp) (GFX_RND).StencilOpSeparate(
		GL_BACK,
		state->stencil.backFail,
		state->stencil.backDepth,
		state->stencil.backPass);

	/* Set all values */
	(GFX_WND)->state = *state;
	(GFX_WND)->state.render.state = extState;
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
	(GFX_RND).DepthMask(extState & GFX_STATE_DEPTH_WRITE ? GL_TRUE : GL_FALSE);
	_gfx_state_set_depth_test(extState);
	_gfx_state_set_cull_face(extState);
	_gfx_state_set_blend(extState);
	_gfx_state_set_stencil_test(extState);

	/* Depth function */
	(GFX_RND).DepthFunc(state->depth.test);

	/* Blending */
	(GFX_RND).BlendEquationSeparate(
		state->blend.stateRGB,
		state->blend.stateA);

	(GFX_RND).BlendFuncSeparate(
		state->blend.sourceRGB,
		state->blend.bufferRGB,
		state->blend.sourceA,
		state->blend.bufferA);

	/* Stencil test */
	(GFX_RND).StencilFuncSeparate(
		GL_FRONT,
		state->stencil.testFront,
		state->stencil.frontRef,
		state->stencil.frontMask);

	(GFX_RND).StencilFuncSeparate(
		GL_BACK,
		state->stencil.testBack,
		state->stencil.backRef,
		state->stencil.backMask);

	(GFX_RND).StencilOpSeparate(
		GL_FRONT,
		state->stencil.frontFail,
		state->stencil.frontDepth,
		state->stencil.frontPass);

	(GFX_RND).StencilOpSeparate(
		GL_BACK,
		state->stencil.backFail,
		state->stencil.backDepth,
		state->stencil.backPass);

	/* Set all values */
	(GFX_WND)->state = *state;
	(GFX_WND)->state.render.state = extState;
}

/******************************************************/
void _gfx_states_set_viewport(

		int           x,
		int           y,
		unsigned int  width,
		unsigned int  height)
{
	if(
		(GFX_RND).x != x ||
		(GFX_RND).y != y ||
		(GFX_RND).width != width ||
		(GFX_RND).height != height)
	{
		(GFX_RND).Viewport(x, y, width, height);

		(GFX_RND).x      = x;
		(GFX_RND).y      = y;
		(GFX_RND).width  = width;
		(GFX_RND).height = height;
	}
}

/******************************************************/
void _gfx_states_set_pixel_pack_alignment(

		unsigned char align)
{
	if((GFX_RND).packAlignment != align)
	{
		(GFX_RND).PixelStorei(GL_PACK_ALIGNMENT, align);
		(GFX_RND).packAlignment = align;
	}
}

/******************************************************/
void _gfx_states_set_pixel_unpack_alignment(

		unsigned char align)
{
	if((GFX_RND).unpackAlignment != align)
	{
		(GFX_RND).PixelStorei(GL_UNPACK_ALIGNMENT, align);
		(GFX_RND).unpackAlignment = align;
	}
}

/******************************************************/
void _gfx_states_set_patch_vertices(

		unsigned int vertices)
{
	if((GFX_RND).patchVertices != vertices)
	{
		(GFX_RND).PatchParameteri(GL_PATCH_VERTICES, vertices);
		(GFX_RND).patchVertices = vertices;
	}
}
