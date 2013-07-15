# Groufix

## Introduction

Groufix is a cross platform hardware accelerated 2D/3D graphics engine built in C using OpenGL. The library has no external dependencies besides native windowing APIs and OpenGL to access the GPU. Desktop OpenGL is supported from 3.2 and up. OpenGL ES is supported from 3.0 and up.

Supported targets and their APIs _(windowing, OGL extension and/or OGL version)_:

* __Unix__,    X11 (Xlib), GLX, OGL _(working)_
* __OS X__,    X11 (Xlib), GLX, OGL _(working)_
* __Windows__, Win32 (Windows XP and up), WGL, OGL _(working)_
* __Various__, EGL, OGL ES _(planned)_
* __OS X__,    Cocoa, CGL, OGL _(planned)_
* __Android__, NDK, OGL ES _(considered)_

The main repository is hosted on [GitHub](https://github.com/Ckef/Groufix).

## Building

A Makefile is shipped with the project, run make without a target to view all build targets. Some of the more uncommon dependencies for OpenGL are shipped with the project as well. Once the library is built, link against it using `-lGroufix`. Use the library in your code by simply including `<groufix.h>`, all core functionality will be made available through that file. If you need to include other platform headers (like windows.h or xlib.h) make sure to include them before groufix.h in case you require a different setup.

### Dependencies

To compile Groufix, you need the standard headers for your platform and OpenGL. These headers should come with the appropriate GPU drivers.

## Acknowledgements

* [Martin Dørum Nygaard](http://www.mortie.org)
