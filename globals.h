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

#define TWM_NAME "tabletwm"

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

struct wincache_element {

	struct wincache_element *prev;
	struct wincache_element *next;

	char    *class_name;
	char    *instance;
	uint32_t window;
	uint32_t type;
	uint8_t  filled;
	uint8_t  mapped;
	uint8_t  resizable;
	uint8_t  is_transient;
	uint8_t  input_flag;
};

extern struct wincache_element wincache_list;

extern xcb_atom_t atoms[];

extern uint16_t width;
extern uint16_t height;

extern xcb_connection_t *conn;
extern xcb_screen_t *scr;

extern char keep_running;

#endif // _H_GLOBALS_
