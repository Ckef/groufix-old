/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
 *
 * Groufix is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Groufix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Groufix.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef GFX_CORE_KEYS_H
#define GFX_CORE_KEYS_H

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
 * Platform independent key input
 *******************************************************/

/** Key codes */
typedef enum GFXKey
{
	GFX_KEY_UNKNOWN        = 0x0000, /* Must be zero to initialize to unknown */

	GFX_KEY_BACKSPACE      = 0x0008,
	GFX_KEY_TAB            = 0x0009,
	GFX_KEY_CLEAR          = 0x000a,
	GFX_KEY_RETURN         = 0x000b, /* ENTER */
	GFX_KEY_ENTER          = 0x000b,
	GFX_KEY_PAUSE          = 0x000c,
	GFX_KEY_SCROLL_LOCK    = 0x000d,
	GFX_KEY_ESCAPE         = 0x000e,
	GFX_KEY_DELETE         = 0x000f,

	GFX_KEY_HOME           = 0x0010,
	GFX_KEY_LEFT           = 0x0011,
	GFX_KEY_UP             = 0x0012,
	GFX_KEY_RIGHT          = 0x0013,
	GFX_KEY_DOWN           = 0x0014,
	GFX_KEY_PAGE_DOWN      = 0x0015, /* NEXT */
	GFX_KEY_NEXT           = 0x0015,
	GFX_KEY_PAGE_UP        = 0x0016, /* PRIOR */
	GFX_KEY_PRIOR          = 0x0016,
	GFX_KEY_END            = 0x0017,

	GFX_KEY_SELECT         = 0x0018,
	GFX_KEY_PRINT          = 0x0019,
	GFX_KEY_EXECUTE        = 0x001a,
	GFX_KEY_INSERT         = 0x001b,
	GFX_KEY_MENU           = 0x001c,
	GFX_KEY_CANCEL         = 0x001d,
	GFX_KEY_HELP           = 0x001e,
	GFX_KEY_NUM_LOCK       = 0x001f,
	GFX_KEY_SPACE          = 0x0020,

	GFX_KEY_KP_RETURN      = 0x0021, /* KP Enter */
	GFX_KEY_KP_ENTER       = 0x0021,
	GFX_KEY_KP_0           = 0x0022,
	GFX_KEY_KP_1           = 0x0023,
	GFX_KEY_KP_2           = 0x0024,
	GFX_KEY_KP_3           = 0x0025,
	GFX_KEY_KP_4           = 0x0026,
	GFX_KEY_KP_5           = 0x0027,
	GFX_KEY_KP_6           = 0x0028,
	GFX_KEY_KP_7           = 0x0029,
	GFX_KEY_KP_8           = 0x002a,
	GFX_KEY_KP_9           = 0x002b,
	GFX_KEY_KP_MULTIPLY    = 0x002c,
	GFX_KEY_KP_ADD         = 0x002d,
	GFX_KEY_KP_SEPARATOR   = 0x002e,
	GFX_KEY_KP_SUBTRACT    = 0x002f,
	GFX_KEY_KP_DECIMAL     = 0x003a, /* Continue from after Unicode numbers */
	GFX_KEY_KP_DIVIDE      = 0x003b,

	GFX_KEY_F1             = 0x005b, /* Continue from after Unicode alphabet */
	GFX_KEY_F2             = 0x005c,
	GFX_KEY_F3             = 0x005d,
	GFX_KEY_F4             = 0x005e,
	GFX_KEY_F5             = 0x005f,
	GFX_KEY_F6             = 0x0060,
	GFX_KEY_F7             = 0x0061,
	GFX_KEY_F8             = 0x0062,
	GFX_KEY_F9             = 0x0063,
	GFX_KEY_F10            = 0x0064,
	GFX_KEY_F11            = 0x0065,
	GFX_KEY_F12            = 0x0066,
	GFX_KEY_F13            = 0x0067,
	GFX_KEY_F14            = 0x0068,
	GFX_KEY_F15            = 0x0069,
	GFX_KEY_F16            = 0x006a,
	GFX_KEY_F17            = 0x006b,
	GFX_KEY_F18            = 0x006c,
	GFX_KEY_F19            = 0x006d,
	GFX_KEY_F20            = 0x006e,
	GFX_KEY_F21            = 0x006f,
	GFX_KEY_F22            = 0x0070,
	GFX_KEY_F23            = 0x0071,
	GFX_KEY_F24            = 0x0072,

	GFX_KEY_SHIFT_LEFT     = 0x0073,
	GFX_KEY_SHIFT_RIGHT    = 0x0074,
	GFX_KEY_CONTROL_LEFT   = 0x0075,
	GFX_KEY_CONTROL_RIGHT  = 0x0076,
	GFX_KEY_ALT_LEFT       = 0x0077,
	GFX_KEY_ALT_RIGHT      = 0x0078,
	GFX_KEY_SUPER_LEFT     = 0x0079, /* Left windows key */
	GFX_KEY_SUPER_RIGHT    = 0x007a, /* Right windows key */


	/* Convertible to Unicode (char) from here */
	GFX_KEY_0              = 0x0030,
	GFX_KEY_1              = 0x0031,
	GFX_KEY_2              = 0x0032,
	GFX_KEY_3              = 0x0033,
	GFX_KEY_4              = 0x0034,
	GFX_KEY_5              = 0x0035,
	GFX_KEY_6              = 0x0036,
	GFX_KEY_7              = 0x0037,
	GFX_KEY_8              = 0x0038,
	GFX_KEY_9              = 0x0039,

	GFX_KEY_A              = 0x0041,
	GFX_KEY_B              = 0x0042,
	GFX_KEY_C              = 0x0043,
	GFX_KEY_D              = 0x0044,
	GFX_KEY_E              = 0x0045,
	GFX_KEY_F              = 0x0046,
	GFX_KEY_G              = 0x0047,
	GFX_KEY_H              = 0x0048,
	GFX_KEY_I              = 0x0049,
	GFX_KEY_J              = 0x004a,
	GFX_KEY_K              = 0x004b,
	GFX_KEY_L              = 0x004c,
	GFX_KEY_M              = 0x004d,
	GFX_KEY_N              = 0x004e,
	GFX_KEY_O              = 0x004f,
	GFX_KEY_P              = 0x0050,
	GFX_KEY_Q              = 0x0051,
	GFX_KEY_R              = 0x0052,
	GFX_KEY_S              = 0x0053,
	GFX_KEY_T              = 0x0054,
	GFX_KEY_U              = 0x0055,
	GFX_KEY_V              = 0x0056,
	GFX_KEY_W              = 0x0057,
	GFX_KEY_X              = 0x0058,
	GFX_KEY_Y              = 0x0059,
	GFX_KEY_Z              = 0x005a

} GFXKey;


/** Mouse key codes */
typedef enum GFXMouseKey
{
	GFX_MOUSE_KEY_LEFT,
	GFX_MOUSE_KEY_RIGHT,
	GFX_MOUSE_KEY_MIDDLE

} GFXMouseKey;


/** Key states */
typedef enum GFXKeyState
{
	GFX_KEY_STATE_SHIFT        = 0x01,
	GFX_KEY_STATE_CONTROL      = 0x02,
	GFX_KEY_STATE_ALT          = 0x04,
	GFX_KEY_STATE_SUPER        = 0x08,
	GFX_KEY_STATE_CAPS_LOCK    = 0x10,
	GFX_KEY_STATE_NUM_LOCK     = 0x20,
	GFX_KEY_STATE_SCROLL_LOCK  = 0x40,

	GFX_KEY_STATE_NONE         = 0x00,
	GFX_KEY_STATE_CAPITALS     = 0x11, /* Either shift or caps lock is active */
	GFX_KEY_STATE_LOCK_ALL     = 0x70  /* All toggle keys are toggled active */

} GFXKeyState;


#ifdef __cplusplus
}
#endif

#endif // GFX_CORE_KEYS_H
