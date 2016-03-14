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

#include "groufix/core/file.h"
#include "groufix/core/platform/win32.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
static WCHAR* _gfx_win32_get_path(

		const char *path)
{
	/* Replace '/' with '\' */
	size_t len = strlen(path) + 1;

	char repl[len];
	while(len--) repl[len] = path[len] == '/' ? '\\' : path[len];

	/* Get wide char path */
	return _gfx_win32_utf8_to_utf16(repl);
}

/******************************************************/
int _gfx_platform_file_open(

		GFX_PlatformFile  *file,
		const char        *path,
		GFXResourceFlags   flags)
{
	/* Validate access method */
	DWORD access =
		(flags & GFX_RESOURCE_READ ? GENERIC_READ : 0) |
		(flags & GFX_RESOURCE_WRITE ? GENERIC_WRITE : 0);

	DWORD share =
		!(flags & GFX_RESOURCE_WRITE) ? FILE_SHARE_READ : 0;

	DWORD creation =
		flags & GFX_RESOURCE_CREATE ?
		(flags & GFX_RESOURCE_EXIST ? CREATE_NEW :
		(flags & GFX_RESOURCE_TRUNCATE ? CREATE_ALWAYS : OPEN_ALWAYS)) :
		(flags & GFX_RESOURCE_EXIST ? 0 :
		(flags & GFX_RESOURCE_TRUNCATE ? TRUNCATE_EXISTING : OPEN_EXISTING));

	if(
		!access || !creation ||
		((flags & GFX_RESOURCE_TRUNCATE) && !(flags & GFX_RESOURCE_WRITE)))
	{
		return 0;
	}

	/* Get path string to use */
	WCHAR *wpath = _gfx_win32_get_path(path);
	if(!wpath) return 0;

	/* Create file */
	*file = CreateFile(
		wpath,
		access,
		share,
		NULL,
		creation,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	free(wpath);

	/* Move file pointer if appending */
	if(flags & GFX_RESOURCE_APPEND)
		SetFilePointer(*file, 0, NULL, FILE_END);

	return *file != INVALID_HANDLE_VALUE;
}

/******************************************************/
int _gfx_platform_file_move(

		const char  *oldPath,
		const char  *newPath)
{
	/* Get paths string to use */
	WCHAR *wold = _gfx_win32_get_path(oldPath);
	WCHAR *wnew = _gfx_win32_get_path(newPath);

	if(!wold || !wnew)
	{
		free(wold);
		free(wnew);
		return 0;
	}

	BOOL ret = MoveFileEx(wold, wnew, MOVEFILE_REPLACE_EXISTING);

	free(wold);
	free(wnew);

	return ret != 0;
}

/******************************************************/
int _gfx_platform_file_remove(

		const char *path)
{
	/* Get path string to use */
	WCHAR *wpath = _gfx_win32_get_path(path);
	if(!wpath) return 0;

	BOOL ret = DeleteFile(wpath);
	free(wpath);

	return ret != 0;
}
