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

#ifndef _H_WINCACHE_
#define _H_WINCACHE_

#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>
#include "globals.h"

void wincache_init();
struct wincache_element *wincache_find_element(uint32_t window);
struct wincache_element *wincache_add_element(uint32_t window);
struct wincache_element *wincache_fill_element(uint32_t window);
uint32_t wincache_find_launcher_window();

#endif // _H_WINCACHE_
