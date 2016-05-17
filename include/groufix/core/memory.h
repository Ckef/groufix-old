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

#ifndef GFX_CORE_MEMORY_H
#define GFX_CORE_MEMORY_H

#include "groufix/core/object.h"
#include "groufix/utils.h"

#include <stddef.h>


/********************************************************
 * Data types and formats associated with the GPU
 *******************************************************/

/** Bit depth */
typedef struct GFXBitDepth
{
	unsigned char data[4]; /* Depth per component (rgba or xyzw) */

} GFXBitDepth;


/** Storage data type */
typedef enum GFXDataType
{
	GFX_BIT,
	GFX_NIBBLE,
	GFX_BYTE,
	GFX_UNSIGNED_BYTE,
	GFX_SHORT,
	GFX_UNSIGNED_SHORT,
	GFX_INT,
	GFX_UNSIGNED_INT,
	GFX_HALF_FLOAT,
	GFX_FLOAT,
	GFX_DOUBLE

} GFXDataType;


/** Format flags */
typedef enum GFXFormatFlags
{
	GFX_FORMAT_NORMALIZED     = 0x001, /* Only useful for server side formats (ignored for client side) */
	GFX_FORMAT_EXPONENT       = 0x002, /* Interpret the last component (of rgba) as a shared exponent (only for floating point) */
	GFX_FORMAT_REVERSE        = 0x004, /* Components are stored as abgr instead of rgba */
	GFX_FORMAT_ALPHA_LAST     = 0x008, /* In conjunction with REVERSE it produces bgra, otherwise rgba */
	GFX_FORMAT_DEPTH          = 0x010, /* First component interpreted as depth */
	GFX_FORMAT_STENCIL        = 0x020, /* First component (after optional depth component) interpreted as stencil */
	GFX_FORMAT_LITTLE_ENDIAN  = 0x040, /* Force each component to be stored in little endian order (as opposed to machine native) */
	GFX_FORMAT_BIG_ENDIAN     = 0x080  /* Same as LITTLE_ENDIAN, only it forces big endian order */

} GFXFormatFlags;


/** Format descriptor */
typedef struct GFXFormat
{
	GFXDataType     type;
	GFXBitDepth     depth;
	GFXFormatFlags  flags;

} GFXFormat;


/**
 * Builds a non-ambiguous format descriptor.
 *
 * @param type  Data type for each component.
 * @param depth Bit depth for each component (0 to omit a component).
 * @param flags Flags to determine how to interpret each component and the order.
 * @return The format, depth will be all 0s on failure.
 *
 */
GFX_API GFXFormat gfx_format(

		GFXDataType     type,
		GFXBitDepth     depth,
		GFXFormatFlags  flags);

/**
 * Builds a format descriptor, implicitly determining the depth from the type.
 *
 * @param type       Data type for each component.
 * @param components Number of components of the format.
 * @param flags      Flags to determine how to interpret each component and the order.
 * @return The format, depth will be all 0s on failure.
 *
 */
GFX_API GFXFormat gfx_format_from_type(

		GFXDataType     type,
		unsigned char   components,
		GFXFormatFlags  flags);


/********************************************************
 * Buffer (arbitrary GPU storage)
 *******************************************************/

/** Buffer usage bitflag */
typedef enum GFXBufferUsage
{
	GFX_BUFFER_CLIENT_STORAGE  = 0x001,
	GFX_BUFFER_READ            = 0x002,
	GFX_BUFFER_WRITE           = 0x004,
	GFX_BUFFER_MAP_READ        = 0x008,
	GFX_BUFFER_MAP_WRITE       = 0x010,
	GFX_BUFFER_MAP_PERSISTENT  = 0x020

} GFXBufferUsage;


/** Buffer */
typedef struct GFXBuffer
{
	/* Super class */
	GFXRenderObject  object;

	/* Read only fields */
	GFXBufferUsage   usage; /* Intended usage of the buffer */
	size_t           size;  /* Size of the buffer in bytes */
	unsigned char    count; /* Number of backbuffers (1 = regular buffering) */

} GFXBuffer;


/**
 * Creates a new buffer.
 *
 * @param usage Usage bitflag, how the buffer is intended to be used.
 * @param size  Size of each individual backbuffer.
 * @param count Number of backbuffers to allocate (0 is the same as 1).
 * @return NULL on failure.
 *
 * Note: if data is not NULL, this data is only copied to the current backbuffer.
 *
 */
GFX_API GFXBuffer* gfx_buffer_create(

		GFXBufferUsage  usage,
		size_t          size,
		const void*     data,
		unsigned char   count);

/**
 * Creates a copy of a buffer.
 *
 * @param usage Usage bitflag, how the buffer is intended to be used.
 * @return Non-zero on success.
 *
 * Note: only copies the current backbuffer.
 *
 */
GFX_API GFXBuffer* gfx_buffer_create_copy(

		const GFXBuffer*  src,
		GFXBufferUsage    usage);

/**
 * Makes sure the buffer is freed properly.
 *
 * This is a no-op if it is called on a groufix thread that is not shared, or called
 * on a non-groufix thread whilst it is still being used on a groufix thread.
 *
 */
GFX_API void gfx_buffer_free(

		GFXBuffer* buffer);

/**
 * Advances to the next backbuffer.
 *
 */
GFX_API void gfx_buffer_swap(

		GFXBuffer* buffer);

/**
 * Reads data from the current backbuffer synchronously.
 *
 * @param size   Size of the data to read, in bytes.
 * @param data   Pointer to write to, cannot be NULL.
 * @param offset Byte offset in the buffer to begin reading at.
 * @return Number of bytes actually read.
 *
 * Note: GFX_BUFFER_READ must be set at creation.
 *
 */
GFX_API size_t gfx_buffer_read(

		const GFXBuffer*  buffer,
		size_t            size,
		void*             data,
		size_t            offset);

/**
 * Writes data to the current backbuffer synchronously.
 *
 * @param size   Size of the data to write, in bytes.
 * @param data   Data to write to the buffer, cannot be NULL.
 * @param offset Byte offset in the buffer to begin writing at.
 * @return Number of bytes actually written.
 *
 * Note: GFX_BUFFER_WRITE must be set at creation.
 *
 */
GFX_API size_t gfx_buffer_write(

		GFXBuffer*   buffer,
		size_t       size,
		const void*  data,
		size_t       offset);

/**
 * Copies the content of one buffer's current backbuffer to another.
 *
 * @param dest       Buffer to write to.
 * @param src        Buffer to read from.
 * @param srcOffset  Byte offset within src to start reading.
 * @param destOffset Byte offset within dest to start writing.
 * @param size       Size of the data to be copied, in bytes.
 * @return Number of bytes actually copied
 *
 * Note: If src and dest are equal and the ranges overlap, undefined behaviour is expected.
 *
 */
GFX_API size_t gfx_buffer_copy(

		GFXBuffer*  dest,
		GFXBuffer*  src,
		size_t      srcOffset,
		size_t      destOffset,
		size_t      size);

/**
 * Copies the content of the current backbuffer to another at the same buffer.
 *
 * @param dest Backbuffer offset counting from the current to write to.
 * @return Number of bytes actually copied
 *
 * Note: if dest points to the same backbuffer and the ranges overlap, undefined behaviour
 * is expected.
 *
 */
GFX_API size_t gfx_buffer_copy_same(

		GFXBuffer*     buffer,
		unsigned char  dest,
		size_t         srcOffset,
		size_t         destOffset,
		size_t         size);

/**
 * Orphans the current backbuffer, meaning the current storage is invalidated.
 *
 * This method allows to allocate a new buffer somewhere on the GPU, and disregard
 * the old storage. This way the renderer can still use the old buffer while the
 * new buffer is ready to be used for some other purpose.
 *
 */
GFX_API void gfx_buffer_orphan(

		GFXBuffer* buffer);

/**
 * Maps the current backbuffer and returns a pointer to the mapped data.
 *
 * @param size   Size of the region to map, also returns the actual mapped size.
 * @param offset Offset within the buffer.
 * @return A pointer in client address space (NULL on failure).
 *
 * Note: this is asynchronously, writing to the pointer whilst the buffer is used
 * elsewhere (for example it is used for rendering) is undefined behaviour.
 *
 * GFX_BUFFER_MAP_WRITE and GFX_BUFFER_MAP_READ given at creation of the buffer
 * dictate what is possible to do with the pointer. If neither is set, this
 * function will return NULL.
 *
 */
GFX_API void* gfx_buffer_map(

		GFXBuffer*  buffer,
		size_t*     size,
		size_t      offset);

/**
 * Unmaps the current backbuffer, invalidating the pointer returned by gfx_buffer_map.
 *
 * @return If zero, the buffer's content is undefined, it may have been corrupted for some reason.
 *
 * This method MUST be called before any other method after gfx_buffer_map in order to continue
 * using the same backbuffer (the others can savely be used).
 *
 */
GFX_API int gfx_buffer_unmap(

		const GFXBuffer* buffer);


/********************************************************
 * Vertex Layout metadata
 *******************************************************/

/** Primitive types */
typedef enum GFXPrimitive
{
	GFX_POINTS,
	GFX_LINES,
	GFX_LINES_ADJACENCY,          /* Requires GFX_EXT_GEOMETRY_SHADER */
	GFX_LINE_LOOP,
	GFX_LINE_STRIP,
	GFX_LINE_STRIP_ADJACENCY,     /* Requires GFX_EXT_GEOMETRY_SHADER */
	GFX_TRIANGLES,
	GFX_TRIANGLES_ADJACENCY,      /* Requires GFX_EXT_GEOMETRY_SHADER */
	GFX_TRIANGLE_STRIP,
	GFX_TRIANGLE_STRIP_ADJACENCY, /* Requires GFX_EXT_GEOMETRY_SHADER */
	GFX_TRIANGLE_FAN,
	GFX_PATCHES,                  /* Requires GFX_EXT_TESSELLATION_SHADER */

} GFXPrimitive;


/** Vertex Attribute */
typedef struct GFXVertexAttribute
{
	unsigned char      size;      /* Number of elements, 0 signifies the type is packed */
	GFXDataType        type;      /* Data type of each element */
	//GFXInterpretFlags  interpret; /* How to interpret each element, DEPTH is equal to FLOAT and STENCIL is equal to INTEGER */
	unsigned int       offset;    /* Offset of the attribute, must be <= GFX_LIM_MAX_VERTEX_ATTRIB_OFFSET */

} GFXVertexAttribute;


/** Vertex Source */
typedef struct GFXVertexSource
{
	GFXPrimitive   primitive;
	unsigned char  indexed;   /* Non-zero if indexed via the index buffer */
	GFXDataType    indexType; /* Can only be an unsigned type */
	size_t         first;     /* First index to start reading at */
	size_t         count;     /* Number of drawable vertices */
	unsigned int   patchSize; /* Number of vertices per patch, ignored if primitive is not GFX_PATCHES */

} GFXVertexSource;


/********************************************************
 * Vertex Layout (vertex specifications + sources)
 *******************************************************/

/** Vertex Layout */
typedef struct GFXVertexLayout
{
	/* Super class */
	GFXRenderObject  object;

	/* Read only fields */
	unsigned char    attributes; /* Number of vertex attributes */
	unsigned char    sources;    /* Number of vertex sources */

} GFXVertexLayout;


/**
 * Creates a new vertex layout.
 *
 * @param attributes Fixed number of (sparse) vertex attributes associated with this layout.
 * @param sources    Fixed number of vertex sources associated with this layout.
 * @return NULL on failure.
 *
 * Note: attributes must be <= GFX_LIM_MAX_VERTEX_ATTRIBS.
 *
 */
GFX_API GFXVertexLayout* gfx_vertex_layout_create(

		unsigned char  attributes,
		unsigned char  sources);

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
 * @param attr   Attribute parameters.
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
 * Changes a source of the vertex layout.
 *
 * @param index  Index of the source (must be < layout->sources).
 * @param source Vertex source parameters.
 * @return Zero on failure or if the source does not exist.
 *
 * This will fail if the source was added to a bucket.
 *
 * Note: To use GFX_PATCHES, GFX_EXT_TESSELLATION_SHADER is required,
 * if using it, source->patchSize must be <= GFX_LIM_MAX_PATCH_VERTICES.
 *
 */
GFX_API int gfx_vertex_layout_set_source(

		GFXVertexLayout*        layout,
		unsigned char           index,
		const GFXVertexSource*  source);

/**
 * Retrieves a source from the vertex layout.
 *
 * @param index  Index of the source to retrieve (must be < layout->sources).
 * @param source Output of the vertex source parameters.
 * @return Zero on failure (nothing is written to source).
 *
 */
GFX_API int gfx_vertex_layout_get_source(

		const GFXVertexLayout*  layout,
		unsigned char           index,
		GFXVertexSource*        source);

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
 * Sets the divisor of a vertex buffer at a vertex layout.
 *
 * @param index   Index of the vertex buffer.
 * @param divisor Rate at which to advance measured in instances, 0 to advance each vertex.
 * @return Zero on failure.
 *
 * Note: requires GFX_EXT_INSTANCED_ATTRIBUTES.
 *
 */
GFX_API int gfx_vertex_layout_set_vertex_divisor(

		GFXVertexLayout*  layout,
		unsigned int      index,
		unsigned int      divisor);

/**
 * Sets the index buffer.
 *
 * @param buffer Buffer to use as index buffer, only the current backbuffer of a multi buffer will be used (can be NULL).
 * @param offset Byte offset within the buffer to start reading at.
 * @return Zero on failure.
 *
 * This will fail if a source was added to a bucket.
 *
 * Note: offset must be a multiple of the size of the largest index used within this layout.
 * If not, adding the source to a bucket will fail.
 *
 */
GFX_API int gfx_vertex_layout_set_index_buffer(

		GFXVertexLayout*  layout,
		const GFXBuffer*  buffer,
		size_t            offset);


/********************************************************
 * Texture & Pixel metadata
 *******************************************************/

/** Texture types */
typedef enum GFXTextureType
{
	GFX_TEXTURE_2D,
	GFX_TEXTURE_3D,
	GFX_CUBEMAP

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
	unsigned char      components; /* Number of components, 0 signifies the type is packed */
	GFXDataType        type;       /* Data type of each component */
	//GFXInterpretFlags  interpret;  /* How to interpret the texture components */

} GFXTextureFormat;


/** Pixel transfer parameters */
typedef struct GFXPixelTransfer
{
	GFXTextureFormat  format;    /* Format of the client memory */
	unsigned char     alignment; /* Row byte alignment of client memory, can be 1, 2, 4 or 8 */

	unsigned int      xOffset;
	unsigned int      yOffset;
	unsigned int      zOffset;   /* Layer offset for 2D textures (including cube maps) */
	size_t            width;
	size_t            height;
	size_t            depth;     /* Layer count for 2D textures (face count for cube maps) */

} GFXPixelTransfer;


/********************************************************
 * Texture (an n-D image stored on the GPU)
 *******************************************************/

/** Texture */
typedef struct GFXTexture
{
	/* Super class */
	GFXRenderObject  object;

	/* Read only fields */
	GFXTextureType  type;    /* Describes image arrangement and sampling */
	unsigned char   mipmaps; /* Number of mipmaps (0 for base texture only) */
	unsigned char   samples; /* Number of samples for multisampled textures (1 for other textures) */

	size_t          width;
	size_t          height;
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
 * @param depth   If 2D texture, acts as an array of images.
 * @return NULL on failure.
 *
 * Note: layers can only be used for 2D textures, or cubemaps if GFX_EXT_LAYERED_CUBEMAP.
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
 * The type of the texture will store GFX_TEXTURE_2D.
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


#endif // GFX_CORE_MEMORY_H
