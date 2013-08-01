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

#ifndef GFX_HARDWARE_H
#define GFX_HARDWARE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Hardware Context handling
 *******************************************************/

/** \brief OpenGL Context */
typedef void* GFXHardwareContext;


/** \brief Hardware extensions */
typedef unsigned int GFXHardwareExtension;

#define GFX_EXT_INSTANCED_ATTRIBUTES  0x0001
#define GFX_EXT_GEOMETRY_SHADER       0x0002

#define GFX_EXT_ALL                   0xffff


/**
 * \brief Get the currently active context.
 *
 * May return NULL in case no window is internally current.
 * If it returned a non NULL pointer, it should be used as context to call other hardware methods.
 *
 */
GFXHardwareContext gfx_hardware_get_context(void);

/**
 * \brief Returns whether certain extensions are supported by a context or not.
 *
 * \return Non-zero if one or more of the given extensions are supported.
 *
 */
int gfx_hardware_is_extension_supported(GFXHardwareExtension extensions, const GFXHardwareContext cnt);

/**
 * \brief Polls all OpenGL errors at any given time.
 *
 * \param description A description to attach to each error (can be NULL).
 * \return The number of errors encountered.
 * 
 */
unsigned int gfx_hardware_poll_errors(const char* description, const GFXHardwareContext cnt);


/********************************************************
 * Buffer (arbitrary storage)
 *******************************************************/

/** \brief Buffer types */
typedef unsigned int GFXBufferTarget;

#define GFX_BUFFER_VERTEX_ARRAY   0x8892
#define GFX_BUFFER_INDEX_ARRAY    0x8893
#define GFX_BUFFER_UNIFORM_BLOCK  0x8a11


/** \brief Buffer usage */
typedef unsigned int GFXBufferUsage;

#define GFX_BUFFER_STATIC_WRITE   0x88e4
#define GFX_BUFFER_STATIC_READ    0x88e5
#define GFX_BUFFER_STATIC_COPY    0x88e6
#define GFX_BUFFER_DYNAMIC_WRITE  0x88e8
#define GFX_BUFFER_DYNAMIC_READ   0x88e9
#define GFX_BUFFER_DYNAMIC_COPY   0x88ea
#define GFX_BUFFER_STREAM_WRITE   0x88e0
#define GFX_BUFFER_STREAM_READ    0x88e1
#define GFX_BUFFER_STREAM_COPY    0x88e2


/** \brief Buffer access bitfield */
typedef unsigned int GFXBufferAccess;

#define GFX_BUFFER_READ           0x01
#define GFX_BUFFER_WRITE          0x02
#define GFX_BUFFER_INVALIDATE     0x06
#define GFX_BUFFER_NO_SYNC        0x20


/** \brief Hardware Buffer */
typedef struct GFXHardwareBuffer
{
	unsigned int  handle; /* OGL handle */
	size_t        size;   /* in bytes */

} GFXHardwareBuffer;


/**
 * \brief Creates a new hardware buffer.
 *
 * \param target Target type this buffer is intended to use for.
 * \param data Data to fill the buffer with, can be NULL.
 * \return NULL on failure.
 *
 */
GFXHardwareBuffer* gfx_hardware_buffer_create(GFXBufferTarget target, GFXBufferUsage use, size_t size, const void* data, const GFXHardwareContext cnt);

/**
 * \brief Creates a copy from a hardware buffer.
 *
 * \param target Target type this buffer is intended to use for.
 * \return NULL on failure.
 *
 */
GFXHardwareBuffer* gfx_hardware_buffer_create_copy(GFXBufferTarget target, GFXBufferUsage use, GFXHardwareBuffer* src, const GFXHardwareContext cnt);

/**
 * \brief Makes sure the buffer is freed properly.
 *
 */
void gfx_hardware_buffer_free(GFXHardwareBuffer* buffer, const GFXHardwareContext cnt);

/**
 * \brief Returns the usage of the buffer.
 *
 */
GFXBufferUsage gfx_hardware_buffer_get_usage(GFXHardwareBuffer* buffer, const GFXHardwareContext cnt);

/**
 * \brief Reallocates the buffer data.
 *
 * \param data Data to fill the buffer with, can be NULL.
 *
 */
void gfx_hardware_buffer_realloc(GFXHardwareBuffer* buffer, GFXBufferUsage use, size_t size, const void* data, const GFXHardwareContext cnt);

/**
 * \brief Writes data to the buffer.
 *
 */
void gfx_hardware_buffer_write(GFXHardwareBuffer* buffer, size_t offset, size_t size, const void* data, const GFXHardwareContext cnt);

/**
 * \brief Reads data from the buffer.
 *
 */
void gfx_hardware_buffer_read(GFXHardwareBuffer* buffer, size_t offset, size_t size, void* data, const GFXHardwareContext cnt);

/**
 * \brief Copies data from one buffer to another.
 *
 * One buffer can be used for both the destination and source, as long as the two areas don't overlap.
 *
 */
void gfx_hardware_buffer_copy(GFXHardwareBuffer* dest, GFXHardwareBuffer* src, size_t destOffset, size_t srcOffset, size_t size, const GFXHardwareContext cnt);

/**
 * \brief Maps the buffer, meaning it can be accessed as a regular array.
 *
 * \param access Bitfield specifying the permitted access methods.
 * \return A pointer to the accessible data (NULL on errors).
 *
 * When accessing the buffer is done, it must be unmapped to allow it to be accessed regularly.
 *
 */
void* gfx_hardware_buffer_map(GFXHardwareBuffer* buffer, GFXBufferAccess access, size_t offset, size_t length, const GFXHardwareContext cnt);

/**
 * \brief Returns the mapped pointer.
 *
 * \return NULL if it doesn't exist.
 *
 */
void* gfx_hardware_buffer_get_map(GFXHardwareBuffer* buffer, const GFXHardwareContext cnt);

/**
 * \brief Disables the mapped pointer, if any.
 *
 * This must be called when accessing the buffer is done.
 *
 */
void gfx_hardware_buffer_unmap(GFXHardwareBuffer* buffer, const GFXHardwareContext cnt);


/********************************************************
 * Layout (vertex specification)
 *******************************************************/

/** \brief Storage types */
typedef unsigned int GFXDataType;

#define GFX_BYTE               0x1400
#define GFX_UNSIGNED_BYTE      0x1401
#define GFX_SHORT              0x1402
#define GFX_UNSIGNED_SHORT     0x1403
#define GFX_INT                0x1404
#define GFX_UNSIGNED_INT       0x1405
#define GFX_FLOAT              0x1406
#define GFX_HALF_FLOAT         0x140b


/** \brief Interpreted data type */
typedef unsigned int GFXInterpretType;

#define GFX_LAYOUT_FLOAT       0x00
#define GFX_LAYOUT_NORMALIZED  0x01
#define GFX_LAYOUT_INTEGER     0x02


/** \brief Hardware Layout */
typedef struct GFXHardwareLayout
{
	unsigned int handle; /* OGL handle */

} GFXHardwareLayout;


/** \brief Vertex Attribute */
typedef struct GFXHardwareAttribute
{
	unsigned short    size;      /* Number of elements */
	GFXDataType       type;      /* Data type of each element */
	GFXInterpretType  interpret; /* How to interpret each element */
	size_t            stride;    /* Byte offset between consecutive attributes */
	size_t            offset;    /* Byte offset of the first occurence of the attribute */

} GFXHardwareAttribute;


/**
 * \brief Creates a new hardware layout.
 *
 * \return NULL on failure.
 *
 * Also binds the layout.
 *
 */
GFXHardwareLayout* gfx_hardware_layout_create(const GFXHardwareContext cnt);

/**
 * \brief Makes sure the layout is freed properly.
 *
 */
void gfx_hardware_layout_free(GFXHardwareLayout* layout, const GFXHardwareContext cnt);

/**
 * \brief Binds the layout as active instructions to render with.
 *
 */
void gfx_hardware_layout_bind(GFXHardwareLayout* layout, const GFXHardwareContext cnt);

/**
 * \brief Returns the maximum number of attributes which can be used.
 *
 * Any index should be smaller than this value (index < maxAttributes).
 *
 */
unsigned int gfx_hardware_layout_get_max_attributes(const GFXHardwareContext cnt);

/**
 * \brief Enables a vertex attribute of the currently bound layout.
 *
 * \return Non-zero if it could enable the attribute.
 *
 * When an attribute is enabled, it will read said attribute from a buffer.
 * If not, it will read a constant value.
 *
 */
int gfx_hardware_layout_enable_attribute(unsigned int index, const GFXHardwareContext cnt);

/**
 * \brief Disables a vertex attribute of the currently bound layout.
 *
 * \return Non-zero if it could disable the attribute.
 *
 */
int gfx_hardware_layout_disable_attribute(unsigned int index, const GFXHardwareContext cnt);

/**
 * \brief Defines a vertex attribute of the currently bound layout.
 *
 * \param src  Vertex buffer to use for this attribute.
 * \param intr How the components should be interpreted.
 * \return Non-zero if it could define the attribute.
 *
 */
int gfx_hardware_layout_set_attribute(unsigned int index, const GFXHardwareAttribute* attr, GFXHardwareBuffer* src, const GFXHardwareContext cnt);

/**
 * \brief Modifies the rate at which attributes advance during instanced rendering (of the currently bound layout).
 *
 * \param instances If zero, the attribute behaves regularly, if not, it will advance once per the number of instances.
 * \return Non-zero if it could set the divisor.
 *
 * Note: this functionality needs GFX_EXT_INSTANCED_ATTRIBUTES.
 *
 */
int gfx_hardware_layout_set_attribute_divisor(unsigned int index, unsigned int instances, const GFXHardwareContext cnt);

/**
 * \brief Returns the previously defined vertex attribute of the currently bound layout.
 *
 * \return Non-zero if it could get the attribute definition.
 *
 */
int gfx_hardware_layout_get_attribute(unsigned int index, GFXHardwareAttribute* attr, const GFXHardwareContext cnt);

/**
 * \brief Binds an index buffer to the currently bound layout.
 *
 * If using indexed drawing, this buffer will be used to pull indices from.
 *
 */
void gfx_hardware_layout_set_index_buffer(GFXHardwareBuffer* buffer, const GFXHardwareContext cnt);


/********************************************************
 * Shader (compiled GPU program unit)
 *******************************************************/

/** \brief Shader stages */
typedef unsigned int GFXShaderStage;

#define GFX_SHADER_VERTEX    0x8b31
#define GFX_SHADER_GEOMETRY  0x8dd9
#define GFX_SHADER_FRAGMENT  0x8b30


/** \brief Hardware Shader */
typedef struct GFXHardwareShader
{
	unsigned int handle; /* OGL handle */

} GFXHardwareShader;


/**
 * \brief Creates a new hardware shader.
 *
 * \return NULL on failure.
 *
 */
GFXHardwareShader* gfx_hardware_shader_create(GFXShaderStage stage, const GFXHardwareContext cnt);

/**
 * \brief Makes sure the shader is freed properly.
 *
 */
void gfx_hardware_shader_free(GFXHardwareShader* shader, const GFXHardwareContext cnt);

/**
 * \brief Returns the stage of a given shader.
 *
 */
GFXShaderStage gfx_hardware_shader_get_stage(GFXHardwareShader* shader, const GFXHardwareContext cnt);

/**
 * \brief Sets the source string of the shader.
 *
 * \param count Number of strings to copy.
 * \param src   Array of null terminated strings to copy.
 *
 * The strings are appended and copied to the shader's storage.
 *
 */
void gfx_hardware_shader_set_source(GFXHardwareShader* shader, size_t count, const char** src, const GFXHardwareContext cnt);

/**
 * \brief Returns the source string of the shader.
 *
 * If the returned string is not NULL, it should be freed manually.
 *
 */
char* gfx_hardware_shader_get_source(GFXHardwareShader* shader, const GFXHardwareContext cnt);

/**
 * \brief Compiles the shader into a single shader unit.
 *
 * \return Non-zero if compilation was successful.
 *
 */
int gfx_hardware_shader_compile(GFXHardwareShader* shader, const GFXHardwareContext cnt);

/**
 * \brief Returns a string describing warnings and/or errors of the last compilation attempt.
 *
 * If the returned string is not NULL, it should be freed manually.
 *
 */
char* gfx_hardware_shader_get_info_log(GFXHardwareShader* shader, const GFXHardwareContext cnt);


/********************************************************
 * Program (compiled GPU program pipeline)
 *******************************************************/

/** \brief Hardware Program */
typedef struct GFXHardwareProgram
{
	unsigned int handle; /* OGL handle */

} GFXHardwareProgram;


/**
 * \brief Creates a new hardware program.
 *
 * \return NULL on failure.
 *
 */
GFXHardwareProgram* gfx_hardware_program_create(const GFXHardwareContext cnt);

/**
 * \brief Makes sure the program is freed properly.
 *
 */
void gfx_hardware_program_free(GFXHardwareProgram* program, const GFXHardwareContext cnt);


#ifdef __cplusplus
}
#endif

#endif // GFX_HARDWARE_H
