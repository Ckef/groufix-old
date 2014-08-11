# Groufix

Groufix is a cross platform hardware accelerated 2D/3D graphics engine built in C using OpenGL. The library has no external dependencies besides native windowing APIs and OpenGL to access the GPU. Desktop OpenGL is supported from 3.2 and up. OpenGL ES is supported from 3.0 and up.

Supported targets and their APIs _(windowing / OGL extension / OGL version, status / compiler collection)_:

* __Unix__, Xlib / GLX / OGL, (_working_ / _GCC_)
* __Windows__, Win32 (Vista and up) / WGL / OGL, (_working_ / _MinGW_)
* __OS X__, Cocoa / CGL / OGL, (_planned_)
* __Android__, NDK/ EGL / OGL ES, (_considered_)

The main repository is hosted on [GitHub](https://github.com/Ckef/Groufix).


## Building

A Makefile is shipped with the project, run make without a target to view all build targets. All necessary OpenGL headers are shipped with the project as well. The appropriate compiler is expected to be installed, this is expected to be a GCC compatible compiler for most platforms. On Windows the recommended compiler is [MinGW-w64](http://mingw-w64.sourceforge.net/). Once the library is built, link against it using `-lGroufix`.

Groufix can be compiled with certain options. Some can only be set while compiling the library, others can be altered for both the library and the program which links against the library. All options are:

* __GFX_GLES__ _(library only)_ To compile Groufix, you need the standard headers for your platform and OpenGL. These headers should come with the appropriate GPU drivers. When developing for OGL ES, the library should be built specifically for it, `GFX_GLES` should be defined by the compiler to compile the correct extension loading mechanism (in most cases this is done automatically through the build target).

* __GFX_NO_SSE__ _(library and program)_ Groufix will compile certain functions using SSE instructions. To disable this feature `GFX_NO_SSE` should be defined by the compiler to compile using ordinary instructions instead of SSE ones. Alternatively, if the makefile is used to compile Groufix, the flag `SSE=NO` can be passed along with the make command to disable SSE instructions.


## Usage

Once Groufix is built, it can be used in your code with `#include <groufix.h>`. All _core_ functionality will be made available through that file. Make sure the include directory in this repository is listed as a directory to search for header files. This directory contains all public header files necessary to use the library. Before using the engine, it should be initialized with a call to `gfx_init`. After being done with the engine, it should be terminated with a call to `gfx_terminate`.

All names starting with `gfx`, `_gfx` and `GFX` are reserved by Groufix, using such a name for any variable or function in conjunction with the engine might result in redefinitions.


#### Headers

* `<groufix.h>` includes all _core_ functionality such as initialization, timing, window management, errors and all low level mechanisms. This header essentially exposes the bare minimum to work with Groufix.

* `<groufix/math.h>` includes all mathematical functions associated with groufix. This includes a handful of constants and linear algebra, namely vectors, matrices and quaternions.

* `<groufix/scene.h>` includes everything related to constructing a scene to render. This also includes high level constructs such as meshes, materials and manners to manage level of detail.

* `<groufix/containers/*.h>` holds a set of headers defining useful container objects. All available containers are `vector`, `deque` and `list`. Replace the asterisk with one of these names.


#### Threading

_The library is thread affine_. All functonality should be executed from the same thread at all times. The engine itself might or might not thread its internal workings, but the external interface can be viewed as if it is executed on the calling thread. It is the calling application's responsibility to execute the engine in a dedicated thread if this is necessary.


#### Termination

As said before, when done with the engine, it should be terminated with a call to `gfx_terminate`. It is important to make this call after the engine is initialized and used. This call will free all OpenGL and window manager related resources. This means the connection to both OpenGL and the windowing manager is lost. It will also clear the error queue, as it will be irrelevant.

_It will not free any other resources_. All user allocated resources must be freed by the user. To make sure everything is freed properly, every `*_create` method must be followed up by the appropriate `*_free` method and every `*_init` method must be followed up by the appropriate `*_clear` method. On a side note, any free method can take NULL as parameter and it will do nothing.


## Acknowledgements

* Grace Fu
* Martin Dørum Nygaard
