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

#ifndef GFX_CORE_ERRORS_H
#define GFX_CORE_ERRORS_H

#include <stddef.h>

/* Default maximum */
#define GFX_MAX_ERRORS_DEFAULT 64

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Platform independent error metadata
 *******************************************************/

/** Error code */
typedef enum GFXErrorCode
{
	GFX_ERROR_UNKNOWN                  = 0x0000,
	GFX_ERROR_INCOMPATIBLE_CONTEXT     = 0x0001,
	GFX_ERROR_PLATFORM_ERROR           = 0x0002,
	GFX_ERROR_MEMORY_CORRUPTION        = 0x0003,
	GFX_ERROR_COMPILE_FAIL             = 0x0004,
	GFX_ERROR_LINK_FAIL                = 0x0005,
	GFX_ERROR_INVALID_ENUM             = 0x0500,
	GFX_ERROR_INVALID_VALUE            = 0x0501,
	GFX_ERROR_INVALID_OPERATION        = 0x0502,
	GFX_ERROR_OVERFLOW                 = 0x0503,
	GFX_ERROR_UNDERFLOW                = 0x0504,
	GFX_ERROR_OUT_OF_MEMORY            = 0x0505,
	GFX_ERROR_INVALID_FRAME_OPERATION  = 0x0506

} GFXErrorCode;


/** Error mode */
typedef enum GFXErrorMode
{
	GFX_ERROR_MODE_NORMAL,
	GFX_ERROR_MODE_DEBUG,

	GFX_ERROR_MODE_DEFAULT = GFX_ERROR_MODE_NORMAL

} GFXErrorMode;


/********************************************************
 * Error handling
 *******************************************************/

/** Error */
typedef struct GFXError
{
	GFXErrorCode  code;
	const char*   description; /* Can be NULL */

} GFXError;


/**
 * Sets the error mode.
 *
 * @param mode The error mode to use, debug reports hardware (OpenGL) errors as well.
 *
 */
void gfx_set_error_mode(

		GFXErrorMode mode);

/**
 * Returns the current error mode.
 *
 */
GFXErrorMode gfx_get_error_mode(void);

/**
 * Returns the number of errors in the queue.
 *
 */
unsigned int gfx_get_num_errors(void);

/**
 * Returns the last error without removing it.
 *
 * @param error Error structure to copy to.
 * @return Whether or not an error was present.
 *
 */
int gfx_errors_peek(

		GFXError* error);

/**
 * Find a specific error code.
 *
 * @param code  The error code to search for.
 * @return non-zero if any error with the given code was found.
 *
 */
int gfx_errors_find(

		GFXErrorCode code);

/**
 * Removes the last error.
 *
 */
void gfx_errors_pop(void);

/**
 * Adds an error to the internal queue.
 *
 * @param code        The error code to add.
 * @param description Optional null terminated message to describe the error (can be NULL).
 *
 * The description will be copied.
 *
 */
void gfx_errors_push(

		GFXErrorCode  code,
		const char*   description);

/**
 * Empty the internal error queue.
 *
 * Should NOT be called frequently for efficiency.
 *
 */
void gfx_errors_empty(void);

/**
 * Sets the maximum number of errors stored.
 *
 */
void gfx_errors_set_maximum(

		size_t max);


#ifdef __cplusplus
}
#endif

#endif // GFX_CORE_ERRORS_H
