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

		GFXRenderState         state,
		const GFX_Extensions*  ext)
{
	GLbitfield mask =
		(state & GFX_CLEAR_COLOR ? GL_COLOR_BUFFER_BIT : 0) |
		(state & GFX_CLEAR_DEPTH ? GL_DEPTH_BUFFER_BIT : 0) |
		(state & GFX_CLEAR_STENCIL ? GL_STENCIL_BUFFER_BIT : 0);

	if(mask) ext->Clear(mask);
}

/******************************************************/
static inline void _gfx_state_set_polygon_mode(

		GFXRenderState         state,
		const GFX_Extensions*  ext)
{
	if(state & GFX_STATE_WIREFRAME)
		ext->PolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	else if(state & GFX_STATE_POINTCLOUD)
		ext->PolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	else ext->PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

/******************************************************/
static inline void _gfx_state_set_rasterizer(

		GFXRenderState         state,
		const GFX_Extensions*  ext)
{
	if(state & GFX_STATE_NO_RASTERIZER) ext->Enable(GL_RASTERIZER_DISCARD);
	else ext->Disable(GL_RASTERIZER_DISCARD);
}

/******************************************************/
static inline void _gfx_state_set_depth_test(

		GFXRenderState         state,
		const GFX_Extensions*  ext)
{
	if(state & GFX_STATE_DEPTH_TEST) ext->Enable(GL_DEPTH_TEST);
	else ext->Disable(GL_DEPTH_TEST);
}

/******************************************************/
static inline void _gfx_state_set_cull_face(

		GFXRenderState         state,
		const GFX_Extensions*  ext)
{
	if(state & (GFX_STATE_CULL_FRONT | GFX_STATE_CULL_BACK))
	{
		ext->Enable(GL_CULL_FACE);
		ext->CullFace(state & GFX_STATE_CULL_FRONT ? GL_FRONT : GL_BACK);
	}
	else ext->Disable(GL_CULL_FACE);
}

/******************************************************/
static inline void _gfx_state_set_blend(

		GFXRenderState         state,
		const GFX_Extensions*  ext)
{
	if(state & GFX_STATE_BLEND) ext->Enable(GL_BLEND);
	else ext->Disable(GL_BLEND);
}

/******************************************************/
static inline void _gfx_state_set_stencil_test(

		GFXRenderState         state,
		const GFX_Extensions*  ext)
{
	if(state & GFX_STATE_STENCIL_TEST) ext->Enable(GL_STENCIL_TEST);
	else ext->Disable(GL_STENCIL_TEST);
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

		GFXPipeState*    state,
		GFX_Extensions*  ext)
{
	/* Clear buffers & check stripped render state */
	_gfx_states_clear_buffers(state->render.state, ext);
	GFXRenderState extState = state->render.state & ~GFX_CLEAR_ALL;

	GFXRenderState diff = extState ^ ext->state.render.state;

	/* Set all boolean states */
	if(diff & (GFX_STATE_WIREFRAME | GFX_STATE_POINTCLOUD))
		_gfx_state_set_polygon_mode(extState, ext);
	if(diff & GFX_STATE_NO_RASTERIZER)
		_gfx_state_set_rasterizer(extState, ext);
	if(diff & GFX_STATE_DEPTH_WRITE)
		ext->DepthMask(extState & GFX_STATE_DEPTH_WRITE ? GL_TRUE : GL_FALSE);
	if(diff & GFX_STATE_DEPTH_TEST)
		_gfx_state_set_depth_test(extState, ext);
	if(diff & (GFX_STATE_CULL_FRONT | GFX_STATE_CULL_BACK))
		_gfx_state_set_cull_face(extState, ext);
	if(diff & GFX_STATE_BLEND)
		_gfx_state_set_blend(extState, ext);
	if(diff & GFX_STATE_STENCIL_TEST)
		_gfx_state_set_stencil_test(extState, ext);

	int comp;

	/* Depth function */
	comp = ext->state.depth.test != state->depth.test;
	if(comp) ext->DepthFunc(state->depth.test);

	/* Blend equation */
	comp =
		(ext->state.blend.stateRGB != state->blend.stateRGB) |
		(ext->state.blend.stateA != state->blend.stateA);

	if(comp) ext->BlendEquationSeparate(
		state->blend.stateRGB,
		state->blend.stateA);

	/* Blend functions */
	comp =
		(ext->state.blend.sourceRGB != state->blend.sourceRGB) |
		(ext->state.blend.bufferRGB != state->blend.bufferRGB) |
		(ext->state.blend.sourceA != state->blend.sourceA) |
		(ext->state.blend.bufferA != state->blend.bufferA);

	if(comp) ext->BlendFuncSeparate(
		state->blend.sourceRGB,
		state->blend.bufferRGB,
		state->blend.sourceA,
		state->blend.bufferA);

	/* Stencil front face functions */
	comp =
		(ext->state.stencil.testFront != state->stencil.testFront) |
		(ext->state.stencil.frontRef != state->stencil.frontRef) |
		(ext->state.stencil.frontMask != state->stencil.frontMask);

	if(comp) ext->StencilFuncSeparate(
		GL_FRONT,
		state->stencil.testFront,
		state->stencil.frontRef,
		state->stencil.frontMask);

	/* Stencil back face functions */
	comp =
		(ext->state.stencil.testBack != state->stencil.testBack) |
		(ext->state.stencil.backRef != state->stencil.backRef) |
		(ext->state.stencil.backMask != state->stencil.backMask);

	if(comp) ext->StencilFuncSeparate(
		GL_BACK,
		state->stencil.testBack,
		state->stencil.backRef,
		state->stencil.backMask);

	/* Stencil front face operations */
	comp =
		(ext->state.stencil.frontFail != state->stencil.frontFail) |
		(ext->state.stencil.frontDepth != state->stencil.frontDepth) |
		(ext->state.stencil.frontPass != state->stencil.frontPass);

	if(comp) ext->StencilOpSeparate(
		GL_FRONT,
		state->stencil.frontFail,
		state->stencil.frontDepth,
		state->stencil.frontPass);

	/* Stencil back face operations */
	comp =
		(ext->state.stencil.backFail != state->stencil.backFail) |
		(ext->state.stencil.backDepth != state->stencil.backDepth) |
		(ext->state.stencil.backPass != state->stencil.backPass);

	if(comp) ext->StencilOpSeparate(
		GL_BACK,
		state->stencil.backFail,
		state->stencil.backDepth,
		state->stencil.backPass);

	/* Set all values */
	ext->state = *state;
	ext->state.render.state = extState;
}

/******************************************************/
void _gfx_states_force_set(

		GFXPipeState*    state,
		GFX_Extensions*  ext)
{
	/* Clear buffers & strip state */
	_gfx_states_clear_buffers(state->render.state, ext);
	GFXRenderState extState = state->render.state & ~GFX_CLEAR_ALL;

	/* Set all boolean states */
	_gfx_state_set_polygon_mode(extState, ext);
	_gfx_state_set_rasterizer(extState, ext);
	ext->DepthMask(extState & GFX_STATE_DEPTH_WRITE ? GL_TRUE : GL_FALSE);
	_gfx_state_set_depth_test(extState, ext);
	_gfx_state_set_cull_face(extState, ext);
	_gfx_state_set_blend(extState, ext);
	_gfx_state_set_stencil_test(extState, ext);

	/* Depth function */
	ext->DepthFunc(state->depth.test);

	/* Blending */
	ext->BlendEquationSeparate(
		state->blend.stateRGB,
		state->blend.stateA);

	ext->BlendFuncSeparate(
		state->blend.sourceRGB,
		state->blend.bufferRGB,
		state->blend.sourceA,
		state->blend.bufferA);

	/* Stencil test */
	ext->StencilFuncSeparate(
		GL_FRONT,
		state->stencil.testFront,
		state->stencil.frontRef,
		state->stencil.frontMask);

	ext->StencilFuncSeparate(
		GL_BACK,
		state->stencil.testBack,
		state->stencil.backRef,
		state->stencil.backMask);

	ext->StencilOpSeparate(
		GL_FRONT,
		state->stencil.frontFail,
		state->stencil.frontDepth,
		state->stencil.frontPass);

	ext->StencilOpSeparate(
		GL_BACK,
		state->stencil.backFail,
		state->stencil.backDepth,
		state->stencil.backPass);

	/* Set all values */
	ext->state = *state;
	ext->state.render.state = extState;
}

/******************************************************/
void _gfx_states_set_viewport(

		unsigned int     width,
		unsigned int     height,
		GFX_Extensions*  ext)
{
	if(ext->width != width || ext->height != height)
	{
		ext->Viewport(0, 0, width, height);
		ext->width = width;
		ext->height = height;
	}
}

/******************************************************/
void _gfx_states_set_pixel_pack_alignment(

		unsigned char    align,
		GFX_Extensions*  ext)
{
	if(ext->packAlignment != align)
	{
		ext->PixelStorei(GL_PACK_ALIGNMENT, align);
		ext->packAlignment = align;
	}
}

/******************************************************/
void _gfx_states_set_pixel_unpack_alignment(

		unsigned char    align,
		GFX_Extensions*  ext)
{
	if(ext->unpackAlignment != align)
	{
		ext->PixelStorei(GL_UNPACK_ALIGNMENT, align);
		ext->unpackAlignment = align;
	}
}

/******************************************************/
void _gfx_states_set_patch_vertices(

		unsigned int     vertices,
		GFX_Extensions*  ext)
{
	if(ext->patchVertices != vertices)
	{
		ext->PatchParameteri(GL_PATCH_VERTICES, vertices);
		ext->patchVertices = vertices;
	}
}
