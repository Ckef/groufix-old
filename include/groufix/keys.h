/**
 * Groufix  :  Graphics Engine produced by Ckef Worx
 * www      :  http://www.ejb.ckef-worx.com
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef GFX_KEYS_H
#define GFX_KEYS_H

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * \brief Platform independent key code
 *******************************************************/
typedef int GFXKey;

#define GFX_KEY_UNKNOWN        0x0000 /* Must be zero to initialize to unknown */

#define GFX_KEY_BACKSPACE      0x0008
#define GFX_KEY_TAB            0x0009
#define GFX_KEY_CLEAR          0x000a
#define GFX_KEY_RETURN         0x000b /* ENTER */
#define GFX_KEY_ENTER          0x000b
#define GFX_KEY_PAUSE          0x000c
#define GFX_KEY_SCROLL_LOCK    0x000d
#define GFX_KEY_ESCAPE         0x000e
#define GFX_KEY_DELETE         0x000f

#define GFX_KEY_HOME           0x0010
#define GFX_KEY_LEFT           0x0011
#define GFX_KEY_UP             0x0012
#define GFX_KEY_RIGHT          0x0013
#define GFX_KEY_DOWN           0x0014
#define GFX_KEY_PAGE_DOWN      0x0015 /* NEXT */
#define GFX_KEY_NEXT           0x0015
#define GFX_KEY_PAGE_UP        0x0016 /* PRIOR */
#define GFX_KEY_PRIOR          0x0016
#define GFX_KEY_END            0x0017

#define GFX_KEY_SELECT         0x0018
#define GFX_KEY_PRINT          0x0019
#define GFX_KEY_EXECUTE        0x001a
#define GFX_KEY_INSERT         0x001b
#define GFX_KEY_MENU           0x001c
#define GFX_KEY_CANCEL         0x001d
#define GFX_KEY_HELP           0x001e
#define GFX_KEY_NUM_LOCK       0x001f
#define GFX_KEY_SPACE          0x0020

#define GFX_KEY_KP_RETURN      0x0021 /* KP Enter */
#define GFX_KEY_KP_ENTER       0x0021
#define GFX_KEY_KP_0           0x0022
#define GFX_KEY_KP_1           0x0023
#define GFX_KEY_KP_2           0x0024
#define GFX_KEY_KP_3           0x0025
#define GFX_KEY_KP_4           0x0026
#define GFX_KEY_KP_5           0x0027
#define GFX_KEY_KP_6           0x0028
#define GFX_KEY_KP_7           0x0029
#define GFX_KEY_KP_8           0x002a
#define GFX_KEY_KP_9           0x002b
#define GFX_KEY_KP_MULTIPLY    0x002c
#define GFX_KEY_KP_ADD         0x002d
#define GFX_KEY_KP_SEPARATOR   0x002e
#define GFX_KEY_KP_SUBTRACT    0x002f
#define GFX_KEY_KP_DECIMAL     0x003a /* Continue from after Unicode numbers */
#define GFX_KEY_KP_DIVIDE      0x003b

#define GFX_KEY_F1             0x005b /* Continue from after Unicode alphabet */
#define GFX_KEY_F2             0x005c
#define GFX_KEY_F3             0x005d
#define GFX_KEY_F4             0x005e
#define GFX_KEY_F5             0x005f
#define GFX_KEY_F6             0x0060
#define GFX_KEY_F7             0x0061
#define GFX_KEY_F8             0x0062
#define GFX_KEY_F9             0x0063
#define GFX_KEY_F10            0x0064
#define GFX_KEY_F11            0x0065
#define GFX_KEY_F12            0x0066
#define GFX_KEY_F13            0x0067
#define GFX_KEY_F14            0x0068
#define GFX_KEY_F15            0x0069
#define GFX_KEY_F16            0x006a
#define GFX_KEY_F17            0x006b
#define GFX_KEY_F18            0x006c
#define GFX_KEY_F19            0x006d
#define GFX_KEY_F20            0x006e
#define GFX_KEY_F21            0x006f
#define GFX_KEY_F22            0x0070
#define GFX_KEY_F23            0x0071
#define GFX_KEY_F24            0x0072

#define GFX_KEY_SHIFT_LEFT     0x0073
#define GFX_KEY_SHIFT_RIGHT    0x0074
#define GFX_KEY_CONTROL_LEFT   0x0075
#define GFX_KEY_CONTROL_RIGHT  0x0076
#define GFX_KEY_ALT_LEFT       0x0077
#define GFX_KEY_ALT_RIGHT      0x0078
#define GFX_KEY_SUPER_LEFT     0x0079 /* Left windows key */
#define GFX_KEY_SUPER_RIGHT    0x007a /* Right windows key */


/* Convertible to Unicode (char) from here */
#define GFX_KEY_0              0x0030
#define GFX_KEY_1              0x0031
#define GFX_KEY_2              0x0032
#define GFX_KEY_3              0x0033
#define GFX_KEY_4              0x0034
#define GFX_KEY_5              0x0035
#define GFX_KEY_6              0x0036
#define GFX_KEY_7              0x0037
#define GFX_KEY_8              0x0038
#define GFX_KEY_9              0x0039

#define GFX_KEY_A              0x0041
#define GFX_KEY_B              0x0042
#define GFX_KEY_C              0x0043
#define GFX_KEY_D              0x0044
#define GFX_KEY_E              0x0045
#define GFX_KEY_F              0x0046
#define GFX_KEY_G              0x0047
#define GFX_KEY_H              0x0048
#define GFX_KEY_I              0x0049
#define GFX_KEY_J              0x004a
#define GFX_KEY_K              0x004b
#define GFX_KEY_L              0x004c
#define GFX_KEY_M              0x004d
#define GFX_KEY_N              0x004e
#define GFX_KEY_O              0x004f
#define GFX_KEY_P              0x0050
#define GFX_KEY_Q              0x0051
#define GFX_KEY_R              0x0052
#define GFX_KEY_S              0x0053
#define GFX_KEY_T              0x0054
#define GFX_KEY_U              0x0055
#define GFX_KEY_V              0x0056
#define GFX_KEY_W              0x0057
#define GFX_KEY_X              0x0058
#define GFX_KEY_Y              0x0059
#define GFX_KEY_Z              0x005a


/********************************************************
 * \brief Platform independent mouse state
 *******************************************************/
typedef int GFXMouseKey;

#define GFX_MOUSE_KEY_LEFT    0x01
#define GFX_MOUSE_KEY_RIGHT   0x02
#define GFX_MOUSE_KEY_MIDDLE  0x03


/********************************************************
 * \brief Platform independent key state
 *******************************************************/
typedef int GFXKeyState;

#define GFX_KEY_STATE_SHIFT        0x01
#define GFX_KEY_STATE_CONTROL      0x02
#define GFX_KEY_STATE_ALT          0x04
#define GFX_KEY_STATE_SUPER        0x08
#define GFX_KEY_STATE_CAPS_LOCK    0x10
#define GFX_KEY_STATE_NUM_LOCK     0x20
#define GFX_KEY_STATE_SCROLL_LOCK  0x40

#define GFX_KEY_STATE_NONE         0x00
#define GFX_KEY_STATE_CAPITALS     0x11 /* Either shift or caps lock is active */
#define GFX_KEY_STATE_LOCK_ALL     0x70 /* All toggle keys are toggled active */


#ifdef __cplusplus
}
#endif

#endif // GFX_KEYS_H
