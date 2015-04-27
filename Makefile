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
COMPILER = SUPPORTED
DEBUG    = NO
SSE      = YES


# Debug flags
ifeq ($(DEBUG),YES)
 DFLAGS = -g
else
 DFLAGS = -DNDEBUG
endif


# Flags for all binaries
CFLAGS          = -Os -O2 -Wall -pedantic -Iinclude $(DFLAGS) -DGFX_COMPILER_$(COMPILER) -DGFX_$(SSE)_SSE
CFLAGS_UNIX_X11 = $(CFLAGS) -std=gnu99
CFLAGS_WIN32    = $(CFLAGS) -std=c99


# Library object files only
OBJFLAGS          = -c -s -Idepend -Isrc -DGFX_BUILD_LIB -DGFX_$(RENDERER)
OBJFLAGS_UNIX_X11 = $(OBJFLAGS) $(CFLAGS_UNIX_X11) -fPIC -pthread
OBJFLAGS_WIN32    = $(OBJFLAGS) $(CFLAGS_WIN32) -DWINVER=0x0600 -D_WIN32_WINNT=0x0600


# Linker flags
LFLAGS          = -shared
LFLAGS_UNIX_X11 = $(LFLAGS) -pthread -lX11 -lGL
LFLAGS_WIN32    = $(LFLAGS) -lwinmm -lopengl32 -lgdi32 -static-libgcc


#################################################################
# Directory management, creating and cleaning
#################################################################

# Creation
$(BIN):
ifeq ($(OS),Windows_NT)
	@if not exist $(BIN)/nul mkdir $(BIN)
else
	@mkdir -p $(BIN)
endif

$(OUT):
ifeq ($(OS),Windows_NT)
	@if not exist $(OUT)/groufix/containers/nul mkdir $(OUT)/groufix/containers
	@if not exist $(OUT)/groufix/core/platform/nul mkdir $(OUT)/groufix/core/platform
	@if not exist $(OUT)/groufix/core/renderer/nul mkdir $(OUT)/groufix/core/renderer
	@if not exist $(OUT)/groufix/scene/nul mkdir $(OUT)/groufix/scene
else
	@mkdir -p $(OUT)/groufix/containers
	@mkdir -p $(OUT)/groufix/core/platform
	@mkdir -p $(OUT)/groufix/core/renderer
	@mkdir -p $(OUT)/groufix/scene
endif


# Cleaning
clean:
ifeq ($(OS),Windows_NT)
	@if exist $(OUT)/nul rmdir /s /q $(OUT)
else
	@rm -Rf $(OUT)
endif

clean-all: clean
ifeq ($(OS),Windows_NT)
	@if exist $(BIN)/nul rmdir /s /q $(BIN)
else
	@rm -Rf $(BIN)
endif


#################################################################
# Shared file dependencies for everything
#################################################################

# Renderer headers
ifeq ($(RENDERER),GL)
 HEADERS_RENDERER = \
  depend/GL/glcorearb.h \
  depend/GL/glxext.h \
  depend/GL/wglext.h \
  src/groufix/core/renderer/gl.h \
  src/groufix/core/renderer/gl_def.h \

else ifeq ($(RENDERER),GLES)
 HEADERS_RENDERER = \
  depend/GLES3/gl31.h \
  depend/GLES3/gl3platform.h \
  depend/KHR/khrplatform.h \
  src/groufix/core/renderer/gl.h \
  src/groufix/core/renderer/gl_def.h \

endif


# All headers
HEADERS = \
 $(HEADERS_RENDERER) \
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
 src/groufix/core/file.h \
 src/groufix/core/internal.h \
 src/groufix/core/platform.h \
 src/groufix/core/renderer.h \
 src/groufix/core/threading.h \
 src/groufix/scene/internal.h \


#################################################################
# Shared source files for everything
#################################################################

# Renderer sources
ifeq ($(RENDERER),GL)
 OBJS_RENDERER = \
  $(OUT)/groufix/core/renderer/gl_binder.o \
  $(OUT)/groufix/core/renderer/gl_emulate.o \
  $(OUT)/groufix/core/renderer/gl_formats.o \
  $(OUT)/groufix/core/renderer/gl_load.o \

else ifeq ($(RENDERER),GLES)
 OBJS_RENDERER = \
  $(OUT)/groufix/core/renderer/gl_binder.o \
  $(OUT)/groufix/core/renderer/gl_emulate.o \
  $(OUT)/groufix/core/renderer/gl_formats.o \
  $(OUT)/groufix/core/renderer/gl_load.o \

endif


# All sources
OBJS = \
 $(OBJS_RENDERER) \
 $(OUT)/groufix/containers/deque.o \
 $(OUT)/groufix/containers/list.o \
 $(OUT)/groufix/containers/thread_pool.o \
 $(OUT)/groufix/containers/vector.o \
 $(OUT)/groufix/core/bucket.o \
 $(OUT)/groufix/core/buffer.o \
 $(OUT)/groufix/core/errors.o \
 $(OUT)/groufix/core/events.o \
 $(OUT)/groufix/core/layout.o \
 $(OUT)/groufix/core/objects.o \
 $(OUT)/groufix/core/pipe.o \
 $(OUT)/groufix/core/pipeline.o \
 $(OUT)/groufix/core/process.o \
 $(OUT)/groufix/core/program.o \
 $(OUT)/groufix/core/program_map.o \
 $(OUT)/groufix/core/property_map.o \
 $(OUT)/groufix/core/sampler.o \
 $(OUT)/groufix/core/screen.o \
 $(OUT)/groufix/core/shader.o \
 $(OUT)/groufix/core/shared_buffer.o \
 $(OUT)/groufix/core/states.o \
 $(OUT)/groufix/core/texture.o \
 $(OUT)/groufix/core/types.o \
 $(OUT)/groufix/core/window.o \
 $(OUT)/groufix/scene/batch.o \
 $(OUT)/groufix/scene/lod_map.o \
 $(OUT)/groufix/scene/material.o \
 $(OUT)/groufix/scene/mesh.o \
 $(OUT)/groufix/math.o \
 $(OUT)/groufix.o


#################################################################
# Unix X11 builds
#################################################################

# Platform headers & sources
HEADERS_UNIX_X11 = \
 $(HEADERS) \
 src/groufix/core/platform/x11.h

OBJS_UNIX_X11 = \
 $(OBJS) \
 $(OUT)/groufix/core/platform/unix_file.o \
 $(OUT)/groufix/core/platform/unix_threading.o \
 $(OUT)/groufix/core/platform/unix_time.o \
 $(OUT)/groufix/core/platform/x11_context.o \
 $(OUT)/groufix/core/platform/x11_init.o \
 $(OUT)/groufix/core/platform/x11_screen.o \
 $(OUT)/groufix/core/platform/x11_window.o \


# All the object and source files
$(OUT)/%.o: src/%.c $(HEADERS_UNIX_X11) | $(OUT)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(BIN)/libGroufix.so: $(OBJS_UNIX_X11) | $(BIN)
	$(CC) $(OBJS_UNIX_X11) -o $@ $(LFLAGS_UNIX_X11)

$(BIN)/%: examples/%.c $(BIN)/libGroufix.so
	$(CC) $(CFLAGS_UNIX_X11) $< -o $@ -L$(BIN)/ -Wl,-rpath='$$ORIGIN' -lGroufix


# Available targets
unix-x11:
	@$(MAKE) $(BIN)/unix-x11/libGroufix.so BIN=$(BIN)/unix-x11 OUT=$(OUT)/unix-x11
unix-x11-examples:
	@$(MAKE) $(BIN)/unix-x11/minimal BIN=$(BIN)/unix-x11 OUT=$(OUT)/unix-x11
	@$(MAKE) $(BIN)/unix-x11/simple BIN=$(BIN)/unix-x11 OUT=$(OUT)/unix-x11


#################################################################
# Windows builds
#################################################################

# Platform headers & sources
HEADERS_WIN32 = \
 $(HEADERS) \
 src/groufix/core/platform/win32.h

OBJS_WIN32 = \
 $(OBJS) \
 $(OUT)/groufix/core/platform/win32_context.o \
 $(OUT)/groufix/core/platform/win32_file.o \
 $(OUT)/groufix/core/platform/win32_init.o \
 $(OUT)/groufix/core/platform/win32_screen.o \
 $(OUT)/groufix/core/platform/win32_strings.o \
 $(OUT)/groufix/core/platform/win32_threading.o \
 $(OUT)/groufix/core/platform/win32_time.o \
 $(OUT)/groufix/core/platform/win32_window.o \


# All the object and source files
$(OUT)/%.o: src/%.c $(HEADERS_WIN32) | $(OUT)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(BIN)/libGroufix.dll: $(OBJS_WIN32) | $(BIN)
	$(CC) $(OBJS_WIN32) -o $@ $(LFLAGS_WIN32)

$(BIN)/%: examples/%.c $(BIN)/libGroufix.dll
	$(CC) $(CFLAGS_WIN32) $< -o $@ -L$(BIN)/ -lGroufix


# Available targets
win32:
	@$(MAKE) $(BIN)/win32/libGroufix.dll BIN=$(BIN)/win32 OUT=$(OUT)/win32
win32-examples:
	@$(MAKE) $(BIN)/win32/minimal BIN=$(BIN)/win32 OUT=$(OUT)/win32
	@$(MAKE) $(BIN)/win32/simple BIN=$(BIN)/win32 OUT=$(OUT)/win32
