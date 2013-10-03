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
	GFXShaderType  type;
	char           compiled;

} GFXShader;


/**
 * Creates a new shader.
 *
 * @return NULL on failure.
 *
 */
GFXShader* gfx_shader_create(GFXShaderType type);

/**
 * Makes sure the shader is freed properly.
 *
 */
void gfx_shader_free(GFXShader* shader);


#ifdef __cplusplus
}
#endif

#endif // GFX_SHADING_H
