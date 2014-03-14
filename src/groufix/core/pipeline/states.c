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
void _gfx_states_set_default(GFX_State* state)
{
	state->state            = GFX_STATE_DEFAULT;
	state->depthFunc        = GFX_FRAG_LESS;
	state->blendRGB         = GFX_BLEND_ADD;
	state->blendAlpha       = GFX_BLEND_ADD;
	state->blendSourceRGB   = GFX_BLEND_ONE;
	state->blendSourceAlpha = GFX_BLEND_ONE;
	state->blendBufferRGB   = GFX_BLEND_ZERO;
	state->blendBufferAlpha = GFX_BLEND_ZERO;
	state->stencilFuncFront = GFX_FRAG_ALWAYS;
	state->stencilFuncBack  = GFX_FRAG_ALWAYS;
	state->stencilRefFront  = 0;
	state->stencilRefBack   = 0;
	state->stencilMaskFront = ~((GLuint)0);
	state->stencilMaskBack  = ~((GLuint)0);
	state->stencilFailFront = GFX_STENCIL_KEEP;
	state->stencilFailBack  = GFX_STENCIL_KEEP;
	state->depthFailFront   = GFX_STENCIL_KEEP;
	state->depthFailBack    = GFX_STENCIL_KEEP;
	state->stencilPassFront = GFX_STENCIL_KEEP;
	state->stencilPassBack  = GFX_STENCIL_KEEP;
}

/******************************************************/
void _gfx_states_set(GFX_State* state, GFX_Extensions* ext)
{
	/* Clear buffers & check stripped state */
	_gfx_states_clear_buffers(state->state, ext);
	GFXPipeState extState = state->state & ~GFX_CLEAR_ALL;

	GFXPipeState diff = extState ^ ext->state.state;

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
	comp = ext->state.depthFunc != state->depthFunc;
	if(comp) ext->DepthFunc(state->depthFunc);

	/* Blend equation */
	comp = ext->state.blendRGB != state->blendRGB;
	comp |= ext->state.blendAlpha != state->blendAlpha;

	if(comp) ext->BlendEquationSeparate(
		state->blendRGB,
		state->blendAlpha
	);

	/* Blend functions */
	comp = ext->state.blendSourceRGB != state->blendSourceRGB;
	comp |= ext->state.blendBufferRGB != state->blendBufferRGB;
	comp |= ext->state.blendSourceAlpha != state->blendSourceAlpha;
	comp |= ext->state.blendBufferAlpha != state->blendBufferAlpha;

	if(comp) ext->BlendFuncSeparate(
		state->blendSourceRGB,
		state->blendBufferRGB,
		state->blendSourceAlpha,
		state->blendBufferAlpha
	);

	/* Stencil front face functions */
	comp = ext->state.stencilFuncFront != state->stencilFuncFront;
	comp |= ext->state.stencilRefFront != state->stencilRefFront;
	comp |= ext->state.stencilMaskFront != state->stencilMaskFront;

	if(comp) ext->StencilFuncSeparate(
		GL_FRONT,
		state->stencilFuncFront,
		state->stencilRefFront,
		state->stencilMaskFront
	);

	/* Stencil back face functions */
	comp = ext->state.stencilFuncBack != state->stencilFuncBack;
	comp |= ext->state.stencilRefBack != state->stencilRefBack;
	comp |= ext->state.stencilMaskBack != state->stencilMaskBack;

	if(comp) ext->StencilFuncSeparate(
		GL_BACK,
		state->stencilFuncBack,
		state->stencilRefBack,
		state->stencilMaskBack
	);

	/* Stencil front face operations */
	comp = ext->state.stencilFailFront != state->stencilFailFront;
	comp |= ext->state.depthFailFront != state->depthFailFront;
	comp |= ext->state.stencilPassFront != state->stencilPassFront;

	if(comp) ext->StencilOpSeparate(
		GL_FRONT,
		state->stencilFailFront,
		state->depthFailFront,
		state->stencilPassFront
	);

	/* Stencil back face operations */
	comp = ext->state.stencilFailBack != state->stencilFailBack;
	comp |= ext->state.depthFailBack != state->depthFailBack;
	comp |= ext->state.stencilPassBack != state->stencilPassBack;

	if(comp) ext->StencilOpSeparate(
		GL_BACK,
		state->stencilFailBack,
		state->depthFailBack,
		state->stencilPassBack
	);

	/* Set all values */
	ext->state = *state;
}

/******************************************************/
void _gfx_states_force_set(GFX_State* state, GFX_Extensions* ext)
{
	/* Clear buffers & strip state */
	_gfx_states_clear_buffers(state->state, ext);
	GFXPipeState extState = state->state & ~GFX_CLEAR_ALL;

	/* Set all boolean states */
	_gfx_state_set_polygon_mode(extState, ext);
	_gfx_state_set_rasterizer(extState, ext);
	ext->DepthMask(extState & GFX_STATE_DEPTH_WRITE ? GL_TRUE : GL_FALSE);
	_gfx_state_set_depth_test(extState, ext);
	_gfx_state_set_cull_face(extState, ext);
	_gfx_state_set_blend(extState, ext);
	_gfx_state_set_stencil_test(extState, ext);

	/* Depth function */
	ext->DepthFunc(state->depthFunc);

	/* Blending */
	ext->BlendEquationSeparate(
		state->blendRGB,
		state->blendAlpha
	);
	ext->BlendFuncSeparate(
		state->blendSourceRGB,
		state->blendBufferRGB,
		state->blendSourceAlpha,
		state->blendBufferAlpha
	);

	/* Stencil test */
	ext->StencilFuncSeparate(
		GL_FRONT,
		state->stencilFuncFront,
		state->stencilRefFront,
		state->stencilMaskFront
	);
	ext->StencilFuncSeparate(
		GL_BACK,
		state->stencilFuncBack,
		state->stencilRefBack,
		state->stencilMaskBack
	);
	ext->StencilOpSeparate(
		GL_FRONT,
		state->stencilFailFront,
		state->depthFailFront,
		state->stencilPassFront
	);
	ext->StencilOpSeparate(
		GL_BACK,
		state->stencilFailBack,
		state->depthFailBack,
		state->stencilPassBack
	);
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

/******************************************************/
void _gfx_states_set_pixel_pack_alignment(unsigned char align, GFX_Extensions* ext)
{
	if(ext->packAlignment != align)
	{
		ext->PixelStorei(GL_PACK_ALIGNMENT, align);
		ext->packAlignment = align;
	}
}

/******************************************************/
void _gfx_states_set_pixel_unpack_alignment(unsigned char align, GFX_Extensions* ext)
{
	if(ext->unpackAlignment != align)
	{
		ext->PixelStorei(GL_UNPACK_ALIGNMENT, align);
		ext->unpackAlignment = align;
	}
}

/******************************************************/
void _gfx_states_set_patch_vertices(unsigned int vertices, GFX_Extensions* ext)
{
	if(ext->patchVertices != vertices)
	{
		ext->PatchParameteri(GL_PATCH_VERTICES, vertices);
		ext->patchVertices = vertices;
	}
}
