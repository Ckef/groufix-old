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

#ifndef GFX_CORE_FILE_H
#define GFX_CORE_FILE_H

#include "groufix/core/platform.h"
#include "groufix/core/resources.h"

/* Required headers */
#if defined(GFX_UNIX)
	#include <unistd.h>
	#include <sys/stat.h>
#endif

/* Relative seeking positions */
#if defined(GFX_UNIX)
	#define GFX_FILE_BEG  SEEK_SET
	#define GFX_FILE_CUR  SEEK_CUR
	#define GFX_FILE_END  SEEK_END
#elif defined(GFX_WIN32)
	#define GFX_FILE_BEG  FILE_BEGIN
	#define GFX_FILE_CUR  FILE_CURRENT
	#define GFX_FILE_END  FILE_END
#endif


/********************************************************
 * Platform file definitions
 *******************************************************/

/** A File */
#if defined(GFX_UNIX)
typedef int GFX_PlatformFile;

#elif defined(GFX_WIN32)
typedef HANDLE GFX_PlatformFile;

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
 * Renames a file.
 *
 * @param oldPath Old path to the file.
 * @param newPath New path to the file.
 * @return Zero on failure.
 *
 * Note: all handles should be closed before it can be moved.
 * If the new path already exists, it attempts to replace it.
 *
 */
int _gfx_platform_file_move(

		const char*  oldPath,
		const char*  newPath);

/**
 * Removes a file permanently.
 *
 * @return Zero on failure.
 *
 * Note: all handles should be closed before it can be removed.
 *
 */
int _gfx_platform_file_remove(

		const char* path);

/**
 * Closes a file, freeing associated resources.
 *
 */
static GFX_ALWAYS_INLINE void _gfx_platform_file_close(

		GFX_PlatformFile file)
{
#if defined(GFX_UNIX)

	close(file);

#elif defined(GFX_WIN32)

	CloseHandle(file);

#endif
}

/**
 * Retrieves the size of a file.
 *
 */
static GFX_ALWAYS_INLINE size_t _gfx_platform_file_get_size(

		GFX_PlatformFile file)
{
#if defined(GFX_UNIX)

	struct stat sb;
	if(fstat(file, &sb) == -1) return 0;

	return sb.st_size;

#elif defined(GFX_WIN32)

	LARGE_INTEGER val;
	BOOL ret = GetFileSizeEx(file, &val);

	return ret ? val.QuadPart : 0;

#endif
}

/**
 * Sets or retrieves the file offset.
 *
 * @param offset Bytes to move the file offset forwards.
 * @param rel    Position to move relative to.
 * @return Current offset relative to the begin of the file (< 0 on failure).
 *
 * Allowed values for rel are GFX_FILE_BEG, GFX_FILE_CUR and GFX_FILE_END.
 *
 */
static GFX_ALWAYS_INLINE intptr_t _gfx_platform_file_seek(

		GFX_PlatformFile  file,
		intptr_t          offset,
		int               rel)
{
#if defined(GFX_UNIX)

	return lseek(file, offset, rel);

#elif defined(GFX_WIN32)

	LARGE_INTEGER val;
	val.QuadPart = offset;

	BOOL ret = SetFilePointerEx(file, val, &val, rel);
	return ret ? val.QuadPart : -1;

#endif
}

/**
 * Retrieves the file offset.
 *
 * @return Current offset relative to the begin of the file (< 0 on failure, for mysterious reasons).
 *
 */
static GFX_ALWAYS_INLINE intptr_t _gfx_platform_file_tell(

		GFX_PlatformFile file)
{
#if defined(GFX_UNIX)

	return lseek(file, 0, SEEK_CUR);

#elif defined(GFX_WIN32)

	LARGE_INTEGER val;
	val.QuadPart = 0;

	BOOL ret = SetFilePointerEx(file, val, &val, FILE_CURRENT);
	return ret ? val.QuadPart : -1;

#endif
}

/**
 * Reads from a file.
 *
 * @param data Buffer to fill with read data.
 * @param num  Number of bytes to read.
 * @return Number of bytes actually read.
 *
 * The file offset is incremented by the return value.
 *
 */
static GFX_ALWAYS_INLINE size_t _gfx_platform_file_read(

		GFX_PlatformFile  file,
		void*             data,
		size_t            num)
{
#if defined(GFX_UNIX)

	ssize_t ret = read(file, data, num);
	return (ret > 0) ? ret : 0;

#elif defined(GFX_WIN32)

	DWORD ret;
	ReadFile(file, data, num, &ret, NULL);

	return ret;

#endif
}

/**
 * Writes to a file.
 *
 * @param data Buffer to read from.
 * @param num  Number of bytes to write.
 * @return Number of bytes actually written.
 *
 * The file offset is incremented by the return value.
 *
 */
static GFX_ALWAYS_INLINE size_t _gfx_platform_file_write(

		GFX_PlatformFile  file,
		const void*       data,
		size_t            num)
{
#if defined(GFX_UNIX)

	ssize_t ret = write(file, data, num);
	return (ret > 0) ? ret : 0;

#elif defined(GFX_WIN32)

	DWORD ret;
	WriteFile(file, data, num, &ret, NULL);

	return ret;

#endif
}


#endif // GFX_CORE_FILE_H
