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

#ifndef GFX_SHADING_H
#define GFX_SHADING_H

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
 * @param length Array containing the lengths of the sources, NULL or negative for null-terminated.
 * @param src    Array containing the sources to append to each other.
 *
 * Note: This will entirely replace a source set by a previous call to the method.
 * The source strings will be copied.
 *
 */
void gfx_shader_set_source(GFXShader* shader, size_t num, const int* length, const char** src);

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
 * Program (linked shaders to form GPU pipeline)
 *******************************************************/

/** Program Binary Format */
typedef unsigned int GFXProgramFormat;


/** Program */
typedef struct GFXProgram
{
	size_t id; /* Hardware Object ID */

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
 * This method only takes effect once the program is linked.
 * Note: an index can be forwarded to multiple names.
 *
 */
int gfx_program_set_attribute(GFXProgram* program, unsigned int index, const char* name);

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


#ifdef __cplusplus
}
#endif

#endif // GFX_SHADING_H
