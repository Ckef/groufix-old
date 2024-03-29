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
	unsigned char data[4]; /* Depth per component (rgba or xyzw) in bits */

} GFXBitDepth;


/** Storage data type */
typedef enum GFXDataType
{
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
	GFX_FORMAT_NORMALIZED     = 0x001, /* If set, an integral type will be interpreted as normalized (in the range [0.0, 1.0]) */
	GFX_FORMAT_EXPONENT       = 0x002, /* Interpret the last component (of rgba) as a shared exponent (only for floating point) */
	GFX_FORMAT_REVERSE        = 0x004, /* Components are stored as bgra instead of rgba (the first three components are reversed) */
	GFX_FORMAT_DEPTH          = 0x008, /* First component interpreted as depth */
	GFX_FORMAT_STENCIL        = 0x010, /* First component (after optional depth component) interpreted as stencil */
	GFX_FORMAT_LITTLE_ENDIAN  = 0x020, /* Force each component to be stored in little endian order (as opposed to machine native) */
	GFX_FORMAT_BIG_ENDIAN     = 0x040  /* Same as LITTLE_ENDIAN, only it forces big endian order */

} GFXFormatFlags;


/** Format descriptor */
typedef struct GFXFormat
{
	GFXDataType     type; /* Underlying type the format consists of, signedness indicates the sign of each component */
	GFXBitDepth     depth;
	GFXFormatFlags  flags;

} GFXFormat;


/**
 * Builds a non-ambiguous format descriptor.
 *
 * @param type  If all components share bitdepth, the type of each component, if not, the type of the entire format.
 * @param depth Bit depth for each component (0 to omit a component).
 * @param flags Flags to determine how to interpret each component and the order.
 * @return The format, depth will be all 0s on failure.
 *
 * Note: Non-ambiguous means the only 0s in depth are trailing 0s.
 * This function also checks for flag collisions, at which point it fails.
 *
 */
GFX_API GFXFormat gfx_format(

		GFXDataType     type,
		GFXBitDepth     depth,
		GFXFormatFlags  flags);

/**
 * Builds a format descriptor, implicitly determining the depth from the type.
 *
 * @param type       The type of each component.
 * @param components Number of components of the format.
 * @return The format, depth will be all 0s on failure.
 *
 */
GFX_API GFXFormat gfx_format_from_type(

		GFXDataType     type,
		unsigned char   components,
		GFXFormatFlags  flags);

/**
 * Returns whether a format is valid (a.k.a not all of depth is 0s).
 *
 */
static GFX_ALWAYS_INLINE int gfx_format_is_valid(

		GFXFormat format)
{
	return
		format.depth.data[0] &&
		format.depth.data[1] &&
		format.depth.data[2] &&
		format.depth.data[3];
}

/**
 * Invalidates a format (a.k.a sets all of depth to 0s)
 *
 */
static GFX_ALWAYS_INLINE void gfx_format_invalidate(

		GFXFormat* format)
{
	format->depth.data[0] = 0;
	format->depth.data[1] = 0;
	format->depth.data[2] = 0;
	format->depth.data[3] = 0;
}


/********************************************************
 * Buffer (arbitrary GPU storage)
 *******************************************************/

/** Object flags associated with all buffers */
GFX_API const GFXRenderObjectFlags GFX_BUFFER_OBJECT_FLAGS;


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
 * This is a no-op if it is called on a different thread than allowed.
 * What threads are allowed is defined by the buffer object flags.
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
	GFXFormat    format; /* Format of the attribute as stored in buffers */
	GFXDataType  type;   /* Type of the attribute as interpreted by shaders */
	size_t       offset; /* Byte offset of the attribute (must be <= GFX_LIM_MAX_VERTEX_OFFSET) */

} GFXVertexAttribute;


/** Vertex Source */
typedef struct GFXVertexSource
{
	GFXPrimitive   primitive;
	char           indexed;   /* Non-zero if indexed via the index buffer */
	GFXDataType    indexType; /* Can only be an unsigned type */
	size_t         first;     /* First index to start reading at */
	size_t         count;     /* Number of drawable vertices (must be > 0) */
	unsigned int   patchSize; /* Number of vertices per patch, ignored if primitive is not GFX_PATCHES */

} GFXVertexSource;


/********************************************************
 * Vertex Layout (vertex specifications + sources)
 *******************************************************/

/** Object flags associated with all layouts */
GFX_API const GFXRenderObjectFlags GFX_VERTEX_LAYOUT_OBJECT_FLAGS;


/** Vertex Layout */
typedef struct GFXVertexLayout
{
	/* Super class */
	GFXRenderObject  object;

	/* Read only fields */
	unsigned char    buffers;    /* Number of vertex buffers */
	unsigned char    attributes; /* Number of vertex attributes */
	unsigned char    sources;    /* Number of vertex sources */

} GFXVertexLayout;


/**
 * Creates a new vertex layout.
 *
 * @param buffers    Fixed number of buffers (must be < GFX_LIM_MAX_VERTEX_BUFFERS).
 * @param attributes Fixed number of (sparse) vertex attributes (must be < GFX_LIM_MAX_VERTEX_ATTRIBS).
 * @param sources    Fixed number of vertex sources.
 * @return NULL on failure.
 *
 */
GFX_API GFXVertexLayout* gfx_vertex_layout_create(

		unsigned char  buffers,
		unsigned char  attributes,
		unsigned char  sources);

/**
 * Makes sure the vertex layout is freed properly.
 *
 * This is a no-op if it is called on a different thread than allowed.
 * What threads are allowed is defined by the vertex layout object flags.
 *
 */
GFX_API void gfx_vertex_layout_free(

		GFXVertexLayout* layout);

/**
 * Sets the index buffer of a vertex layout.
 *
 * @param buffer Buffer to use, the current backbuffer is used (can be NULL).
 * @param offset Byte offset within the buffer to start reading at.
 * @return Zero on failure.
 *
 */
GFX_API int gfx_vertex_layout_set_index_buffer(

		GFXVertexLayout*  layout,
		const GFXBuffer*  buffer,
		size_t            offset);

/**
 * Retrieves the index buffer of a vertex layout.
 *
 * @param buffer Returns the buffer that is being used.
 * @param offset Returns the byte offset within the buffer.
 * @return Zero if no buffer is set (no output is written to).
 *
 */
GFX_API int gfx_vertex_layout_get_index_buffer(

		const GFXVertexLayout*  layout,
		GFXBuffer**             buffer,
		size_t*                 offset);

/**
 * Sets a vertex buffer of a vertex layout.
 *
 * @param index   Index of the vertex buffer to set (must be < layout->buffers).
 * @param buffer  Buffer to use, the current backbuffer is used (can be NULL to disable the buffer).
 * @param offset  Byte offset within the buffer to start reading at.
 * @param stride  Byte offset between consecutive attributes.
 * @param divisor Rate at which to advance measured in instances, 0 to advance each vertex.
 * @return Zero on failure.
 *
 * Note: divisor can only be non-zero if GFX_EXT_VERTEX_INSTANCING
 * and stride must be <= GFX_LIM_MAX_VERTEX_STRIDE.
 *
 */
GFX_API int gfx_vertex_layout_set_vertex_buffer(

		GFXVertexLayout*  layout,
		unsigned char     index,
		const GFXBuffer*  buffer,
		size_t            offset,
		size_t            stride,
		unsigned int      divisor);

/**
 * Retrieves a vertex buffer of a vertex layout.
 *
 * @param buffer  Returns the buffer that is being used.
 * @param offset  Returns the byte offset within the buffer.
 * @param stride  Returns the byte offset between consecutive attributes.
 * @param divisor Returns the rate at which it advances in instances.
 * @return Zero if it was never set or if it was disabled (no output is written to).
 *
 */
GFX_API int gfx_vertex_layout_get_vertex_buffer(

		const GFXVertexLayout*  layout,
		unsigned char           index,
		GFXBuffer**             buffer,
		size_t*                 offset,
		size_t*                 stride,
		unsigned int*           divisor);

/**
 * Sets an attribute of a vertex layout.
 *
 * @param index  Index of the attribute to set (must be < layout->attributes).
 * @param attrib Values to set it to (can be NULL to disable).
 * @return Zero on failure.
 *
 * Note: to use GFX_DOUBLE as attribute or format type, GFX_EXT_VERTEX_DOUBLE_PRECISION is required.
 *
 */
GFX_API int gfx_vertex_layout_set_attribute(

		GFXVertexLayout*           layout,
		unsigned char              index,
		const GFXVertexAttribute*  attrib);

/**
 * Retrieves an attribute of a vertex layout.
 *
 * @param attrib Returns the attribute.
 * @return Zero if no attribute is set (no output is written to).
 *
 */
GFX_API int gfx_vertex_layout_get_attribute(

		const GFXVertexLayout*  layout,
		unsigned char           index,
		GFXVertexAttribute*     attrib);

/**
 * Sets the buffer to sample from at an attribute.
 *
 * @param buffer Index of the vertex buffer to use (must be < layout->buffers).
 * @return Zero if either index is out of bounds.
 *
 */
GFX_API int gfx_vertex_layout_set_attribute_buffer(

		GFXVertexLayout*  layout,
		unsigned char     attribute,
		unsigned char     buffer);

/**
 * Retrieves the buffer an attribute samples from.
 *
 * @return Zero if either index is out of bounds or the buffer was never set (no output is written to).
 *
 */
GFX_API int gfx_vertex_layout_get_attribute_buffer(

		const GFXVertexLayout*  layout,
		unsigned char           attribute,
		unsigned char*          buffer);

/**
 * Sets a source of a vertex layout.
 *
 * @param index Index of the source to set (must be < layout->sources).
 * @param src   Values to set it to (can be NULL to disable).
 * @return Zero on failure.
 *
 */
GFX_API int gfx_vertex_layout_set_source(

		GFXVertexLayout*        layout,
		unsigned char           index,
		const GFXVertexSource*  src);

/**
 * Retrieves a source of a vertex layout.
 *
 * @param src Returns the source.
 * @return Zero if no source is set (no output is written to).
 *
 */
GFX_API int gfx_vertex_layout_get_source(

		const GFXVertexLayout*  layout,
		unsigned char           index,
		GFXVertexSource*        src);


/********************************************************
 * Texture & Pixel metadata
 *******************************************************/

/** Texture types */
typedef enum GFXTextureType
{
	GFX_TEXTURE_1D,
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


/** Pixel transfer parameters */
typedef struct GFXPixelTransfer
{
	GFXFormat      format;    /* Format of the client memory */
	unsigned char  alignment; /* Row byte alignment of client memory, can be 1, 2, 4 or 8 */

	unsigned int   xOffset;
	unsigned int   yOffset;   /* Array offset for 1D textures */
	unsigned int   zOffset;   /* Array offset for 2D textures (including cube maps) */
	size_t         width;
	size_t         height;    /* Array size for 1D textures */
	size_t         depth;     /* Array size for 2D textures (face count for cube maps) */

} GFXPixelTransfer;


/********************************************************
 * Texture (an n-D image stored on the GPU)
 *******************************************************/

/** Object flags associated with all textures */
GFX_API const GFXRenderObjectFlags GFX_TEXTURE_OBJECT_FLAGS;


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
	size_t          height;  /* Array size for 1D textures */
	size_t          depth;   /* Array size for 2D textures */

} GFXTexture;


/** Single image of a texture */
typedef struct GFXTextureImage
{
	const GFXTexture*  texture;
	GFXTextureFace     face;   /* Face of the cubemap */
	unsigned char      mipmap; /* Mipmap index */
	unsigned int       index;  /* Index of an array texture */

} GFXTextureImage;


/**
 * Creates a new texture.
 *
 * @param mipmaps Number of mipmaps to allocate, 0 for just the base texture, < 0 to use all mipmap levels.
 * @param height  If 1D texture, acts as an array of images.
 * @param depth   If 2D texture, acts as an array of images.
 * @return NULL on failure.
 *
 * Note: arrays can always be used for 2D textures, 1D textures if GFX_EXT_TEXTURE_ARRAY_1D
 * and cubemaps if GFX_EXT_TEXTURE_ARRAY_CUBEMAP.
 *
 */
GFX_API GFXTexture* gfx_texture_create(

		GFXTextureType  type,
		GFXFormat       format,
		int             mipmaps,
		size_t          width,
		size_t          height,
		size_t          depth);

/**
 * Creates a new multisampled 2D texture.
 *
 * @param depth Number of images within the texture, acts as an array of images.
 * @return NULL on failure.
 *
 * When rendered to, this texture will be multisampled, but you cannot write to this texture.
 * Note: requires GFX_EXT_TEXTURE_MULTISAMPLE, additionally, for depth
 * GFX_EXT_TEXTURE_ARRAY_MULTISAMPLE is required.
 *
 */
GFX_API GFXTexture* gfx_texture_create_multisample(

		GFXFormat      format,
		unsigned char  samples,
		size_t         width,
		size_t         height,
		size_t         depth);

/**
 * Creates a new texture associated with a 1D buffer, it will only see the current backbuffer of the buffer.
 *
 * @return NULL on failure.
 *
 * This texture will share memory with the buffer, the format cannot be packed or interpreted as depth.
 * Also, a multi buffer swap will have no effect on the texture.
 *
 * Note: requires GFX_EXT_TEXTURE_BUFFER.
 *
 */
GFX_API GFXTexture* gfx_texture_create_buffer_link(

		GFXFormat         format,
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
GFX_API GFXFormat gfx_texture_get_format(

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
