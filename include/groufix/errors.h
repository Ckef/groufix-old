/**
 * Groufix  :  Graphics Engine produced by Ckef Worx
 * www      :  http://www.ejb.ckef-worx.com
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef GFX_ERRORS_H
#define GFX_ERRORS_H

/* Default maximum */
#define GFX_MAX_ERRORS_DEFAULT  64

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * \brief Platform independent error code
 *******************************************************/
typedef unsigned int GFXErrorCode;

#define GFX_ERROR_UNKNOWN                        0x0000
#define GFX_ERROR_INCOMPATIBLE_CONTEXT           0x0001
#define GFX_ERROR_MEMORY_CORRUPTION              0x0002
#define GFX_ERROR_COMPILE_FAILURE                0x0003
#define GFX_ERROR_INVALID_ENUM                   0x0500
#define GFX_ERROR_INVALID_VALUE                  0x0501
#define GFX_ERROR_INVALID_OPERATION              0x0502
#define GFX_ERROR_OVERFLOW                       0x0503
#define GFX_ERROR_UNDERFLOW                      0x0504
#define GFX_ERROR_OUT_OF_MEMORY                  0x0505
#define GFX_ERROR_INVALID_FRAMEBUFFER_OPERATION  0x0506


/********************************************************
 * \brief Error structure
 *******************************************************/
typedef struct GFXError
{
	GFXErrorCode  code;
	const char*   description; /* Can be NULL */

} GFXError;


/**
 * \brief Returns the last error without removing it.
 *
 * \param error Error structure to copy to.
 * \return Whether or not an error was present.
 *
 */
int gfx_errors_peek(GFXError* error);

/**
 * \brief Find a specific error code.
 *
 * \param code  The error code to search for.
 * \return non-zero if any error with the given code was found.
 *
 */
int gfx_errors_find(GFXErrorCode code);

/**
 * \brief Removes the last error.
 *
 */
void gfx_errors_pop(void);

/**
 * \brief Adds an error to the internal queue.
 *
 * \param code        The error code to add.
 * \param description Optional null terminated message to describe the error (can be NULL).
 *
 */
void gfx_errors_push(GFXErrorCode code, const char* description);

/**
 * \brief Empty the internal error queue.
 *
 * Should NOT be called frequently for efficiency.
 *
 */
void gfx_errors_empty(void);

/**
 * \brief Sets the maximum number of errors stored.
 *
 */
void gfx_errors_set_maximum(unsigned int max);


#ifdef __cplusplus
}
#endif

#endif // GFX_ERRORS_H
