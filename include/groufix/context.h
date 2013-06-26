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

#ifndef GFX_PLATFORM_CONTEXT_H
#define GFX_PLATFORM_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Returns whether the extension can be found in the space seperated string.
 *
 * Helper function for platforms.
 *
 */
int _gfx_context_is_extension_in_string(const char* str, const char* ext);


#ifdef __cplusplus
}
#endif

#endif // GFX_PLATFORM_CONTEXT_H
