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

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * \brief Platform independent error code
 *******************************************************/
typedef int GFXErrorCode;

#define GFX_NO_ERROR                             0x0000
#define GFX_ERROR_UNKNOWN                        0x0001
#define GFX_ERROR_INCOMPATIBLE_CONTEXT           0x0002
#define GFX_ERROR_INCOMPATIBLE_PLATFORM          0x0003
#define GFX_ERROR_INVALID_ENUM                   0x0004
#define GFX_ERROR_INVALID_VALUE                  0x0005
#define GFX_ERROR_INVALID_OPERATION              0x0006
#define GFX_ERROR_INVALID_FRAMEBUFFER_OPERATION  0x0007
#define GFX_ERROR_OUT_OF_MEMORY                  0x0008
#define GFX_ERROR_UNDERFLOW                      0x0009
#define GFX_ERROR_OVERFLOW                       0x000a
#define GFX_ERROR_ACCESS_DENIED                  0x000b

#define GFX_NUM_ERRORS                           0x000c


/********************************************************
 * \brief Error structure
 *******************************************************/
typedef struct GFXError
{
	GFXErrorCode  code;
	char*         description; /* Can be NULL */

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
 * \brief Removes the last error.
 *
 */
void gfx_errors_pop(void);

/**
 * \brief Adds an error to the internal queue.
 *
 * \param error       The error code to add, GFX_NO_ERROR will be ignored.
 * \param description Optional message to describe the error (can be NULL).
 *
 */
void gfx_errors_push(GFXErrorCode error, const char* description);

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
void gfx_errors_set_maximum(size_t max);


#ifdef __cplusplus
}
#endif

#endif // GFX_ERRORS_H
