# Groufix

## Introduction

Groufix is a cross platform hardware accelerated 2D/3D graphics engine built in C using OpenGL. The library has no external dependencies besides native windowing APIs and OpenGL to access the GPU. Desktop OpenGL is supported from 3.2 and up. OpenGL ES is supported from 2.0 and up.

Supported targets and their APIs _(windowing, OGL extension, OGL version)_:

* __Unix__,    X11 (Xlib), GLX, OGL _(working)_
* __OS X__,    X11 (Xlib), GLX, OGL _(working)_
* __Windows__, Win32 (Windows XP and up), WGL, OGL _(working)_
* __OS X__,    Cocoa, CGL, OGL _(planned)_
* __Android__, NDK, OGL ES _(considered)_
* __Web__,     emscripten, OGL ES _(considered)_

The main repository is hosted on [GitHub](https://github.com/Ckef/Groufix).

## Acknowledgements

* Martin Dørum Nygaard
