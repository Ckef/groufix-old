##
# Groufix  :  Graphics Engine produced by Ckef Worx.
# www      :  <http://www.ckef-worx.com>.
#
# This file is part of Groufix.
#
# Copyright (C) Stef Velzel :: All Rights Reserved.
#
# Groufix is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Groufix is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with Groufix.  If not, see <http://www.gnu.org/licenses/>.
#
##

#################################################################
# If no target was given
#################################################################
default:
	@echo ""
	@echo "Use one of the following commands to build Groufix:"
	@echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	@echo " $(MAKE) clean             Clean temporary files."
	@echo " $(MAKE) clean-all         Clean all files $(MAKE) produced."
	@echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	@echo " $(MAKE) unix-x11          Build the Groufix Unix target using X11."
	@echo " $(MAKE) unix-x11-minimal  Build the minimal example Unix target using X11."
	@echo " $(MAKE) unix-x11-simple   Build the simple example Unix target using X11."
	@echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	@echo " $(MAKE) win32             Build the Groufix Windows target."
	@echo " $(MAKE) win32-minimal     Build the minimal example Windows target."
	@echo " $(MAKE) win32-simple      Build the simple example Windows target."
	@echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	@echo ""


#################################################################
# Compiler/Linker options for all build targets
#################################################################

# Use MinGW on windows
CC      = gcc
BIN     = bin
OUT     = obj
DEPEND  = depend
INCLUDE = include
SRC     = src
SSE     = YES

# Flags for all compiler calls
CFLAGS            = -Os -O2 -Wall -pedantic -I$(INCLUDE) -DGFX_$(SSE)_SSE
CFLAGS_UNIX_X11   = $(CFLAGS) -std=gnu99
CFLAGS_WIN32      = $(CFLAGS) -std=c99

# Object files only
OBJFLAGS          = -c -s -I$(DEPEND) -I$(SRC)
OBJFLAGS_UNIX_X11 = $(CFLAGS_UNIX_X11) $(OBJFLAGS) -fPIC
OBJFLAGS_WIN32    = $(CFLAGS_WIN32) $(OBJFLAGS)

# Libraries to link to
LIBS_UNIX_X11     = -lX11 -lGL
LIBS_WIN32        = -lwinmm -lopengl32 -lgdi32


#################################################################
# Header files for all window APIs (platforms)
#################################################################
HEADERS = \
 $(INCLUDE)/groufix/containers/deque.h \
 $(INCLUDE)/groufix/containers/list.h \
 $(INCLUDE)/groufix/containers/vector.h \
 $(INCLUDE)/groufix/core/errors.h \
 $(INCLUDE)/groufix/core/keys.h \
 $(INCLUDE)/groufix/core/memory.h \
 $(INCLUDE)/groufix/core/pipeline.h \
 $(INCLUDE)/groufix/core/shading.h \
 $(INCLUDE)/groufix/core/window.h \
 $(INCLUDE)/groufix/math/mat.h \
 $(INCLUDE)/groufix/math/quat.h \
 $(INCLUDE)/groufix/math/vec.h \
 $(INCLUDE)/groufix/math.h \
 $(INCLUDE)/groufix/utils.h \
 $(INCLUDE)/groufix.h

HEADERS_LIB = \
 $(HEADERS) \
 $(DEPEND)/GL/glcorearb.h \
 $(DEPEND)/GLES3/gl3.h \
 $(DEPEND)/GLES3/gl3platform.h \
 $(DEPEND)/KHR/khrplatform.h \
 $(SRC)/groufix/core/memory/internal.h \
 $(SRC)/groufix/core/pipeline/internal.h \
 $(SRC)/groufix/core/shading/internal.h \
 $(SRC)/groufix/core/internal.h \
 $(SRC)/groufix/core/platform.h

HEADERS_X11 = \
 $(HEADERS_LIB) \
 $(DEPEND)/GL/glxext.h \
 $(SRC)/groufix/core/platform/x11.h

HEADERS_WIN32 = \
 $(HEADERS_LIB) \
 $(DEPEND)/GL/wglext.h \
 $(SRC)/groufix/core/platform/win32.h


#################################################################
# Clean either temporary files or all of it
#################################################################
clean:
ifeq ($(OS),Windows_NT)
	if exist $(OUT)\nul rmdir /s /q $(OUT)
else
	rm -Rf $(OUT)
endif

clean-all: clean
ifeq ($(OS),Windows_NT)
	if exist $(BIN)\nul rmdir /s /q $(BIN)
else
	rm -Rf $(BIN)
endif


#################################################################
# Unix X11 builds
#################################################################
OBJS_UNIX_X11 = \
 $(OUT)/unix-x11/groufix/containers/deque.o \
 $(OUT)/unix-x11/groufix/containers/list.o \
 $(OUT)/unix-x11/groufix/containers/vector.o \
 $(OUT)/unix-x11/groufix/core/memory/buffer.o \
 $(OUT)/unix-x11/groufix/core/memory/formats.o \
 $(OUT)/unix-x11/groufix/core/memory/layout.o \
 $(OUT)/unix-x11/groufix/core/memory/shared_buffer.o \
 $(OUT)/unix-x11/groufix/core/memory/texture.o \
 $(OUT)/unix-x11/groufix/core/pipeline/bucket.o \
 $(OUT)/unix-x11/groufix/core/pipeline/pipe.o \
 $(OUT)/unix-x11/groufix/core/pipeline/pipeline.o \
 $(OUT)/unix-x11/groufix/core/pipeline/process.o \
 $(OUT)/unix-x11/groufix/core/pipeline/states.o \
 $(OUT)/unix-x11/groufix/core/platform/context.o \
 $(OUT)/unix-x11/groufix/core/platform/x11_context.o \
 $(OUT)/unix-x11/groufix/core/platform/x11_init.o \
 $(OUT)/unix-x11/groufix/core/platform/x11_screen.o \
 $(OUT)/unix-x11/groufix/core/platform/x11_time.o \
 $(OUT)/unix-x11/groufix/core/platform/x11_window.o \
 $(OUT)/unix-x11/groufix/core/shading/binder.o \
 $(OUT)/unix-x11/groufix/core/shading/program.o \
 $(OUT)/unix-x11/groufix/core/shading/property_map.o \
 $(OUT)/unix-x11/groufix/core/shading/shader.o \
 $(OUT)/unix-x11/groufix/core/errors.o \
 $(OUT)/unix-x11/groufix/core/events.o \
 $(OUT)/unix-x11/groufix/core/extensions.o \
 $(OUT)/unix-x11/groufix/core/hardware.o \
 $(OUT)/unix-x11/groufix/core/screen.o \
 $(OUT)/unix-x11/groufix/core/window.o \
 $(OUT)/unix-x11/groufix/math.o \
 $(OUT)/unix-x11/groufix.o

unix-x11: before-unix-x11 $(OBJS_UNIX_X11)
	$(CC) -shared $(OBJS_UNIX_X11) -o $(BIN)/unix-x11/libGroufix.so $(LIBS_UNIX_X11)

unix-x11-minimal: examples/minimal.c unix-x11 
	$(CC) $(CFLAGS_UNIX_X11) $< -o $(BIN)/unix-x11/minimal -L$(BIN)/unix-x11/ -Wl,-rpath='$$ORIGIN' -lGroufix

unix-x11-simple: examples/simple.c unix-x11 
	$(CC) $(CFLAGS_UNIX_X11) $< -o $(BIN)/unix-x11/simple -L$(BIN)/unix-x11/ -Wl,-rpath='$$ORIGIN' -lGroufix

before-unix-x11:
	mkdir -p $(BIN)/unix-x11
	mkdir -p $(OUT)/unix-x11/groufix/containers
	mkdir -p $(OUT)/unix-x11/groufix/core/memory
	mkdir -p $(OUT)/unix-x11/groufix/core/pipeline
	mkdir -p $(OUT)/unix-x11/groufix/core/platform
	mkdir -p $(OUT)/unix-x11/groufix/core/shading


# All the object files

$(OUT)/unix-x11/groufix/containers/deque.o: $(SRC)/groufix/containers/deque.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/containers/list.o: $(SRC)/groufix/containers/list.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/containers/vector.o: $(SRC)/groufix/containers/vector.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/memory/buffer.o: $(SRC)/groufix/core/memory/buffer.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/memory/formats.o: $(SRC)/groufix/core/memory/formats.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/memory/layout.o: $(SRC)/groufix/core/memory/layout.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/memory/shared_buffer.o: $(SRC)/groufix/core/memory/shared_buffer.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/memory/texture.o: $(SRC)/groufix/core/memory/texture.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/pipeline/bucket.o: $(SRC)/groufix/core/pipeline/bucket.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/pipeline/pipe.o: $(SRC)/groufix/core/pipeline/pipe.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/pipeline/pipeline.o: $(SRC)/groufix/core/pipeline/pipeline.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/pipeline/process.o: $(SRC)/groufix/core/pipeline/process.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/pipeline/states.o: $(SRC)/groufix/core/pipeline/states.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/platform/context.o: $(SRC)/groufix/core/platform/context.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/platform/x11_context.o: $(SRC)/groufix/core/platform/x11_context.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/platform/x11_init.o: $(SRC)/groufix/core/platform/x11_init.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/platform/x11_screen.o: $(SRC)/groufix/core/platform/x11_screen.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/platform/x11_time.o: $(SRC)/groufix/core/platform/x11_time.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/platform/x11_window.o: $(SRC)/groufix/core/platform/x11_window.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/shading/binder.o: $(SRC)/groufix/core/shading/binder.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/shading/program.o: $(SRC)/groufix/core/shading/program.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/shading/property_map.o: $(SRC)/groufix/core/shading/property_map.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/shading/shader.o: $(SRC)/groufix/core/shading/shader.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/errors.o: $(SRC)/groufix/core/errors.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/events.o: $(SRC)/groufix/core/events.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/extensions.o: $(SRC)/groufix/core/extensions.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/hardware.o: $(SRC)/groufix/core/hardware.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/screen.o: $(SRC)/groufix/core/screen.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/core/window.o: $(SRC)/groufix/core/window.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/math.o: $(SRC)/groufix/math.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix.o: $(SRC)/groufix.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@


#################################################################
# Windows builds
#################################################################
OBJS_WIN32 = \
 $(OUT)/win32/groufix/containers/deque.o \
 $(OUT)/win32/groufix/containers/list.o \
 $(OUT)/win32/groufix/containers/vector.o \
 $(OUT)/win32/groufix/core/memory/buffer.o \
 $(OUT)/win32/groufix/core/memory/formats.o \
 $(OUT)/win32/groufix/core/memory/layout.o \
 $(OUT)/win32/groufix/core/memory/shared_buffer.o \
 $(OUT)/win32/groufix/core/memory/texture.o \
 $(OUT)/win32/groufix/core/pipeline/bucket.o \
 $(OUT)/win32/groufix/core/pipeline/pipe.o \
 $(OUT)/win32/groufix/core/pipeline/pipeline.o \
 $(OUT)/win32/groufix/core/pipeline/process.o \
 $(OUT)/win32/groufix/core/pipeline/states.o \
 $(OUT)/win32/groufix/core/platform/context.o \
 $(OUT)/win32/groufix/core/platform/win32_context.o \
 $(OUT)/win32/groufix/core/platform/win32_init.o \
 $(OUT)/win32/groufix/core/platform/win32_screen.o \
 $(OUT)/win32/groufix/core/platform/win32_time.o \
 $(OUT)/win32/groufix/core/platform/win32_window.o \
 $(OUT)/win32/groufix/core/shading/binder.o \
 $(OUT)/win32/groufix/core/shading/program.o \
 $(OUT)/win32/groufix/core/shading/property_map.o \
 $(OUT)/win32/groufix/core/shading/shader.o \
 $(OUT)/win32/groufix/core/errors.o \
 $(OUT)/win32/groufix/core/events.o \
 $(OUT)/win32/groufix/core/extensions.o \
 $(OUT)/win32/groufix/core/hardware.o \
 $(OUT)/win32/groufix/core/screen.o \
 $(OUT)/win32/groufix/core/window.o \
 $(OUT)/win32/groufix/math.o \
 $(OUT)/win32/groufix.o

win32: before-win32 $(OBJS_WIN32)
	$(CC) -shared $(OBJS_WIN32) -o $(BIN)/win32/libGroufix.dll $(LIBS_WIN32)

win32-minimal: examples/minimal.c win32
	$(CC) $(CFLAGS_WIN32) $< -o $(BIN)/win32/minimal -L$(BIN)/win32/ -lGroufix

win32-simple: examples/simple.c win32
	$(CC) $(CFLAGS_WIN32) $< -o $(BIN)/win32/simple -L$(BIN)/win32/ -lGroufix

before-win32:
	if not exist $(BIN)\win32\nul mkdir $(BIN)\win32
	if not exist $(OUT)\win32\groufix\containers\nul mkdir $(OUT)\win32\groufix\containers
	if not exist $(OUT)\win32\groufix\core\memory\nul mkdir $(OUT)\win32\groufix\core\memory
	if not exist $(OUT)\win32\groufix\core\pipeline\nul mkdir $(OUT)\win32\groufix\core\pipeline
	if not exist $(OUT)\win32\groufix\core\platform\nul mkdir $(OUT)\win32\groufix\core\platform
	if not exist $(OUT)\win32\groufix\core\shading\nul mkdir $(OUT)\win32\groufix\core\shading


# All the object files

$(OUT)/win32/groufix/containers/deque.o: $(SRC)/groufix/containers/deque.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/containers/list.o: $(SRC)/groufix/containers/list.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/containers/vector.o: $(SRC)/groufix/containers/vector.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/memory/buffer.o: $(SRC)/groufix/core/memory/buffer.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/memory/formats.o: $(SRC)/groufix/core/memory/formats.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/memory/layout.o: $(SRC)/groufix/core/memory/layout.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/memory/shared_buffer.o: $(SRC)/groufix/core/memory/shared_buffer.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/memory/texture.o: $(SRC)/groufix/core/memory/texture.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/pipeline/bucket.o: $(SRC)/groufix/core/pipeline/bucket.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/pipeline/pipe.o: $(SRC)/groufix/core/pipeline/pipe.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/pipeline/pipeline.o: $(SRC)/groufix/core/pipeline/pipeline.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/pipeline/process.o: $(SRC)/groufix/core/pipeline/process.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/pipeline/states.o: $(SRC)/groufix/core/pipeline/states.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/platform/context.o: $(SRC)/groufix/core/platform/context.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/platform/win32_context.o: $(SRC)/groufix/core/platform/win32_context.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/platform/win32_init.o: $(SRC)/groufix/core/platform/win32_init.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/platform/win32_screen.o: $(SRC)/groufix/core/platform/win32_screen.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/platform/win32_time.o: $(SRC)/groufix/core/platform/win32_time.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/platform/win32_window.o: $(SRC)/groufix/core/platform/win32_window.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/shading/binder.o: $(SRC)/groufix/core/shading/binder.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/shading/program.o: $(SRC)/groufix/core/shading/program.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/shading/property_map.o: $(SRC)/groufix/core/shading/property_map.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/shading/shader.o: $(SRC)/groufix/core/shading/shader.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/errors.o: $(SRC)/groufix/core/errors.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/events.o: $(SRC)/groufix/core/events.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/extensions.o: $(SRC)/groufix/core/extensions.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/hardware.o: $(SRC)/groufix/core/hardware.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/screen.o: $(SRC)/groufix/core/screen.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/core/window.o: $(SRC)/groufix/core/window.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/math.o: $(SRC)/groufix/math.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix.o: $(SRC)/groufix.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

