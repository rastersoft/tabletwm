/* Simple Window Manager
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

#include "globals.h"

xcb_atom_t atom_wm_size_hints;
xcb_atom_t atom_wm_normal_hints;
xcb_atom_t atom_wm_protocols;
xcb_atom_t atom_wm_delete_window;
xcb_atom_t atom_wm_transient_for;
xcb_atom_t atom_net_wm_window_type;

uint16_t width;
uint16_t height;
