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

#include <stdlib.h>

/******************************************************/
GFXHardwareProgram* gfx_hardware_program_create(const GFXHardwareContext cnt)
{
	const GFX_Extensions* ext = VOID_TO_EXT(cnt);

	/* Allocate */
	GFXHardwareProgram* program = (GFXHardwareProgram*)malloc(sizeof(GFXHardwareProgram));
	if(!program) return NULL;

	program->handle = ext->CreateProgram();

	return program;
}

/******************************************************/
void gfx_hardware_program_free(GFXHardwareProgram* program, const GFXHardwareContext cnt)
{
	if(program)
	{
		const GFX_Extensions* ext = VOID_TO_EXT(cnt);

		ext->DeleteProgram(program->handle);
		free(program);
	}
}
