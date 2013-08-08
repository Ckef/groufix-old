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
unsigned int gfx_hardware_poll_errors(const char* description);


/********************************************************
 * Generic Hardware Object
 *******************************************************/

/** \brief Generic hardware object */
typedef void* GFXHardwareObject;

typedef void (*GFXHardwareFreeFunc)    (GFXHardwareObject, const GFXHardwareContext);
typedef void* (*GFXHardwareSaveFunc)   (GFXHardwareObject, const GFXHardwareContext);
typedef void (*GFXHardwareRestoreFunc) (GFXHardwareObject, void*, const GFXHardwareContext);


/** \brief Hardware vtable */
typedef struct GFXHardwareFuncs
{
	GFXHardwareFreeFunc     free;    /* Request free */
	GFXHardwareSaveFunc     save;    /* Store data in client side buffer, returns arbitrary address (NULL to not restore) */
	GFXHardwareRestoreFunc  restore; /* Restore data from client side buffer */

} GFXHardwareFuncs;


/**
 * \brief Registers a new generic hardware object for global operations.
 *
 * \return Non-zero on success.
 *
 */
int gfx_hardware_object_register(GFXHardwareObject object, const GFXHardwareFuncs* funcs);

/**
 * \brief Makes sure the hardware object is freed properly.
 *
 */
void gfx_hardware_object_unregister(GFXHardwareObject object);


#ifdef __cplusplus
}
#endif

#endif // GFX_HARDWARE_H
