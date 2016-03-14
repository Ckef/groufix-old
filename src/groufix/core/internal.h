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

#ifndef GFX_CORE_INTERNAL_H
#define GFX_CORE_INTERNAL_H

#include "groufix/containers/list.h"
#include "groufix/core/renderer.h"

/* Internal renderer headers */
#if defined(GFX_RENDERER_GL)
	#include "groufix/core/renderer/gl.h"
#endif


/********************************************************
 * Internal definitions
 *******************************************************/

/** Internal Pipe */
typedef struct GFX_Pipe
{
	/* Super class */
	GFXList node;

	GFXPipeType    type;
	GFXPipe        ptr;  /* Public pointer */
	GFXPipeState   state;

	/* Associated pipeline */
	GFXPipeline   *pipeline;

} GFX_Pipe;


/********************************************************
 * Error initialization and termination
 *******************************************************/

/**
 * Initializes the error queue.
 *
 * @param mode Error mode to use for the renderer.
 * @return Zero on failure.
 *
 */
int _gfx_errors_init(

		GFXErrorMode mode);

/**
 * Returns the error mode the error queue was initialized with.
 *
 */
GFXErrorMode _gfx_errors_get_mode(void);

/**
 * Terminates the error queue.
 *
 * This method has no effect if the error queue was not yet initialized.
 *
 */
void _gfx_errors_terminate(void);


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
 * Data & type helpers
 *******************************************************/

/**
 * Finds a string in a longer, null-terminated, space-separated string.
 *
 * @param haystack Null-terminated, space-separated string to search in (can be NULL).
 * @param needle   String to find (cannot be NULL).
 * @return Non-zero if it was found.
 *
 */
int _gfx_contains_string(

		const char  *haystack,
		const char  *needle);

/**
 * Splits depth into 3 channels.
 *
 */
void _gfx_split_depth(

		unsigned short   depth,
		unsigned short  *red,
		unsigned short  *green,
		unsigned short  *blue);

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
 * State management
 *******************************************************/

/**
 * Sets all values of a state to their defaults.
 *
 * @param state Structure who's values will be set to their defaults.
 *
 */
void _gfx_states_set_default(

		GFXPipeState *state);

/**
 * Sets the state of the current context.
 *
 */
void _gfx_states_set(

		const GFXPipeState *state,
		GFX_CONT_ARG);

/**
 * Forces all state fields of the current context.
 *
 * This will reset all state fields, regardless of previous states.
 *
 */
void _gfx_states_force_set(

		const GFXPipeState *state,
		GFX_CONT_ARG);

/**
 * Sets the viewport size of the current context.
 *
 */
void _gfx_states_set_viewport(

		GFXViewport viewport,
		GFX_CONT_ARG);

/**
 * Sets the pixel pack alignment of the current context.
 *
 */
void _gfx_states_set_pixel_pack_alignment(

		unsigned char align,
		GFX_CONT_ARG);

/**
 * Sets the pixel unpack alignment of the current context.
 *
 */
void _gfx_states_set_pixel_unpack_alignment(

		unsigned char align,
		GFX_CONT_ARG);

/**
 * Sets the number of vertices per patch.
 *
 */
void _gfx_states_set_patch_vertices(

		unsigned int vertices,
		GFX_CONT_ARG);


/********************************************************
 * Internal vertex layout usage
 *******************************************************/

/**
 * Blocks the layout from changing the source or the index buffer.
 *
 * @param index Index of the source at the layout to block.
 * @return Zero on failure.
 *
 */
int _gfx_vertex_layout_block(

		GFXVertexLayout  *layout,
		unsigned char     index);

/**
 * Unblocks the source at the layout.
 *
 * Must be called equally many times as _gfx_vertex_layout_block has been called.
 *
 */
void _gfx_vertex_layout_unblock(

		GFXVertexLayout  *layout,
		unsigned char     index);


/********************************************************
 * Internal sampler usage
 *******************************************************/

/**
 * Creates a new sampler.
 *
 * @param values Initial values to use, values will be clamped to valid range.
 * @return NULL on failure.
 *
 */
GFXSampler* _gfx_sampler_create(

		const GFXSampler *values);

/**
 * References a sampler to postpone its destruction.
 *
 * @return Zero on overflow.
 *
 */
int _gfx_sampler_reference(

		GFXSampler *sampler);

/**
 * Makes sure the sampler is freed properly.
 *
 * Decreases the reference counter before freeing,
 * only freeing if the counter hits 0.
 *
 */
void _gfx_sampler_free(

		GFXSampler *sampler);

/**
 * Sets the state values of a sampler.
 *
 * @param values Values to use, values will be clamped to valid range.
 * @return Zero on failure.
 *
 */
int _gfx_sampler_set(

		GFXSampler        *sampler,
		const GFXSampler  *values);


/********************************************************
 * Internal program, program map & property map usage
 *******************************************************/

/**
 * References a program to postpone its destruction.
 *
 * @param references Number of times to reference it.
 * @return Zero on overflow.
 *
 * This increases the number of required gfx_program_free calls
 * to actually destroy the program.
 *
 */
int _gfx_program_reference(

		GFXProgram   *program,
		unsigned int  references);

/**
 * Blocks the program map from adding anymore programs.
 *
 * @return Zero on failure.
 *
 * This so it can link the programs.
 *
 */
int _gfx_program_map_block(

		GFXProgramMap *map);

/**
 * Unblocks the program map from adding anymore programs.
 *
 * Must be called equally many times as _gfx_program_map_block has been called.
 *
 */
void _gfx_program_map_unblock(

		GFXProgramMap *map);

/**
 * Acts as if _gfx_render_objects_save was called for only the program map.
 *
 */
void _gfx_program_map_save(

		GFXProgramMap *map);

/**
 * Acts as if _gfx_render_objects_restore was called for only the program map.
 *
 * The source render object container is implied from _gfx_program_map_save.
 *
 */
void _gfx_program_map_restore(

		GFXProgramMap      *map,
		GFX_RenderObjects  *cont);

/**
 * Calls _gfx_program_map_use and uploads appropriate data.
 *
 * @param copy Index of the copy of the map to use.
 * @param base Base instance to use.
 *
 * Note: undefined behaviour if copy is out of bounds!
 *
 */
void _gfx_property_map_use(

		const GFXPropertyMap  *map,
		unsigned int           copy,
		unsigned int           base,
		GFX_CONT_ARG);


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

		GFXPipeline    *pipeline,
		unsigned char   bits);

/**
 * Creates a new process pipe.
 *
 * @param window Target window to draw to, NULL to render using current pipeline.
 * @param swap   Whether to swap the window's internal buffers afterwards or not.
 * @return NULL on failure.
 *
 */
GFX_Pipe* _gfx_pipe_create_process(

		GFXPipeline  *pipeline,
		GFXWindow    *target,
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

		GFX_Pipe *pipe);


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

		unsigned char bits);

/**
 * Makes sure the bucket is freed properly.
 *
 */
void _gfx_bucket_free(

		GFXBucket *bucket);

/**
 * Processes the bucket, drawing all units.
 *
 */
void _gfx_bucket_process(

		GFXBucket           *bucket,
		const GFXPipeState  *state,
		GFX_CONT_ARG);


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

		GFXWindow  *target,
		int         swap);

/**
 * Makes sure the pipe process is freed properly.
 *
 */
void _gfx_pipe_process_free(

		GFXPipeProcess process);

/**
 * Prepares the current context for render to texture.
 *
 * @return Non-zero on success.
 *
 */
int _gfx_pipe_process_prepare(void);

/**
 * Makes sure no pipe process targets the current context anymore, ever.
 *
 * @param last Non-zero if this is the last context to be unprepared.
 *
 */
void _gfx_pipe_process_unprepare(

		int last);

/**
 * Makes sure all the pipes that target the current context target a new one.
 *
 * @param target New target for all pipes.
 *
 */
void _gfx_pipe_process_retarget(

		GFX_Context *target);

/**
 * Forwards a new size of a window to all processes.
 *
 * @param target Context of which its window was resized.
 * @param width  New width.
 * @param height New height.
 *
 */
void _gfx_pipe_process_resize(

		const GFX_Context  *target,
		unsigned int       width,
		unsigned int       height);

/**
 * Executes the pipe process.
 *
 * @param pipeline Calling pipeline.
 *
 */
void _gfx_pipe_process_execute(

		GFXPipeProcess       process,
		const GFXPipeState  *state,
		GFX_CONT_ARG);


#endif // GFX_CORE_INTERNAL_H
