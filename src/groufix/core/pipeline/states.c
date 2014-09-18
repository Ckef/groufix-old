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

		GFXRenderState  state,
		GFX_Renderer*   rend)
{
	GLbitfield mask =
		(state & GFX_CLEAR_COLOR ? GL_COLOR_BUFFER_BIT : 0) |
		(state & GFX_CLEAR_DEPTH ? GL_DEPTH_BUFFER_BIT : 0) |
		(state & GFX_CLEAR_STENCIL ? GL_STENCIL_BUFFER_BIT : 0);

	if(mask) rend->Clear(mask);
}

/******************************************************/
static inline void _gfx_state_set_polygon_mode(

		GFXRenderState  state,
		GFX_Renderer*   rend)
{
	if(state & GFX_STATE_WIREFRAME)
		rend->PolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	else if(state & GFX_STATE_POINTCLOUD)
		rend->PolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	else rend->PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

/******************************************************/
static inline void _gfx_state_set_rasterizer(

		GFXRenderState  state,
		GFX_Renderer*   rend)
{
	if(state & GFX_STATE_NO_RASTERIZER) rend->Enable(GL_RASTERIZER_DISCARD);
	else rend->Disable(GL_RASTERIZER_DISCARD);
}

/******************************************************/
static inline void _gfx_state_set_depth_test(

		GFXRenderState  state,
		GFX_Renderer*   rend)
{
	if(state & GFX_STATE_DEPTH_TEST) rend->Enable(GL_DEPTH_TEST);
	else rend->Disable(GL_DEPTH_TEST);
}

/******************************************************/
static inline void _gfx_state_set_cull_face(

		GFXRenderState  state,
		GFX_Renderer*   rend)
{
	if(state & (GFX_STATE_CULL_FRONT | GFX_STATE_CULL_BACK))
	{
		rend->Enable(GL_CULL_FACE);
		rend->CullFace(state & GFX_STATE_CULL_FRONT ? GL_FRONT : GL_BACK);
	}
	else rend->Disable(GL_CULL_FACE);
}

/******************************************************/
static inline void _gfx_state_set_blend(

		GFXRenderState  state,
		GFX_Renderer*   rend)
{
	if(state & GFX_STATE_BLEND) rend->Enable(GL_BLEND);
	else rend->Disable(GL_BLEND);
}

/******************************************************/
static inline void _gfx_state_set_stencil_test(

		GFXRenderState  state,
		GFX_Renderer*   rend)
{
	if(state & GFX_STATE_STENCIL_TEST) rend->Enable(GL_STENCIL_TEST);
	else rend->Disable(GL_STENCIL_TEST);
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

		GFXPipeState*  state,
		GFX_Window*    window)
{
	/* Clear buffers & check stripped render state */
	_gfx_states_clear_buffers(state->render.state, &window->renderer);
	GFXRenderState extState = state->render.state & ~GFX_CLEAR_ALL;

	GFXRenderState diff = extState ^ window->state.render.state;

	/* Set all boolean states */
	if(diff & (GFX_STATE_WIREFRAME | GFX_STATE_POINTCLOUD))
		_gfx_state_set_polygon_mode(extState, &window->renderer);
	if(diff & GFX_STATE_NO_RASTERIZER)
		_gfx_state_set_rasterizer(extState, &window->renderer);
	if(diff & GFX_STATE_DEPTH_WRITE)
		window->renderer.DepthMask(extState & GFX_STATE_DEPTH_WRITE ? GL_TRUE : GL_FALSE);
	if(diff & GFX_STATE_DEPTH_TEST)
		_gfx_state_set_depth_test(extState, &window->renderer);
	if(diff & (GFX_STATE_CULL_FRONT | GFX_STATE_CULL_BACK))
		_gfx_state_set_cull_face(extState, &window->renderer);
	if(diff & GFX_STATE_BLEND)
		_gfx_state_set_blend(extState, &window->renderer);
	if(diff & GFX_STATE_STENCIL_TEST)
		_gfx_state_set_stencil_test(extState, &window->renderer);

	int comp;

	/* Depth function */
	comp = window->state.depth.test != state->depth.test;
	if(comp) window->renderer.DepthFunc(state->depth.test);

	/* Blend equation */
	comp =
		(window->state.blend.stateRGB != state->blend.stateRGB) |
		(window->state.blend.stateA != state->blend.stateA);

	if(comp) window->renderer.BlendEquationSeparate(
		state->blend.stateRGB,
		state->blend.stateA);

	/* Blend functions */
	comp =
		(window->state.blend.sourceRGB != state->blend.sourceRGB) |
		(window->state.blend.bufferRGB != state->blend.bufferRGB) |
		(window->state.blend.sourceA != state->blend.sourceA) |
		(window->state.blend.bufferA != state->blend.bufferA);

	if(comp) window->renderer.BlendFuncSeparate(
		state->blend.sourceRGB,
		state->blend.bufferRGB,
		state->blend.sourceA,
		state->blend.bufferA);

	/* Stencil front face functions */
	comp =
		(window->state.stencil.testFront != state->stencil.testFront) |
		(window->state.stencil.frontRef != state->stencil.frontRef) |
		(window->state.stencil.frontMask != state->stencil.frontMask);

	if(comp) window->renderer.StencilFuncSeparate(
		GL_FRONT,
		state->stencil.testFront,
		state->stencil.frontRef,
		state->stencil.frontMask);

	/* Stencil back face functions */
	comp =
		(window->state.stencil.testBack != state->stencil.testBack) |
		(window->state.stencil.backRef != state->stencil.backRef) |
		(window->state.stencil.backMask != state->stencil.backMask);

	if(comp) window->renderer.StencilFuncSeparate(
		GL_BACK,
		state->stencil.testBack,
		state->stencil.backRef,
		state->stencil.backMask);

	/* Stencil front face operations */
	comp =
		(window->state.stencil.frontFail != state->stencil.frontFail) |
		(window->state.stencil.frontDepth != state->stencil.frontDepth) |
		(window->state.stencil.frontPass != state->stencil.frontPass);

	if(comp) window->renderer.StencilOpSeparate(
		GL_FRONT,
		state->stencil.frontFail,
		state->stencil.frontDepth,
		state->stencil.frontPass);

	/* Stencil back face operations */
	comp =
		(window->state.stencil.backFail != state->stencil.backFail) |
		(window->state.stencil.backDepth != state->stencil.backDepth) |
		(window->state.stencil.backPass != state->stencil.backPass);

	if(comp) window->renderer.StencilOpSeparate(
		GL_BACK,
		state->stencil.backFail,
		state->stencil.backDepth,
		state->stencil.backPass);

	/* Set all values */
	window->state = *state;
	window->state.render.state = extState;
}

/******************************************************/
void _gfx_states_force_set(

		GFXPipeState*  state,
		GFX_Window*    window)
{
	/* Clear buffers & strip state */
	_gfx_states_clear_buffers(state->render.state, &window->renderer);
	GFXRenderState extState = state->render.state & ~GFX_CLEAR_ALL;

	/* Set all boolean states */
	_gfx_state_set_polygon_mode(extState, &window->renderer);
	_gfx_state_set_rasterizer(extState, &window->renderer);
	window->renderer.DepthMask(extState & GFX_STATE_DEPTH_WRITE ? GL_TRUE : GL_FALSE);
	_gfx_state_set_depth_test(extState, &window->renderer);
	_gfx_state_set_cull_face(extState, &window->renderer);
	_gfx_state_set_blend(extState, &window->renderer);
	_gfx_state_set_stencil_test(extState, &window->renderer);

	/* Depth function */
	window->renderer.DepthFunc(state->depth.test);

	/* Blending */
	window->renderer.BlendEquationSeparate(
		state->blend.stateRGB,
		state->blend.stateA);

	window->renderer.BlendFuncSeparate(
		state->blend.sourceRGB,
		state->blend.bufferRGB,
		state->blend.sourceA,
		state->blend.bufferA);

	/* Stencil test */
	window->renderer.StencilFuncSeparate(
		GL_FRONT,
		state->stencil.testFront,
		state->stencil.frontRef,
		state->stencil.frontMask);

	window->renderer.StencilFuncSeparate(
		GL_BACK,
		state->stencil.testBack,
		state->stencil.backRef,
		state->stencil.backMask);

	window->renderer.StencilOpSeparate(
		GL_FRONT,
		state->stencil.frontFail,
		state->stencil.frontDepth,
		state->stencil.frontPass);

	window->renderer.StencilOpSeparate(
		GL_BACK,
		state->stencil.backFail,
		state->stencil.backDepth,
		state->stencil.backPass);

	/* Set all values */
	window->state = *state;
	window->state.render.state = extState;
}

/******************************************************/
void _gfx_states_set_viewport(

		int            x,
		int            y,
		unsigned int   width,
		unsigned int   height,
		GFX_Renderer*  rend)
{
	if(
		rend->x != x ||
		rend->y != y ||
		rend->width != width ||
		rend->height != height)
	{
		rend->Viewport(x, y, width, height);

		rend->x      = x;
		rend->y      = y;
		rend->width  = width;
		rend->height = height;
	}
}

/******************************************************/
void _gfx_states_set_pixel_pack_alignment(

		unsigned char  align,
		GFX_Renderer*  rend)
{
	if(rend->packAlignment != align)
	{
		rend->PixelStorei(GL_PACK_ALIGNMENT, align);
		rend->packAlignment = align;
	}
}

/******************************************************/
void _gfx_states_set_pixel_unpack_alignment(

		unsigned char  align,
		GFX_Renderer*  rend)
{
	if(rend->unpackAlignment != align)
	{
		rend->PixelStorei(GL_UNPACK_ALIGNMENT, align);
		rend->unpackAlignment = align;
	}
}

/******************************************************/
void _gfx_states_set_patch_vertices(

		unsigned int   vertices,
		GFX_Renderer*  rend)
{
	if(rend->patchVertices != vertices)
	{
		rend->PatchParameteri(GL_PATCH_VERTICES, vertices);
		rend->patchVertices = vertices;
	}
}
