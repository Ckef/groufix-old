/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
 *
 * Groufix is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Groufix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Groufix.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef GFX_CORE_SHADING_H
#define GFX_CORE_SHADING_H

#include "groufix/core/memory.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Shader (compiled GLSL unit)
 *******************************************************/

/** Shader types */
typedef enum GFXShaderType
{
	GFX_VERTEX_SHADER        = 0x8b31,
	GFX_TESS_CONTROL_SHADER  = 0x8e88, /* requires GFX_EXT_TESSELLATION_SHADER */
	GFX_TESS_EVAL_SHADER     = 0x8e87, /* requires GFX_EXT_TESSELLATION_SHADER */
	GFX_GEOMETRY_SHADER      = 0x8dd9, /* requires GFX_EXT_GEOMETRY_SHADER */
	GFX_FRAGMENT_SHADER      = 0x8b30

} GFXShaderType;


/** Shader */
typedef struct GFXShader
{
	size_t         id;       /* Hardware Object ID */
	GFXShaderType  type;     /* Shader stage of GPU pipeline */
	char           compiled; /* Non-zero if compiled with latest changes. */

} GFXShader;


/**
 * Creates a new shader.
 *
 * @param type Type denoting the GPU pipeline stage to participate in.
 * @return NULL on failure.
 *
 */
GFXShader* gfx_shader_create(GFXShaderType type);

/**
 * Makes sure the shader is freed properly.
 *
 */
void gfx_shader_free(GFXShader* shader);

/**
 * Sets the source of a shader.
 *
 * @param src Array containing the sources to append to each other (all NULL terminated).
 * @return Non-zero on success.
 *
 * Note: This will entirely replace a source set by a previous call to the method.
 * The source strings will be copied.
 *
 */
int gfx_shader_set_source(GFXShader* shader, size_t num, const char** src);

/**
 * Returns the source of a shader (null terminated string).
 *
 * @param length Length of the returned source, excluding null terminator (can be NULL).
 * @return NULL if no source was set yet.
 *
 * If the returned pointer is not NULL, it should be freed manually.
 *
 */
char* gfx_shader_get_source(GFXShader* shader, size_t* length);

/**
 * Compiles the shader if necessary.
 *
 * @return Non-zero on success.
 *
 * Additionally, an error will be generated on failure.
 *
 */
int gfx_shader_compile(GFXShader* shader);


/********************************************************
 * Program properties
 *******************************************************/

/** Property type */
typedef enum GFXPropertyType
{
	GFX_VECTOR_PROPERTY, /* Can only be of type GFX_FLOAT, GFX_INT or GFX_UNSIGNED_INT */
	GFX_MATRIX_PROPERTY, /* Can only be of type GFX_FLOAT */
	GFX_SAMPLER_PROPERTY

} GFXPropertyType;


/** Property */
typedef struct GFXProperty
{
	GFXPropertyType  type;
	GFXUnpackedType  dataType;   /* Data type of each component */
	unsigned char    components; /* Number of components, or coordinate dimensions for samplers (can be 0 if unsupported) */
	size_t           count;      /* Number of array elements (can be 0 if unsupported) */

} GFXProperty;


/********************************************************
 * Program block properties
 *******************************************************/

/** Property Block Member */
typedef struct GFXPropertyBlockMember
{
	unsigned short  property; /* Property index */
	size_t          offset;   /* Byte offset from the beginning of the block */
	unsigned char   stride;   /* If a matrix, column stride, if not a matrix, array stride */

} GFXPropertyBlockMember;


/** Property Block */
typedef struct GFXPropertyBlock
{
	unsigned short           numProperties;
	GFXPropertyBlockMember*  properties;

} GFXPropertyBlock;


/********************************************************
 * Program (linked shaders to form GPU pipeline)
 *******************************************************/

/** Program Binary Format */
typedef unsigned int GFXProgramFormat;


/** Feedback Mode */
typedef enum GFXFeedbackMode
{
	GFX_FEEDBACK_INTERLEAVED  = 0x8c8c,
	GFX_FEEDBACK_SEPARATE     = 0x8c8d

} GFXFeedbackMode;


/** Program */
typedef struct GFXProgram
{
	size_t          id;         /* Hardware Object ID */
	unsigned short  properties; /* Accessible properties */
	unsigned short  blocks;     /* Accessible property blocks */

} GFXProgram;


/**
 * Creates a new program.
 *
 * @return NULL on failure.
 *
 */
GFXProgram* gfx_program_create(void);

/**
 * Makes sure the program is freed properly.
 *
 */
void gfx_program_free(GFXProgram* program);

/**
 * Forwards data send to the given index to a given name within the program.
 *
 * @param index Index of the attribute (must be < GFX_LIM_MAX_VERTEX_ATTRIBS).
 * @param name  Name of the attribute in the shaders (the string is copied).
 * @return Zero on failure.
 *
 * This method takes effect once the program is linked, not after.
 * Note: an index can be forwarded to multiple names.
 *
 */
int gfx_program_set_attribute(GFXProgram* program, unsigned int index, const char* name);

/**
 * Forwards data received from output attributes to a given index.
 *
 * @param num   Size of names, must be <= GFX_LIM_MAX_FEEDBACK_BUFFERS.
 * @param names Array containing the names of the output attributes in the shader (the strings are copied).
 * @param mode  Feedback mode to use, INTERLEAVED to write to a single buffer, SEPARATE to write to multiple buffers.
 * @return Zero on failure.
 *
 * This method takes effect once the program is linked, not after.
 * Note: any name cannot occur twice in names.
 *
 */
int gfx_program_set_feedback(GFXProgram* program, size_t num, const char** names, GFXFeedbackMode mode);

/**
 * Links given shaders into a program.
 *
 * @shaders All shader objects to link into the program (cannot be NULL).
 * @return Non-zero on success.
 *
 * Note: this call will attempt to compile all given shaders.
 *
 * Additionally, an error will be generated on failure.
 * This will remove any evidence of a previous link operation.
 *
 */
int gfx_program_link(GFXProgram* program, size_t num, GFXShader** shaders);

/**
 * Retrieves the binary representation of a program.
 *
 * @param format Returns the format of the data (implementation dependent).
 * @param size   Number of bytes of the binary data.
 * @return Binary data, NULL on failure.
 *
 * If the returned pointer is not NULL, it should be freed manually.
 *
 */
void* gfx_program_get_binary(GFXProgram* program, GFXProgramFormat* format, size_t* size);

/**
 * Sets the binary representation of a program.
 *
 * @param data   Data to read.
 * @param format Format of the data (implementation dependent).
 * @param size   Number of bytes to read.
 * @return Non-zero on success.
 *
 * This operation can be considered to perform an implicit linking operation.
 *
 */
int gfx_program_set_binary(GFXProgram* program, GFXProgramFormat format, size_t size, const void* data);

/**
 * Returns a property of the program.
 *
 * @param index Index of the property, must be < program->properties.
 * @return NULL if the property does not exist.
 *
 * Note: properties only exist after the program is linked successfully.
 *
 */
const GFXProperty* gfx_program_get_property(GFXProgram* program, unsigned short index);

/**
 * Returns the index of the property with the given uniform name.
 *
 * @param name Name of the property in the shader (the string is copied).
 * @return program->properties on failure, index otherwise.
 *
 */
unsigned short gfx_program_get_named_property(GFXProgram* program, const char* name);

/**
 * Returns a property block of the program.
 *
 * @param index Index of the property block, must be < program->blocks.
 * @return NULL if the block does not exist.
 *
 * Note: blocks only exist after the program is linked successfully.
 *
 */
const GFXPropertyBlock* gfx_program_get_property_block(GFXProgram* program, unsigned short index);

/**
 * Returns the index of the property block with the given uniform name.
 *
 * @param name Name of the property block in the shader (the string is copied).
 * @return program->blocks on failure, index otherwise.
 *
 */
unsigned short gfx_program_get_named_property_block(GFXProgram* program, const char* name);


/********************************************************
 * Property Map (inputs associated with a program)
 *******************************************************/

/** Property map */
typedef struct GFXPropertyMap
{
	GFXProgram*    program;    /* Program it references (cannot be changed or freed while the map is alive) */
	unsigned char  properties; /* Number of properties */

} GFXPropertyMap;


/**
 * Creates a new property map.
 *
 * @param program    Program to set inputs for.
 * @param properties Fixed number of property indices associated with this map.
 * @return NULL on failure.
 *
 */
GFXPropertyMap* gfx_property_map_create(GFXProgram* program, unsigned char properties);

/**
 * Makes sure the property map is freed properly.
 *
 */
void gfx_property_map_free(GFXPropertyMap* map);

/**
 * Forwards data send to a given index to a property within the program.
 *
 * @param index    Index to forward to the program (must be < map->properties).
 * @param property Index of the program property to forward to.
 * @return Zero on failure.
 *
 */
int gfx_property_map_forward(GFXPropertyMap* map, unsigned char index, unsigned short property);

/**
 * Forwards data send to a given index to a given uniform name within the program.
 *
 * @param index Index to forward to the program.
 * @param name  Uniform name within the program to forward to.
 * @return Zero on failure.
 *
 */
int gfx_property_map_forward_named(GFXPropertyMap* map, unsigned char index, const char* name);

/**
 * Forwards data send to a given index to a property block within the program.
 *
 * @param index Index to forward to the program (must be < map->properties).
 * @param block Index of the program property block to forward to.
 * @return Zero on failure.
 *
 */
int gfx_property_map_forward_block(GFXPropertyMap* map, unsigned char index, unsigned short block);

/**
 * Forwards data send to a given index to a given uniform block name within the program.
 *
 * @param index Index to forward to the program.
 * @param name  Uniform block name within the program to forward to.
 * @return Zero on failure.
 *
 */
int gfx_property_map_forward_named_block(GFXPropertyMap* map, unsigned char index, const char* name);

/**
 * Sets the value of a vector/matrix property.
 *
 * @param index  Index of the property to set the value of.
 * @param value  Value to set it to, the content will be copied.
 * @param offset Byte offset in the mapped value to write to.
 * @param size   Size of the given data in bytes (0 to calculate the size based on the property type).
 * @return Non-zero on success.
 *
 */
int gfx_property_map_set_value(GFXPropertyMap* map, unsigned char index, const void* value, size_t offset, size_t size);

/**
 * Sets the value of a sampler property.
 *
 * @param index Index of the property to set the value of.
 * @return Non-zero on success.
 *
 * Note: There can only be GFX_LIM_MAX_SAMPLER_PROPERTIES number of sampler properties forwarded.
 *
 */
int gfx_property_map_set_sampler(GFXPropertyMap* map, unsigned char index, const GFXTexture* texture);

/**
 * Sets the value of a property block.
 *
 * @param index  Index of the property to set the value of.
 * @param offset Offset in the buffer in bytes.
 * @param size   Size of the data in bytes.
 * @return Non-zero on success.
 *
 * Note: There can only be GFX_LIM_MAX_BUFFER_PROPERTIES number of block properties forwarded.
 *
 */
int gfx_property_map_set_buffer(GFXPropertyMap* map, unsigned char index, const GFXBuffer* buffer, size_t offset, size_t size);

/**
 * Sets the value of a property block.
 *
 * @param index  Index of the property to set the value of.
 * @param offset Offset in the shared buffer in bytes.
 * @param size   Size of the data in bytes.
 * @return Non-zero on success.
 *
 */
int gfx_property_map_set_shared_buffer(GFXPropertyMap* map, unsigned char index, const GFXSharedBuffer* buffer, size_t offset, size_t size);

/**
 * Swaps the value of two properties.
 *
 * @return Non-zero on success.
 *
 */
int gfx_property_map_swap(GFXPropertyMap* map, unsigned char index1, unsigned char index2);


#ifdef __cplusplus
}
#endif

#endif // GFX_CORE_SHADING_H