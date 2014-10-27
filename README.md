# Groufix

Groufix is a cross platform hardware accelerated 2D/3D graphics engine built in C using OpenGL. The library has no external dependencies besides native windowing APIs and OpenGL to access the GPU. Desktop OpenGL is supported from 3.2 and up. OpenGL ES is supported from 3.0 and up.

Supported targets and their APIs:

* __Unix__, Xlib / OGL, (_working_ / _[GCC](https://gcc.gnu.org/)_)

* __Windows__, Win32 (Vista+) / OGL, (_working_ / _[MinGW-w64](http://mingw-w64.sourceforge.net/)_)

* __OS X__, Cocoa / OGL, (_planned_)

* __Android__, NDK / OGL ES, (_considered_)

The main repository is hosted on [GitHub](https://github.com/Ckef/Groufix).


## Building

A Makefile is shipped with the project, run make without a target to view all build targets. All necessary OpenGL headers are shipped with the project as well. The appropriate compiler is expected to be installed, see above for all expected compiler collections. Once the library is built, link against it using `-lGroufix`.

Groufix can be compiled with different OGL renderers. This must be given as a makefile flag in the form of `RENDERER=VALUE`, in which `VALUE` can be:

* __GL__, To compile using desktop OpenGL, this is the default value.

* __GLES__, To compile using OpenGL ES.

Along with the renderer value Groufix accepts more flags which can be defined while both compiling Groufix itself and any program or library using Groufix. All makefile flags are:

* __COMPILER=ANY__ Groufix will use compiler specific macros. If compiled with an unsupported compiler it will throw an error. Use this flag to turn the error off and force it to use compiler agnostic functionality. To disable it in a program or library using Groufix, `GFX_COMPILER_ANY` should be defined by the compiler.

* __SSE=NO__ Groufix will compile certain functions using SSE instructions. Use this flag to disable this feature. This feature is disabled if `GFX_COMPILER_ANY` is defined. To disable it in a program or library using Groufix, `GFX_NO_SSE` should be defined by the compiler.

## Usage

Once Groufix is built, it can be used in your code with `#include <groufix.h>`. All _core_ functionality will be made available through that file. Make sure the include directory in this repository is listed as a directory to search for header files. This directory contains all public header files necessary to use the library. Before using the engine, it should be initialized with a call to `gfx_init`. After being done with the engine, it should be terminated with a call to `gfx_terminate`. Additionally, to create any rendering related resources, at least one window should be created. This window will act as a context to communicate to the graphics card, if no window exists, there are no means of communicating.

All names starting with `gfx`, `_gfx` and `GFX` are reserved by Groufix, using such a name for any variable or function in conjunction with the engine might result in redefinitions.


#### Headers

* `<groufix.h>` includes all _core_ functionality such as initialization, timing, window management, errors and all low level mechanisms. This header essentially exposes the bare minimum to work with Groufix. To create any resources related to rendering, at least one window should be created.

* `<groufix/math.h>` includes all mathematical functions associated with groufix. This includes a handful of constants and linear algebra, namely vectors, matrices and quaternions.

* `<groufix/scene.h>` includes everything related to constructing a scene to render. This also includes high level constructs such as meshes, materials and manners to manage level of detail. To create any of these constructs, at least one window should be created.

* `<groufix/containers/*.h>` holds a set of headers defining useful container objects. All available containers are `deque`, `list` and `vector`. Replace the asterisk with one of these names.


#### Threading

_The library is thread affine_. All functonality should be executed from the same thread at all times. The engine itself might or might not thread its internal workings, but the external interface can be viewed as if it is executed on the calling thread. It is the calling application's responsibility to execute the engine in a dedicated thread if this is necessary.


#### Termination

As said before, when done with the engine, it should be terminated with a call to `gfx_terminate`. It is important to make this call after the engine is initialized and used. This call will free all OpenGL and window manager related resources. This means the connection to both OpenGL and the windowing manager is lost. It will also clear the error queue, as it will be irrelevant.

_It will not free any other resources_. All user allocated resources must be freed by the user. To make sure everything is freed properly, every `*_create` method must be followed up by the appropriate `*_free` method and every `*_init` method must be followed up by the appropriate `*_clear` method. On a side note, any free method can take NULL as parameter and it will do nothing.


## Acknowledgements

* Francis Edward Wharf
* Grace Fu
* Martin Dørum Nygaard
* Michael Andrews
