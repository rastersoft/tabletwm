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

#ifndef _H_MENUWIN_
#define _H_MENUWIN_

#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xtest.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xcb.h>
#include <xkbcommon/xkbcommon.h>

#define BUTTON_RADIUS 5
#define BUTTON_MARGIN 1
// BUTTON_E_RADIUS = BUTTON_MARGIN + BUTTON_RADIUS
#define BUTTON_E_RADIUS 6

#define KEYS_PER_ROW 12.0
#define KEYS_FONT_SIZE 1.2
#define KEYS_JUMP_FONT_SIZE 0.8
#define KEYS_TEXT_FONT_SIZE 0.8
#define KEYS_H_DIVISOR 12.0

// (((c*1.2)>1.0) ? (1.0) : (c*1.2))
#define ADD_COMPONENT(c) (c)
#define SUB_COMPONENT(c) (c*0.9)

void menuwin_init();
void menuwin_set_window();
void menuwin_expose(xcb_expose_event_t *ee);
void menuwin_paint_button(cairo_t *cr,int x, int y, int w, int h, float r, float g, float b);
void menuwin_press_key_at(int x, int y);
uint32_t init_utf8_to_keysym(unsigned char *data);
void destroy_keycodes();

#endif // _H_MENUWIN
