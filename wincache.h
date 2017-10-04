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
	uint32_t cur_width;
	uint32_t cur_height;
	uint8_t  filled;
	uint8_t  mapped;
	uint8_t  resizable;
	uint8_t  is_transient;
	uint8_t  input_flag;
};

extern struct wincache_element *wincache_list;

#define WINCACHE_WINDOW_TYPE   1 << 0
#define WINCACHE_TRANSIENT_FOR 1 << 1
#define WINCACHE_HINTS         1 << 2
#define WINCACHE_WMCLASS       1 << 3
#define WINCACHE_WINDOW_HINTS  1 << 4
#define WINCACHE_GEOMETRY      1 << 5
#define WINCACHE_ALL           0xFFFFFFFF

void wincache_init();
struct wincache_element *wincache_find_element(uint32_t window);
struct wincache_element *wincache_add_element(uint32_t window);
struct wincache_element *wincache_fill_element(uint32_t window, uint32_t flags);
void wincache_destroy_element(uint32_t window);
uint32_t wincache_find_launcher_window();

#endif // _H_WINCACHE_
