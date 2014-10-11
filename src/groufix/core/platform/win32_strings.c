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

#include "groufix/core/platform/win32.h"

/******************************************************/
WCHAR* _gfx_win32_utf8_to_utf16(

		const char* str)
{
	/* Get length */
	int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	if(!len) return NULL;

	/* Create string */
	WCHAR* out = malloc(sizeof(WCHAR) * len);

	if(!MultiByteToWideChar(CP_UTF8, 0, str, -1, out, len))
	{
		free(out);
		return NULL;
	}

	return out;
}

/******************************************************/
char* _gfx_win32_utf16_to_utf8(

		const WCHAR* str)
{
	/* Get length */
	int len = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
	if(!len) return NULL;

	char* out = malloc(len);

	if(!WideCharToMultiByte(CP_UTF8, 0, str, -1, out, len, NULL, NULL))
	{
		free(out);
		return NULL;
	}

	return out;
}
