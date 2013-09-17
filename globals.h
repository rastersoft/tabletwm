/* Tablet Window Manager
 * An XCB-based minimalist window manager, oriented to tablet devices
 * Based on code (C)2011 CINOLT
 *
 * (C)2013 Raster Software Vigo (Sergio Costas)
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _H_GLOBALS_
#define _H_GLOBALS_

#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xcb.h>
#include <X11/keysym.h>

#define TWM_NAME "tabletwm"

//#define DEBUG

#define CONFIG_FILE "tabletwm.cfg"
#define BASE_CONFIG_DIR "/etc/tabletwm.d"

enum TWM_ATOM_DEFS {
	TWM_ATOM_WM_SIZE_HINTS=0,
	TWM_ATOM_WM_NORMAL_HINTS,
	TWM_ATOM_WM_PROTOCOLS,
	TWM_ATOM_WM_DELETE_WINDOW,
	TWM_ATOM_WM_TRANSIENT_FOR,
	TWM_ATOM__NET_SUPPORTING_WM_CHECK,
	TWM_ATOM__NET_WM_NAME,
	TWM_ATOM__NET_SUPPORTED,

	TWM_ATOM__NET_WM_WINDOW_TYPE,
	TWM_ATOM__NET_WM_WINDOW_TYPE_DESKTOP,
	TWM_ATOM__NET_WM_WINDOW_TYPE_DOCK,
	TWM_ATOM__NET_WM_WINDOW_TYPE_TOOLBAR,
	TWM_ATOM__NET_WM_WINDOW_TYPE_MENU,
	TWM_ATOM__NET_WM_WINDOW_TYPE_UTILITY,
	TWM_ATOM__NET_WM_WINDOW_TYPE_SPLASH,
	TWM_ATOM__NET_WM_WINDOW_TYPE_DIALOG,
	TWM_ATOM__NET_WM_WINDOW_TYPE_DROPDOWN_MENU,
	TWM_ATOM__NET_WM_WINDOW_TYPE_POPUP_MENU,
	TWM_ATOM__NET_WM_WINDOW_TYPE_TOOLTIP,
	TWM_ATOM__NET_WM_WINDOW_TYPE_NOTIFICATION,
	TWM_ATOM__NET_WM_WINDOW_TYPE_COMBO,
	TWM_ATOM__NET_WM_WINDOW_TYPE_DND,
	TWM_ATOM__NET_WM_WINDOW_TYPE_NORMAL,

	TWM_ATOM__NET_WM_ALLOWED_ACTIONS,
	TWM_ATOM__NET_WM_ACTION_MOVE,
	TWM_ATOM__NET_WM_ACTION_RESIZE,
	TWM_ATOM__NET_WM_ACTION_MINIMIZE,
	TWM_ATOM__NET_WM_ACTION_SHADE,
	TWM_ATOM__NET_WM_ACTION_STICK,
	TWM_ATOM__NET_WM_ACTION_MAXIMIZE_HORZ,
	TWM_ATOM__NET_WM_ACTION_MAXIMIZE_VERT,
	TWM_ATOM__NET_WM_ACTION_FULLSCREEN,
	TWM_ATOM__NET_WM_ACTION_CHANGE_DESKTOP,
	TWM_ATOM__NET_WM_ACTION_CLOSE,
	TWM_ATOM__NET_WM_ACTION_ABOVE,
	TWM_ATOM__NET_WM_ACTION_BELOW,

	TWM_ATOM_WM_CLASS,
	
	TWM_ATOM_WM_STATE,
	TWM_ATOM__NET_ACTIVE_WINDOW,
	
	TWM_ATOM_WM_HINTS,

	TWM_ATOM_LAST_VALUE};

struct support_new_size {

	int32_t x;
	int32_t y;
	int32_t w;
	int32_t h;
	uint8_t new_x; // x variable contains a value
	uint8_t new_y; // y variable contains a value
	uint8_t new_w; // w variable contains a value
	uint8_t new_h; // h variable contains a value
	uint8_t force_change;
};

struct wincache_element {

	struct wincache_element *prev;
	struct wincache_element *next;

	char    *class_name;
	char    *instance;
	uint32_t window;
	uint32_t type;
	uint32_t min_width;
	uint32_t min_height;
	uint32_t max_width;
	uint32_t max_height;
	uint8_t  filled;
	uint8_t  mapped;
	uint8_t  resizable;
	uint8_t  is_transient;
	uint8_t  input_flag;
	uint8_t  asked_for_new_size;
	struct   support_new_size new_size;
};

extern struct wincache_element wincache_list;

extern xcb_atom_t atoms[];

extern uint16_t width;
extern uint16_t height;

extern xcb_connection_t *conn;
extern xcb_screen_t *scr;
extern xcb_visualtype_t *visual_type;

struct key_win_s {
	xcb_window_t window;
	struct wincache_element *cache;
	uint8_t possition; // 0: minimized; 1: expanded
	uint8_t has_keyboard; // 0: no keyboard bottom; 1: no keyboard top; 2: keyboard bottom; 3: keyboard top
	uint8_t enabled_by_mouse;
	uint8_t mouse_grabed;
	uint32_t width;
	uint32_t height;
	cairo_surface_t *surface;
	cairo_t *cr;
};

enum key_type {KEY_BLANK, KEY_PH, KEY_TAB, KEY_SPACE, KEY_RETURN, KEY_DELETE, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_JUMPTO};

struct key_element {
	enum key_type type;
	xcb_keycode_t keycode;
	xcb_keycode_t modifier;
	xcb_keysym_t  keysym;
	char w;
	char h;
	char g_element[8]; // to allow UTF-8 elements
	float size;
};

#ifndef M_PI
#define M_PI           3.1415926536
#define M_PI2          1.570796327
#define M_PI32         4.71238898
#define M_PI42         6.283185307
#endif

extern int keyboard_blocks;
extern int keyboard_current_block;

extern struct key_element *keyboard_lowercase;

extern struct key_win_s key_win;
extern struct key_win_s shutdown_win;
extern char keep_running;

extern char lang_xkbmap[];
extern char lang_onscreen[];
extern char launcher_program[];

#endif // _H_GLOBALS_
