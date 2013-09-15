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

#ifndef _H_SHUTDOWN_
#define _H_SHUTDOWN_

#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xtest.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xcb.h>

#include "menuwin.h"

void shutdown_init();
void shutdown_destroy();
void shutdown_show();
void shutdown_hide();
void shutdown_expose();
void shutdown_press(uint16_t x,uint16_t y);

#endif // _H_SHUTDOWN_
