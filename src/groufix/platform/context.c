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

#define GL_GLEXT_PROTOTYPES
#include "groufix/internal.h"

#include <string.h>

/******************************************************/
/* GLSL version strings */
static const char* _gfx_glsl_versions[] =
{
	"150\0",
	"330\0",
	"400\0",
	"410\0",
	"420\0",
	"430\0",
	"440\0"
};

/******************************************************/
void _gfx_platform_context_get(int* major, int* minor)
{
	GLint ma, mi;
	glGetIntegerv(GL_MAJOR_VERSION, &ma);
	glGetIntegerv(GL_MINOR_VERSION, &mi);

	*major = ma;
	*minor = mi;
}

/******************************************************/
const char* _gfx_platform_context_get_glsl(int major, int minor)
{
	switch(major)
	{
		case 3 : switch(minor)
		{
			case 2 : return _gfx_glsl_versions[0];
			case 3 : return _gfx_glsl_versions[1];
			default : return NULL;
		}
		case 4 : switch(minor)
		{
			case 0 : return _gfx_glsl_versions[2];
			case 1 : return _gfx_glsl_versions[3];
			case 2 : return _gfx_glsl_versions[4];
			case 3 : return _gfx_glsl_versions[5];
			case 4 : return _gfx_glsl_versions[6];
			default : return NULL;
		}

		/* Good luck */
		default : return NULL;
	}
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
		if(*end == ' ' || *end == '\0')
		{
			/* To avoid segfault */
			if(found == str) return 1;
			if(*(found - 1) == ' ') return 1;
		}
		found = strstr(end, ext);
	}

	return 0;
}
