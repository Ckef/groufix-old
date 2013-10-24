##
# Groufix  :  Graphics Engine produced by Ckef Worx
# www      :  http://www.ejb.ckef-worx.com
#
# Copyright (C) Stef Velzel :: All Rights Reserved
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

# Flags for all compiler calls
CFLAGS = -Os -O2 -Wall -std=c99 -pedantic -I$(INCLUDE)

# Object files only
OBJFLAGS          = $(CFLAGS) -c -I$(DEPEND) -I$(SRC)
OBJFLAGS_UNIX_X11 = $(OBJFLAGS) -fPIC -s
OBJFLAGS_WIN32    = $(OBJFLAGS) -s

# Libraries to link to
LIBS_UNIX_X11 = -lX11 -lGL
LIBS_WIN32    = -lopengl32 -lgdi32


#################################################################
# Header files for all window APIs (platforms)
#################################################################
HEADERS = \
 $(INCLUDE)/groufix/containers/deque.h \
 $(INCLUDE)/groufix/containers/list.h \
 $(INCLUDE)/groufix/containers/vector.h \
 $(INCLUDE)/groufix/math/mat.h \
 $(INCLUDE)/groufix/math/quat.h \
 $(INCLUDE)/groufix/math/vec.h \
 $(INCLUDE)/groufix/errors.h \
 $(INCLUDE)/groufix/geometry.h \
 $(INCLUDE)/groufix/math.h \
 $(INCLUDE)/groufix/memory.h \
 $(INCLUDE)/groufix/pipeline.h \
 $(INCLUDE)/groufix/shading.h \
 $(INCLUDE)/groufix/utils.h \
 $(INCLUDE)/groufix/window.h \
 $(INCLUDE)/groufix.h

HEADERS_LIB = \
 $(HEADERS) \
 $(DEPEND)/GL/glcorearb.h \
 $(DEPEND)/GLES3/gl3.h \
 $(DEPEND)/GLES3/gl3platform.h \
 $(DEPEND)/KHR/khrplatform.h \
 $(SRC)/groufix/memory/datatypes.h \
 $(SRC)/groufix/internal.h \
 $(SRC)/groufix/platform.h

HEADERS_X11 = \
 $(HEADERS_LIB) \
 $(DEPEND)/GL/glxext.h \
 $(SRC)/groufix/platform/x11.h

HEADERS_WIN32 = \
 $(HEADERS_LIB) \
 $(DEPEND)/GL/wglext.h \
 $(SRC)/groufix/platform/win32.h


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
 $(OUT)/unix-x11/groufix/geometry/layout.o \
 $(OUT)/unix-x11/groufix/memory/buffer.o \
 $(OUT)/unix-x11/groufix/memory/formats.o \
 $(OUT)/unix-x11/groufix/memory/texture.o \
 $(OUT)/unix-x11/groufix/pipeline/bucket.o \
 $(OUT)/unix-x11/groufix/pipeline/pipeline.o \
 $(OUT)/unix-x11/groufix/platform/x11_context.o \
 $(OUT)/unix-x11/groufix/platform/x11_init.o \
 $(OUT)/unix-x11/groufix/platform/x11_screen.o \
 $(OUT)/unix-x11/groufix/platform/x11_window.o \
 $(OUT)/unix-x11/groufix/shading/program.o \
 $(OUT)/unix-x11/groufix/shading/shader.o \
 $(OUT)/unix-x11/groufix/errors.o \
 $(OUT)/unix-x11/groufix/events.o \
 $(OUT)/unix-x11/groufix/extensions.o \
 $(OUT)/unix-x11/groufix/hardware.o \
 $(OUT)/unix-x11/groufix/math.o \
 $(OUT)/unix-x11/groufix/window.o \
 $(OUT)/unix-x11/groufix.o

unix-x11: before-unix-x11 $(OBJS_UNIX_X11)
	$(CC) -shared $(OBJS_UNIX_X11) -o $(BIN)/unix-x11/libGroufix.so $(LIBS_UNIX_X11)

unix-x11-minimal: examples/minimal.c unix-x11 
	$(CC) $(CFLAGS) $< -o $(BIN)/unix-x11/minimal -L$(BIN)/unix-x11/ -Wl,-rpath='$$ORIGIN' -lGroufix

unix-x11-simple: examples/simple.c unix-x11 
	$(CC) $(CFLAGS) $< -o $(BIN)/unix-x11/simple -L$(BIN)/unix-x11/ -Wl,-rpath='$$ORIGIN' -lGroufix

before-unix-x11:
	mkdir -p $(BIN)/unix-x11
	mkdir -p $(OUT)/unix-x11/groufix/containers
	mkdir -p $(OUT)/unix-x11/groufix/geometry
	mkdir -p $(OUT)/unix-x11/groufix/memory
	mkdir -p $(OUT)/unix-x11/groufix/pipeline
	mkdir -p $(OUT)/unix-x11/groufix/platform
	mkdir -p $(OUT)/unix-x11/groufix/shading


# All the object files

$(OUT)/unix-x11/groufix/containers/deque.o: $(SRC)/groufix/containers/deque.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/containers/list.o: $(SRC)/groufix/containers/list.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/containers/vector.o: $(SRC)/groufix/containers/vector.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/geometry/layout.o: $(SRC)/groufix/geometry/layout.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/memory/buffer.o: $(SRC)/groufix/memory/buffer.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/memory/formats.o: $(SRC)/groufix/memory/formats.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/memory/texture.o: $(SRC)/groufix/memory/texture.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/pipeline/bucket.o: $(SRC)/groufix/pipeline/bucket.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/pipeline/pipeline.o: $(SRC)/groufix/pipeline/pipeline.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/platform/x11_context.o: $(SRC)/groufix/platform/x11_context.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/platform/x11_init.o: $(SRC)/groufix/platform/x11_init.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/platform/x11_screen.o: $(SRC)/groufix/platform/x11_screen.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/platform/x11_window.o: $(SRC)/groufix/platform/x11_window.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/shading/program.o: $(SRC)/groufix/shading/program.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/shading/shader.o: $(SRC)/groufix/shading/shader.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/errors.o: $(SRC)/groufix/errors.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/events.o: $(SRC)/groufix/events.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/extensions.o: $(SRC)/groufix/extensions.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/hardware.o: $(SRC)/groufix/hardware.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/math.o: $(SRC)/groufix/math.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/window.o: $(SRC)/groufix/window.c $(HEADERS_X11)
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
 $(OUT)/win32/groufix/geometry/layout.o \
 $(OUT)/win32/groufix/memory/buffer.o \
 $(OUT)/win32/groufix/memory/formats.o \
 $(OUT)/win32/groufix/memory/texture.o \
 $(OUT)/win32/groufix/pipeline/bucket.o \
 $(OUT)/win32/groufix/pipeline/pipeline.o \
 $(OUT)/win32/groufix/platform/win32_context.o \
 $(OUT)/win32/groufix/platform/win32_init.o \
 $(OUT)/win32/groufix/platform/win32_screen.o \
 $(OUT)/win32/groufix/platform/win32_window.o \
 $(OUT)/win32/groufix/shading/program.o \
 $(OUT)/win32/groufix/shading/shader.o \
 $(OUT)/win32/groufix/errors.o \
 $(OUT)/win32/groufix/events.o \
 $(OUT)/win32/groufix/extensions.o \
 $(OUT)/win32/groufix/hardware.o \
 $(OUT)/win32/groufix/math.o \
 $(OUT)/win32/groufix/window.o \
 $(OUT)/win32/groufix.o

win32: before-win32 $(OBJS_WIN32)
	$(CC) -shared $(OBJS_WIN32) -o $(BIN)/win32/libGroufix.dll $(LIBS_WIN32)

win32-minimal: examples/minimal.c win32
	$(CC) $(CFLAGS) $< -o $(BIN)/win32/minimal -L$(BIN)/win32/ -lGroufix

win32-simple: examples/simple.c win32
	$(CC) $(CFLAGS) $< -o $(BIN)/win32/simple -L$(BIN)/win32/ -lGroufix

before-win32:
	if not exist $(BIN)\win32\nul mkdir $(BIN)\win32
	if not exist $(OUT)\win32\groufix\containers\nul mkdir $(OUT)\win32\groufix\containers
	if not exist $(OUT)\win32\groufix\geometry\nul mkdir $(OUT)\win32\groufix\geometry
	if not exist $(OUT)\win32\groufix\memory\nul mkdir $(OUT)\win32\groufix\memory
	if not exist $(OUT)\win32\groufix\pipeline\nul mkdir $(OUT)\win32\groufix\pipeline
	if not exist $(OUT)\win32\groufix\platform\nul mkdir $(OUT)\win32\groufix\platform
	if not exist $(OUT)\win32\groufix\shading\nul mkdir $(OUT)\win32\groufix\shading


# All the object files

$(OUT)/win32/groufix/containers/deque.o: $(SRC)/groufix/containers/deque.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/containers/list.o: $(SRC)/groufix/containers/list.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/containers/vector.o: $(SRC)/groufix/containers/vector.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/geometry/layout.o: $(SRC)/groufix/geometry/layout.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/memory/buffer.o: $(SRC)/groufix/memory/buffer.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/memory/formats.o: $(SRC)/groufix/memory/formats.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/memory/texture.o: $(SRC)/groufix/memory/texture.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/pipeline/bucket.o: $(SRC)/groufix/pipeline/bucket.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/pipeline/pipeline.o: $(SRC)/groufix/pipeline/pipeline.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/platform/win32_context.o: $(SRC)/groufix/platform/win32_context.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/platform/win32_init.o: $(SRC)/groufix/platform/win32_init.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/platform/win32_screen.o: $(SRC)/groufix/platform/win32_screen.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/platform/win32_window.o: $(SRC)/groufix/platform/win32_window.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/shading/program.o: $(SRC)/groufix/shading/program.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/shading/shader.o: $(SRC)/groufix/shading/shader.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/errors.o: $(SRC)/groufix/errors.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/events.o: $(SRC)/groufix/events.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/extensions.o: $(SRC)/groufix/extensions.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/hardware.o: $(SRC)/groufix/hardware.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/math.o: $(SRC)/groufix/math.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/window.o: $(SRC)/groufix/window.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix.o: $(SRC)/groufix.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

