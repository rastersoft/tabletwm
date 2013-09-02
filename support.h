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

struct support_new_size {

	uint8_t force_change;
	uint8_t new_x; // x variable contains a value
	uint8_t new_y; // y variable contains a value
	uint8_t new_w; // w variable contains a value
	uint8_t new_h; // h variable contains a value
	int32_t x;
	int32_t y;
	int32_t w;
	int32_t h;
};

void support_calculate_new_size(xcb_window_t window, struct support_new_size *size);

void support_close_window();
void support_set_focus();

#endif // _H_SUPPORT_
