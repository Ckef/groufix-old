/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#include "groufix/core/platform/x11.h"
#include "groufix/core/errors.h"

#include <stdlib.h>
#include <string.h>

/******************************************************/
/* Instance */
GFX_X11_Connection* _gfx_x11 = NULL;


/******************************************************/
static int _gfx_x11_is_extension_supported(

		int          screenNumber,
		const char*  ext)
{
	/* Get extensions */
	const char* extensions = glXQueryExtensionsString(
		_gfx_x11->display,
		screenNumber
	);

	/* Get extension length */
	size_t len = strlen(ext);
	if(!extensions || !len) return 0;

	/* Try to find a complete match */
	char* found = strstr(extensions, ext);
	while(found)
	{
		char* end = found + len;
		if(*end == ' ' || *end == '\0')
		{
			/* To avoid segfault */
			if(found == extensions) return 1;
			if(*(found - 1) == ' ') return 1;
		}
		found = strstr(end, ext);
	}

	return 0;
}

/******************************************************/
static int _gfx_x11_load_extensions(void)
{
	int num = XDefaultScreen(_gfx_x11->display);

	/* Check all vital extensions */
	if(
		!_gfx_x11_is_extension_supported(num, "GLX_ARB_get_proc_address") ||
		!_gfx_x11_is_extension_supported(num, "GLX_ARB_create_context") ||
		!_gfx_x11_is_extension_supported(num, "GLX_ARB_create_context_profile"))
	{
		gfx_errors_push(GFX_ERROR_INCOMPATIBLE_CONTEXT, "Vital GLX extensions are missing.");
		return 0;
	}

	/* Load all functions */
	_gfx_x11->extensions.CreateContextAttribsARB =
		(PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");
	_gfx_x11->extensions.SwapIntervalEXT =
		(PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddressARB((const GLubyte*)"glXSwapIntervalEXT");
	_gfx_x11->extensions.EXT_swap_control_tear =
		_gfx_x11_is_extension_supported(num, "GLX_EXT_swap_control_tear") ? 1 : 0;

	/* Check non-vital extensions */
	if(!_gfx_x11_is_extension_supported(num, "GLX_EXT_swap_control"))
		_gfx_x11->extensions.SwapIntervalEXT = NULL;

	return 1;
}

/******************************************************/
static int _gfx_x11_error_handler(

		Display*      display,
		XErrorEvent*  evt)
{
	if(_gfx_x11->errors)
	{
		size_t length = sizeof(char) * (GFX_X11_ERROR_LENGTH);
		char* text = malloc(length);

		XGetErrorText(display, evt->error_code, text, length);

		/* Make sure it's null terminated */
		text[GFX_X11_ERROR_LENGTH - 1] = 0;

		gfx_errors_push(GFX_ERROR_PLATFORM_ERROR, text);

		free(text);
	}

	return 0;
}

/******************************************************/
static int _gfx_x11_init_monitors(

		unsigned int  major,
		unsigned int  minor)
{
	/* Iterate over all screens */
	Screen* def = XDefaultScreenOfDisplay(_gfx_x11->display);
	unsigned int count = XScreenCount(_gfx_x11->display);

	while(count--)
	{
		/* Get screen resources */
		Screen* scr =
			XScreenOfDisplay(_gfx_x11->display, count);
		Window root =
			XRootWindowOfScreen(scr);
		XRRScreenResources* res =
			XRRGetScreenResources(_gfx_x11->display, root);
		RROutput prim =
			res->outputs[0];

		if(major > 1 || (major == 1 && minor > 2))
			prim = XRRGetOutputPrimary(_gfx_x11->display, root);

		/* Iterate through outputs */
		unsigned int i;
		for(i = 0; i < res->noutput; ++i)
		{
			/* Validate output */
			XRROutputInfo* out =
				XRRGetOutputInfo(_gfx_x11->display, res, res->outputs[i]);

			if(out->connection != RR_Connected)
			{
				XRRFreeOutputInfo(out);
				continue;
			}

			/* Create new monitor */
			XRRCrtcInfo* crtc =
				XRRGetCrtcInfo(_gfx_x11->display, res, out->crtc);
			int rot =
				crtc->rotation & (RR_Rotate_90 | RR_Rotate_270);

			GFX_X11_Monitor mon =
			{
				.screen = scr,
				.crtc   = out->crtc,
				.x      = crtc->x,
				.y      = crtc->y,
				.width  = rot ? crtc->height : crtc->width,
				.height = rot ? crtc->width : crtc->height
			};

			/* Insert at beginning if primary */
			GFXVectorIterator monPos =
				scr == def && res->outputs[i] == prim ?
				_gfx_x11->monitors.begin : _gfx_x11->monitors.end;

			gfx_vector_insert(&_gfx_x11->monitors, &mon, monPos);

			XRRFreeCrtcInfo(crtc);
			XRRFreeOutputInfo(out);
		}

		XRRFreeScreenResources(res);
	}

	/* Need at least one monitor */
	return _gfx_x11->monitors.begin != _gfx_x11->monitors.end;
}

/******************************************************/
static GFXKey _gfx_x11_get_key(

		KeySym symbol)
{
	/* Unicode numbers */
	if(symbol >= XK_0 && symbol <= XK_9)
		return (GFXKey)(symbol - XK_0 + GFX_KEY_0);

	/* Keypad numbers */
	if(symbol >= XK_KP_0 && symbol <= XK_KP_9)
		return (GFXKey)(symbol - XK_KP_0 + GFX_KEY_KP_0);

	/* Unicode capitals */
	if(symbol >= XK_A && symbol <= XK_Z)
		return (GFXKey)(symbol - XK_A + GFX_KEY_A);

	/* Unicode lowercase */
	if(symbol >= XK_a && symbol <= XK_z)
		return (GFXKey)(symbol - XK_a + GFX_KEY_A);

	/* Function keys */
	if(symbol >= XK_F1 && symbol <= XK_F24)
		return (GFXKey)(symbol - XK_F1 + GFX_KEY_F1);

	/* Non-unicode */
	switch(symbol)
	{
		case XK_VoidSymbol   : return GFX_KEY_UNKNOWN;

		case XK_BackSpace    : return GFX_KEY_BACKSPACE;
		case XK_Tab          : return GFX_KEY_TAB;
		case XK_KP_Tab       : return GFX_KEY_TAB;
		case XK_Clear        : return GFX_KEY_CLEAR;
		case XK_Return       : return GFX_KEY_RETURN;
		case XK_Pause        : return GFX_KEY_PAUSE;
		case XK_Scroll_Lock  : return GFX_KEY_SCROLL_LOCK;
		case XK_Escape       : return GFX_KEY_ESCAPE;
		case XK_Delete       : return GFX_KEY_DELETE;
		case XK_KP_Delete    : return GFX_KEY_DELETE;

		case XK_Home         : return GFX_KEY_HOME;
		case XK_KP_Home      : return GFX_KEY_HOME;
		case XK_Left         : return GFX_KEY_LEFT;
		case XK_KP_Left      : return GFX_KEY_LEFT;
		case XK_Up           : return GFX_KEY_UP;
		case XK_KP_Up        : return GFX_KEY_UP;
		case XK_Right        : return GFX_KEY_RIGHT;
		case XK_KP_Right     : return GFX_KEY_RIGHT;
		case XK_Down         : return GFX_KEY_DOWN;
		case XK_KP_Down      : return GFX_KEY_DOWN;
		case XK_Page_Down    : return GFX_KEY_PAGE_DOWN;
		case XK_KP_Page_Down : return GFX_KEY_PAGE_DOWN;
		case XK_Page_Up      : return GFX_KEY_PAGE_UP;
		case XK_KP_Page_Up   : return GFX_KEY_PAGE_UP;
		case XK_End          : return GFX_KEY_END;
		case XK_KP_End       : return GFX_KEY_END;

		case XK_Select       : return GFX_KEY_SELECT;
		case XK_Print        : return GFX_KEY_PRINT;
		case XK_Execute      : return GFX_KEY_EXECUTE;
		case XK_Insert       : return GFX_KEY_INSERT;
		case XK_KP_Insert    : return GFX_KEY_INSERT;
		case XK_Menu         : return GFX_KEY_MENU;
		case XK_Cancel       : return GFX_KEY_CANCEL;
		case XK_Help         : return GFX_KEY_HELP;
		case XK_Num_Lock     : return GFX_KEY_NUM_LOCK;
		case XK_KP_Space     : return GFX_KEY_SPACE;
		case XK_space        : return GFX_KEY_SPACE;

		case XK_KP_Enter     : return GFX_KEY_KP_RETURN;
		case XK_KP_Multiply  : return GFX_KEY_KP_MULTIPLY;
		case XK_KP_Add       : return GFX_KEY_KP_ADD;
		case XK_KP_Separator : return GFX_KEY_KP_SEPARATOR;
		case XK_KP_Subtract  : return GFX_KEY_KP_SUBTRACT;
		case XK_KP_Decimal   : return GFX_KEY_KP_DECIMAL;
		case XK_KP_Divide    : return GFX_KEY_KP_DIVIDE;

		case XK_Shift_L      : return GFX_KEY_SHIFT_LEFT;
		case XK_Shift_R      : return GFX_KEY_SHIFT_RIGHT;
		case XK_Control_L    : return GFX_KEY_CONTROL_LEFT;
		case XK_Control_R    : return GFX_KEY_CONTROL_RIGHT;
		case XK_Alt_L        : return GFX_KEY_ALT_LEFT;
		case XK_Alt_R        : return GFX_KEY_ALT_RIGHT;
		case XK_Super_L      : return GFX_KEY_SUPER_LEFT;
		case XK_Super_R      : return GFX_KEY_SUPER_RIGHT;
	}

	return GFX_KEY_UNKNOWN;
}

/******************************************************/
static void _gfx_x11_create_key_table(void)
{
	/* Get permitted keycodes and their symbols */
	int minKey, maxKey;
	XDisplayKeycodes(_gfx_x11->display, &minKey, &maxKey);
	maxKey = maxKey > GFX_X11_MAX_KEYCODE ? GFX_X11_MAX_KEYCODE : maxKey;

	int numKeys = maxKey - minKey + 1;

	int symbolsPerKey;
	KeySym* symbols = XGetKeyboardMapping(
		_gfx_x11->display,
		minKey,
		numKeys,
		&symbolsPerKey
	);

	/* Use the first symbol of all keycodes */
	size_t i;
	for(i = minKey; i <= maxKey; ++i) _gfx_x11->keys[i] = _gfx_x11_get_key(
		symbols[(i - minKey) * symbolsPerKey]);

	XFree(symbols);
}

/******************************************************/
GFX_X11_Window* _gfx_x11_get_window_from_handle(

		Window handle)
{
	if(!_gfx_x11) return NULL;

	GFX_X11_Window* it;
	for(
		it = _gfx_x11->windows.begin;
		it != _gfx_x11->windows.end;
		it = gfx_vector_next(&_gfx_x11->windows, it))
	{
		if(it->handle == handle) break;
	}

	return it != _gfx_x11->windows.end ? it : NULL;
}

/******************************************************/
GFX_X11_Window* _gfx_x11_get_window_from_context(

		GLXContext context)
{
	if(!_gfx_x11) return NULL;

	GFX_X11_Window* it;
	for(
		it = _gfx_x11->windows.begin;
		it != _gfx_x11->windows.end;
		it = gfx_vector_next(&_gfx_x11->windows, it))
	{
		if(it->context == context) break;
	}

	return it != _gfx_x11->windows.end ? it : NULL;
}

/******************************************************/
int _gfx_platform_init(void)
{
	if(!_gfx_x11)
	{
		/* Allocate */
		_gfx_x11 = calloc(1, sizeof(GFX_X11_Connection));
		if(!_gfx_x11) return 0;

		/* Connect to X Server */
		_gfx_x11->display = XOpenDisplay(NULL);

		_gfx_x11->errors = 1;
		XSetErrorHandler(_gfx_x11_error_handler);

		/* Load extensions */
		/* RandR 1.2 is required */
		int major;
		int minor;

		if(
			!_gfx_x11->display ||
			!_gfx_x11_load_extensions() ||
			!XRRQueryVersion(_gfx_x11->display, &major, &minor) ||
			major < 1 ||
			(major == 1 && minor < 2))
		{
			if(_gfx_x11->display) XCloseDisplay(_gfx_x11->display);

			free(_gfx_x11);
			_gfx_x11 = NULL;

			return 0;
		}

		/* Setup memory */
		gfx_vector_init(&_gfx_x11->monitors, sizeof(GFX_X11_Monitor));
		gfx_vector_init(&_gfx_x11->windows, sizeof(GFX_X11_Window));

		/* Init monitors */
		if(!_gfx_x11_init_monitors(major, minor))
		{
			_gfx_platform_terminate();
			return 0;
		}

		/* Load atoms */
		_gfx_x11->MOTIF_WM_HINTS =
			XInternAtom(_gfx_x11->display, "_MOTIF_WM_HINTS", False);
		_gfx_x11->NET_WM_BYPASS_COMPOSITOR =
			XInternAtom(_gfx_x11->display, "_NET_WM_BYPASS_COMPOSITOR", False);
		_gfx_x11->NET_WM_STATE =
			XInternAtom(_gfx_x11->display, "_NET_WM_STATE", False);
		_gfx_x11->NET_WM_STATE_ABOVE =
			XInternAtom(_gfx_x11->display, "_NET_WM_STATE_ABOVE", False);
		_gfx_x11->WM_DELETE_WINDOW =
			XInternAtom(_gfx_x11->display, "WM_DELETE_WINDOW", False);

		/* Construct a keycode lookup */
		_gfx_x11_create_key_table();
	}

	return 1;
}

/******************************************************/
void _gfx_platform_terminate(void)
{
	if(_gfx_x11)
	{
		/* Close connection (destroys all resources) */
		if(_gfx_x11->display) XCloseDisplay(_gfx_x11->display);

		gfx_vector_clear(&_gfx_x11->monitors);
		gfx_vector_clear(&_gfx_x11->windows);

		/* Deallocate */
		free(_gfx_x11);
		_gfx_x11 = NULL;
	}
}
