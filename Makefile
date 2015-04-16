##
# Groufix  :  Graphics Engine produced by Ckef Worx.
# www      :  <http://www.ckef-worx.com>.
#
# This file is part of Groufix.
#
# Copyright (C) Stef Velzel :: All Rights Reserved.
#
# Groufix is licensed under the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the license,
# or (at your option) any later version.
#
##


#################################################################
# If no target was given
#################################################################
help:
	@echo ""
	@echo "Use one of the following commands to build Groufix:"
	@echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	@echo " $(MAKE) clean              Clean temporary files."
	@echo " $(MAKE) clean-all          Clean all files make produced."
	@echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	@echo " $(MAKE) unix-x11           Build the Groufix Unix-X11 target."
	@echo " $(MAKE) unix-x11-examples  Build all targets and examples for Unix-X11."
	@echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	@echo " $(MAKE) win32              Build the Groufix Windows target."
	@echo " $(MAKE) win32-examples     Build all tragets and examples for Windows."
	@echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	@echo ""


#################################################################
# Compiler/Linker options for all build targets
#################################################################

# Environment & Flags
CC       = gcc
BIN      = bin
OUT      = obj

RENDERER = GL
DEBUG    = NO
COMPILER = SUPPORTED
SSE      = YES


# Debug flags
ifeq ($(DEBUG),YES)
 DFLAGS = -g
else
 DFLAGS = -DNDEBUG
endif


# Flags for all binaries files
CFLAGS            = -Os -O2 -Wall -pedantic -Iinclude $(DFLAGS) -DGFX_COMPILER_$(COMPILER) -DGFX_$(SSE)_SSE
CFLAGS_UNIX_X11   = $(CFLAGS) -std=gnu99
CFLAGS_WIN32      = $(CFLAGS) -std=c99


# Library object files only
OBJFLAGS          = -c -s -Idepend -Isrc -DGFX_BUILD_LIB -DGFX_$(RENDERER)
OBJFLAGS_UNIX_X11 = $(OBJFLAGS) $(CFLAGS_UNIX_X11) -fPIC -pthread
OBJFLAGS_WIN32    = $(OBJFLAGS) $(CFLAGS_WIN32) -DWINVER=0x0600 -D_WIN32_WINNT=0x0600


# Linker flags
LFLAGS            = -shared
LFLAGS_UNIX_X11   = $(LFLAGS) -pthread -lX11 -lGL
LFLAGS_WIN32      = $(LFLAGS) -lwinmm -lopengl32 -lgdi32 -static-libgcc


#################################################################
# Directory management, creating and cleaning
#################################################################

# Creation
$(BIN):
ifeq ($(OS),Windows_NT)
	@if not exist $(BIN)\win32\nul mkdir $(BIN)\win32
else
	@mkdir -p $(BIN)/unix-x11
endif

$(OUT):
ifeq ($(OS),Windows_NT)
	@if not exist $(OUT)\win32\groufix\containers\nul mkdir $(OUT)\win32\groufix\containers
	@if not exist $(OUT)\win32\groufix\core\platform\nul mkdir $(OUT)\win32\groufix\core\platform
	@if not exist $(OUT)\win32\groufix\core\renderer\nul mkdir $(OUT)\win32\groufix\core\renderer
	@if not exist $(OUT)\win32\groufix\scene\nul mkdir $(OUT)\win32\groufix\scene
else
	@mkdir -p $(OUT)/unix-x11/groufix/containers
	@mkdir -p $(OUT)/unix-x11/groufix/core/platform
	@mkdir -p $(OUT)/unix-x11/groufix/core/renderer
	@mkdir -p $(OUT)/unix-x11/groufix/scene
endif


# Cleaning
clean:
ifeq ($(OS),Windows_NT)
	@if exist $(OUT)\nul rmdir /s /q $(OUT)
else
	@rm -Rf $(OUT)
endif

clean-all: clean
ifeq ($(OS),Windows_NT)
	@if exist $(BIN)\nul rmdir /s /q $(BIN)
else
	@rm -Rf $(BIN)
endif


#################################################################
# Header file dependencies for everything
#################################################################

HEADERS = \
 depend/GL/glcorearb.h \
 depend/GLES3/gl31.h \
 depend/GLES3/gl3platform.h \
 depend/KHR/khrplatform.h \
 include/groufix/containers/deque.h \
 include/groufix/containers/list.h \
 include/groufix/containers/thread_pool.h \
 include/groufix/containers/vector.h \
 include/groufix/core/errors.h \
 include/groufix/core/keys.h \
 include/groufix/core/memory.h \
 include/groufix/core/pipeline.h \
 include/groufix/core/shading.h \
 include/groufix/core/window.h \
 include/groufix/math/mat.h \
 include/groufix/math/quat.h \
 include/groufix/math/vec.h \
 include/groufix/scene/lod.h \
 include/groufix/scene/material.h \
 include/groufix/scene/mesh.h \
 include/groufix/scene/object.h \
 include/groufix/math.h \
 include/groufix/resources.h \
 include/groufix/scene.h \
 include/groufix/utils.h \
 include/groufix.h \
 src/groufix/core/renderer/gl.h \
 src/groufix/core/renderer/gl_def.h \
 src/groufix/core/file.h \
 src/groufix/core/internal.h \
 src/groufix/core/platform.h \
 src/groufix/core/renderer.h \
 src/groufix/core/threading.h \
 src/groufix/scene/internal.h \


#################################################################
# Unix X11 builds
#################################################################

HEADERS_UNIX_X11 = \
 $(HEADERS) \
 depend/GL/glxext.h \
 src/groufix/core/platform/x11.h

OBJS_UNIX_X11 = \
 $(OUT)/unix-x11/groufix/containers/deque.o \
 $(OUT)/unix-x11/groufix/containers/list.o \
 $(OUT)/unix-x11/groufix/containers/thread_pool.o \
 $(OUT)/unix-x11/groufix/containers/vector.o \
 $(OUT)/unix-x11/groufix/core/platform/unix_file.o \
 $(OUT)/unix-x11/groufix/core/platform/unix_threading.o \
 $(OUT)/unix-x11/groufix/core/platform/unix_time.o \
 $(OUT)/unix-x11/groufix/core/platform/x11_context.o \
 $(OUT)/unix-x11/groufix/core/platform/x11_init.o \
 $(OUT)/unix-x11/groufix/core/platform/x11_screen.o \
 $(OUT)/unix-x11/groufix/core/platform/x11_window.o \
 $(OUT)/unix-x11/groufix/core/renderer/gl_binder.o \
 $(OUT)/unix-x11/groufix/core/renderer/gl_emulate.o \
 $(OUT)/unix-x11/groufix/core/renderer/gl_formats.o \
 $(OUT)/unix-x11/groufix/core/renderer/gl_load.o \
 $(OUT)/unix-x11/groufix/core/bucket.o \
 $(OUT)/unix-x11/groufix/core/buffer.o \
 $(OUT)/unix-x11/groufix/core/errors.o \
 $(OUT)/unix-x11/groufix/core/events.o \
 $(OUT)/unix-x11/groufix/core/layout.o \
 $(OUT)/unix-x11/groufix/core/objects.o \
 $(OUT)/unix-x11/groufix/core/pipe.o \
 $(OUT)/unix-x11/groufix/core/pipeline.o \
 $(OUT)/unix-x11/groufix/core/process.o \
 $(OUT)/unix-x11/groufix/core/program.o \
 $(OUT)/unix-x11/groufix/core/program_map.o \
 $(OUT)/unix-x11/groufix/core/property_map.o \
 $(OUT)/unix-x11/groufix/core/sampler.o \
 $(OUT)/unix-x11/groufix/core/screen.o \
 $(OUT)/unix-x11/groufix/core/shader.o \
 $(OUT)/unix-x11/groufix/core/shared_buffer.o \
 $(OUT)/unix-x11/groufix/core/states.o \
 $(OUT)/unix-x11/groufix/core/texture.o \
 $(OUT)/unix-x11/groufix/core/types.o \
 $(OUT)/unix-x11/groufix/core/window.o \
 $(OUT)/unix-x11/groufix/scene/batch.o \
 $(OUT)/unix-x11/groufix/scene/lod_map.o \
 $(OUT)/unix-x11/groufix/scene/material.o \
 $(OUT)/unix-x11/groufix/scene/mesh.o \
 $(OUT)/unix-x11/groufix/math.o \
 $(OUT)/unix-x11/groufix.o

EXAMPLES_UNIX_X11 = \
 $(BIN)/unix-x11/minimal \
 $(BIN)/unix-x11/simple


# All the object and source files
$(OUT)/unix-x11%.o: src%.c $(HEADERS_UNIX_X11) | $(OUT)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(BIN)/unix-x11/libGroufix.so: $(OBJS_UNIX_X11) | $(BIN)
	$(CC) $(OBJS_UNIX_X11) -o $@ $(LFLAGS_UNIX_X11)

$(BIN)/unix-x11%: examples%.c $(BIN)/unix-x11/libGroufix.so
	$(CC) $(CFLAGS_UNIX_X11) $< -o $@ -L$(BIN)/unix-x11/ -Wl,-rpath='$$ORIGIN' -lGroufix


# Available targets
unix-x11: $(BIN)/unix-x11/libGroufix.so
unix-x11-examples: $(EXAMPLES_UNIX_X11)


#################################################################
# Windows builds
#################################################################

HEADERS_WIN32 = \
 $(HEADERS) \
 depend/GL/wglext.h \
 src/groufix/core/platform/win32.h

OBJS_WIN32 = \
 $(OUT)/win32/groufix/containers/deque.o \
 $(OUT)/win32/groufix/containers/list.o \
 $(OUT)/win32/groufix/containers/thread_pool.o \
 $(OUT)/win32/groufix/containers/vector.o \
 $(OUT)/win32/groufix/core/platform/win32_context.o \
 $(OUT)/win32/groufix/core/platform/win32_file.o \
 $(OUT)/win32/groufix/core/platform/win32_init.o \
 $(OUT)/win32/groufix/core/platform/win32_screen.o \
 $(OUT)/win32/groufix/core/platform/win32_strings.o \
 $(OUT)/win32/groufix/core/platform/win32_threading.o \
 $(OUT)/win32/groufix/core/platform/win32_time.o \
 $(OUT)/win32/groufix/core/platform/win32_window.o \
 $(OUT)/win32/groufix/core/renderer/gl_binder.o \
 $(OUT)/win32/groufix/core/renderer/gl_emulate.o \
 $(OUT)/win32/groufix/core/renderer/gl_formats.o \
 $(OUT)/win32/groufix/core/renderer/gl_load.o \
 $(OUT)/win32/groufix/core/bucket.o \
 $(OUT)/win32/groufix/core/buffer.o \
 $(OUT)/win32/groufix/core/errors.o \
 $(OUT)/win32/groufix/core/events.o \
 $(OUT)/win32/groufix/core/layout.o \
 $(OUT)/win32/groufix/core/objects.o \
 $(OUT)/win32/groufix/core/pipe.o \
 $(OUT)/win32/groufix/core/pipeline.o \
 $(OUT)/win32/groufix/core/process.o \
 $(OUT)/win32/groufix/core/program.o \
 $(OUT)/win32/groufix/core/program_map.o \
 $(OUT)/win32/groufix/core/property_map.o \
 $(OUT)/win32/groufix/core/sampler.o \
 $(OUT)/win32/groufix/core/screen.o \
 $(OUT)/win32/groufix/core/shader.o \
 $(OUT)/win32/groufix/core/shared_buffer.o \
 $(OUT)/win32/groufix/core/states.o \
 $(OUT)/win32/groufix/core/texture.o \
 $(OUT)/win32/groufix/core/types.o \
 $(OUT)/win32/groufix/core/window.o \
 $(OUT)/win32/groufix/scene/batch.o \
 $(OUT)/win32/groufix/scene/lod_map.o \
 $(OUT)/win32/groufix/scene/material.o \
 $(OUT)/win32/groufix/scene/mesh.o \
 $(OUT)/win32/groufix/math.o \
 $(OUT)/win32/groufix.o

EXAMPLES_WIN32 = \
 $(BIN)/win32/minimal \
 $(BIN)/win32/simple


# All the object and source files
$(OUT)/win32%.o: src%.c $(HEADERS_WIN32) | $(OUT)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(BIN)/win32/libGroufix.dll: $(OBJS_WIN32) | $(BIN)
	$(CC) $(OBJS_WIN32) -o $@ $(LFLAGS_WIN32)

$(BIN)/win32%: examples%.c $(BIN)/win32/libGroufix.dll
	$(CC) $(CFLAGS_WIN32) $< -o $@ -L$(BIN)/win32/ -lGroufix


# Available targets
win32: $(BIN)/win32/libGroufix.dll
win32-examples: $(EXAMPLES_WIN32)
