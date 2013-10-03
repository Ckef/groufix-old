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
	GFX_TESS_CONTROL_SHADER  = 0x8e88,
	GFX_TESS_EVAL_SHADER     = 0x8e87,
	GFX_GEOMETRY_SHADER      = 0x8dd9,
	GFX_FRAGMENT_SHADER      = 0x8b30

} GFXShaderType;


/** Shader */
typedef struct GFXShader
{
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


#ifdef __cplusplus
}
#endif

#endif // GFX_SHADING_H
