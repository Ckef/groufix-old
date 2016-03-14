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

#ifndef GFX_CORE_THREADING_H
#define GFX_CORE_THREADING_H

#include "groufix/core/platform.h"

/* Required threading headers */
#if defined(GFX_WIN32)
	#include <process.h>
#elif defined(GFX_UNIX)
	#include <pthread.h>
#endif


/********************************************************
 * Platform thread definitions
 *******************************************************/

/** Thread Address */
typedef unsigned int (*GFX_ThreadAddress)(void*);


/** A Thread */
#if defined(GFX_WIN32)
typedef HANDLE GFX_PlatformThread;

#elif defined(GFX_UNIX)
typedef pthread_t GFX_PlatformThread;

#else
typedef void *GFX_PlatformThread;

#endif


/** Thread local data key */
#if defined(GFX_WIN32)
typedef DWORD GFX_PlatformKey;

#elif defined(GFX_UNIX)
typedef pthread_key_t GFX_PlatformKey;

#else
typedef void *GFX_PlatformKey;

#endif


/** A Mutex */
#if defined(GFX_WIN32)
typedef CRITICAL_SECTION GFX_PlatformMutex;

#elif defined(GFX_UNIX)
typedef pthread_mutex_t GFX_PlatformMutex;

#else
typedef void *GFX_PlatformMutex;

#endif


/** A Condition Variable */
#if defined(GFX_WIN32)
typedef CONDITION_VARIABLE GFX_PlatformCond;

#elif defined(GFX_UNIX)
typedef pthread_cond_t GFX_PlatformCond;

#else
typedef void *GFX_PlatformCond;

#endif


/********************************************************
 * Threading
 *******************************************************/

/**
 * Initializes a new thread.
 *
 * @param thread   Returns the thread handle.
 * @param func     Starting address of the thread, cannot be NULL.
 * @param arg      Argument to give to func.
 * @param joinable If zero, the thread cannot be joined.
 * @return Zero on failure.
 *
 */
int _gfx_platform_thread_init(

		GFX_PlatformThread  *thread,
		GFX_ThreadAddress    func,
		void                *arg,
		int                  joinable);

/**
 * Wait for a thread to terminate and frees all resources associated with it.
 *
 * @param ret Value the thread has returned (can be NULL).
 * @return Zero on failure.
 *
 * Note: if a thread was created to be joinable, this call must be made to free resources.
 *
 */
int _gfx_platform_thread_join(

		GFX_PlatformThread  thread,
		unsigned int       *ret);

/**
 * Detaches a thread, making it unjoinable.
 *
 */
static GFX_ALWAYS_INLINE void _gfx_platform_thread_detach(

		GFX_PlatformThread thread)
{
#if defined(GFX_WIN32)

	CloseHandle(thread);

#elif defined(GFX_UNIX)

	pthread_detach(thread);

#endif
}

/**
 * Exits the calling thread.
 *
 * @param ret Return value of the thread.
 *
 */
static GFX_ALWAYS_INLINE void _gfx_platform_thread_exit(

		unsigned int ret)
{
#if defined(GFX_WIN32)

	_endthreadex(ret);

#elif defined(GFX_UNIX)

	pthread_exit(GFX_UINT_TO_VOID(ret));

#endif
}

/**
 * Initializes a new thread local data key.
 *
 * @param key Returns the key object.
 * @return Zero on failure.
 *
 */
static GFX_ALWAYS_INLINE int _gfx_platform_key_init(

		GFX_PlatformKey *key)
{
#if defined(GFX_WIN32)

	*key = TlsAlloc();
	return *key != TLS_OUT_OF_INDEXES;

#elif defined(GFX_UNIX)

	return !pthread_key_create(key, NULL);

#endif
}

/**
 * Makes sure the data key is freed properly.
 *
 * Note: this does not free any of the associated values!
 *
 */
static GFX_ALWAYS_INLINE void _gfx_platform_key_clear(

		GFX_PlatformKey key)
{
#if defined(GFX_WIN32)

	TlsFree(key);

#elif defined(GFX_UNIX)

	pthread_key_delete(key);

#endif
}

/**
 * Associate a thread specific value with a data key.
 *
 * @return Zero on failure.
 *
 */
static GFX_ALWAYS_INLINE int _gfx_platform_key_set(

		GFX_PlatformKey  key,
		void            *value)
{
#if defined(GFX_WIN32)

	return TlsSetValue(key, value);

#elif defined(GFX_UNIX)

	return !pthread_setspecific(key, value);

#endif
}

/**
 * Retrieve the thread specific value associated with a data key.
 *
 * @return The stored value, NULL if no value is associated.
 *
 */
static GFX_ALWAYS_INLINE void* _gfx_platform_key_get(

		GFX_PlatformKey key)
{
#if defined(GFX_WIN32)

	return TlsGetValue(key);

#elif defined(GFX_UNIX)

	return pthread_getspecific(key);

#endif
}

/**
 * Initializes a new mutex.
 *
 * @param mutex Returns the mutex object.
 * @return Zero on failure.
 *
 * Note: NEVER copy the initialized mutex, the same pointer must always be used!
 *
 */
static GFX_ALWAYS_INLINE int _gfx_platform_mutex_init(

		GFX_PlatformMutex *mutex)
{
#if defined(GFX_WIN32)

	InitializeCriticalSection(mutex);
	return 1;

#elif defined(GFX_UNIX)

	return !pthread_mutex_init(mutex, NULL);

#endif
}

/**
 * Makes sure a mutex is freed properly.
 *
 * Note: Clearing a locked mutex results in undefined behaviour.
 *
 */
static GFX_ALWAYS_INLINE void _gfx_platform_mutex_clear(

		GFX_PlatformMutex *mutex)
{
#if defined(GFX_WIN32)

	DeleteCriticalSection(mutex);

#elif defined(GFX_UNIX)

	pthread_mutex_destroy(mutex);

#endif
}

/**
 * Blocks until the calling thread is granted ownership of the mutex.
 *
 * Note: locking a mutex you already own results in undefined behaviour.
 *
 */
static GFX_ALWAYS_INLINE void _gfx_platform_mutex_lock(

		GFX_PlatformMutex *mutex)
{
#if defined(GFX_WIN32)

	EnterCriticalSection(mutex);

#elif defined(GFX_UNIX)

	pthread_mutex_lock(mutex);

#endif
}

/**
 * Try to get ownership of the mutex, but do not block.
 *
 * @return Non-zero if ownership was granted.
 *
 */
static GFX_ALWAYS_INLINE int _gfx_platform_mutex_try_lock(

		GFX_PlatformMutex *mutex)
{
#if defined(GFX_WIN32)

	return TryEnterCriticalSection(mutex);

#elif defined(GFX_UNIX)

	return !pthread_mutex_trylock(mutex);

#endif
}

/**
 * Releases the mutex, making it available to other threads.
 *
 * Note: unlocking a mutex which was not locked results in undefined behaviour.
 *
 */
static GFX_ALWAYS_INLINE void _gfx_platform_mutex_unlock(

		GFX_PlatformMutex *mutex)
{
#if defined(GFX_WIN32)

	LeaveCriticalSection(mutex);

#elif defined(GFX_UNIX)

	pthread_mutex_unlock(mutex);

#endif
}

/**
 * Initializes a new condition variable.
 *
 * @param cond Returns the condition object.
 * @return Zero on failure.
 *
 * Note: NEVER copy the initialized condition, the same pointer must always be used!
 *
 */
static GFX_ALWAYS_INLINE int _gfx_platform_cond_init(

		GFX_PlatformCond *cond)
{
#if defined(GFX_WIN32)

	InitializeConditionVariable(cond);
	return 1;

#elif defined(GFX_UNIX)

	return !pthread_cond_init(cond, NULL);

#endif
}

/**
 * Makes sure a condition variable is freed properly.
 *
 * Note: Clearing a condition upon which threads are waiting results in undefined behaviour.
 *
 */
static GFX_ALWAYS_INLINE void _gfx_platform_cond_clear(

		GFX_PlatformCond *cond)
{
#if defined(GFX_WIN32)

	/* No-op on windows */

#elif defined(GFX_UNIX)

	pthread_cond_destroy(cond);

#endif
}

/**
 * Releases the mutex and blocks until the condition variable is signalled.
 *
 * @return Zero on failure (no blocking occurred), the mutex shall be locked when this call returns.
 *
 * Note: waiting with an unlocked mutex or with different mutexes is undefined behaviour.
 *
 */
static GFX_ALWAYS_INLINE int _gfx_platform_cond_wait(

		GFX_PlatformCond   *cond,
		GFX_PlatformMutex  *mutex)
{
#if defined(GFX_WIN32)

	return SleepConditionVariableCS(cond, mutex, INFINITE);

#elif defined(GFX_UNIX)

	return !pthread_cond_wait(cond, mutex);

#endif
}

/**
 * Behaves like _gfx_platform_cond_wait, except it returns when a nsec nanoseconds have passed.
 *
 * @return Negative if a minimum of nsec nanoseconds have passed, otherwise equivalent to cond_wait.
 *
 * Note: the time might not be as accurate on all implementations.
 *
 */
int _gfx_platform_cond_wait_time(

		GFX_PlatformCond   *cond,
		GFX_PlatformMutex  *mutex,
		uint64_t            nsec);

/**
 * Unblocks at least one of the threads waiting for the condition variable.
 *
 * If no threads are waiting, the call simply returns.
 *
 */
static GFX_ALWAYS_INLINE void _gfx_platform_cond_signal(

		GFX_PlatformCond *cond)
{
#if defined(GFX_WIN32)

	WakeConditionVariable(cond);

#elif defined(GFX_UNIX)

	pthread_cond_signal(cond);

#endif
}

/**
 * Unblocks all threads waiting for the condition variable.
 *
 * If no threads are waiting, the call simply returns.
 *
 */
static GFX_ALWAYS_INLINE void _gfx_platform_cond_broadcast(

		GFX_PlatformCond *cond)
{
#if defined(GFX_WIN32)

	WakeAllConditionVariable(cond);

#elif defined(GFX_UNIX)

	pthread_cond_broadcast(cond);

#endif
}


#endif // GFX_CORE_THREADING_H
