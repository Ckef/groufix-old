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

#ifndef GFX_CORE_SHADING_H
#define GFX_CORE_SHADING_H

#include "groufix/core/memory.h"

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Shader (compiled GLSL unit)
 *******************************************************/

/** Shader stages */
typedef enum GFXShaderStage
{
	GFX_VERTEX_SHADER        = 0x8b31,
	GFX_TESS_CONTROL_SHADER  = 0x8e88, /* requires GFX_EXT_TESSELLATION_SHADER */
	GFX_TESS_EVAL_SHADER     = 0x8e87, /* requires GFX_EXT_TESSELLATION_SHADER */
	GFX_GEOMETRY_SHADER      = 0x8dd9, /* requires GFX_EXT_GEOMETRY_SHADER */
	GFX_FRAGMENT_SHADER      = 0x8b30,

	GFX_ALL_SHADERS          = 0xffff

} GFXShaderStage;


/** Shader */
typedef struct GFXShader
{
	GFXShaderStage  stage;    /* Shader stage of GPU pipeline */
	char            compiled; /* Non-zero if compiled with latest changes. */

} GFXShader;


/**
 * Creates a new shader.
 *
 * @param stage The GPU pipeline stage to participate in.
 * @return NULL on failure.
 *
 */
GFX_API GFXShader* gfx_shader_create(

		GFXShaderStage stage);

/**
 * Makes sure the shader is freed properly.
 *
 */
GFX_API void gfx_shader_free(

		GFXShader* shader);

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
GFX_API int gfx_shader_set_source(

		GFXShader*    shader,
		size_t        num,
		const char**  src);

/**
 * Compiles the shader if necessary.
 *
 * @return Non-zero on success.
 *
 * Additionally, an error will be generated on failure.
 *
 */
GFX_API int gfx_shader_compile(

		GFXShader* shader);


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
	size_t                   size;          /* Byte size to hold all uniforms within the block */
	unsigned short           numProperties; /* Number of elements in properties */
	GFXPropertyBlockMember*  properties;

} GFXPropertyBlock;


/********************************************************
 * Program (linked shaders to form a GPU program)
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
	unsigned short  properties; /* Accessible properties */
	unsigned short  blocks;     /* Accessible property blocks */
	size_t          instances;  /* Number of instances that can be drawn at once */
	char            linked;     /* Non-zero if linked with latest changes. */

} GFXProgram;


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
GFX_API int gfx_program_set_attribute(

		GFXProgram*   program,
		unsigned int  index,
		const char*   name);

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
GFX_API int gfx_program_set_feedback(

		GFXProgram*      program,
		size_t           num,
		const char**     names,
		GFXFeedbackMode  mode);

/**
 * Links given shaders into a program.
 *
 * @param shaders All shader objects to link into the program (cannot be NULL).
 * @param binary  If non-zero, the binary representation can be fetched afterwards.
 * @return Non-zero on success.
 *
 * Note: this call will attempt to compile all given shaders.
 *
 * Additionally, an error will be generated on failure.
 * This will remove any evidence of a previous link operation.
 *
 */
GFX_API int gfx_program_link(

		GFXProgram*  program,
		size_t       num,
		GFXShader**  shaders,
		int          binary);

/**
 * Retrieves the binary representation of a program.
 *
 * @param format Returns the format of the data (implementation dependent).
 * @param size   Number of bytes of the binary data.
 * @return Binary data, NULL on failure.
 *
 * If the returned pointer is not NULL, it should be freed manually.
 * Note: requires GFX_EXT_PROGRAM_BINARY.
 *
 */
GFX_API void* gfx_program_get_binary(

		GFXProgram*        program,
		GFXProgramFormat*  format,
		size_t*            size);

/**
 * Sets the binary representation of a program.
 *
 * @param data   Data to read.
 * @param format Format of the data (implementation dependent).
 * @param size   Number of bytes to read.
 * @return Non-zero on success.
 *
 * This operation can be considered to perform an implicit linking operation.
 * Note: requires GFX_EXT_PROGRAM_BINARY.
 *
 */
GFX_API int gfx_program_set_binary(

		GFXProgram*       program,
		GFXProgramFormat  format,
		size_t            size,
		const void*       data);

/**
 * Returns a property of the program.
 *
 * @param index Index of the property, must be < program->properties.
 * @return NULL if the property does not exist.
 *
 * Note: properties only exist after the program is linked successfully.
 *
 */
GFX_API const GFXProperty* gfx_program_get_property(

		GFXProgram*     program,
		unsigned short  index);

/**
 * Returns the index of the property with the given uniform name.
 *
 * @param name Name of the property in the shader (the string is copied).
 * @return program->properties on failure, index otherwise.
 *
 */
GFX_API unsigned short gfx_program_get_named_property(

		GFXProgram*  program,
		const char*  name);

/**
 * Returns a property block of the program.
 *
 * @param index Index of the property block, must be < program->blocks.
 * @return NULL if the block does not exist.
 *
 * Note: blocks only exist after the program is linked successfully.
 *
 */
GFX_API const GFXPropertyBlock* gfx_program_get_property_block(

		GFXProgram*     program,
		unsigned short  index);

/**
 * Returns the index of the property block with the given uniform name.
 *
 * @param name Name of the property block in the shader (the string is copied).
 * @return program->blocks on failure, index otherwise.
 *
 */
GFX_API unsigned short gfx_program_get_named_property_block(

		GFXProgram*  program,
		const char*  name);


/********************************************************
 * Program Map (programs mapped to stages)
 *******************************************************/

/** Program map */
typedef struct GFXProgramMap
{
	size_t instances; /* Number of instances that can be drawn at once, 0 for infinite */

} GFXProgramMap;


/**
 * Creates a new program map.
 *
 * @return NULL on failure.
 *
 */
GFX_API GFXProgramMap* gfx_program_map_create(void);

/**
 * Makes sure the program map is freed properly.
 *
 */
GFX_API void gfx_program_map_free(

		GFXProgramMap* map);

/**
 * Creates a new program and maps it to a given shader stage.
 *
 * @param stage     Stage of the program to use for this program map, GFX_ALL_SHADERS is allowed.
 * @param instances Number of instances that can be drawn in a single draw call, 0 for infinite.
 * @return The new program on success, NULL on failure.
 *
 * Note: if GFX_EXT_PROGRAM_MAP is not supported, stage will be ignored and the
 * function will act as if stage was set to 0.
 *
 */
GFX_API GFXProgram* gfx_program_map_add(

		GFXProgramMap*  map,
		GFXShaderStage  stage,
		size_t          instances);

/**
 * Shares a program and maps it to a given shader stage.
 *
 * @param stage Stage of the program to use for this program map, GFX_ALL_SHADERS is allowed.
 * @param share Program to share, or NULL to deactivate the given stage.
 * @return Zero on failure.
 *
 * Note: if GFX_EXT_PROGRAM_MAP is not supported, stage will be ignored and the
 * function will act as if stage was set to 0.
 *
 */
GFX_API int gfx_program_map_add_share(

		GFXProgramMap*  map,
		GFXShaderStage  stage,
		GFXProgram*     share);

/**
 * Returns the program mapped to a given stage.
 *
 * @param stage Stage to retrieve the program of.
 * @return Mapped program, can be NULL.
 *
 */
GFX_API GFXProgram* gfx_program_map_get(

		GFXProgramMap*  map,
		GFXShaderStage  stage);


/********************************************************
 * Property Map (inputs associated with a program)
 *******************************************************/

/** Property map */
typedef struct GFXPropertyMap
{
	GFXProgramMap*  programMap; /* Program map it references (cannot be changed or freed while the map is alive) */
	unsigned char   properties; /* Number of properties */
	unsigned int    copies;     /* Number of property copies present, the initial value is 1 */

} GFXPropertyMap;


/**
 * Creates a new property map.
 *
 * @param programMap Program map to set inputs for.
 * @param properties Fixed number of property indices associated with this map.
 * @return NULL on failure.
 *
 * Note: this will signal the program map to setup its executable pipeline.
 * After this the program map cannot be altered until it is not in use by
 * any property maps anymore.
 *
 */
GFX_API GFXPropertyMap* gfx_property_map_create(

		GFXProgramMap*  programMap,
		unsigned char   properties);

/**
 * Makes sure the property map is freed properly.
 *
 */
GFX_API void gfx_property_map_free(

		GFXPropertyMap* map);

/**
 * Appends more copies to the properties containing copies.
 *
 * @param num Number of copies to append.
 * @return Zero on failure.
 *
 */
GFX_API int gfx_property_map_expand(

		GFXPropertyMap*  map,
		unsigned int     num);

/**
 * Removes multiple copies at the end of the properties containing copies.
 *
 * @param num Number of copies to remove.
 * @return Number of copies actually removed.
 *
 */
GFX_API unsigned int gfx_property_map_shrink(

		GFXPropertyMap*  map,
		unsigned int     num);

/**
 * Moves the values of all properties of a given copy to another copy.
 *
 * @param dest Index of the destination copy.
 * @param src  Index of the source copy (can equal dest).
 * @return Zero if either copy does not exist.
 *
 * The values of the source copies remain unaltered.
 *
 */
GFX_API int gfx_property_map_move(

		GFXPropertyMap*  map,
		unsigned int     dest,
		unsigned int     src);

/**
 * Forwards data send to a given index to a property within a program.
 *
 * @param index    Index to forward to the program (must be < map->properties).
 * @param copies   Non-zero if this property should contain copies of itself.
 * @param ptr      If non-zero, a vector/matrix value will store a pointer instead of the actual value.
 * @param stage    Shader stage within the program map to use as program.
 * @param property Index of the program property to forward to.
 * @return Zero on failure.
 *
 * If the property is not a vector/matrix, ptr is ignored.
 *
 */
GFX_API int gfx_property_map_forward(

		GFXPropertyMap*  map,
		unsigned char    index,
		int              copies,
		int              ptr,
		GFXShaderStage   stage,
		unsigned short   property);

/**
 * Forwards data send to a given index to a given uniform name within the program.
 *
 * @param name Uniform name within the program to forward to.
 * @return Zero on failure.
 *
 */
GFX_API int gfx_property_map_forward_named(

		GFXPropertyMap*  map,
		unsigned char    index,
		int              copies,
		int              ptr,
		GFXShaderStage   stage,
		const char*      name);

/**
 * Forwards data send to a given index to a property block within the program.
 *
 * @param index  Index to forward to the program (must be < map->properties).
 * @param copies Non-zero if this property should contain copies of itself.
 * @param stage    Shader stage within the program map to use as program.
 * @param block  Index of the program property block to forward to.
 * @return Zero on failure.
 *
 */
GFX_API int gfx_property_map_forward_block(

		GFXPropertyMap*  map,
		unsigned char    index,
		int              copies,
		GFXShaderStage   stage,
		unsigned short   block);

/**
 * Forwards data send to a given index to a given uniform block name within the program.
 *
 * @param name Uniform block name within the program to forward to.
 * @return Zero on failure.
 *
 */
GFX_API int gfx_property_map_forward_named_block(

		GFXPropertyMap*  map,
		unsigned char    index,
		int              copies,
		GFXShaderStage   stage,
		const char*      name);

/**
 * Sets the value of a vector/matrix property.
 *
 * @param index  Index of the property to set the value of.
 * @param copy   Index of the copy to set the value of.
 * @param value  Value to set it to, the content will be copied.
 * @param offset Byte offset in the mapped value to write to.
 * @param size   Size of the given data in bytes (0 to calculate the size based on the property type).
 * @return Zero on failure or if a pointer value.
 *
 */
GFX_API int gfx_property_map_set_value(

		GFXPropertyMap*  map,
		unsigned char    index,
		unsigned int     copy,
		const void*      value,
		size_t           offset,
		size_t           size);

/**
 * Sets the pointer of a vector/matrix property.
 *
 * @param ptr    Pointer to store for this vector/matrix property.
 * @param offset Offset to add to the pointer measured in array elements.
 * @param size   Size of the readable data pointed to by ptr measured in array elements.
 * @return Zero on failure or if not a pointer value.
 *
 * Note: it will copy the amount of bytes to fill the entire property.
 *
 */
GFX_API int gfx_property_map_set_value_pointer(

		GFXPropertyMap*  map,
		unsigned char    index,
		unsigned int     copy,
		const void*      ptr,
		size_t           offset,
		size_t           size);

/**
 * Sets the offset of each instance of a pointer vector/matrix property.
 *
 * @param offset Relative offset of instances measured in array elements.
 * @return Zero if not a pointer vector/matrix value.
 *
 * Note: a base instance will subtract from the number of uploaded instances.
 *
 */
GFX_API int gfx_property_map_set_instance_offset(

		GFXPropertyMap*  map,
		unsigned char    index,
		unsigned int     copy,
		unsigned int     offset);

/**
 * Sets the value of a sampler property.
 *
 * @return Non-zero on success.
 *
 * Note: There can only be GFX_LIM_MAX_SAMPLER_PROPERTIES number of sampler properties forwarded.
 *
 */
GFX_API int gfx_property_map_set_sampler(

		GFXPropertyMap*    map,
		unsigned char      index,
		unsigned int       copy,
		const GFXTexture*  texture);

/**
 * Sets the value of a property block.
 *
 * @param offset Offset in the buffer in bytes.
 * @param size   Size of the data in bytes.
 * @return Non-zero on success.
 *
 * Note: There can only be GFX_LIM_MAX_BUFFER_PROPERTIES number of block properties forwarded.
 *
 */
GFX_API int gfx_property_map_set_buffer(

		GFXPropertyMap*   map,
		unsigned char     index,
		unsigned int      copy,
		const GFXBuffer*  buffer,
		size_t            offset,
		size_t            size);

/**
 * Sets the value of a property block.
 *
 * @param offset Offset in the shared buffer in bytes.
 * @return Non-zero on success.
 *
 */
GFX_API int gfx_property_map_set_shared_buffer(

		GFXPropertyMap*         map,
		unsigned char           index,
		unsigned int            copy,
		const GFXSharedBuffer*  buffer,
		size_t                  offset,
		size_t                  size);


#ifdef __cplusplus
}
#endif

#endif // GFX_CORE_SHADING_H
