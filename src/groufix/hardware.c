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

#include "groufix/internal.h"
#include "groufix/errors.h"

/******************************************************/
GFXHardwareContext gfx_hardware_get_context(void)
{
	GFX_Internal_Window* wind = _gfx_window_get_current();
	if(!wind) return NULL;

	return (GFXHardwareContext)&wind->extensions;
}

/******************************************************/
int gfx_hardware_is_extension_supported(GFXHardwareExtension extension, const GFXHardwareContext cnt)
{
	return VOID_TO_EXT(cnt)->flags[extension];
}

/******************************************************/
unsigned int gfx_hardware_poll_errors(const char* description, const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	unsigned int count = 0;

	/* Loop over all errors */
	GLenum err = ext->GetError();
	while(err != GL_NO_ERROR)
	{
		gfx_errors_push(err, description);
		err = ext->GetError();

		++count;
	}

	return count;
}
