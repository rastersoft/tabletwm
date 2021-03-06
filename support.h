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

#ifndef _H_SUPPORT_
#define _H_SUPPORT_

#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>
#include <xcb/xcb_icccm.h>
#include "globals.h"

void support_capture_key(uint32_t mods,uint32_t key);
void support_calculate_new_size(xcb_window_t window, struct support_new_size *size);

void support_send_dock_up(xcb_query_tree_reply_t *r2,xcb_window_t *wp2);
void support_next_window(int next_app);
void support_close_window();
void support_set_focus();
void support_resize_all_windows();

void support_launch_manager();

#endif // _H_SUPPORT_
