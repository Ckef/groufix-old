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

#include "groufix/containers/vector.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Hardware Context handling
 *******************************************************/

/** \brief OpenGL Context */
typedef void* GFXHardwareContext;


/** \brief Hardware Extensions */
typedef unsigned int GFXHardwareExtension;

#define GFX_EXT_GEOMETRY_SHADER       0x0000
#define GFX_EXT_INSTANCED_ATTRIBUTES  0x0001
#define GFX_EXT_PROGRAM_BINARY        0x0002
#define GFX_EXT_TESSELLATION_SHADER   0x0003

#define GFX_EXT_COUNT                 0x0004


/**
 * \brief Get the currently active context.
 *
 * May return NULL in case no window is internally current.
 * If it returned a non NULL pointer, it should be used as context to call other hardware methods.
 *
 */
GFXHardwareContext gfx_hardware_get_context(void);

/**
 * \brief Returns whether a given extension is supported by a context or not.
 *
 */
int gfx_hardware_is_extension_supported(GFXHardwareExtension extension, const GFXHardwareContext cnt);

/**
 * \brief Polls all OpenGL errors at any given time.
 *
 * \param description A description to attach to each error (can be NULL).
 * \return The number of errors encountered.
 * 
 */
unsigned int gfx_hardware_poll_errors(const char* description, const GFXHardwareContext cnt);


/********************************************************
 * Draw calls (render issues)
 *******************************************************/

/** \brief Storage types */
typedef unsigned int GFXDataType;

#define GFX_BYTE            0x1400
#define GFX_UNSIGNED_BYTE   0x1401
#define GFX_SHORT           0x1402
#define GFX_UNSIGNED_SHORT  0x1403
#define GFX_INT             0x1404
#define GFX_UNSIGNED_INT    0x1405
#define GFX_FLOAT           0x1406
#define GFX_HALF_FLOAT      0x140b


/** \brief Primitive types */
typedef unsigned int GFXPrimitiveType;

#define GFX_POINTS          0x0000
#define GFX_LINES           0x0001
#define GFX_LINE_LOOP       0x0002
#define GFX_LINE_STRIP      0x0003
#define GFX_TRIANGLES       0x0004
#define GFX_TRIANGLE_STRIP  0x0005
#define GFX_TRIANGLE_FAN    0x0006


/**
 * \brief Renders primitives from a vertex buffer.
 *
 * \param start Starting index.
 * \param count Number of indices to render.
 *
 */
void gfx_hardware_draw(GFXPrimitiveType prim, size_t start, size_t count, const GFXHardwareContext cnt);

/**
 * \brief Instanced version of the above call.
 *
 * \param inst Number of instances to draw.
 *
 */
void gfx_hardware_draw_instanced(GFXPrimitiveType prim, size_t start, size_t count, size_t inst, const GFXHardwareContext cnt);

/**
 * \brief Renders primitives from a vertex buffer indexed by an index buffer.
 *
 * \param type  Type of the indices, must be an unsigned type.
 * \param start Byte offset in the index buffer.
 * \param count Number of indices to render.
 *
 */
void gfx_hardware_draw_indices(GFXPrimitiveType prim, GFXDataType type, size_t start, size_t count, const GFXHardwareContext cnt);

/**
 * \brief Instanced version of the above call.
 *
 * \param inst Number of instances to draw.
 *
 */
void gfx_hardware_draw_indices_instanced(GFXPrimitiveType prim, GFXDataType type, size_t start, size_t count, size_t inst, const GFXHardwareContext cnt);


/********************************************************
 * Buffer (arbitrary storage)
 *******************************************************/

/** \brief Buffer types */
typedef unsigned int GFXBufferTarget;

#define GFX_BUFFER_VERTEX_ARRAY   0x8892
#define GFX_BUFFER_INDEX_ARRAY    0x8893 /* Can only be created if a HardwareLayout is bound */
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

/** \brief Interpreted data type */
typedef unsigned int GFXInterpretType;

#define GFX_LAYOUT_FLOAT       0x00
#define GFX_LAYOUT_NORMALIZED  0x01
#define GFX_LAYOUT_INTEGER     0x02


/** \brief Vertex Attribute */
typedef struct GFXHardwareAttribute
{
	unsigned short    size;      /* Number of elements */
	GFXDataType       type;      /* Data type of each element, can be any */
	GFXInterpretType  interpret; /* How to interpret each element */
	size_t            stride;    /* Byte offset between consecutive attributes */
	size_t            offset;    /* Byte offset of the first occurence of the attribute */
	unsigned int      divisor;   /* Rate at which attributes advance, 0 for no instancing */

} GFXHardwareAttribute;


/** \brief Hardware Layout */
typedef struct GFXHardwareLayout
{
	unsigned int handle; /* OGL handle */

} GFXHardwareLayout;


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
 * \brief Unbinds the currently bound layout, making sure no changes will be made to any layout.
 *
 */
void gfx_hardware_layout_unbind(const GFXHardwareContext cnt);

/**
 * \brief Returns the maximum number of attributes which can be used.
 *
 * Any index should be smaller than this value (index < maxAttributes), or errors will occur.
 *
 */
unsigned int gfx_hardware_layout_get_max_attributes(const GFXHardwareContext cnt);

/**
 * \brief Enables a vertex attribute of the currently bound layout.
 *
 * When an attribute is enabled, it will read said attribute from a buffer.
 *
 */
void gfx_hardware_layout_enable_attribute(unsigned int index, const GFXHardwareContext cnt);

/**
 * \brief Disables a vertex attribute of the currently bound layout.
 *
 */
void gfx_hardware_layout_disable_attribute(unsigned int index, const GFXHardwareContext cnt);

/**
 * \brief Returns whether a vertex attribute of the currently bound layout is enabled or not.
 *
 */
int gfx_hardware_layout_attribute_enabled(unsigned int index, const GFXHardwareContext cnt);

/**
 * \brief Defines a vertex attribute of the currently bound layout stored in a specified vertex buffer.
 *
 * \param src  Vertex buffer to use for this attribute.
 * \param intr How the components should be interpreted.
 *
 * Note: attr->divisor requires GFX_EXT_INSTANCED_ATTRIBUTES.
 *
 */
void gfx_hardware_layout_set_attribute(unsigned int index, const GFXHardwareAttribute* attr, GFXHardwareBuffer* src, const GFXHardwareContext cnt);

/**
 * \brief Returns the previously defined vertex attribute of the currently bound layout.
 *
 * Note: if GFX_EXT_INSTANCED_ATTRIBUTES is not supported, attr->divisor will always be set to 0.
 *
 */
void gfx_hardware_layout_get_attribute(unsigned int index, GFXHardwareAttribute* attr, const GFXHardwareContext cnt);

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

#define GFX_SHADER_VERTEX        0x8b31
#define GFX_SHADER_TESS_CONTROL  0x8e88 /* Requires GFX_EXT_TESSELLATION_SHADER */
#define GFX_SHADER_TESS_EVAL     0x8e87 /* Requires GFX_EXT_TESSELLATION_SHADER */
#define GFX_SHADER_GEOMETRY      0x8dd9 /* Requires GFX_EXT_GEOMETRY_SHADER */
#define GFX_SHADER_FRAGMENT      0x8b30


/** \brief Hardware Shader */
typedef struct GFXHardwareShader
{
	unsigned int  handle;   /* OGL handle */
	char          compiled; /* Non-zero if compiled with latest changes */

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
 * Freeing a shader which is attached to a non-linked program will result in undefined behaviour.
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

/** \brief Binary format */
typedef unsigned int GFXProgramFormat;


/** \brief Hardware Program */
typedef struct GFXHardwareProgram
{
	unsigned int  handle; /* OGL handle */
	Vector*       shaders;
	char          linked; /* Non-zero if linked with latest shaders */

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

/**
 * \brief Installs a program as part of the render pipeline.
 *
 * Note: if the program is not yet linked, this will produce errors.
 *
 */
void gfx_hardware_program_use(GFXHardwareProgram* program, const GFXHardwareContext cnt);

/**
 * \brief Attaches a shader to a program, making it an active unit in the linking process.
 *
 * \return Whether it could attach the shader or not.
 *
 * Once the program is being linked, all attached shaders will be linked and detached.
 * Freeing a shader which is attached to a non-linked program will result in undefined behaviour.
 *
 */
int gfx_hardware_program_attach_shader(GFXHardwareProgram* program, GFXHardwareShader* shader, const GFXHardwareContext cnt);

/**
 * \brief Detaches a shader from a program, removing it from the linking process.
 *
 * \return Non-zero if the shader was found and detached.
 *
 */
int gfx_hardware_program_detach_shader(GFXHardwareProgram* program, GFXHardwareShader* shader, const GFXHardwareContext cnt);

/**
 * \brief Binds an attribute name within the program to an attribute index.
 *
 * \param index Index to bind to, an index CAN send its data to multiple names.
 * \param name  Name to bind, a name CANNOT retrieve from multiple indices (doing so will override the previous index).
 *
 * Once the program is being linked, the attributes are set as defined.
 *
 */
void gfx_hardware_program_set_attribute(GFXHardwareProgram* program, unsigned int index, const char* name, const GFXHardwareContext cnt);

/**
 * \brief Retrieves the index the name is bound to.
 *
 * \return Index of the given name, -1 if not found.
 *
 * This method can only be called once the program is successfully linked.
 *
 */
int gfx_hardware_program_get_attribute(GFXHardwareProgram* program, const char* name, const GFXHardwareContext cnt);

/**
 * \brief Links the shader units into a single executable program.
 *
 * \param binary Non-zero: hint to suggest usage of gfx_hardware_program_get_binary, zero: regular linking.
 * \return Non-zero if linking was successful.
 *
 * If successful, this will also detach all shaders.
 *
 */
int gfx_hardware_program_link(GFXHardwareProgram* program, int binary, const GFXHardwareContext cnt);

/**
 * \brief Returns a string describing warnings and/or errors of the last linking attempt.
 *
 * If the returned string is not NULL, it should be freed manually.
 *
 */
char* gfx_hardware_program_get_info_log(GFXHardwareProgram* program, const GFXHardwareContext cnt);

/**
 * \brief Returns the binary representation of a previously linked program.
 *
 * \param length Returns the length of the binary data.
 * \param format Returns the format of the data (Vendor dependent).
 * \return If not NULL, it must be freed manually.
 *
 * Note: this functionality requires GFX_EXT_PROGRAM_BINARY.
 *
 */
void* gfx_hardware_program_get_binary(GFXHardwareProgram* program, unsigned int* length, GFXProgramFormat* format, const GFXHardwareContext cnt);

/**
 * \brief Sets the binary representation of a program (implicit linking).
 *
 * \param length The length previously returned by the above call.
 * \param format The format previously returned by the above call.
 * \return Non-zero on success.
 *
 * Note: this functionality requires GFX_EXT_PROGRAM_BINARY.
 *
 */
int gfx_hardware_program_set_binary(GFXHardwareProgram* program, const void* bin, unsigned int length, GFXProgramFormat format, const GFXHardwareContext cnt);


#ifdef __cplusplus
}
#endif

#endif // GFX_HARDWARE_H
