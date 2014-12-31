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

#ifndef GFX_CORE_INTERNAL_H
#define GFX_CORE_INTERNAL_H

#include "groufix/containers/list.h"
#include "groufix/core/renderer.h"


#ifdef GFX_RENDERER_GL

	/* Compatibility texture defines */
	#ifndef GL_TEXTURE_1D
		#define GL_TEXTURE_1D                    0x0de0
	#endif
	#ifndef GL_TEXTURE_1D_ARRAY
		#define GL_TEXTURE_1D_ARRAY              0x8c18
	#endif
	#ifndef GL_TEXTURE_2D_MULTISAMPLE
		#define GL_TEXTURE_2D_MULTISAMPLE        0x9100
	#endif
	#ifndef GL_TEXTURE_2D_MULTISAMPLE_ARRAY
		#define GL_TEXTURE_2D_MULTISAMPLE_ARRAY  0x9102
	#endif
	#ifndef GL_TEXTURE_BUFFER
		#define GL_TEXTURE_BUFFER                0x8c2a
	#endif
	#ifndef GL_TEXTURE_CUBE_MAP_ARRAY
		#define GL_TEXTURE_CUBE_MAP_ARRAY        0x9009
	#endif

#endif


#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Internal definitions
 *******************************************************/

/** Internal draw type */
typedef enum GFX_DrawType
{
	GFX_INT_DRAW,
	GFX_INT_DRAW_INSTANCED,
	GFX_INT_DRAW_INSTANCED_BASE

} GFX_DrawType;


/** Internal Pipe */
typedef struct GFX_Pipe
{
	/* Super class */
	GFXList node;

	GFXPipeType    type;
	GFXPipe        ptr;  /* Public pointer */
	GFXPipeState   state;

	/* Associated pipeline */
	GFXPipeline*   pipeline;

} GFX_Pipe;


/********************************************************
 * Event triggers (must be called manually by platform)
 *******************************************************/

/**
 * Called when a window is requested to close.
 *
 * Used to intercept a user requested window termination.
 *
 */
void _gfx_event_window_close(

		GFX_PlatformWindow handle);

/**
 * Called when a window has been moved.
 *
 * @param x The new x coordinate.
 * @param y The new y coordinate.
 *
 */
void _gfx_event_window_move(

		GFX_PlatformWindow  handle,
		int                 x,
		int                 y);

/**
 * Called when a window has been resized.
 *
 * @param width  The new width.
 * @param height The new height.
 *
 */
void _gfx_event_window_resize(

		GFX_PlatformWindow  handle,
		unsigned int        width,
		unsigned int        height);

/**
 * Called when a window receives input focus.
 *
 */
void _gfx_event_window_focus(

		GFX_PlatformWindow handle);

/**
 * Called when a window loses input focus.
 *
 */
void _gfx_event_window_blur(

		GFX_PlatformWindow handle);

/**
 * Handles a key press event.
 *
 * @param key   What key is pressed.
 * @param state State of some special keys.
 *
 */
void _gfx_event_key_press(

		GFX_PlatformWindow  handle,
		GFXKey              key,
		GFXKeyState         state);

/**
 * Handles a key release event.
 *
 * @param key   What key is released.
 * @param state State of some special keys.
 *
 */
void _gfx_event_key_release(

		GFX_PlatformWindow  handle,
		GFXKey              key,
		GFXKeyState         state);

/**
 * Called when the cursor moves in a window.
 *
 * @param x     X coordinate of the cursor relative to the window.
 * @param y     Y coordinate of the cursor relative to the window.
 * @param state State of some special keys.
 *
 */
void _gfx_event_mouse_move(

		GFX_PlatformWindow  handle,
		int                 x,
		int                 y,
		GFXKeyState         state);

/**
 * Called when the cursor enters a window.
 *
 * @param x     X coordinate of the cursor relative to the window.
 * @param y     Y coordinate of the cursor relative to the window.
 * @param state State of some special keys.
 *
 */
void _gfx_event_mouse_enter(

		GFX_PlatformWindow  handle,
		int                 x,
		int                 y,
		GFXKeyState         state);

/**
 * Called when the cursor leaves a window.
 *
 * @param x     X coordinate of the cursor relative to the window.
 * @param y     Y coordinate of the cursor relative to the window.
 * @param state State of some special keys.
 *
 */
void _gfx_event_mouse_leave(

		GFX_PlatformWindow  handle,
		int                 x,
		int                 y,
		GFXKeyState         state);

/**
 * Handles a mouse key press event.
 *
 * @param key   What key is pressed.
 * @param x     X coordinate of the cursor relative to the window.
 * @param y     Y coordinate of the cursor relative to the window.
 * @param state State of some special keys.
 *
 */
void _gfx_event_mouse_press(

		GFX_PlatformWindow  handle,
		GFXMouseKey         key,
		int                 x,
		int                 y,
		GFXKeyState         state);

/**
 * Handles a mouse key release event.
 *
 * @param key   What key is pressed.
 * @param x     X coordinate of the cursor relative to the window.
 * @param y     Y coordinate of the cursor relative to the window.
 * @param state State of some special keys.
 *
 */
void _gfx_event_mouse_release(

		GFX_PlatformWindow  handle,
		GFXMouseKey         key,
		int                 x,
		int                 y,
		GFXKeyState         state);

/**
 * Handles a mouse wheel event.
 *
 * @param xoffset Mouse wheel tilt (negative = left, positive = right).
 * @param yoffset Mouse wheel rotate (negative = down, positive = up).
 * @param x       X coordinate of the cursor relative to the window.
 * @param y       Y coordinate of the cursor relative to the window.
 * @param state   State of some special keys.
 *
 */
void _gfx_event_mouse_wheel(

		GFX_PlatformWindow  handle,
		int                 xoffset,
		int                 yoffset,
		int                 x,
		int                 y,
		GFXKeyState         state);


/********************************************************
 * State management
 *******************************************************/

/**
 * Sets all values of a state to their defaults.
 *
 * @param state Structure who's values will be set to their defaults.
 *
 */
void _gfx_states_set_default(

		GFXPipeState* state);

/**
 * Sets the state of the current context.
 *
 */
void _gfx_states_set(

		GFXPipeState* state,
		GFX_WIND_ARG);

/**
 * Forces all state fields of the current context.
 *
 * This will reset all state fields, regardless of previous states.
 *
 */
void _gfx_states_force_set(

		GFXPipeState* state,
		GFX_WIND_ARG);

/**
 * Sets the viewport size of the current context.
 *
 */
void _gfx_states_set_viewport(

		GFXViewport viewport,
		GFX_WIND_ARG);

/**
 * Sets the pixel pack alignment of the current context.
 *
 */
void _gfx_states_set_pixel_pack_alignment(

		unsigned char align,
		GFX_WIND_ARG);

/**
 * Sets the pixel unpack alignment of the current context.
 *
 */
void _gfx_states_set_pixel_unpack_alignment(

		unsigned char align,
		GFX_WIND_ARG);

/**
 * Sets the number of vertices per patch.
 *
 */
void _gfx_states_set_patch_vertices(

		unsigned int vertices,
		GFX_WIND_ARG);


/********************************************************
 * Internal unit binding
 *******************************************************/

/**
 * Binds a buffer to the appropriate uniform buffer index.
 *
 * @param prioritize Non-zero signifies this buffer must stay bound as long as possible.
 * @return the uniform buffer index it was bound to.
 *
 */
size_t _gfx_binder_bind_uniform_buffer(

		GLuint      buffer,
		GLintptr    offset,
		GLsizeiptr  size,
		int         prioritize,
		GFX_WIND_ARG);

/**
 * Makes sure a buffer is unbound from any uniform buffer index.
 *
 */
void _gfx_binder_unbind_uniform_buffer(

		GLuint buffer,
		GFX_WIND_ARG);

/**
 * Binds a texture to the appropriate unit.
 *
 * @param target     Internal target of the texture.
 * @param prioritize Non-zero signifies this texture must stay bound as long as possible.
 * @return the texture unit it was bound to.
 *
 */
size_t _gfx_binder_bind_texture(

		GLuint  texture,
		GLenum  target,
		int     prioritize,
		GFX_WIND_ARG);

/**
 * Makes sure a texture is unbound from any unit.
 *
 */
void _gfx_binder_unbind_texture(

		GLuint texture,
		GFX_WIND_ARG);


/********************************************************
 * Internal object binding & draw calls
 *******************************************************/

/**
 * Sets the layout handle as currently bound to the current context.
 *
 */
void _gfx_vertex_layout_bind(

		GLuint vao,
		GFX_WIND_ARG);

/**
 * Sets the framebuffer handle associated with a pipeline as current for the current context.
 *
 */
void _gfx_pipeline_bind(

		GLenum  target,
		GLuint  framebuffer,
		GFX_WIND_ARG);

/**
 * Sets the program pipeline as currently in use.
 *
 */
void _gfx_program_map_use(

		GFXProgramMap* map,
		GFX_WIND_ARG);

/**
 * Calls _gfx_program_map_use and uploads appropriate data.
 *
 * @param copy Index of the copy of the map to use.
 * @param base Base instance to use.
 *
 */
void _gfx_property_map_use(

		GFXPropertyMap*  map,
		unsigned int     copy,
		unsigned int     base,
		GFX_WIND_ARG);

/**
 * Performs a complete draw operation.
 *
 * @param source Draw calls of the layout to issue.
 * @param type   Function to use for drawing.
 *
 */
void _gfx_vertex_layout_draw(

		const GFXVertexLayout*  layout,
		GFXVertexSource         source,
		size_t                  inst,
		unsigned int            base,
		GFX_DrawType            type,
		GFX_WIND_ARG);


/********************************************************
 * Internal pipe
 *******************************************************/

/**
 * Creates a new bucket pipe.
 *
 * @param bits Number of manual bits to sort by (LSB = 1st bit, 0 for all bits).
 * @return NULL on failure
 *
 */
GFX_Pipe* _gfx_pipe_create_bucket(

		GFXPipeline*    pipeline,
		unsigned char   bits,
		GFXBucketFlags  flags);

/**
 * Creates a new process pipe.
 *
 * @param window Target window to draw to, NULL to render using current pipeline.
 * @param swap   Whether to swap the window's internal buffers afterwards or not.
 * @return NULL on failure.
 *
 */
GFX_Pipe* _gfx_pipe_create_process(

		GFXPipeline*  pipeline,
		GFXWindow*    target,
		int           swap);

/**
 * Makes sure the pipe is freed properly.
 *
 * @return The pipe taking its place (can be NULL).
 *
 * If no pipe takes its place, it will try to return the previous pipe instead.
 *
 */
GFX_Pipe* _gfx_pipe_free(

		GFX_Pipe* pipe);


/********************************************************
 * Internal bucket creation & processing
 *******************************************************/

/**
 * Creates a new bucket.
 *
 * @param bits Number of manual bits to sort by (LSB = 1st bit, 0 for all bits).
 * @return NULL on failure.
 *
 */
GFXBucket* _gfx_bucket_create(

		unsigned char   bits,
		GFXBucketFlags  flags);

/**
 * Makes sure the bucket is freed properly.
 *
 */
void _gfx_bucket_free(

		GFXBucket* bucket);

/**
 * Processes the bucket, drawing all batches.
 *
 */
void _gfx_bucket_process(

		GFXBucket*     bucket,
		GFXPipeState*  state,
		GFX_WIND_ARG);


/********************************************************
 * Internal pipe process creation & execution
 *******************************************************/

/**
 * Creates a new process.
 *
 * @param window Target window to draw to, NULL to render using current pipeline.
 * @param swap   Whether to swap the window's internal buffers afterwards or not.
 * @return NULL on failure.
 *
 */
GFXPipeProcess _gfx_pipe_process_create(

		GFXWindow*  target,
		int         swap);

/**
 * Makes sure the pipe process is freed properly.
 *
 */
void _gfx_pipe_process_free(

		GFXPipeProcess process);

/**
 * Prepares the current window for render to texture.
 *
 * @return Non-zero on success.
 *
 * If a window was never prepared, it would mean it cannot be used in post processing in any way.
 *
 */
int _gfx_pipe_process_prepare(void);

/**
 * Makes sure no pipe process targets the current window anymore, ever.
 *
 * @param last Non-zero if this is the last window to be unprepared.
 *
 */
void _gfx_pipe_process_unprepare(

		int last);

/**
 * Makes sure all the pipes that target the current window target a new one.
 *
 * @param target New target for all pipes.
 *
 */
void _gfx_pipe_process_retarget(

		GFX_Window* target);

/**
 * Forwards a new size of a window to all processes.
 *
 * @target Window which was resized.
 * @width  New width.
 * @height New height.
 *
 */
void _gfx_pipe_process_resize(

		GFX_Window*   target,
		unsigned int  width,
		unsigned int  height);

/**
 * Executes the pipe process.
 *
 * @param pipeline Calling pipeline.
 *
 */
void _gfx_pipe_process_execute(

		GFXPipeProcess  process,
		GFXPipeState*   state,
		GFX_WIND_ARG);


/********************************************************
 * Internal program & program map usage
 *******************************************************/

/**
 * Creates a new program.
 *
 * @param instances Number of instances that can be drawn in a single draw call, 0 for infinite.
 * @return NULL on failure.
 *
 */
GFXProgram* _gfx_program_create(

		size_t instances);

/**
 * References a program to postpone its destruction.
 *
 * @param references Number of times to reference it.
 * @return Zero on overflow.
 *
 */
int _gfx_program_reference(

		GFXProgram*   program,
		unsigned int  references);

/**
 * Makes sure the program is freed properly.
 *
 * Decrease the reference counter before freeing,
 * only freeing if the counter hits 0.
 *
 */
void _gfx_program_free(

		GFXProgram* program);

/**
 * Get the location of a property (a.k.a uniform).
 *
 * @return Negative on failure, the location otherwise.
 *
 */
GLint _gfx_program_get_location(

		GFXProgram*     program,
		unsigned short  index);

/**
 * Blocks the program map from adding anymore programs.
 *
 * @return Zero on failure.
 *
 * This so it can link the programs.
 *
 */
int _gfx_program_map_block(

		GFXProgramMap* map);

/**
 * Unblocks the program map from adding anymore programs.
 *
 * Must be called equally many times as _gfx_program_map_block has been called.
 *
 */
void _gfx_program_map_unblock(

		GFXProgramMap* map);

/**
 * Acts as if _gfx_render_objects_save was called for only the program map.
 *
 */
void _gfx_program_map_save(

		GFXProgramMap* map);

/**
 * Acts as if _gfx_render_objects_restore was called for only the program map.
 *
 * The source render object container is implied from _gfx_program_map_save.
 *
 */
void _gfx_program_map_restore(

		GFXProgramMap*      map,
		GFX_RenderObjects*  cont);


/********************************************************
 * Datatype helpers
 *******************************************************/

/**
 * Returns 1 if packed data, 0 if unpacked.
 *
 */
int _gfx_is_data_type_packed(

		GFXDataType type);

/**
 * Returns the size of a data type in bytes.
 *
 */
unsigned char _gfx_sizeof_data_type(

		GFXDataType type);


/********************************************************
 * Format retrieval (of textures) & converters
 *******************************************************/

/**
 * Returns the internal target of a texture (a.k.a type).
 *
 */
GLenum _gfx_texture_get_internal_target(

		const GFXTexture* texture);

/**
 * Converts a texture format to a client pixel format.
 *
 * @return Negative on failure.
 *
 */
GLint _gfx_texture_format_to_pixel_format(

		GFXTextureFormat format);

/**
 * Converts a texture format to an internal format.
 *
 * @return Negative on failure.
 *
 */
GLint _gfx_texture_format_to_internal(

		GFXTextureFormat format);

/**
 * Converts an internal format to a texture format.
 *
 */
GFXTextureFormat _gfx_texture_format_from_internal(

		GLint format);


#ifdef __cplusplus
}
#endif

#endif // GFX_CORE_INTERNAL_H
