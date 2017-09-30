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

#ifndef _H_ACTIONS_
#define _H_ACTIONS_

#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>

void action_xrandr_screen_change_notify(xcb_generic_event_t *e);
void action_unmap_notify(xcb_generic_event_t *e);
void action_map_request(xcb_generic_event_t *e);
void action_configure_request(xcb_generic_event_t *e);
void action_key(xcb_generic_event_t *e);
void action_expose(xcb_generic_event_t *e);
void action_destroy_notify(xcb_generic_event_t *e);
void action_configure_notify(xcb_generic_event_t *e);
void action_mouse_click(xcb_generic_event_t *e);

void action_mouse_leave(xcb_generic_event_t *e);
void action_mouse_enter(xcb_generic_event_t *e);

#endif //_H_ACTIONS_
