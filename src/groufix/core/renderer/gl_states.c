/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#include "groufix/core/renderer.h"


/* Compatibility defines */
#ifndef GL_FILL
	#define GL_FILL   0x1b02
#endif
#ifndef GL_LINE
	#define GL_LINE   0x1b01
#endif
#ifndef GL_POINT
	#define GL_POINT  0x1b00
#endif


/******************************************************/
static inline GLenum _gfx_gl_from_fragment_test(

		GFXFragmentTest test)
{
	switch(test)
	{
	case GFX_FRAG_NEVER :
		return GL_NEVER;
	case GFX_FRAG_LESS :
		return GL_LESS;
	case GFX_FRAG_LESS_EQUAL :
		return GL_LEQUAL;
	case GFX_FRAG_GREATER :
		return GL_GREATER;
	case GFX_FRAG_GREATER_EQUAL :
		return GL_GEQUAL;
	case GFX_FRAG_EQUAL :
		return GL_EQUAL;
	case GFX_FRAG_NOT_EQUAL :
		return GL_NOTEQUAL;
	case GFX_FRAG_ALWAYS :
		return GL_ALWAYS;
	}

	return GL_LESS;
}

/******************************************************/
static inline GLenum _gfx_gl_from_blend_state(

		GFXBlendState state)
{
	switch(state)
	{
	case GFX_BLEND_ADD :
		return GL_FUNC_ADD;
	case GFX_BLEND_SUBTRACT :
		return GL_FUNC_SUBTRACT;
	case GFX_BLEND_REV_SUBTRACT :
		return GL_FUNC_REVERSE_SUBTRACT;
	case GFX_BLEND_MIN :
		return GL_MIN;
	case GFX_BLEND_MAX :
		return GL_MAX;
	}

	return GL_FUNC_ADD;
}

/******************************************************/
static inline GLenum _gfx_gl_from_blend_func(

		GFXBlendFunc func)
{
	switch(func)
	{
	case GFX_BLEND_ZERO :
		return GL_ZERO;
	case GFX_BLEND_ONE :
		return GL_ONE;

	case GFX_BLEND_SOURCE :
		return GL_SRC_COLOR;
	case GFX_BLEND_BUFFER :
		return GL_DST_COLOR;
	case GFX_BLEND_ONE_MINUS_SOURCE :
		return GL_ONE_MINUS_SRC_COLOR;
	case GFX_BLEND_ONE_MINUS_BUFFER :
		return GL_ONE_MINUS_DST_COLOR;

	case GFX_BLEND_SOURCE_ALPHA :
		return GL_SRC_ALPHA;
	case GFX_BLEND_BUFFER_ALPHA :
		return GL_DST_ALPHA;
	case GFX_BLEND_ONE_MINUS_SOURCE_ALPHA :
		return GL_ONE_MINUS_SRC_ALPHA;
	case GFX_BLEND_ONE_MINUS_BUFFER_ALPHA :
		return GL_ONE_MINUS_DST_ALPHA;

	case GFX_BLEND_ALPHA_SATURATE :
		return GL_SRC_ALPHA_SATURATE;
	}

	return GL_ZERO;
}

/******************************************************/
static inline GLenum _gfx_gl_from_stencil_func(

		GFXStencilFunc func)
{
	switch(func)
	{
	case GFX_STENCIL_KEEP :
		return GL_KEEP;
	case GFX_STENCIL_ZERO :
		return GL_ZERO;
	case GFX_STENCIL_REPLACE :
		return GL_REPLACE;
	case GFX_STENCIL_INCREASE :
		return GL_INCR;
	case GFX_STENCIL_INCREASE_WRAP :
		return GL_INCR_WRAP;
	case GFX_STENCIL_DECREASE :
		return GL_DECR;
	case GFX_STENCIL_DECREASE_WRAP :
		return GL_DECR_WRAP;
	case GFX_STENCIL_INVERT :
		return GL_INVERT;
	}

	return GL_KEEP;
}

/******************************************************/
static inline void _gfx_gl_states_clear_buffers(

		GFXRenderState state,
		GFX_CONT_ARG)
{
	GLbitfield mask =
		(state & GFX_CLEAR_COLOR ? GL_COLOR_BUFFER_BIT : 0) |
		(state & GFX_CLEAR_DEPTH ? GL_DEPTH_BUFFER_BIT : 0) |
		(state & GFX_CLEAR_STENCIL ? GL_STENCIL_BUFFER_BIT : 0);

	if(mask) GFX_REND_GET.Clear(mask);
}

/******************************************************/
static inline void _gfx_gl_state_set_polygon_mode(

		GFXRenderState state,
		GFX_CONT_ARG)
{
	if(state & GFX_STATE_WIREFRAME)
		GFX_REND_GET.PolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else if(state & GFX_STATE_POINTCLOUD)
		GFX_REND_GET.PolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	else
		GFX_REND_GET.PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

/******************************************************/
static inline void _gfx_gl_state_set_rasterizer(

		GFXRenderState state,
		GFX_CONT_ARG)
{
	if(state & GFX_STATE_NO_RASTERIZER)
		GFX_REND_GET.Enable(GL_RASTERIZER_DISCARD);
	else
		GFX_REND_GET.Disable(GL_RASTERIZER_DISCARD);
}

/******************************************************/
static inline void _gfx_gl_state_set_depth_test(

		GFXRenderState state,
		GFX_CONT_ARG)
{
	if(state & GFX_STATE_DEPTH_TEST)
		GFX_REND_GET.Enable(GL_DEPTH_TEST);
	else
		GFX_REND_GET.Disable(GL_DEPTH_TEST);
}

/******************************************************/
static inline void _gfx_gl_state_set_cull_face(

		GFXRenderState state,
		GFX_CONT_ARG)
{
	if(state & (GFX_STATE_CULL_FRONT | GFX_STATE_CULL_BACK))
	{
		GFX_REND_GET.Enable(GL_CULL_FACE);
		GFX_REND_GET.CullFace(state & GFX_STATE_CULL_FRONT ? GL_FRONT : GL_BACK);
	}
	else GFX_REND_GET.Disable(GL_CULL_FACE);
}

/******************************************************/
static inline void _gfx_gl_state_set_blend(

		GFXRenderState state,
		GFX_CONT_ARG)
{
	if(state & GFX_STATE_BLEND)
		GFX_REND_GET.Enable(GL_BLEND);
	else
		GFX_REND_GET.Disable(GL_BLEND);
}

/******************************************************/
static inline void _gfx_gl_state_set_stencil_test(

		GFXRenderState state,
		GFX_CONT_ARG)
{
	if(state & GFX_STATE_STENCIL_TEST)
		GFX_REND_GET.Enable(GL_STENCIL_TEST);
	else
		GFX_REND_GET.Disable(GL_STENCIL_TEST);
}

/******************************************************/
void _gfx_renderer_states_set(

		const GFXPipeState* state,
		GFX_CONT_ARG)
{
	/* Clear buffers & check stripped render state */
	_gfx_gl_states_clear_buffers(state->render.state, GFX_CONT_AS_ARG);
	GFXRenderState extState = state->render.state & ~GFX_CLEAR_ALL;

	GFXRenderState diff = extState ^ GFX_REND_GET.state.render.state;

	/* Set all boolean states */
	if(diff & (GFX_STATE_WIREFRAME | GFX_STATE_POINTCLOUD))
		_gfx_gl_state_set_polygon_mode(extState, GFX_CONT_AS_ARG);
	if(diff & GFX_STATE_NO_RASTERIZER)
		_gfx_gl_state_set_rasterizer(extState, GFX_CONT_AS_ARG);
	if(diff & GFX_STATE_DEPTH_WRITE)
		GFX_REND_GET.DepthMask(extState & GFX_STATE_DEPTH_WRITE ? GL_TRUE : GL_FALSE);
	if(diff & GFX_STATE_DEPTH_TEST)
		_gfx_gl_state_set_depth_test(extState, GFX_CONT_AS_ARG);
	if(diff & (GFX_STATE_CULL_FRONT | GFX_STATE_CULL_BACK))
		_gfx_gl_state_set_cull_face(extState, GFX_CONT_AS_ARG);
	if(diff & GFX_STATE_BLEND)
		_gfx_gl_state_set_blend(extState, GFX_CONT_AS_ARG);
	if(diff & GFX_STATE_STENCIL_TEST)
		_gfx_gl_state_set_stencil_test(extState, GFX_CONT_AS_ARG);

	/* Set all other state */
	int comp;

	/* Depth function */
	comp =
		GFX_REND_GET.state.depth.test != state->depth.test;

	if(comp) GFX_REND_GET.DepthFunc(
		_gfx_gl_from_fragment_test(state->depth.test));

	/* Blend equation */
	comp =
		(GFX_REND_GET.state.blend.stateRGB != state->blend.stateRGB) |
		(GFX_REND_GET.state.blend.stateA != state->blend.stateA);

	if(comp) GFX_REND_GET.BlendEquationSeparate(
		_gfx_gl_from_blend_state(state->blend.stateRGB),
		_gfx_gl_from_blend_state(state->blend.stateA));

	/* Blend functions */
	comp =
		(GFX_REND_GET.state.blend.sourceRGB != state->blend.sourceRGB) |
		(GFX_REND_GET.state.blend.bufferRGB != state->blend.bufferRGB) |
		(GFX_REND_GET.state.blend.sourceA != state->blend.sourceA) |
		(GFX_REND_GET.state.blend.bufferA != state->blend.bufferA);

	if(comp) GFX_REND_GET.BlendFuncSeparate(
		_gfx_gl_from_blend_func(state->blend.sourceRGB),
		_gfx_gl_from_blend_func(state->blend.bufferRGB),
		_gfx_gl_from_blend_func(state->blend.sourceA),
		_gfx_gl_from_blend_func(state->blend.bufferA));

	/* Stencil front face functions */
	comp =
		(GFX_REND_GET.state.stencil.testFront != state->stencil.testFront) |
		(GFX_REND_GET.state.stencil.frontRef != state->stencil.frontRef) |
		(GFX_REND_GET.state.stencil.frontMask != state->stencil.frontMask);

	if(comp) GFX_REND_GET.StencilFuncSeparate(
		GL_FRONT,
		_gfx_gl_from_fragment_test(state->stencil.testFront),
		state->stencil.frontRef,
		state->stencil.frontMask);

	/* Stencil back face functions */
	comp =
		(GFX_REND_GET.state.stencil.testBack != state->stencil.testBack) |
		(GFX_REND_GET.state.stencil.backRef != state->stencil.backRef) |
		(GFX_REND_GET.state.stencil.backMask != state->stencil.backMask);

	if(comp) GFX_REND_GET.StencilFuncSeparate(
		GL_BACK,
		_gfx_gl_from_fragment_test(state->stencil.testBack),
		state->stencil.backRef,
		state->stencil.backMask);

	/* Stencil front face operations */
	comp =
		(GFX_REND_GET.state.stencil.frontFail != state->stencil.frontFail) |
		(GFX_REND_GET.state.stencil.frontDepth != state->stencil.frontDepth) |
		(GFX_REND_GET.state.stencil.frontPass != state->stencil.frontPass);

	if(comp) GFX_REND_GET.StencilOpSeparate(
		GL_FRONT,
		_gfx_gl_from_stencil_func(state->stencil.frontFail),
		_gfx_gl_from_stencil_func(state->stencil.frontDepth),
		_gfx_gl_from_stencil_func(state->stencil.frontPass));

	/* Stencil back face operations */
	comp =
		(GFX_REND_GET.state.stencil.backFail != state->stencil.backFail) |
		(GFX_REND_GET.state.stencil.backDepth != state->stencil.backDepth) |
		(GFX_REND_GET.state.stencil.backPass != state->stencil.backPass);

	if(comp) GFX_REND_GET.StencilOpSeparate(
		GL_BACK,
		_gfx_gl_from_stencil_func(state->stencil.backFail),
		_gfx_gl_from_stencil_func(state->stencil.backDepth),
		_gfx_gl_from_stencil_func(state->stencil.backPass));

	/* Set all values */
	/* No need to worry about threading as GL threads can only be current in one thread anyway */
	GFX_REND_GET.state = *state;
	GFX_REND_GET.state.render.state = extState;
}

/******************************************************/
void _gfx_renderer_states_force_set(

		const GFXPipeState* state,
		GFX_CONT_ARG)
{
	/* Clear buffers & strip state */
	_gfx_gl_states_clear_buffers(state->render.state, GFX_CONT_AS_ARG);
	GFXRenderState extState = state->render.state & ~GFX_CLEAR_ALL;

	/* Set all boolean states */
	_gfx_gl_state_set_polygon_mode(extState, GFX_CONT_AS_ARG);
	_gfx_gl_state_set_rasterizer(extState, GFX_CONT_AS_ARG);
	GFX_REND_GET.DepthMask(extState & GFX_STATE_DEPTH_WRITE ? GL_TRUE : GL_FALSE);
	_gfx_gl_state_set_depth_test(extState, GFX_CONT_AS_ARG);
	_gfx_gl_state_set_cull_face(extState, GFX_CONT_AS_ARG);
	_gfx_gl_state_set_blend(extState, GFX_CONT_AS_ARG);
	_gfx_gl_state_set_stencil_test(extState, GFX_CONT_AS_ARG);

	/* Depth function */
	GFX_REND_GET.DepthFunc(
		_gfx_gl_from_fragment_test(state->depth.test));

	/* Blending */
	GFX_REND_GET.BlendEquationSeparate(
		_gfx_gl_from_blend_state(state->blend.stateRGB),
		_gfx_gl_from_blend_state(state->blend.stateA));

	GFX_REND_GET.BlendFuncSeparate(
		_gfx_gl_from_blend_func(state->blend.sourceRGB),
		_gfx_gl_from_blend_func(state->blend.bufferRGB),
		_gfx_gl_from_blend_func(state->blend.sourceA),
		_gfx_gl_from_blend_func(state->blend.bufferA));

	/* Stencil test */
	GFX_REND_GET.StencilFuncSeparate(
		GL_FRONT,
		_gfx_gl_from_fragment_test(state->stencil.testFront),
		state->stencil.frontRef,
		state->stencil.frontMask);

	GFX_REND_GET.StencilFuncSeparate(
		GL_BACK,
		_gfx_gl_from_fragment_test(state->stencil.testBack),
		state->stencil.backRef,
		state->stencil.backMask);

	GFX_REND_GET.StencilOpSeparate(
		GL_FRONT,
		_gfx_gl_from_stencil_func(state->stencil.frontFail),
		_gfx_gl_from_stencil_func(state->stencil.frontDepth),
		_gfx_gl_from_stencil_func(state->stencil.frontPass));

	GFX_REND_GET.StencilOpSeparate(
		GL_BACK,
		_gfx_gl_from_stencil_func(state->stencil.backFail),
		_gfx_gl_from_stencil_func(state->stencil.backDepth),
		_gfx_gl_from_stencil_func(state->stencil.backPass));

	/* Set all values */
	GFX_REND_GET.state = *state;
	GFX_REND_GET.state.render.state = extState;
}

/******************************************************/
void _gfx_gl_states_set_patch_vertices(

		unsigned int vertices,
		GFX_CONT_ARG)
{
	if(GFX_REND_GET.patchVertices != vertices)
	{
		GFX_REND_GET.PatchParameteri(GL_PATCH_VERTICES, vertices);
		GFX_REND_GET.patchVertices = vertices;
	}
}

/******************************************************/
void _gfx_gl_states_set_pixel_pack_alignment(

		unsigned char align,
		GFX_CONT_ARG)
{
	if(GFX_REND_GET.packAlignment != align)
	{
		GFX_REND_GET.PixelStorei(GL_PACK_ALIGNMENT, align);
		GFX_REND_GET.packAlignment = align;
	}
}

/******************************************************/
void _gfx_gl_states_set_pixel_unpack_alignment(

		unsigned char align,
		GFX_CONT_ARG)
{
	if(GFX_REND_GET.unpackAlignment != align)
	{
		GFX_REND_GET.PixelStorei(GL_UNPACK_ALIGNMENT, align);
		GFX_REND_GET.unpackAlignment = align;
	}
}

/******************************************************/
void _gfx_gl_states_set_viewport(

		GFXViewport viewport,
		GFX_CONT_ARG)
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
