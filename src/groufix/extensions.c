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

#define GL_GLEXT_PROTOTYPES
#include "groufix/platform.h"
#include "groufix/errors.h"
#include "groufix/hardware.h"

#include <string.h>

/******************************************************/
GFXHardwareContext gfx_hardware_get_context(void)
{
	GFX_Internal_Window* wind = _gfx_window_get_current();
	if(!wind) return NULL;

	return (GFXHardwareContext)&wind->extensions;
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


#ifdef GFX_GLES

/******************************************************/
static void _gfx_gles_get_buffer_sub_data(GLenum target, GLintptr offset, GLsizeiptr size, GLvoid* data)
{
	void* map = glMapBufferRange(target, offset, size, GL_MAP_READ_BIT);

	if(map)
	{
		memcpy(data, map, size);

		if(!glUnmapBuffer(target)) gfx_errors_push(
			GFX_ERROR_MEMORY_CORRUPTION,
			"Reading from a buffer might have corrupted its memory."
		);
	}
}

#endif


/******************************************************/
void _gfx_extensions_load(GFX_Extensions* ext)
{

#ifdef GFX_GLES

	/* GLES */
	ext->BindBuffer           = glBindBuffer;
	ext->BufferData           = glBufferData;
	ext->BufferSubData        = glBufferSubData;
	ext->DeleteBuffers        = glDeleteBuffers;
	ext->GenBuffers           = glGenBuffers;
	ext->GetBufferParameteriv = glGetBufferParameteriv;
	ext->GetBufferPointerv    = glGetBufferPointerv;
	ext->GetBufferSubData     = _gfx_gles_get_buffer_sub_data;
	ext->MapBufferRange       = glMapBufferRange;
	ext->UnmapBuffer          = glUnmapBuffer;

#else

	/* Core */
	ext->BindBuffer           = (GFX_BINDBUFFERPROC)_gfx_platform_get_proc_address("glBindBuffer");
	ext->BufferData           = (GFX_BUFFERDATAPROC)_gfx_platform_get_proc_address("glBufferData");
	ext->BufferSubData        = (GFX_BUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glBufferSubData");
	ext->DeleteBuffers        = (GFX_DELETEBUFFERSPROC)_gfx_platform_get_proc_address("glDeleteBuffers");
	ext->GenBuffers           = (GFX_GENBUFFERSPROC)_gfx_platform_get_proc_address("glGenBuffers");
	ext->GetBufferParameteriv = (GFX_GETBUFFERPARAMETERIVPROC)_gfx_platform_get_proc_address("glGetBufferParameteriv");
	ext->GetBufferPointerv    = (GFX_GETBUFFERPOINTERVPROC)_gfx_platform_get_proc_address("glGetBufferPointerv");
	ext->GetBufferSubData     = (GFX_GETBUFFERSUBDATAPROC)_gfx_platform_get_proc_address("glGetBufferSubData");
	ext->MapBufferRange       = (GFX_MAPBUFFERRANGEPROC)_gfx_platform_get_proc_address("glMapBufferRange");
	ext->UnmapBuffer          = (GFX_UNMAPBUFFERPROC)_gfx_platform_get_proc_address("glUnmapBuffer");

#endif

	/* Same everywhere */
	ext->GetError             = glGetError;
	ext->GetIntegerv          = glGetIntegerv;
}

/******************************************************/
int _gfx_extensions_is_in_string(const char* str, const char* ext)
{
	/* Get extension length */
	size_t len = strlen(ext);
	if(!len) return 0;

	/* Try to find a complete match */
	char* found = strstr(str, ext);
	while(found)
	{
		char* end = found + len;
		if((found == str || *(found - 1) == ' ') && (*end == ' ' || *end == '\0'))
			return 1;

		found = strstr(end, ext);
	}

	return 0;
}
