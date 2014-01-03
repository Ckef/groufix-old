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

#ifndef GFX_SHADING_INTERNAL_H
#define GFX_SHADING_INTERNAL_H

#include "groufix/internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Internal unit binding & property map usage
 *******************************************************/

/**
 * Changes reference count for the binder.
 *
 * @param ref Number to increase/decrease reference count with.
 *
 * Note: this call is needed to initialize and clear memory.
 *
 */
void _gfx_binder_reference(int ref);

/**
 * Sets the program handle as currently in use for the given context.
 *
 */
void _gfx_property_map_use(GFXPropertyMap* map, GFX_Extensions* ext);


#ifdef __cplusplus
}
#endif

#endif // GFX_SHADING_INTERNAL_H
