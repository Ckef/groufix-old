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
	@echo "  $(MAKE) clean            Clean temporary files."
	@echo "  $(MAKE) clean-all        Clean all files $(MAKE) produced."
	@echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	@echo "  $(MAKE) unix-x11         Build the Groufix Unix target using X11."
	@echo "  $(MAKE) unix-x11-simple  Build the simple example Unix target using X11."
	@echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	@echo "  $(MAKE) osx-x11          Build the Groufix OS X target using X11."
	@echo "  $(MAKE) osx-x11-simple   Build the simple example OS X target using X11."
	@echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	@echo "  $(MAKE) win32            Build the Groufix Windows target."
	@echo "  $(MAKE) win32-simple     Build the simple example Windows target."
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
OBJFLAGS_OSX_X11  = $(OBJFLAGS) -fPIC -I/usr/X11/include
OBJFLAGS_WIN32    = $(OBJFLAGS) -s

# Libraries to link to
LIBS_UNIX_X11 = -lX11 -lGL
LIBS_OSX_X11  = -L/usr/X11/lib -lX11 -lGL
LIBS_WIN32    = -lopengl32 -lgdi32


#################################################################
# Header files for all window APIs (platforms)
#################################################################
HEADERS = \
 $(INCLUDE)/groufix/containers/deque.h \
 $(INCLUDE)/groufix/containers/vector.h \
 $(INCLUDE)/groufix/math/mat.h \
 $(INCLUDE)/groufix/math/quat.h \
 $(INCLUDE)/groufix/math/vec.h \
 $(INCLUDE)/groufix/errors.h \
 $(INCLUDE)/groufix/hardware.h \
 $(INCLUDE)/groufix/keys.h \
 $(INCLUDE)/groufix/math.h \
 $(INCLUDE)/groufix/screen.h \
 $(INCLUDE)/groufix/utils.h \
 $(INCLUDE)/groufix/window.h \
 $(INCLUDE)/groufix.h

HEADERS_LIB = \
 $(HEADERS) \
 $(DEPEND)/GL/glcorearb.h \
 $(SRC)/groufix/internal.h \
 $(SRC)/groufix/platform.h

HEADERS_WIN32 = \
 $(HEADERS_LIB) \
 $(DEPEND)/GL/wglext.h \
 $(SRC)/groufix/platform/win32.h

HEADERS_X11 = \
 $(HEADERS_LIB) \
 $(DEPEND)/GL/glxext.h \
 $(SRC)/groufix/platform/x11.h


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
 $(OUT)/unix-x11/groufix/containers/vector.o \
 $(OUT)/unix-x11/groufix/hardware/buffer.o \
 $(OUT)/unix-x11/groufix/platform/x11_context.o \
 $(OUT)/unix-x11/groufix/platform/x11_init.o \
 $(OUT)/unix-x11/groufix/platform/x11_screen.o \
 $(OUT)/unix-x11/groufix/platform/x11_window.o \
 $(OUT)/unix-x11/groufix/errors.o \
 $(OUT)/unix-x11/groufix/events.o \
 $(OUT)/unix-x11/groufix/extensions.o \
 $(OUT)/unix-x11/groufix/hardware.o \
 $(OUT)/unix-x11/groufix/math.o \
 $(OUT)/unix-x11/groufix/screen.o \
 $(OUT)/unix-x11/groufix/window.o \
 $(OUT)/unix-x11/groufix.o

unix-x11: before-unix-x11 $(OBJS_UNIX_X11)
	$(CC) -shared $(OBJS_UNIX_X11) -o $(BIN)/unix-x11/libGroufix.so $(LIBS_UNIX_X11)

unix-x11-simple: examples/simple.c unix-x11 
	$(CC) $(CFLAGS) $< -o $(BIN)/unix-x11/simple -L$(BIN)/unix-x11/ -Wl,-rpath='$$ORIGIN' -lGroufix

before-unix-x11:
	mkdir -p $(BIN)/unix-x11
	mkdir -p $(OUT)/unix-x11/groufix/containers
	mkdir -p $(OUT)/unix-x11/groufix/hardware
	mkdir -p $(OUT)/unix-x11/groufix/platform


# All the object files

$(OUT)/unix-x11/groufix/containers/deque.o: $(SRC)/groufix/containers/deque.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/containers/vector.o: $(SRC)/groufix/containers/vector.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/hardware/buffer.o: $(SRC)/groufix/hardware/buffer.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/platform/x11_context.o: $(SRC)/groufix/platform/x11_context.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/platform/x11_init.o: $(SRC)/groufix/platform/x11_init.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/platform/x11_screen.o: $(SRC)/groufix/platform/x11_screen.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/platform/x11_window.o: $(SRC)/groufix/platform/x11_window.c $(HEADERS_X11)
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

$(OUT)/unix-x11/groufix/screen.o: $(SRC)/groufix/screen.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix/window.o: $(SRC)/groufix/window.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@

$(OUT)/unix-x11/groufix.o: $(SRC)/groufix.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_UNIX_X11) $< -o $@


#################################################################
# OS X X11 builds
#################################################################
OBJS_OSX_X11 = \
 $(OUT)/osx-x11/groufix/containers/deque.o \
 $(OUT)/osx-x11/groufix/containers/vector.o \
 $(OUT)/osx-x11/groufix/hardware/buffer.o \
 $(OUT)/osx-x11/groufix/platform/x11_context.o \
 $(OUT)/osx-x11/groufix/platform/x11_init.o \
 $(OUT)/osx-x11/groufix/platform/x11_screen.o \
 $(OUT)/osx-x11/groufix/platform/x11_window.o \
 $(OUT)/osx-x11/groufix/errors.o \
 $(OUT)/osx-x11/groufix/events.o \
 $(OUT)/osx-x11/groufix/extensions.o \
 $(OUT)/osx-x11/groufix/hardware.o \
 $(OUT)/osx-x11/groufix/math.o \
 $(OUT)/osx-x11/groufix/screen.o \
 $(OUT)/osx-x11/groufix/window.o \
 $(OUT)/osx-x11/groufix.o

osx-x11: before-osx-x11 $(OBJS_OSX_X11)
	$(CC) -dynamiclib -install_name 'libGroufix.dylib' $(OBJS_OSX_X11) -o $(BIN)/osx-x11/libGroufix.dylib $(LIBS_OSX_X11)

osx-x11-simple: examples/simple.c osx-x11
	$(CC) $(CFLAGS) $< -o $(BIN)/osx-x11/simple -L$(BIN)/osx-x11/ -lGroufix

before-osx-x11:
	mkdir -p $(BIN)/osx-x11
	mkdir -p $(OUT)/osx-x11/groufix/containers
	mkdir -p $(OUT)/osx-x11/groufix/hardware
	mkdir -p $(OUT)/osx-x11/groufix/platform


# All the object files

$(OUT)/osx-x11/groufix/containers/deque.o: $(SRC)/groufix/containers/deque.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_OSX_X11) $< -o $@

$(OUT)/osx-x11/groufix/containers/vector.o: $(SRC)/groufix/containers/vector.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_OSX_X11) $< -o $@

$(OUT)/osx-x11/groufix/hardware/buffer.o: $(SRC)/groufix/hardware/buffer.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_OSX_X11) $< -o $@

$(OUT)/osx-x11/groufix/platform/x11_context.o: $(SRC)/groufix/platform/x11_context.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_OSX_X11) $< -o $@

$(OUT)/osx-x11/groufix/platform/x11_init.o: $(SRC)/groufix/platform/x11_init.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_OSX_X11) $< -o $@

$(OUT)/osx-x11/groufix/platform/x11_screen.o: $(SRC)/groufix/platform/x11_screen.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_OSX_X11) $< -o $@

$(OUT)/osx-x11/groufix/platform/x11_window.o: $(SRC)/groufix/platform/x11_window.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_OSX_X11) $< -o $@

$(OUT)/osx-x11/groufix/errors.o: $(SRC)/groufix/errors.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_OSX_X11) $< -o $@

$(OUT)/osx-x11/groufix/events.o: $(SRC)/groufix/events.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_OSX_X11) $< -o $@

$(OUT)/osx-x11/groufix/extensions.o: $(SRC)/groufix/extensions.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_OSX_X11) $< -o $@

$(OUT)/osx-x11/groufix/hardware.o: $(SRC)/groufix/hardware.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_OSX_X11) $< -o $@

$(OUT)/osx-x11/groufix/math.o: $(SRC)/groufix/math.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_OSX_X11) $< -o $@

$(OUT)/osx-x11/groufix/screen.o: $(SRC)/groufix/screen.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_OSX_X11) $< -o $@

$(OUT)/osx-x11/groufix/window.o: $(SRC)/groufix/window.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_OSX_X11) $< -o $@

$(OUT)/osx-x11/groufix.o: $(SRC)/groufix.c $(HEADERS_X11)
	$(CC) $(OBJFLAGS_OSX_X11) $< -o $@


#################################################################
# Windows builds
#################################################################
OBJS_WIN32 = \
 $(OUT)/win32/groufix/containers/deque.o \
 $(OUT)/win32/groufix/containers/vector.o \
 $(OUT)/win32/groufix/hardware/buffer.o \
 $(OUT)/win32/groufix/platform/win32_context.o \
 $(OUT)/win32/groufix/platform/win32_init.o \
 $(OUT)/win32/groufix/platform/win32_screen.o \
 $(OUT)/win32/groufix/platform/win32_window.o \
 $(OUT)/win32/groufix/errors.o \
 $(OUT)/win32/groufix/events.o \
 $(OUT)/win32/groufix/extensions.o \
 $(OUT)/win32/groufix/hardware.o \
 $(OUT)/win32/groufix/math.o \
 $(OUT)/win32/groufix/screen.o \
 $(OUT)/win32/groufix/window.o \
 $(OUT)/win32/groufix.o

win32: before-win32 $(OBJS_WIN32)
	$(CC) -shared $(OBJS_WIN32) -o $(BIN)/win32/libGroufix.dll $(LIBS_WIN32)

win32-simple: examples/simple.c win32
	$(CC) $(CFLAGS) $< -o $(BIN)/win32/simple -L$(BIN)/win32/ -lGroufix

before-win32:
	if not exist $(BIN)\win32\nul mkdir $(BIN)\win32
	if not exist $(OUT)\win32\groufix\containers\nul mkdir $(OUT)\win32\groufix\containers
	if not exist $(OUT)\win32\groufix\hardware\nul mkdir $(OUT)\win32\groufix\hardware
	if not exist $(OUT)\win32\groufix\platform\nul mkdir $(OUT)\win32\groufix\platform


# All the object files

$(OUT)/win32/groufix/containers/deque.o: $(SRC)/groufix/containers/deque.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/containers/vector.o: $(SRC)/groufix/containers/vector.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/hardware/buffer.o: $(SRC)/groufix/hardware/buffer.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/platform/win32_context.o: $(SRC)/groufix/platform/win32_context.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/platform/win32_init.o: $(SRC)/groufix/platform/win32_init.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/platform/win32_screen.o: $(SRC)/groufix/platform/win32_screen.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/platform/win32_window.o: $(SRC)/groufix/platform/win32_window.c $(HEADERS_WIN32)
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

$(OUT)/win32/groufix/screen.o: $(SRC)/groufix/screen.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/window.o: $(SRC)/groufix/window.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix.o: $(SRC)/groufix.c $(HEADERS_WIN32)
	$(CC) $(OBJFLAGS_WIN32) $< -o $@

