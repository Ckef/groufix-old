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

/** Buffer usage hint */
typedef enum GFXBufferUsage
{
	GFX_BUFFER_READ    = 0x01,
	GFX_BUFFER_WRITE   = 0x02,
	GFX_BUFFER_STREAM  = 0x04

} GFXBufferUsage;


/** Buffer access */
typedef enum GFXBufferAccess
{
	GFX_ACCESS_READ   = 0x0001,
	GFX_ACCESS_WRITE  = 0x0002,
	GFX_ACCESS_ASYNC  = 0x0020

} GFXBufferAccess;


/** Buffer target */
typedef enum GFXBufferTarget
{
	GFX_VERTEX_BUFFER  = 0x8892,
	GFX_INDEX_BUFFER   = 0x8893

} GFXBufferTarget;


/** Buffer */
typedef struct GFXBuffer
{
	size_t           size;   /* Size of the buffer */
	GFXBufferUsage   usage;  /* Intended usage of the buffer */
	GFXBufferTarget  target; /* Storage type the buffer is targeted for */
	unsigned char    multi;  /* Number of extra buffers (0 = regular buffering) */

} GFXBuffer;


/**
 * Creates a new buffer.
 *
 * @param usage  Usage bitflag, how the buffer is intended to be used.
 * @param target Storage type the buffer is targeted for.
 * @param data   Data to copy to the buffer (can be NULL).
 * @param multi  Number of extra buffers to allocate (> 0 for multi buffering, 0 for regular buffering).
 * @return Non-zero on success.
 *
 * Note: if a data pointer is given, this data is NOT copied to any extra buffers.
 *
 */
GFXBuffer* gfx_buffer_create(GFXBufferUsage usage, GFXBufferTarget target, size_t size, const void* data, unsigned char multi);

/**
 * Creates a copy of a buffer.
 *
 * @param usage  Usage bitflag, how the buffer is intended to be used.
 * @param target Storage type the buffer is targeted for.
 * @return Non-zero on success.
 *
 * Note: only copies the current buffer when multi buffering.
 *
 */
GFXBuffer* gfx_buffer_create_copy(GFXBuffer* src, GFXBufferUsage usage, GFXBufferTarget target);

/**
 * Makes sure the buffer is freed properly.
 *
 */
void gfx_buffer_free(GFXBuffer* buffer);

/**
 * Advances to the next backbuffer.
 *
 * @return Zero if this buffer is not multi buffered (and thus no swap occurred).
 *
 */
int gfx_buffer_swap(GFXBuffer* buffer);

/**
 * Allocates more buffers for multibuffering.
 *
 * @param num Number of extra buffers to allocate.
 * @return Non-zero on success.
 *
 */
int gfx_buffer_expand(GFXBuffer* buffer, unsigned char num);

/**
 * Deallocates buffers from multibuffering.
 *
 * @param num Number of buffers to remove.
 * @return Number of buffers actually removed.
 *
 */
int gfx_buffer_shrink(GFXBuffer* buffer, unsigned char num);

/**
 * Writes data to the buffer synchronously.
 *
 * @param data Data to write to the buffer, cannot be NULL.
 *
 */
void gfx_buffer_write(GFXBuffer* buffer, size_t size, const void* data, size_t offset);

/**
 * Reads data from the buffer synchronously.
 *
 * @param data Pointer to write to, cannot be NULL.
 *
 */
void gfx_buffer_read(GFXBuffer* buffer, size_t size, void* data, size_t offset);

/**
 * Maps the buffer and returns a pointer to the mapped data.
 *
 * @param access Access rules to optimize (which must be followed by the client).
 * @return A pointer in client address space (NULL on failure).
 *
 * Note: BUFFER_ACCESS_ASYNC and BUFFER_ACCESS_READ cannot be set simultaneously.
 *
 */
void* gfx_buffer_map(GFXBuffer* buffer, size_t size, size_t offset, GFXBufferAccess access);

/**
 * Unmaps the buffer, invalidating the pointer returned by gfx_buffer_map.
 *
 * This method MUST be called immediately after gfx_buffer_map in order to continue using the buffer.
 *
 */
void gfx_buffer_unmap(GFXBuffer* buffer);


/********************************************************
 * Buffer Segmentation (auto synchronization)
 *******************************************************/

/** Buffer Segment */
typedef struct GFXBufferSegment
{
	GFXBuffer*  buffer;
	size_t      size;   /* Byte size of the segment */
	size_t      offset; /* Byte offset within the buffer */

} GFXBufferSegment;


/**
 * Creates a new buffer segment.
 *
 * @param size Size of the segment, will be clipped to buffer size.
 *
 */
GFXBufferSegment* gfx_buffer_segment_create(GFXBuffer* buffer, size_t size);

/**
 * Makes sure the buffer segment is freed properly.
 *
 */
void gfx_buffer_segment_free(GFXBufferSegment* segment);

/**
 * Advances to the next block in the buffer.
 *
 * @return Zero if no swap could be made (thus the same memory block is pointed to).
 *
 * Note: this method implicitly calls the swap of the buffer itself.
 *
 */
int gfx_buffer_segment_swap(GFXBufferSegment* segment);


#ifdef __cplusplus
}
#endif

#endif // GFX_BUFFER_H
