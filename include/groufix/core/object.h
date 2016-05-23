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

#ifndef GFX_CORE_OBJECT_H
#define GFX_CORE_OBJECT_H


/********************************************************
 * Core renderer object
 *******************************************************/

/** Flags associated with a render object */
typedef enum GFXRenderObjectFlags
{
	GFX_OBJECT_NEEDS_REFERENCE  = 0x01, /* If set, the object must be created on a groufix enabled thread. */
	GFX_OBJECT_CAN_SHARE        = 0x02  /* If set, the object can be shared amongst groufix enabled threads. */

} GFXRenderObjectFlags;


/** A render object reference */
/* TODO: In the future this will hold a reference to the associated GPU */
typedef void* GFXRenderObject;


#endif // GFX_CORE_OBJECT_H
