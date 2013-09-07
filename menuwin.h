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

#ifndef M_PI
#define M_PI           3.1415926536
#define M_PI2          1.570796327
#define M_PI32         4.71238898
#define M_PI42         6.283185307
#endif

#ifndef _H_MENUWIN_
#define _H_MENUWIN_

#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xcb.h>

#define BUTTON_RADIUS 5
#define BUTTON_MARGIN 1
// BUTTON_E_RADIUS=BUTTON_MARGIN+BUTTON_RADIUS
#define BUTTON_E_RADIUS 6

#define KEYS_PER_ROW 12

void menuwin_set_window();
void menuwin_expose(xcb_expose_event_t *ee);
void menuwin_paint_button(int x, int y, int w, int h, float r, float g, float b);

#endif // _H_MENUWIN
