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

#ifndef GFX_PLATFORM_WIN32_H
#define GFX_PLATFORM_WIN32_H

#include <wchar.h>

/* Windows 2000 */
#define WINVER 0x0500

/* Unicode */
#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

/* Groufix window class */
#define GFX_WIN32_WND_CLASS L"GROUFIX"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * \brief Win32 Instance
 *******************************************************/
typedef struct GFX_Win32_Instance
{
	/* Monitors */
	unsigned int  numMonitors;
	void**        monitors;       /* (of type HMONITOR*) */

	/* Windows */
	void*         windowClass;    /* (of type WNDCLASSEX*) */
	unsigned int  numWindows;
	void**        windows;        /* (of type HWND*) */
	void**        windowMonitors; /* Monitors assigned to the windows (of type HMONITOR*) */

} GFX_Win32_Instance;

/**
 * \brief Instance pointer
 */
extern GFX_Win32_Instance* _gfx_win32;

/**
 * \brief Converts a UTF-8 string to a wide string.
 *
 * If the returned pointer is not NULL, it should be freed manually.
 *
 */
wchar_t* utf8_to_wchar(const char* str);

/**
 * \brief Converts a wide string to a UTF-8 string.
 *
 * If the returned pointer is not NULL, it should be freed manually.
 *
 */
char* wchar_to_utf8(const wchar_t* str);


#ifdef __cplusplus
}
#endif

#endif // GFX_PLATFORM_WIN32_H
