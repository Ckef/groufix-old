# Groufix

## Introduction

Groufix is a cross platform hardware accelerated 2D/3D graphics engine built in C using OpenGL. The library has no external dependencies besides native windowing APIs and OpenGL to access the GPU. Desktop OpenGL is supported from 3.2 and up. OpenGL ES is supported from 3.0 and up.

Supported targets and their APIs _(windowing, OGL extension and/or OGL version)_:

* __Unix__,    X11 (Xlib), GLX, OGL _(working)_
* __OS X__,    X11 (Xlib), GLX, OGL _(working)_
* __Windows__, Win32 (Windows XP and up), WGL, OGL _(working)_
* __OS X__,    Cocoa, CGL, OGL _(planned)_
* __Android__, NDK, OGL ES _(considered)_

The main repository is hosted on [GitHub](https://github.com/Ckef/Groufix).

## Building

A Makefile is shipped with the project, run make without a target to view all build targets. All necessary OpenGL headers are shipped with the project as well. Platform files (such as windows.h or xlib.h) are not shipped as they should be made available by the platform itself. Once the library is built, link against it using `-lGroufix`.

To compile Groufix, you need the standard headers for your platform and OpenGL. These headers should come with the appropriate GPU drivers. When developing for OGL ES, the library should be built specifically for it, `GFX_GLES` should be defined by the compiler to compile the correct extension loading mechanism (in most cases this is done automatically through the build target).

## Usage

Once groufix is built, it can be used in your code with `#include <groufix.h>`. All core functionality will be made available through that file. Make sure the `include` directory in this repository is listed as a directory to search for header files. This directory contains all public header files necessary to use the library.

### Threading

_The library is not thread safe_. All windowing and hardware functionality should be executed from the same thread at all times. Due to the complex nature of GPU interaction it is easier and safer to execute all graphics related operations on the same thread. However, there are components in the engine which are specifically designed to be used in multiple threads. Note, no other functionality is guaranteed to work concurrently.

* __Buffer Mapping__, `gfx_hardware_buffer_map` and `gfx_hardware_buffer_get_map` will return a pointer to the mapped buffer, this pointer can be used in multiple threads to concurrently upload data.

## Acknowledgements

* [Martin Dørum Nygaard](http://www.mortie.org)
