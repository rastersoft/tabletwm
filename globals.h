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

#define TWM_NAME "TabletWindowManager"

enum TWM_ATOM_DEFS {TWM_ATOM_WM_SIZE_HINTS, TWM_ATOM_WM_NORMAL_HINTS, TWM_ATOM_WM_PROTOCOLS,
	TWM_ATOM_WM_DELETE_WINDOW, TWM_ATOM_WM_TRANSIENT_FOR, TWM_ATOM__NET_WM_WINDOW_TYPE,
	TWM_ATOM__NET_SUPPORTING_WM_CHECK,TWM_ATOM__NET_WM_NAME,
	TWM_ATOM_LAST_VALUE};

extern xcb_atom_t atoms[];

extern uint16_t width;
extern uint16_t height;

extern xcb_connection_t *conn;
extern xcb_screen_t *scr;

#endif // _H_GLOBALS_
