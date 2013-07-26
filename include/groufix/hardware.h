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


/** \brief Hardware Handle */
typedef unsigned int GFXHardwareHandle;


/**
 * \brief Get the currently active context.
 *
 * May return NULL in case no window is internally current.
 * If it returned a non NULL pointer, it should be used as context to call other hardware methods.
 *
 */
GFXHardwareContext gfx_hardware_get_context(void);

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

/** \brief Buffer handle */
typedef GFXHardwareHandle GFXHardwareBuffer;


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


/**
 * \brief Creates a new hardware buffer.
 *
 * \param target Target type this buffer is intended to use for.
 * \return NULL on failure.
 *
 * Also binds the buffer to the given target.
 *
 */
GFXHardwareBuffer gfx_hardware_buffer_create(GFXBufferTarget target, const GFXHardwareContext cnt);

/**
 * \brief Makes sure the buffer is freed properly.
 *
 */
void gfx_hardware_buffer_free(GFXHardwareBuffer buffer, const GFXHardwareContext cnt);

/**
 * \brief Binds a buffer to the target.
 *
 * \param buffer Buffer to bind, 0 to unbind the current buffer.
 *
 */
void gfx_hardware_buffer_bind(GFXBufferTarget target, GFXHardwareBuffer buffer, const GFXHardwareContext cnt);

/**
 * \brief Get the buffer bound to the given target.
 *
 * \return 0 if no buffer is bound.
 *
 */
GFXHardwareBuffer gfx_hardware_buffer_get(GFXBufferTarget target, const GFXHardwareContext cnt);

/**
 * \brief Returns the size of the buffer bound to the given target.
 *
 */
size_t gfx_hardware_buffer_get_size(GFXBufferTarget target, const GFXHardwareContext cnt);

/**
 * \brief Returns the usage of the buffer to the given target.
 *
 */
GFXBufferUsage gfx_hardware_buffer_get_usage(GFXBufferTarget target, const GFXHardwareContext cnt);

/**
 * \brief (Re)allocates the buffer data bound to the given target.
 *
 * \param data Data to fill the buffer with, can be NULL.
 *
 */
void gfx_hardware_buffer_allocate(GFXBufferTarget target, size_t size, const void* data, GFXBufferUsage use, const GFXHardwareContext cnt);

/**
 * \brief Writes data to the buffer bound to the given target.
 *
 * \return Number of bytes written.
 *
 */
size_t gfx_hardware_buffer_write(GFXBufferTarget target, size_t offset, size_t size, const void* data, const GFXHardwareContext cnt);

/**
 * \brief Reads data from the buffer bound to the given target.
 *
 * \return Number of bytes read.
 *
 */
size_t gfx_hardware_buffer_read(GFXBufferTarget target, size_t offset, size_t size, void* data, const GFXHardwareContext cnt);

/**
 * \brief Maps the buffer bound to the given target, meaning it can be accessed as a regular array.
 *
 * \param access Bitfield specifying the permitted access methods.
 * \param length Lenth to be mapped, the actual mapped length is written to it as well (cannot be NULL!).
 * \return A pointer to the accessible data (NULL on errors).
 *
 * When accessing the buffer is done, it must be unmapped to allow it to be accessed regularly.
 *
 */
void* gfx_hardware_buffer_map(GFXBufferTarget target, size_t offset, size_t* length, GFXBufferAccess access, const GFXHardwareContext cnt);

/**
 * \brief Returns the mapped pointer.
 *
 * \return NULL if it doesn't exist.
 *
 */
void* gfx_hardware_buffer_get_map(GFXBufferTarget target, const GFXHardwareContext cnt);

/**
 * \brief Disables the mapped pointer, if any.
 *
 * This must be called when accessing the buffer is done.
 *
 */
void gfx_hardware_buffer_unmap(GFXBufferTarget target, const GFXHardwareContext cnt);


/********************************************************
 * Object (vertex specification)
 *******************************************************/

/** \brief Object handle */
typedef GFXHardwareHandle GFXHardwareObject;


/** \brief Storage types */
typedef unsigned int GFXDataType;

#define GFX_BYTE              0x1400
#define GFX_UNSIGNED_BYTE     0x1401
#define GFX_SHORT             0x1402
#define GFX_UNSIGNED_SHORT    0x1403
#define GFX_INT               0x1404
#define GFX_UNSIGNED_INT      0x1405
#define GFX_FLOAT             0x1406
#define GFX_HALF_FLOAT        0x140b


/** \brief Interpetred data type */
typedef unsigned int GFXInterpretType;

#define GFX_OBJECT_FLOAT      0x00
#define GFX_OBJECT_NORMALIZE  0x01
#define GFX_OBJECT_INTEGER    0x02


/**
 * \brief Creates a new hardware object.
 *
 * \return NULL on failure.
 *
 * Also binds the object.
 *
 */
GFXHardwareObject gfx_hardware_object_create(const GFXHardwareContext cnt);

/**
 * \brief Makes sure the object is freed properly.
 *
 */
void gfx_hardware_object_free(GFXHardwareObject object, const GFXHardwareContext cnt);

/**
 * \brief Binds the object as active render object.
 *
 * \param object The object to be bind, 0 to unbind the currently bound object.
 *
 */
void gfx_hardware_object_bind(GFXHardwareObject object, const GFXHardwareContext cnt);

/**
 * \brief Enables a vertex attribute of the currently bound object.
 *
 * \return Non-zero if it could enable the attribute.
 *
 * When an attribute is enabled, it will read said attribute from a buffer.
 * If not, it will read a constant value.
 *
 */
int gfx_hardware_object_enable_attribute(unsigned int index, const GFXHardwareContext cnt);

/**
 * \brief Disables a vertex attribute of the currently bound object.
 *
 * \return Non-zero if it could disable the attribute.
 *
 */
int gfx_hardware_object_disable_attribute(unsigned int index, const GFXHardwareContext cnt);

/**
 * \brief Defines a vertex attribute.
 *
 * \param size   Number of components of the attribute (must be between 1 and 4).
 * \param type   Data type of each stored component.
 * \param intr   How the components should be interpreted.
 * \param stride Byte offset between consecutive attributes.
 * \param offset Byte offset of the first occurence of the attribute.
 * \return Non-zero if it could define the attribute.
 *
 * The buffer currently bound to the VERTEX_ARRAY target will be used for this attribute.
 *
 */
int gfx_hardware_object_set_attribute(unsigned int index, size_t size, GFXDataType type, GFXInterpretType intr, size_t stride, size_t offset, const GFXHardwareContext cnt);


#ifdef __cplusplus
}
#endif

#endif // GFX_HARDWARE_H
