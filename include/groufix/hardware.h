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


#ifdef __cplusplus
}
#endif

#endif // GFX_HARDWARE_H
