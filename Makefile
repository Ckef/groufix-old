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
INCLUDE = include
SRC     = src

# Executables as well
CFLAGS = -Os -O2 -Wall -std=c99 -I$(INCLUDE)

# Object files only
CFLAGS_UNIX  = $(CFLAGS) -c -fPIC -s
CFLAGS_OSX   = $(CFLAGS) -c -fPIC
CFLAGS_WIN32 = $(CFLAGS) -c -s


#################################################################
# Header files for all window APIs (platforms)
#################################################################
HEADERS = \
 $(INCLUDE)/GL/glcorearb.h \
 $(INCLUDE)/groufix/math/mat.h \
 $(INCLUDE)/groufix/math/quat.h \
 $(INCLUDE)/groufix/math/vec.h \
 $(INCLUDE)/groufix/events.h \
 $(INCLUDE)/groufix/keys.h \
 $(INCLUDE)/groufix/math.h \
 $(INCLUDE)/groufix/platform.h \
 $(INCLUDE)/groufix/utils.h \
 $(INCLUDE)/groufix.h

HEADERS_WIN32 = \
 $(HEADERS) \
 $(INCLUDE)/groufix/platform/win32.h

HEADERS_X11 = \
 $(HEADERS) \
 $(INCLUDE)/groufix/platform/x11.h


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
 $(OUT)/unix-x11/groufix/platform/x11_context.o \
 $(OUT)/unix-x11/groufix/platform/x11_init.o \
 $(OUT)/unix-x11/groufix/platform/x11_screen.o \
 $(OUT)/unix-x11/groufix/platform/x11_window.o \
 $(OUT)/unix-x11/groufix/events.o \
 $(OUT)/unix-x11/groufix/math.o \
 $(OUT)/unix-x11/groufix.o

unix-x11: before-unix-x11 $(OBJS_UNIX_X11)
	$(CC) -shared $(OBJS_UNIX_X11) -o $(BIN)/unix-x11/libGroufix.so -lX11 -lGL

unix-x11-simple: examples/simple.c unix-x11 
	$(CC) $(CFLAGS) $< -o $(BIN)/unix-x11/simple -L$(BIN)/unix-x11/ -Wl,-rpath='$$ORIGIN' -lGroufix

before-unix-x11:
	mkdir -p $(BIN)/unix-x11
	mkdir -p $(OUT)/unix-x11/groufix/platform


# All the object files

$(OUT)/unix-x11/groufix/platform/x11_context.o: $(SRC)/groufix/platform/x11_context.c $(HEADERS_X11)
	$(CC) $(CFLAGS_UNIX) $< -o $@

$(OUT)/unix-x11/groufix/platform/x11_init.o: $(SRC)/groufix/platform/x11_init.c $(HEADERS_X11)
	$(CC) $(CFLAGS_UNIX) $< -o $@

$(OUT)/unix-x11/groufix/platform/x11_screen.o: $(SRC)/groufix/platform/x11_screen.c $(HEADERS_X11)
	$(CC) $(CFLAGS_UNIX) $< -o $@

$(OUT)/unix-x11/groufix/platform/x11_window.o: $(SRC)/groufix/platform/x11_window.c $(HEADERS_X11)
	$(CC) $(CFLAGS_UNIX) $< -o $@

$(OUT)/unix-x11/groufix/events.o: $(SRC)/groufix/events.c $(HEADERS_X11)
	$(CC) $(CFLAGS_UNIX) $< -o $@

$(OUT)/unix-x11/groufix/math.o: $(SRC)/groufix/math.c $(HEADERS_X11)
	$(CC) $(CFLAGS_UNIX) $< -o $@

$(OUT)/unix-x11/groufix.o: $(SRC)/groufix.c $(HEADERS_X11)
	$(CC) $(CFLAGS_UNIX) $< -o $@


#################################################################
# OS X X11 builds
#################################################################
OBJS_OSX_X11 = \
 $(OUT)/osx-x11/groufix/platform/x11_context.o \
 $(OUT)/osx-x11/groufix/platform/x11_init.o \
 $(OUT)/osx-x11/groufix/platform/x11_screen.o \
 $(OUT)/osx-x11/groufix/platform/x11_window.o \
 $(OUT)/osx-x11/groufix/events.o \
 $(OUT)/osx-x11/groufix/math.o \
 $(OUT)/osx-x11/groufix.o

osx-x11: before-osx-x11 $(OBJS_OSX_X11)
	$(CC) -dynamiclib $(OBJS_OSX_X11) -o $(BIN)/osx-x11/libGroufix.dylib -lX11 -lGL

osx-x11-simple: examples/simple.c osx-x11
	$(CC) $(CFLAGS) -DGFX_OSX_X11 $< -o $(BIN)/osx-x11/simple -L$(BIN)/osx-x11/ -lGroufix

before-osx-x11:
	mkdir -p $(BIN)/osx-x11
	mkdir -p $(OUT)/osx-x11/groufix/platform


# All the object files

$(OUT)/osx-x11/groufix/platform/x11_context.o: $(SRC)/groufix/platform/x11_context.c $(HEADERS_X11)
	$(CC) $(CFLAGS_OSX) -DGFX_OSX_X11 $< -o $@

$(OUT)/osx-x11/groufix/platform/x11_init.o: $(SRC)/groufix/platform/x11_init.c $(HEADERS_X11)
	$(CC) $(CFLAGS_OSX) -DGFX_OSX_X11 $< -o $@

$(OUT)/osx-x11/groufix/platform/x11_screen.o: $(SRC)/groufix/platform/x11_screen.c $(HEADERS_X11)
	$(CC) $(CFLAGS_OSX) -DGFX_OSX_X11 $< -o $@

$(OUT)/osx-x11/groufix/platform/x11_window.o: $(SRC)/groufix/platform/x11_window.c $(HEADERS_X11)
	$(CC) $(CFLAGS_OSX) -DGFX_OSX_X11 $< -o $@

$(OUT)/osx-x11/groufix/events.o: $(SRC)/groufix/events.c $(HEADERS_X11)
	$(CC) $(CFLAGS_OSX) -DGFX_OSX_X11 $< -o $@

$(OUT)/osx-x11/groufix/math.o: $(SRC)/groufix/math.c $(HEADERS_X11)
	$(CC) $(CFLAGS_OSX) -DGFX_OSX_X11 $< -o $@

$(OUT)/osx-x11/groufix.o: $(SRC)/groufix.c $(HEADERS_X11)
	$(CC) $(CFLAGS_OSX) -DGFX_OSX_X11 $< -o $@


#################################################################
# Windows builds
#################################################################
OBJS_WIN32 = \
 $(OUT)/win32/groufix/platform/win32_context.o \
 $(OUT)/win32/groufix/platform/win32_init.o \
 $(OUT)/win32/groufix/platform/win32_screen.o \
 $(OUT)/win32/groufix/platform/win32_window.o \
 $(OUT)/win32/groufix/events.o \
 $(OUT)/win32/groufix/math.o \
 $(OUT)/win32/groufix.o

win32: before-win32 $(OBJS_WIN32)
	$(CC) -shared $(OBJS_WIN32) -o $(BIN)/win32/libGroufix.dll -luser32 -lopengl32

win32-simple: examples/simple.c win32
	$(CC) $(CFLAGS) $< -o $(BIN)/win32/simple -L$(BIN)/win32/ -lGroufix

before-win32:
	if not exist $(BIN)\nul mkdir $(BIN)\win32
	if not exist $(OUT)\nul mkdir $(OUT)\win32\groufix\platform


# All the object files

$(OUT)/win32/groufix/platform/win32_context.o: $(SRC)/groufix/platform/win32_context.c $(HEADERS_WIN32)
	$(CC) $(CFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/platform/win32_init.o: $(SRC)/groufix/platform/win32_init.c $(HEADERS_WIN32)
	$(CC) $(CFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/platform/win32_screen.o: $(SRC)/groufix/platform/win32_screen.c $(HEADERS_WIN32)
	$(CC) $(CFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/platform/win32_window.o: $(SRC)/groufix/platform/win32_window.c $(HEADERS_WIN32)
	$(CC) $(CFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/events.o: $(SRC)/groufix/events.c $(HEADERS_WIN32)
	$(CC) $(CFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix/math.o: $(SRC)/groufix/math.c $(HEADERS_WIN32)
	$(CC) $(CFLAGS_WIN32) $< -o $@

$(OUT)/win32/groufix.o: $(SRC)/groufix.c $(HEADERS_WIN32)
	$(CC) $(CFLAGS_WIN32) $< -o $@

