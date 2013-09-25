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

#ifndef GFX_MEMORY_H
#define GFX_MEMORY_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Data types associated with the GPU
 *******************************************************/

/** Storage data type */
typedef enum GFXDataType
{
	GFX_BYTE            = 0x1400,
	GFX_UNSIGNED_BYTE   = 0x1401,
	GFX_SHORT           = 0x1402,
	GFX_UNSIGNED_SHORT  = 0x1403,
	GFX_INT             = 0x1404,
	GFX_UNSIGNED_INT    = 0x1405,
	GFX_FLOAT           = 0x1406,
	GFX_HALF_FLOAT      = 0x140b

} GFXDataType;


/** Interpreted type */
typedef enum GFXInterpretType
{
	GFX_INTERPRET_FLOAT       = 0x00,
	GFX_INTERPRET_NORMALIZED  = 0x01,
	GFX_INTERPRET_INTEGER     = 0x02

} GFXInterpretType;


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


/** Buffer target */
typedef enum GFXBufferTarget
{
	GFX_VERTEX_BUFFER  = 0x8892,
	GFX_INDEX_BUFFER   = 0x8893

} GFXBufferTarget;


/** Buffer */
typedef struct GFXBuffer
{
	size_t           size;   /* Size of the buffer in bytes */
	GFXBufferUsage   usage;  /* Intended usage of the buffer */
	GFXBufferTarget  target; /* Storage type the buffer is targeted for */
	unsigned char    multi;  /* Number of extra buffers (0 = regular buffering) */

	size_t id; /* Unique ID */

} GFXBuffer;


/**
 * Creates a new buffer.
 *
 * @param usage    Usage bitflag, how the buffer is intended to be used.
 * @param target   Storage type the buffer is targeted for.
 * @param multi    Number of extra buffers to allocate (> 0 for multi buffering, 0 for regular buffering).
 * @param segments Number of segments per buffer.
 * @return Non-zero on success.
 *
 * Note: if data is not NULL, this data is NOT copied to any extra buffers.
 *
 */
GFXBuffer* gfx_buffer_create(GFXBufferUsage usage, GFXBufferTarget target, size_t size, const void* data, unsigned char multi, unsigned char segments);

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
 * Advances to the next segment and/or backbuffer.
 *
 * @return Zero if this buffer is not multi buffered or segmented (and thus no swap occurred).
 *
 * Additionally, this is the command which issues a fence sync.
 * It is appropriate to call this immediately after writing/reading to/from the segment is done.
 *
 */
int gfx_buffer_swap(GFXBuffer* buffer);

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
 */
void* gfx_buffer_map(GFXBuffer* buffer, size_t size, size_t offset, GFXBufferUsage access);

/**
 * Maps the buffer's current segment and returns a pointer to the mapped data.
 *
 * @param access Access rules to optimize (which must be followed by the client).
 * @return A pointer in client address space (NULL on failure).
 *
 * This method will automatically try to asynchronously upload.
 *
 */
void* gfx_buffer_map_segment(GFXBuffer* buffer, GFXBufferUsage access);

/**
 * Unmaps the buffer, invalidating the pointer returned by gfx_buffer_map.
 *
 * This method MUST be called immediately after gfx_buffer_map in order to continue using the buffer.
 *
 */
void gfx_buffer_unmap(GFXBuffer* buffer);


#ifdef __cplusplus
}
#endif

#endif // GFX_MEMORY_H
