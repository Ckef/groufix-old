# Groufix

## Introduction

Groufix is a cross platform hardware accelerated 2D/3D graphics engine built in C using OpenGL. The library has no external dependencies besides native windowing APIs and OpenGL to access the GPU. Desktop OpenGL is supported from 3.2 and up. OpenGL ES is supported from 3.0 and up.

Supported targets and their APIs _(windowing, OGL extension and/or OGL version)_:

* __Unix__,    X11 (Xlib), GLX, OGL _(working)_
* __Windows__, Win32 (Windows XP and up), WGL, OGL _(working)_
* __OS X__,    Cocoa, CGL, OGL _(planned)_
* __Android__, NDK, OGL ES _(considered)_

The main repository is hosted on [GitHub](https://github.com/Ckef/Groufix).

## Building

A Makefile is shipped with the project, run make without a target to view all build targets. All necessary OpenGL headers are shipped with the project as well. Platform files (such as windows.h or xlib.h) are not shipped as they should be made available by the platform itself. Once the library is built, link against it using `-lGroufix`.

To compile Groufix, you need the standard headers for your platform and OpenGL. These headers should come with the appropriate GPU drivers. When developing for OGL ES, the library should be built specifically for it, `GFX_GLES` should be defined by the compiler to compile the correct extension loading mechanism (in most cases this is done automatically through the build target).

## Usage

Once Groufix is built, it can be used in your code with `#include <groufix.h>`. All _core_ functionality will be made available through that file. Make sure the include directory in this repository is listed as a directory to search for header files. This directory contains all public header files necessary to use the library. Before using the engine, it should be intialized with a call to `gfx_init`. After being done with the engine, it should be terminated with a call to `gfx_terminate`.

All names starting with `gfx`, `_gfx` and `GFX` are reserved by Groufix, using such a name for any variable or function in conjunction with the engine might result in redefinitions.

### Threading

_The core library is thread affine_. All windowing and hardware functonality should should be executed from the same thread at all times. Due to the complex nature of GPU interaction and windowing on most platforms is it faster and safer to execute all graphics related operations on the same thread. The engine itself might or might not thread its internal workings, but the _core_ external interface can be viewed as if it is executed on the calling thread. It is the calling application's responsibility to execute the engine in a dedicated thread if this is necessary.

This being said, the core engine, and only the _core_ engine is thread affine. All other functionality is thread safe. In practice this means a clean seperation between the graphics thread and all other threads. It is advisable to have a thread running to handle all core graphics, and communicate with it using the non-core functionality. `groufix.h` only includes core functionality, all other functionality should be included manually.

Even though the core engine is designed to run on a single thread, there are components which aren't thread affine. They are still not thread safe, but they can be used across multiple threads. All core features exhibiting this behaviour are listed below.

* __Buffer Mapping__, `gfx_buffer_map` returns a pointer which may be used from within any location in the program, as long as it is not unmapped.

* __Error Handling__, all functionality in `groufix/errors.h` can be issued from within any thread. Note: hardware errors should be polled using `gfx_hardware_poll_errors` in order to receive them, which is still thread affine.

* __Hardware Extension & Limit fetching__, `gfx_hardware_is_extension_supported` and `gfx_hardware_get_limit` are two functions which make use of prefetched data and can be issued from within any thread.

* __Property Map value setting__, setting the value of a property in a property map using `gfx_property_map_set_*` defers any transfers to the GPU, which makes it suitable for usage across threads.

### Termination

As said before, when done with the engine, it should be terminated with a call to `gfx_terminate`. It is important to make this call after the engine is initialized and used. This call will free all hardware (GPU) and window manager related resources. This means the connection to both the GPU and windowing manager is lost. It will also clear the error queue, as it will be irrelevant.

_It will not free any other resources_. All user allocated resources must be freed by the user. To make sure everything is freed properly, every _create_ method must be followed up by the appropriate _free_ method and every _init_ method must be followed up by the appropriate _clear_ method. On a side note, any free method can take NULL as parameter and it will do nothing.

## Acknowledgements

* Grace Fu
* [Martin Dørum Nygaard](http://www.mortie.org)
