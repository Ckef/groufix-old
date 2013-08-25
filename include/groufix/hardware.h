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

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Hardware Context & Extension handling
 *******************************************************/

/** Hardware Extensions */
typedef enum GFXExtension
{
	GFX_EXT_GEOMETRY_SHADER,
	GFX_EXT_INSTANCED_ATTRIBUTES,
	GFX_EXT_PROGRAM_BINARY,
	GFX_EXT_TESSELLATION_SHADER,

	GFX_EXT_COUNT

} GFXExtension;


/**
 * Returns whether a given extension is supported or not.
 *
 */
int gfx_hardware_is_extension_supported(GFXExtension extension);

/**
 * Polls all OpenGL errors at any given time.
 *
 * @param description A description to attach to each error (can be NULL).
 * @return The number of errors encountered.
 * 
 */
unsigned int gfx_hardware_poll_errors(const char* description);


#ifdef __cplusplus
}
#endif

#endif // GFX_HARDWARE_H
