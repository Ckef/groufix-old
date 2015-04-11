/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#include "groufix/core/file.h"

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

/******************************************************/
int _gfx_platform_file_open(

		GFX_PlatformFile*  file,
		const char*        path,
		GFXResourceFlags   flags)
{
	/* Validate access method */
	int oflags =
		flags & GFX_RESOURCE_READ ?
		(flags & GFX_RESOURCE_WRITE ? O_RDWR : O_RDONLY) :
		(flags & GFX_RESOURCE_WRITE ? O_WRONLY : 0);

	if(
		!oflags ||
		((flags & GFX_RESOURCE_EXIST) && !(flags & GFX_RESOURCE_CREATE)) ||
		((flags & GFX_RESOURCE_TRUNCATE) && !(flags & GFX_RESOURCE_WRITE)))
	{
		return 0;
	}

	/* Open file */
	*file = open(path, oflags |
		(flags & GFX_RESOURCE_APPEND ? O_APPEND : 0) |
		(flags & GFX_RESOURCE_TRUNCATE ? O_TRUNC : 0) |
		(flags & GFX_RESOURCE_CREATE ? O_CREAT : 0) |
		(flags & GFX_RESOURCE_EXIST ? O_EXCL : 0),
		S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
	);

	/* Check if actually a file */
	struct stat sb;
	if(fstat(*file, &sb) == -1 || (sb.st_mode & S_IFMT) != S_IFREG)
	{
		close(*file);
		return 0;
	}

	return 1;
}

/******************************************************/
int _gfx_platform_file_move(

		const char*  oldPath,
		const char*  newPath)
{
	/* Check if it is in fact a file */
	struct stat sb;
	if(stat(oldPath, &sb) == -1 || (sb.st_mode & S_IFMT) != S_IFREG)
		return 0;

	return rename(oldPath, newPath) != -1;
}

/******************************************************/
int _gfx_platform_file_remove(

		const char* path)
{
	return unlink(path) != -1;
}
