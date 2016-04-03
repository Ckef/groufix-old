/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#ifndef GFX_RESOURCES_H
#define GFX_RESOURCES_H


/********************************************************
 * Resource definitions
 *******************************************************/

/** Resource flags */
typedef enum GFXResourceFlags
{
	GFX_RESOURCE_READ      = 0x001,
	GFX_RESOURCE_WRITE     = 0x002,
	GFX_RESOURCE_APPEND    = 0x004,
	GFX_RESOURCE_TRUNCATE  = 0x008, /* Fail if no GFX_RESOURCE_WRITE */

	GFX_RESOURCE_CREATE    = 0x010, /* Create new if not exists */
	GFX_RESOURCE_EXIST     = 0x020  /* Fail if already exists */

} GFXResourceFlags;


#endif // GFX_RESOURCES_H
