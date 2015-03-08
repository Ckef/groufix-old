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

#ifndef GFX_CORE_MEMORY_H
#define GFX_CORE_MEMORY_H

#include "groufix/utils.h"

#include <stddef.h>

/* Default shared buffer pool size (1 MB) */
#define GFX_SHARED_BUFFER_SIZE_DEFAULT  0x100000

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Data types associated with the GPU
 *******************************************************/

/** Unpacked storage data type */
typedef enum GFXUnpackedType
{
	GFX_BIT,
	GFX_NIBBLE,
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
	GFX_INTERPRET_FLOAT       = 0x001,
	GFX_INTERPRET_NORMALIZED  = 0x002,
	GFX_INTERPRET_INTEGER     = 0x004,
	GFX_INTERPRET_DEPTH       = 0x008,
	GFX_INTERPRET_STENCIL     = 0x010

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

/** Buffer usage bitflag */
typedef enum GFXBufferUsage
{
	GFX_BUFFER_READ     = 0x01,
	GFX_BUFFER_WRITE    = 0x02,
	GFX_BUFFER_STREAM   = 0x04,
	GFX_BUFFER_DYNAMIC  = 0x08

} GFXBufferUsage;


/** Buffer target */
typedef enum GFXBufferTarget
{
	GFX_FEEDBACK_BUFFER  = 0x8c8e,
	GFX_INDEX_BUFFER     = 0x8893,
	GFX_PROPERTY_BUFFER  = 0x8a11,
	GFX_TEXTURE_BUFFER   = 0x8c2a, /* Requires GFX_EXT_BUFFER_TEXTURE */
	GFX_VERTEX_BUFFER    = 0x8892

} GFXBufferTarget;


/** Buffer */
typedef struct GFXBuffer
{
	size_t           size;   /* Size of the buffer in bytes */
	unsigned char    multi;  /* Number of extra buffers (0 = regular buffering) */

	GFXBufferUsage   usage;  /* Intended usage of the buffer */
	GFXBufferTarget  target; /* Storage type the buffer is targeted for */

} GFXBuffer;


/**
 * Creates a new buffer.
 *
 * @param usage    Usage bitflag, how the buffer is intended to be used.
 * @param target   Storage type the buffer is targeted for.
 * @param size     Size of each individual backbuffer.
 * @param multi    Number of extra backbuffers to allocate (> 0 for multi buffering, 0 for regular buffering).
 * @return NULL on failure.
 *
 * Note: if data is not NULL, this data is NOT copied to any extra buffers.
 *
 */
GFX_API GFXBuffer* gfx_buffer_create(

		GFXBufferUsage   usage,
		GFXBufferTarget  target,
		size_t           size,
		const void*      data,
		unsigned char    multi);

/**
 * Creates a copy of a buffer.
 *
 * @param usage  Usage bitflag, how the buffer is intended to be used.
 * @param target Storage type the buffer is targeted for.
 * @return Non-zero on success.
 *
 * Note: only copies the current backbuffer when multi buffering.
 *
 */
GFX_API GFXBuffer* gfx_buffer_create_copy(

		const GFXBuffer*  src,
		GFXBufferUsage    usage,
		GFXBufferTarget   target);

/**
 * Makes sure the buffer is freed properly.
 *
 */
GFX_API void gfx_buffer_free(

		GFXBuffer* buffer);

/**
 * Allocates more backbuffers for multibuffering.
 *
 * @param num Number of extra buffers to allocate.
 * @return Non-zero on success.
 *
 */
GFX_API int gfx_buffer_expand(

		GFXBuffer*     buffer,
		unsigned char  num);

/**
 * Deallocates backbuffers from multibuffering.
 *
 * @param num Number of buffers to remove.
 * @return Number of buffers actually removed.
 *
 */
GFX_API int gfx_buffer_shrink(

		GFXBuffer*     buffer,
		unsigned char  num);

/**
 * Advances to the next backbuffer.
 *
 */
GFX_API void gfx_buffer_swap(

		GFXBuffer* buffer);

/**
 * Writes data to the current backbuffer synchronously.
 *
 * @param size   Size of the data to write, in bytes.
 * @param data   Data to write to the buffer, cannot be NULL.
 * @param offset Byte offset in the buffer to begin writing at.
 *
 */
GFX_API void gfx_buffer_write(

		const GFXBuffer*  buffer,
		size_t            size,
		const void*       data,
		size_t            offset);

/**
 * Reads data from the current backbuffer synchronously.
 *
 * @param size   Size of the data to read, in bytes.
 * @param data   Pointer to write to, cannot be NULL.
 * @param offset Byte offset in the buffer to begin reading at.
 *
 */
GFX_API void gfx_buffer_read(

		const GFXBuffer*  buffer,
		size_t            size,
		void*             data,
		size_t            offset);

/**
 * Maps the current backbuffer and returns a pointer to the mapped data.
 *
 * @param access Access rules to optimize (which must be followed by the client).
 * @param offset Offset within the buffer.
 * @return A pointer in client address space (NULL on failure).
 *
 */
GFX_API void* gfx_buffer_map(

		const GFXBuffer*  buffer,
		size_t            size,
		size_t            offset,
		GFXBufferUsage    access);

/**
 * Unmaps the buffer, invalidating the pointer returned by gfx_buffer_map.
 *
 * This method MUST be called immediately after gfx_buffer_map in order to continue using the buffer.
 *
 */
GFX_API void gfx_buffer_unmap(

		const GFXBuffer* buffer);


/********************************************************
 * Shared Buffer (pooled arbitrary GPU storage)
 *******************************************************/

/** Shared buffer */
typedef struct GFXSharedBuffer
{
	void*   reference;
	size_t  offset; /* In bytes */

} GFXSharedBuffer;


/**
 * Requests a new size if a new buffer pool needs to be created.
 *
 * @param size Pool size in bytes, the default is GFX_SHARED_BUFFER_SIZE_DEFAULT.
 *
 */
GFX_API void gfx_shared_buffer_request_size(

		unsigned long size);

/**
 * Initializes a shared buffer.
 *
 * @param target Storage type the buffer is targeted for.
 * @return Non-zero on success (it will not touch buffer on failure).
 *
 */
GFX_API int gfx_shared_buffer_init(

		GFXSharedBuffer*  buffer,
		GFXBufferTarget   target,
		size_t            size,
		const void*       data);

/**
 * Clears a shared buffer, freeing the internal data.
 *
 */
GFX_API void gfx_shared_buffer_clear(

		GFXSharedBuffer* buffer);


/********************************************************
 * Vertex Layout metadata
 *******************************************************/

/** Primitive types */
typedef enum GFXPrimitive
{
	GFX_POINTS                    = 0x0000,
	GFX_LINES                     = 0x0001,
	GFX_LINES_ADJACENCY           = 0x000a, /* Requires GFX_EXT_GEOMETRY_SHADER */
	GFX_LINE_LOOP                 = 0x0002,
	GFX_LINE_STRIP                = 0x0003,
	GFX_LINE_STRIP_ADJACENCY      = 0x000b, /* Requires GFX_EXT_GEOMETRY_SHADER */
	GFX_TRIANGLES                 = 0x0004,
	GFX_TRIANGLES_ADJACENCY       = 0x000c, /* Requires GFX_EXT_GEOMETRY_SHADER */
	GFX_TRIANGLE_STRIP            = 0x0005,
	GFX_TRIANGLE_STRIP_ADJACENCY  = 0x000d, /* Requires GFX_EXT_GEOMETRY_SHADER */
	GFX_TRIANGLE_FAN              = 0x0006,
	GFX_PATCHES                   = 0x000e  /* Requires GFX_EXT_TESSELLATION_SHADER */

} GFXPrimitive;


/** Vertex Attribute */
typedef struct GFXVertexAttribute
{
	unsigned char     size;      /* Number of elements */
	GFXDataType       type;      /* Data type of each element, packed types override the size and interpret type */
	GFXInterpretType  interpret; /* How to interpret each element, DEPTH is equal to FLOAT and STENCIL is equal to INTEGER */
	unsigned int      offset;    /* Offset of the attribute, must be <= GFX_LIM_MAX_VERTEX_ATTRIB_OFFSET */

} GFXVertexAttribute;


/** Feedback buffer argument */
typedef struct GFXFeedbackBuffer
{
	GFXBuffer*  buffer;
	size_t      offset; /* Must be 4-byte aligned */
	size_t      size;

} GFXFeedbackBuffer;


/** Draw call */
typedef struct GFXDrawCall
{
	GFXPrimitive     primitive;
	uintptr_t        first;     /* Byte offset if an index buffer is used, first index otherwise */
	size_t           count;     /* Number of vertices to draw */
	GFXUnpackedType  indexType; /* Can only be an unsigned type */

} GFXDrawCall;


/********************************************************
 * Vertex Layout (vertex specifications + draw calls)
 *******************************************************/

/** Vertex Layout */
typedef struct GFXVertexLayout
{
	unsigned char drawCalls; /* Number of draw calls */

} GFXVertexLayout;


/**
 * Creates a new vertex layout.
 *
 * @param drawCalls Fixed number of draw calls associated with this layout.
 * @return NULL on failure.
 *
 */
GFX_API GFXVertexLayout* gfx_vertex_layout_create(

		unsigned char drawCalls);

/**
 * Makes sure the vertex layout is freed properly.
 *
 */
GFX_API void gfx_vertex_layout_free(

		GFXVertexLayout* layout);

/**
 * Adds/sets an attribute of a vertex layout.
 *
 * @param index  Index of the attribute to set (must be < GFX_LIM_MAX_VERTEX_ATTRIBS).
 * @param attr   Attribute parameters (cannot be NULL).
 * @param buffer Index of the vertex buffer to use.
 * @return Zero on failure.
 *
 * The attribute's type can be unpacked, or, (UNSIGNED_)INT_10_10_10_2 if GFX_EXT_PACKED_ATTRIBUTES is supported.
 * Note: for the attribute to have any effect the given buffer should be set.
 *
 */
GFX_API int gfx_vertex_layout_set_attribute(

		GFXVertexLayout*           layout,
		unsigned int               index,
		const GFXVertexAttribute*  attr,
		unsigned int               buffer);

/**
 * Changes the buffer an attribute samples from.
 *
 * @param index  Index of the attribute to change.
 * @param buffer Index of the vertex buffer to use.
 * @return Zero on failure.
 *
 * Note: the attribute must have been set for this call to have any effect.
 *
 */
GFX_API int gfx_vertex_layout_set_attribute_buffer(

		GFXVertexLayout*  layout,
		unsigned int      index,
		unsigned int      buffer);

/**
 * Adds/sets a vertex buffer of a vertex layout.
 *
 * @param index  Index of the vertex buffer to set (must be < GFX_LIM_MAX_VERTEX_BUFFERS).
 * @param buffer Buffer to use, only the current backbuffer of a multi buffer will be used (can be NULL to disable).
 * @param offset Byte offset within the buffer to start reading at.
 * @param stride Byte offset between consecutive attributes (must be <= GFX_LIM_MAX_VERTEX_STRIDE).
 * @return Zero on failure.
 *
 */
GFX_API int gfx_vertex_layout_set_vertex_buffer(

		GFXVertexLayout*  layout,
		unsigned int      index,
		const GFXBuffer*  buffer,
		size_t            offset,
		size_t            stride);

/**
 * Adds/sets a shared vertex buffer of a vertex layout.
 *
 * @param index  Index of the vertex buffer to set (equal to that of non-shared vertex buffers).
 * @param buffer Shared buffer to use (can be NULL to disable).
 * @return Zero on failure.
 *
 */
GFX_API int gfx_vertex_layout_set_shared_vertex_buffer(

		GFXVertexLayout*        layout,
		unsigned int            index,
		const GFXSharedBuffer*  buffer,
		size_t                  offset,
		size_t                  stride);

/**
 * Changes a draw call of the vertex layout.
 *
 * @param index Index of the draw call (must be < layout->drawCalls).
 * @param call  Draw call parameters (cannot be NULL).
 * @return Zero on failure or if the draw call does not exist.
 *
 */
GFX_API int gfx_vertex_layout_set_draw_call(

		GFXVertexLayout*    layout,
		unsigned char       index,
		const GFXDrawCall*  call);

/**
 * Retrieves a draw call from the vertex layout.
 *
 * @param index Index of the draw call to retrieve (must be < layout->drawCalls).
 * @return Zero on failure (nothing is written to the output parameters).
 *
 * If a shared buffer was used, call->first will be the given offset + the shared buffer offset.
 *
 */
GFX_API int gfx_vertex_layout_get_draw_call(

		const GFXVertexLayout*  layout,
		unsigned char           index,
		GFXDrawCall*            call);

/**
 * Sets the index buffer source of a draw call.
 *
 * @param buffer Buffer to use as index buffer, only the current backbuffer of a multi buffer will be used (can be NULL).
 * @param offset Byte offset within the buffer to start reading at.
 *
 */
GFX_API void gfx_vertex_layout_set_index_buffer(

		GFXVertexLayout*  layout,
		const GFXBuffer*  buffer,
		size_t            offset);

/**
 * Sets the (shared) index buffer source of a draw call.
 *
 * @param buffer Shared buffer to use as index buffer (can be NULL).
 * @param offset Byte offset within the buffer to start reading at.
 *
 */
GFX_API void gfx_vertex_layout_set_shared_index_buffer(

		GFXVertexLayout*        layout,
		const GFXSharedBuffer*  buffer,
		size_t                  offset);

/**
 * Sets the divisor of a vertex buffer at a vertex layout.
 *
 * @param index   Index of the vertex buffer.
 * @param divisor Rate at which to advance measured in instances, 0 to advance each vertex.
 *
 * Note: requires GFX_EXT_INSTANCED_ATTRIBUTES.
 *
 */
GFX_API int gfx_vertex_layout_set_vertex_divisor(

		GFXVertexLayout*  layout,
		unsigned int      index,
		unsigned int      divisor);

/**
 * Sets the number of vertices per patch (GFX_PATCHES).
 *
 * @param vertices Number of vertices per patch, must be <= GFX_LIM_MAX_PATCH_VERTICES.
 * @return Zero on failure.
 *
 * Note: requires GFX_EXT_TESSELLATION_SHADER.
 *
 */
GFX_API int gfx_vertex_layout_set_patch_vertices(

		GFXVertexLayout*  layout,
		unsigned int      vertices);

/**
 * Sets a feedback attribute of a vertex layout.
 *
 * @param primitive Primitive to output to the buffers, can only be GFX_POINTS, GFX_LINES or GFX_TRIANGLES.
 * @param num       Size of buffers, must be <= GFX_LIM_MAX_FEEDBACK_BUFFERS.
 * @param buffers   Array of buffers to receive program feedback from.
 * @return Zero on failure.
 *
 * Note: any two given buffer ranges cannot overlap.
 *
 */
GFX_API int gfx_vertex_layout_set_feedback(

		GFXVertexLayout*          layout,
		GFXPrimitive              primitive,
		size_t                    num,
		const GFXFeedbackBuffer*  buffers);

/**
 * Returns the number of instanced vertex buffers.
 *
 * This is equivalent to any buffer having a divisor not equal to zero.
 *
 */
GFX_API unsigned int gfx_vertex_layout_count_instanced(

		const GFXVertexLayout* layout);


/********************************************************
 * Texture & Pixel metadata
 *******************************************************/

/** Texture types */
typedef enum GFXTextureType
{
	GFX_TEXTURE_1D, /* requires GFX_EXT_1D_TEXTURE */
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

	unsigned int      xOffset;
	unsigned int      yOffset;   /* Layer offset for 1D textures */
	unsigned int      zOffset;   /* Layer offset for 2D textures (including cube maps) */
	size_t            width;
	size_t            height;    /* Layer count for 1D textures */
	size_t            depth;     /* Layer count for 2D textures (face count for cube maps) */

} GFXPixelTransfer;


/********************************************************
 * Texture (an n-D image stored on the GPU)
 *******************************************************/

/** Texture */
typedef struct GFXTexture
{
	GFXTextureType  type;    /* Describes image arrangement and sampling */
	unsigned char   mipmaps; /* Number of mipmaps (0 for base texture only) */
	unsigned char   samples; /* Number of samples for multisampled textures (1 for other textures) */

	size_t          width;
	size_t          height;  /* Layer count for 1D textures */
	size_t          depth;   /* Layer count for 2D textures */

} GFXTexture;


/** Single image of a texture */
typedef struct GFXTextureImage
{
	const GFXTexture*  texture;
	GFXTextureFace     face;   /* Face of the cubemap */
	unsigned char      mipmap; /* Mipmap index */
	unsigned int       layer;  /* Index of an array texture */

} GFXTextureImage;


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
GFX_API GFXTexture* gfx_texture_create(

		GFXTextureType    type,
		GFXTextureFormat  format,
		int               mipmaps,
		size_t            width,
		size_t            height,
		size_t            depth);

/**
 * Creates a new multisampled 2D texture.
 *
 * @param depth Number of images within the texture, acts as an array of images.
 * @return NULL on failure.
 *
 * When rendered to, this texture will be multisampled, but you cannot write to this texture.
 * Note: requires GFX_EXT_MULTISAMPLE_TEXTURE,
 * additionally, for depth GFX_EXT_LAYERED_MULTISAMPLE_TEXTURE is required.
 *
 */
GFX_API GFXTexture* gfx_texture_create_multisample(

		GFXTextureFormat  format,
		unsigned char     samples,
		size_t            width,
		size_t            height,
		size_t            depth);

/**
 * Creates a new texture associated with a 1D buffer, it will only see the current backbuffer of the buffer.
 *
 * @return NULL on failure.
 *
 * This texture will share memory with the buffer, the format cannot be packed or interpreted as depth.
 * Also, a multi buffer swap will have no effect on the texture.
 *
 * Note: requires GFX_EXT_BUFFER_TEXTURE.
 *
 */
GFX_API GFXTexture* gfx_texture_create_buffer_link(

		GFXTextureFormat  format,
		const GFXBuffer*  buffer);

/**
 * Makes sure the texture is freed properly.
 *
 */
GFX_API void gfx_texture_free(

		GFXTexture* texture);

/**
 * Returns the internal format of the texture.
 *
 * Note: this might differ from a previously given format if it contained a packed type.
 *
 */
GFX_API GFXTextureFormat gfx_texture_get_format(

		const GFXTexture* texture);

/**
 * Writes to the texture synchronously.
 *
 * @param data Data to write to the texture, cannot be NULL.
 *
 * Note: if the texture is linked to a buffer, the client format must be equal to the texture format.
 *
 */
GFX_API void gfx_texture_write(

		GFXTextureImage          image,
		const GFXPixelTransfer*  transfer,
		const void*              data);

/**
 * Writes to the texture from a buffer, it will read from its current backbuffer.
 *
 * This method is asynchronous, it performs a DMA transfer.
 * Note: if the texture is linked to a buffer, the client format must be equal to the texture format.
 *
 */
GFX_API void gfx_texture_write_from_buffer(

		GFXTextureImage          image,
		const GFXPixelTransfer*  transfer,
		const GFXBuffer*         buffer,
		size_t                   offset);

/**
 * Auto generates all mipmap levels.
 *
 * Note: the base mipmap level (level 0) is assumed to contain correct data.
 * All other mipmap levels will be overridden.
 *
 */
GFX_API void gfx_texture_generate_mipmaps(

		const GFXTexture* texture);


#ifdef __cplusplus
}
#endif

#endif // GFX_CORE_MEMORY_H
