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

#ifndef GFX_CORE_FILE_H
#define GFX_CORE_FILE_H

#include "groufix/core/platform.h"
#include "groufix/resources.h"

/* Required threading headers */
#if defined(GFX_UNIX)
	#include <unistd.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Platform file definitions
 *******************************************************/

/** A File */
#if defined(GFX_WIN32)
typedef HANDLE GFX_PlatformFile;

#elif defined(GFX_UNIX)
typedef int GFX_PlatformFile;

#else
typedef void* GFX_PlatformFile;

#endif


/********************************************************
 * File management
 *******************************************************/

/**
 * Opens a new file.
 *
 * @param file  Returns the file handle.
 * @param path  The path to the file, cannot be NULL.
 * @param flags Flags to open the file with.
 * @return Zero on failure.
 *
 * A path uses / as directory separator, . as current directory
 * and .. as parent directory. Any path starting with / is considered
 * an absolute path, if not it is considered a relative (to the
 * working directory) path.
 *
 */
int _gfx_platform_file_open(

		GFX_PlatformFile*  file,
		const char*        path,
		GFXResourceFlags   flags);

/**
 * Closes a file, freeing associated resources.
 *
 */
static GFX_ALWAYS_INLINE void _gfx_platform_file_close(

		GFX_PlatformFile file)
{
#if defined(GFX_WIN32)

	CloseHandle(file);

#elif defined(GFX_UNIX)

	close(file);

#endif
}


#ifdef __cplusplus
}
#endif

#endif // GFX_CORE_FILE_H
