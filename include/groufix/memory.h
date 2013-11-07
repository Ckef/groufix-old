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
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Data types associated with the GPU
 *******************************************************/

/** Unpacked storage data type */
typedef enum GFXUnpackedType
{
	GFX_BYTE            = 0x1400,
	GFX_UNSIGNED_BYTE   = 0x1401,
	GFX_SHORT           = 0x1402,
	GFX_UNSIGNED_SHORT  = 0x1403,
	GFX_INT             = 0x1404,
	GFX_UNSIGNED_INT    = 0x1405,
	GFX_HALF_FLOAT      = 0x140b,
	GFX_FLOAT           = 0x1406

} GFXUnpackedType;


/** Packed storage data type */
typedef enum GFXPackedType
{
	GFX_UNSIGNED_SHORT_5_6_5      = 0x8363,
	GFX_UNSIGNED_SHORT_4_4_4_4    = 0x8033,
	GFX_UNSIGNED_SHORT_5_5_5_1    = 0x8034,
	GFX_INT_10_10_10_2            = 0x8d9f,
	GFX_UNSIGNED_INT_10_10_10_2   = 0x8368, /* Interpreted as integer */
	GFX_UNSIGNED_INT_11F_11F_10F  = 0x8c3b,
	GFX_UNSIGNED_INT_9_9_9_5E     = 0x8c3e, /* Shared exponent of 5 bits */
	GFX_UNSIGNED_INT_24_8         = 0x84fa, /* Depth/stencil only */
	GFX_FLOAT_UNSIGNED_INT_24_8   = 0x8dad  /* Depth/stencil only */

} GFXPackedType;


/** Interpreted type */
typedef enum GFXInterpretType
{
	GFX_INTERPRET_FLOAT       = 0x00,
	GFX_INTERPRET_NORMALIZED  = 0x01,
	GFX_INTERPRET_INTEGER     = 0x02,
	GFX_INTERPRET_DEPTH       = 0x04

} GFXInterpretType;


/** Storage data type */
typedef union GFXDataType
{
	GFXUnpackedType  unpacked;
	GFXPackedType    packed;

} GFXDataType;


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
	size_t           id;      /* Hardware Object ID */

	size_t           size;    /* Size of the buffer in bytes */
	size_t           segSize; /* Size of a segment in the buffer (can equal size) */
	unsigned char    multi;   /* Number of extra buffers (0 = regular buffering) */

	GFXBufferUsage   usage;   /* Intended usage of the buffer */
	GFXBufferTarget  target;  /* Storage type the buffer is targeted for */

} GFXBuffer;


/**
 * Creates a new buffer.
 *
 * @param usage    Usage bitflag, how the buffer is intended to be used.
 * @param target   Storage type the buffer is targeted for.
 * @param multi    Number of extra buffers to allocate (> 0 for multi buffering, 0 for regular buffering).
 * @param segments Number of extra segments per buffer (> 0 for segmentation, 0 for one big segment).
 * @return NULL on failure.
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
 * @param offset Offset within the segment.
 * @return A pointer in client address space (NULL on failure).
 *
 * This method will automatically try to asynchronously upload.
 *
 */
void* gfx_buffer_map_segment(GFXBuffer* buffer, size_t size, size_t offset, GFXBufferUsage access);

/**
 * Unmaps the buffer, invalidating the pointer returned by gfx_buffer_map.
 *
 * This method MUST be called immediately after gfx_buffer_map in order to continue using the buffer.
 *
 */
void gfx_buffer_unmap(GFXBuffer* buffer);


/********************************************************
 * Vertex Layout metadata
 *******************************************************/

/** Primitive types */
typedef enum GFXPrimitive
{
	GFX_POINTS          = 0x0000,
	GFX_LINES           = 0x0001,
	GFX_LINE_LOOP       = 0x0002,
	GFX_LINE_STRIP      = 0x0003,
	GFX_TRIANGLES       = 0x0004,
	GFX_TRIANGLE_STRIP  = 0x0005,
	GFX_TRIANGLE_FAN    = 0x0006

} GFXPrimitive;


/** Draw call */
typedef struct GFXDrawCall
{
	GFXPrimitive     primitive;
	uintptr_t        first;     /* First index (regular) or byte offset (indexed) */
	size_t           count;     /* Number of vertices to draw */

	GFXUnpackedType  indexType; /* Can only be an unsigned type */
	GFXBuffer*       buffer;    /* Index buffer */

} GFXDrawCall;


/** Vertex Attribute */
typedef struct GFXVertexAttribute
{
	unsigned char     size;      /* Number of elements */
	GFXDataType       type;      /* Data type of each element, packed types override the size and interpret type */
	GFXInterpretType  interpret; /* How to interpret each element, DEPTH is equal to FLOAT */

	size_t            stride;    /* Byte offset between consecutive attributes */
	size_t            offset;    /* Byte offset of the first occurrence of the attribute */
	unsigned int      divisor;   /* Rate at which to advance, 0 for no instancing, requires GFX_EXT_INSTANCED_ATTRIBUTES */

} GFXVertexAttribute;


/********************************************************
 * Vertex Layout (vertex specifications + draw calls)
 *******************************************************/

/** Vertex Layout */
typedef struct GFXVertexLayout
{
	size_t         id;        /* Hardware Object ID */
	unsigned char  drawCalls; /* Number of draw calls */

} GFXVertexLayout;


/**
 * Creates a new vertex layout.
 *
 * @param drawCalls Fixed number of draw calls associated with this layout.
 * @return NULL on failure.
 *
 */
GFXVertexLayout* gfx_vertex_layout_create(unsigned char drawCalls);

/**
 * Makes sure the vertex layout is freed properly.
 *
 */
void gfx_vertex_layout_free(GFXVertexLayout* layout);

/**
 * Sets an attribute of a vertex layout.
 *
 * @param index  Index of the attribute to set (must be < GFX_LIM_MAX_VERTEX_ATTRIBS).
 * @param buffer Buffer to read this attribute from (cannot be NULL), a multi buffer swap will have no effect on the attribute.
 * @return Zero on failure.
 *
 * The attribute's type can be unpacked or (UNSIGNED_)INT_10_10_10_2 if GFX_EXT_PACKED_ATTRIBUTES is supported.
 *
 */
int gfx_vertex_layout_set_attribute(GFXVertexLayout* layout, unsigned int index, const GFXVertexAttribute* attr, const GFXBuffer* buffer);

/**
 * Retrieves an attribute from a vertex layout.
 *
 * @param index Index of the attribute to retrieve.
 * @return Zero on failure (nothing is written to the output parameters).
 *
 */
int gfx_vertex_layout_get_attribute(GFXVertexLayout* layout, unsigned int index, GFXVertexAttribute* attr);

/**
 * Removes an attribute from a vertex layout.
 *
 * @param index Index of the attribute to remove.
 *
 */
void gfx_vertex_layout_remove_attribute(GFXVertexLayout* layout, unsigned int index);

/**
 * Changes a draw call of the vertex layout.
 *
 * @param index Index of the draw call (must be < layout->drawCalls).
 * @return Non-zero if the draw call could be changed.
 *
 */
int gfx_vertex_layout_set(GFXVertexLayout* layout, unsigned char index, const GFXDrawCall* call);

/**
 * Retrieves a draw call from the vertex layout.
 *
 * @param index Index of the draw call to retrieve (must be < layout->drawCalls).
 * @return Zero on failure (nothing is written to the output parameters).
 *
 */
int gfx_vertex_layout_get(GFXVertexLayout* layout, unsigned char index, GFXDrawCall* call);


/********************************************************
 * Texture & Pixel metadata
 *******************************************************/

/** Texture types */
typedef enum GFXTextureType
{
	GFX_TEXTURE_1D, /* requires GFX_EXT_1D_TEXTURE (or implicitly GFX_EXT_BUFFER_TEXTURE) */
	GFX_TEXTURE_2D,
	GFX_TEXTURE_3D,
	GFX_CUBEMAP,

} GFXTextureType;


/** Faces of a cubemap */
typedef enum GFXTextureFace
{
	GFX_FACE_POSITIVE_X,
	GFX_FACE_NEGATIVE_X,
	GFX_FACE_POSITIVE_Y,
	GFX_FACE_NEGATIVE_Y,
	GFX_FACE_POSITIVE_Z,
	GFX_FACE_NEGATIVE_Z

} GFXTextureFace;


/** Texture format */
typedef struct GFXTextureFormat
{
	unsigned char     components; /* Number of components */
	GFXDataType       type;       /* Data type of each component, packed types override the entire format */
	GFXInterpretType  interpret;  /* How to interpret the texture components */

} GFXTextureFormat;


/** Pixel transfer parameters */
typedef struct GFXPixelTransfer
{
	GFXTextureFormat  format;    /* Format of the client memory */
	unsigned char     alignment; /* Row byte alignment of client memory, can be 1, 2, 4 or 8 */

	unsigned char     mipmap;    /* Mipmap index to write to/read from */
	GFXTextureFace    face;      /* Face of the cubemap to write to/read from */

	unsigned int      xOffset;
	unsigned int      yOffset;   /* Layer offset for 1D textures */
	unsigned int      zOffset;   /* Layer offset for 2D textures */
	size_t            width;
	size_t            height;    /* Layer count for 1D textures */
	size_t            depth;     /* Layer count for 2D textures */

} GFXPixelTransfer;


/********************************************************
 * Texture (an n-D image stored on the GPU)
 *******************************************************/

/** Texture */
typedef struct GFXTexture
{
	size_t          id;      /* Hardware Object ID */

	GFXTextureType  type;    /* Describes image arrangement and sampling */
	unsigned char   mipmaps; /* Number of mipmaps (0 for none) */

	size_t          width;
	size_t          height;
	size_t          depth;

} GFXTexture;


/**
 * Creates a new texture.
 *
 * @param mipmaps Number of mipmaps to allocate, 0 for just the base texture, < 0 to use all mipmap levels.
 * @param height  If 1D texture, acts as an array of images.
 * @param depth   If 2D texture, acts as an array of images.
 * @return NULL on failure.
 *
 * Note: layers can only be used for 1D or 2D textures, or cubemaps if GFX_EXT_LAYERED_CUBEMAP.
 *
 */
GFXTexture* gfx_texture_create(GFXTextureType type, GFXTextureFormat format, int mipmaps, size_t width, size_t height, size_t depth);

/**
 * Creates a new multisampled 2D texture.
 *
 * @param layers Number of images within the texture, acts as an array of images (stored as depth).
 * @return NULL on failure.
 *
 * When rendered to, this texture will be multisampled, but you cannot write to this texture.
 * Note: requires GFX_EXT_MULTISAMPLE_TEXTURE.
 *
 */
GFXTexture* gfx_texture_create_multisample(GFXTextureFormat format, unsigned char samples, size_t width, size_t height, size_t layers);

/**
 * Creates a new texture associated with a 1D buffer.
 *
 * @return NULL on failure.
 *
 * This texture will share memory with the buffer, the format cannot be packed or interpreted as depth.
 * Note: requires GFX_EXT_BUFFER_TEXTURE.
 *
 */
GFXTexture* gfx_texture_create_buffer_link(GFXTextureFormat format, const GFXBuffer* buffer);

/**
 * Makes sure the texture is freed properly.
 *
 */
void gfx_texture_free(GFXTexture* texture);

/**
 * Returns the internal format of the texture.
 *
 * Note: this might differ from a previously given format if it contained a packed type.
 *
 */
GFXTextureFormat gfx_texture_get_format(GFXTexture* texture);

/**
 * Writes to the texture synchronously.
 *
 * @param data Data to write to the texture, cannot be NULL.
 *
 * Note: if the texture is linked to a buffer, the client format must be equal to the texture format.
 *
 */
void gfx_texture_write(GFXTexture* texture, const GFXPixelTransfer* transfer, const void* data);

/**
 * Writes to the texture from a buffer.
 *
 * This method is asynchronous, it performs a DMA transfer.
 * Note: if the texture is linked to a buffer, the client format must be equal to the texture format.
 *
 */
void gfx_texture_write_from_buffer(GFXTexture* texture, const GFXPixelTransfer* transfer, const GFXBuffer* buffer, size_t offset);

/**
 * Auto generates all mipmap levels.
 *
 * Note: the base mipmap level (level 0) is assumed to contain correct data.
 * All other mipmap levels will be overridden.
 *
 */
void gfx_texture_generate_mipmaps(GFXTexture* texture);


#ifdef __cplusplus
}
#endif

#endif // GFX_MEMORY_H
