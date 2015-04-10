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

	if(!oflags)
		return 0;

	/* Check flag compatibility */
	if((flags & GFX_RESOURCE_TRUNCATE) && !(flags & GFX_RESOURCE_WRITE))
		return 0;

	if((flags & GFX_RESOURCE_EXIST) && !(flags & GFX_RESOURCE_CREATE))
		return 0;

	/* Open file */
	*file = open(path, oflags |
		(flags & GFX_RESOURCE_APPEND ? O_APPEND : 0) |
		(flags & GFX_RESOURCE_TRUNCATE ? O_TRUNC : 0) |
		(flags & GFX_RESOURCE_CREATE ? O_CREAT : 0) |
		(flags & GFX_RESOURCE_EXIST ? O_EXCL : 0),
		S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
	);

	return *file != -1;
}
