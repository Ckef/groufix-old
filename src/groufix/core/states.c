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

		GFXRenderState state,
		GFX_WIND_ARG)
{
	GLbitfield mask =
		(state & GFX_CLEAR_COLOR ? GL_COLOR_BUFFER_BIT : 0) |
		(state & GFX_CLEAR_DEPTH ? GL_DEPTH_BUFFER_BIT : 0) |
		(state & GFX_CLEAR_STENCIL ? GL_STENCIL_BUFFER_BIT : 0);

	if(mask) GFX_REND_GET.Clear(mask);
}

/******************************************************/
static inline void _gfx_state_set_polygon_mode(

		GFXRenderState state,
		GFX_WIND_ARG)
{
	if(state & GFX_STATE_WIREFRAME)
		GFX_REND_GET.PolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else if(state & GFX_STATE_POINTCLOUD)
		GFX_REND_GET.PolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	else
		GFX_REND_GET.PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

/******************************************************/
static inline void _gfx_state_set_rasterizer(

		GFXRenderState state,
		GFX_WIND_ARG)
{
	if(state & GFX_STATE_NO_RASTERIZER)
		GFX_REND_GET.Enable(GL_RASTERIZER_DISCARD);
	else
		GFX_REND_GET.Disable(GL_RASTERIZER_DISCARD);
}

/******************************************************/
static inline void _gfx_state_set_depth_test(

		GFXRenderState state,
		GFX_WIND_ARG)
{
	if(state & GFX_STATE_DEPTH_TEST)
		GFX_REND_GET.Enable(GL_DEPTH_TEST);
	else
		GFX_REND_GET.Disable(GL_DEPTH_TEST);
}

/******************************************************/
static inline void _gfx_state_set_cull_face(

		GFXRenderState state,
		GFX_WIND_ARG)
{
	if(state & (GFX_STATE_CULL_FRONT | GFX_STATE_CULL_BACK))
	{
		GFX_REND_GET.Enable(GL_CULL_FACE);
		GFX_REND_GET.CullFace(state & GFX_STATE_CULL_FRONT ? GL_FRONT : GL_BACK);
	}
	else GFX_REND_GET.Disable(GL_CULL_FACE);
}

/******************************************************/
static inline void _gfx_state_set_blend(

		GFXRenderState state,
		GFX_WIND_ARG)
{
	if(state & GFX_STATE_BLEND)
		GFX_REND_GET.Enable(GL_BLEND);
	else
		GFX_REND_GET.Disable(GL_BLEND);
}

/******************************************************/
static inline void _gfx_state_set_stencil_test(

		GFXRenderState state,
		GFX_WIND_ARG)
{
	if(state & GFX_STATE_STENCIL_TEST)
		GFX_REND_GET.Enable(GL_STENCIL_TEST);
	else
		GFX_REND_GET.Disable(GL_STENCIL_TEST);
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

		const GFXPipeState* state,
		GFX_WIND_ARG)
{
	/* Clear buffers & check stripped render state */
	_gfx_states_clear_buffers(state->render.state, GFX_WIND_AS_ARG);
	GFXRenderState extState = state->render.state & ~GFX_CLEAR_ALL;

	GFXRenderState diff = extState ^ GFX_WIND_GET.state.render.state;

	/* Set all boolean states */
	if(diff & (GFX_STATE_WIREFRAME | GFX_STATE_POINTCLOUD))
		_gfx_state_set_polygon_mode(extState, GFX_WIND_AS_ARG);
	if(diff & GFX_STATE_NO_RASTERIZER)
		_gfx_state_set_rasterizer(extState, GFX_WIND_AS_ARG);
	if(diff & GFX_STATE_DEPTH_WRITE)
		GFX_REND_GET.DepthMask(extState & GFX_STATE_DEPTH_WRITE ? GL_TRUE : GL_FALSE);
	if(diff & GFX_STATE_DEPTH_TEST)
		_gfx_state_set_depth_test(extState, GFX_WIND_AS_ARG);
	if(diff & (GFX_STATE_CULL_FRONT | GFX_STATE_CULL_BACK))
		_gfx_state_set_cull_face(extState, GFX_WIND_AS_ARG);
	if(diff & GFX_STATE_BLEND)
		_gfx_state_set_blend(extState, GFX_WIND_AS_ARG);
	if(diff & GFX_STATE_STENCIL_TEST)
		_gfx_state_set_stencil_test(extState, GFX_WIND_AS_ARG);

	int comp;

	/* Depth function */
	comp = GFX_WIND_GET.state.depth.test != state->depth.test;
	if(comp) GFX_REND_GET.DepthFunc(state->depth.test);

	/* Blend equation */
	comp =
		(GFX_WIND_GET.state.blend.stateRGB != state->blend.stateRGB) |
		(GFX_WIND_GET.state.blend.stateA != state->blend.stateA);

	if(comp) GFX_REND_GET.BlendEquationSeparate(
		state->blend.stateRGB,
		state->blend.stateA);

	/* Blend functions */
	comp =
		(GFX_WIND_GET.state.blend.sourceRGB != state->blend.sourceRGB) |
		(GFX_WIND_GET.state.blend.bufferRGB != state->blend.bufferRGB) |
		(GFX_WIND_GET.state.blend.sourceA != state->blend.sourceA) |
		(GFX_WIND_GET.state.blend.bufferA != state->blend.bufferA);

	if(comp) GFX_REND_GET.BlendFuncSeparate(
		state->blend.sourceRGB,
		state->blend.bufferRGB,
		state->blend.sourceA,
		state->blend.bufferA);

	/* Stencil front face functions */
	comp =
		(GFX_WIND_GET.state.stencil.testFront != state->stencil.testFront) |
		(GFX_WIND_GET.state.stencil.frontRef != state->stencil.frontRef) |
		(GFX_WIND_GET.state.stencil.frontMask != state->stencil.frontMask);

	if(comp) GFX_REND_GET.StencilFuncSeparate(
		GL_FRONT,
		state->stencil.testFront,
		state->stencil.frontRef,
		state->stencil.frontMask);

	/* Stencil back face functions */
	comp =
		(GFX_WIND_GET.state.stencil.testBack != state->stencil.testBack) |
		(GFX_WIND_GET.state.stencil.backRef != state->stencil.backRef) |
		(GFX_WIND_GET.state.stencil.backMask != state->stencil.backMask);

	if(comp) GFX_REND_GET.StencilFuncSeparate(
		GL_BACK,
		state->stencil.testBack,
		state->stencil.backRef,
		state->stencil.backMask);

	/* Stencil front face operations */
	comp =
		(GFX_WIND_GET.state.stencil.frontFail != state->stencil.frontFail) |
		(GFX_WIND_GET.state.stencil.frontDepth != state->stencil.frontDepth) |
		(GFX_WIND_GET.state.stencil.frontPass != state->stencil.frontPass);

	if(comp) GFX_REND_GET.StencilOpSeparate(
		GL_FRONT,
		state->stencil.frontFail,
		state->stencil.frontDepth,
		state->stencil.frontPass);

	/* Stencil back face operations */
	comp =
		(GFX_WIND_GET.state.stencil.backFail != state->stencil.backFail) |
		(GFX_WIND_GET.state.stencil.backDepth != state->stencil.backDepth) |
		(GFX_WIND_GET.state.stencil.backPass != state->stencil.backPass);

	if(comp) GFX_REND_GET.StencilOpSeparate(
		GL_BACK,
		state->stencil.backFail,
		state->stencil.backDepth,
		state->stencil.backPass);

	/* Set all values */
	GFX_WIND_GET.state = *state;
	GFX_WIND_GET.state.render.state = extState;
}

/******************************************************/
void _gfx_states_force_set(

		const GFXPipeState* state,
		GFX_WIND_ARG)
{
	/* Clear buffers & strip state */
	_gfx_states_clear_buffers(state->render.state, GFX_WIND_AS_ARG);
	GFXRenderState extState = state->render.state & ~GFX_CLEAR_ALL;

	/* Set all boolean states */
	_gfx_state_set_polygon_mode(extState, GFX_WIND_AS_ARG);
	_gfx_state_set_rasterizer(extState, GFX_WIND_AS_ARG);
	GFX_REND_GET.DepthMask(extState & GFX_STATE_DEPTH_WRITE ? GL_TRUE : GL_FALSE);
	_gfx_state_set_depth_test(extState, GFX_WIND_AS_ARG);
	_gfx_state_set_cull_face(extState, GFX_WIND_AS_ARG);
	_gfx_state_set_blend(extState, GFX_WIND_AS_ARG);
	_gfx_state_set_stencil_test(extState, GFX_WIND_AS_ARG);

	/* Depth function */
	GFX_REND_GET.DepthFunc(state->depth.test);

	/* Blending */
	GFX_REND_GET.BlendEquationSeparate(
		state->blend.stateRGB,
		state->blend.stateA);

	GFX_REND_GET.BlendFuncSeparate(
		state->blend.sourceRGB,
		state->blend.bufferRGB,
		state->blend.sourceA,
		state->blend.bufferA);

	/* Stencil test */
	GFX_REND_GET.StencilFuncSeparate(
		GL_FRONT,
		state->stencil.testFront,
		state->stencil.frontRef,
		state->stencil.frontMask);

	GFX_REND_GET.StencilFuncSeparate(
		GL_BACK,
		state->stencil.testBack,
		state->stencil.backRef,
		state->stencil.backMask);

	GFX_REND_GET.StencilOpSeparate(
		GL_FRONT,
		state->stencil.frontFail,
		state->stencil.frontDepth,
		state->stencil.frontPass);

	GFX_REND_GET.StencilOpSeparate(
		GL_BACK,
		state->stencil.backFail,
		state->stencil.backDepth,
		state->stencil.backPass);

	/* Set all values */
	GFX_WIND_GET.state = *state;
	GFX_WIND_GET.state.render.state = extState;
}

/******************************************************/
void _gfx_states_set_viewport(

		GFXViewport viewport,
		GFX_WIND_ARG)
{
	if(
		viewport.x != GFX_REND_GET.viewport.x ||
		viewport.y != GFX_REND_GET.viewport.y ||
		viewport.width != GFX_REND_GET.viewport.width ||
		viewport.height != GFX_REND_GET.viewport.height)
	{
		GFX_REND_GET.Viewport(
			viewport.x,
			viewport.y,
			viewport.width,
			viewport.height
		);

		GFX_REND_GET.viewport = viewport;
	}
}

/******************************************************/
void _gfx_states_set_pixel_pack_alignment(

		unsigned char align,
		GFX_WIND_ARG)
{
	if(GFX_REND_GET.packAlignment != align)
	{
		GFX_REND_GET.PixelStorei(GL_PACK_ALIGNMENT, align);
		GFX_REND_GET.packAlignment = align;
	}
}

/******************************************************/
void _gfx_states_set_pixel_unpack_alignment(

		unsigned char align,
		GFX_WIND_ARG)
{
	if(GFX_REND_GET.unpackAlignment != align)
	{
		GFX_REND_GET.PixelStorei(GL_UNPACK_ALIGNMENT, align);
		GFX_REND_GET.unpackAlignment = align;
	}
}

/******************************************************/
void _gfx_states_set_patch_vertices(

		unsigned int vertices,
		GFX_WIND_ARG)
{
	if(GFX_REND_GET.patchVertices != vertices)
	{
		GFX_REND_GET.PatchParameteri(GL_PATCH_VERTICES, vertices);
		GFX_REND_GET.patchVertices = vertices;
	}
}
