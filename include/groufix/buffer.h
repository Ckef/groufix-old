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

#ifndef GFX_BUFFER_H
#define GFX_BUFFER_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Buffer (arbitrary GPU storage)
 *******************************************************/

/** Buffer */
typedef struct GFXBuffer
{
	size_t         size;        /* Size of the buffer */
	unsigned char  backbuffers; /* Number of back buffers */

} GFXBuffer;


/**
 * Creates a new buffer.
 *
 * @param size Size of the buffer.
 * @param back Number of backbuffers to allocate (> 0 for multi buffering, 0 for regular buffer).
 * @return Non-zero on success.
 *
 */
GFXBuffer* gfx_buffer_create(size_t size, unsigned char back);

/**
 * Makes sure the buffer is freed properly.
 *
 */
void gfx_buffer_free(GFXBuffer* buffer);


#ifdef __cplusplus
}
#endif

#endif // GFX_BUFFER_H
