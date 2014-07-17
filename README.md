# Groufix

## Introduction

Groufix is a cross platform hardware accelerated 2D/3D graphics engine built in C using OpenGL. The library has no external dependencies besides native windowing APIs and OpenGL to access the GPU. Desktop OpenGL is supported from 3.2 and up. OpenGL ES is supported from 3.0 and up.

Supported targets and their APIs _(windowing / OGL extension / OGL version, status / compiler collection)_:

* __Unix__,    Xlib / GLX / OGL, (_working_ / _GCC__
* __Windows__, Win32 (XP and up) / WGL / OGL, (_working_ / _MinGW_)
* __OS X__,    Cocoa / CGL / OGL, (_planned_)
* __Android__, NDK / OGL ES, (_considered_)

The main repository is hosted on [GitHub](https://github.com/Ckef/Groufix).

## Building

A Makefile is shipped with the project, run make without a target to view all build targets. All necessary OpenGL headers are shipped with the project as well. Platform files (such as windows.h or xlib.h) are not shipped as they should be made available by the platform itself. Once the library is built, link against it using `-lGroufix`.

Groufix can be compiled with certain options. Some can only be set while compiling the library, others can be altered for both the library and the program which links against the library. All options are:

* __GFX_GLES__ _(library only)_ To compile Groufix, you need the standard headers for your platform and OpenGL. These headers should come with the appropriate GPU drivers. When developing for OGL ES, the library should be built specifically for it, `GFX_GLES` should be defined by the compiler to compile the correct extension loading mechanism (in most cases this is done automatically through the build target).

* __GFX_NO_SSE__ _(library and program)_ Groufix will compile certain functions using SSE instructions. To disable this feature `GFX_NO_SSE` should be defined by the compiler to compile using ordinary instructions instead of SSE ones. Alternatively, if the makefile is used to compile Groufix, the flag `SSE=NO` can be passed along with the make command to disable SSE instructions.

## Usage

Once Groufix is built, it can be used in your code with `#include <groufix.h>`. All _core_ functionality will be made available through that file. Make sure the include directory in this repository is listed as a directory to search for header files. This directory contains all public header files necessary to use the library. Before using the engine, it should be intialized with a call to `gfx_init`. After being done with the engine, it should be terminated with a call to `gfx_terminate`.

All names starting with `gfx`, `_gfx` and `GFX` are reserved by Groufix, using such a name for any variable or function in conjunction with the engine might result in redefinitions.

### Threading

_The library is thread affine_. All functonality should be executed from the same thread at all times. The engine itself might or might not thread its internal workings, but the external interface can be viewed as if it is executed on the calling thread. It is the calling application's responsibility to execute the engine in a dedicated thread if this is necessary.

### Termination

As said before, when done with the engine, it should be terminated with a call to `gfx_terminate`. It is important to make this call after the engine is initialized and used. This call will free all hardware (GPU) and window manager related resources. This means the connection to both the GPU and windowing manager is lost. It will also clear the error queue, as it will be irrelevant.

_It will not free any other resources_. All user allocated resources must be freed by the user. To make sure everything is freed properly, every _create_ method must be followed up by the appropriate _free_ method and every _init_ method must be followed up by the appropriate _clear_ method. On a side note, any free method can take NULL as parameter and it will do nothing.

## Acknowledgements

* Grace Fu
* [Martin Dørum Nygaard](http://www.mortie.org)
