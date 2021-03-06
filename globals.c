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

#include "globals.h"

xcb_atom_t atoms[TWM_ATOM_LAST_VALUE];

uint16_t width;
uint16_t height;

int menuwin_free_zone = 0;

char keep_running;

xcb_connection_t *conn;
xcb_screen_t *scr;
xcb_visualtype_t *visual_type;

struct key_win_s key_win;
struct key_win_s shutdown_win;

struct key_element *keyboard_lowercase;
int keyboard_blocks;
int keyboard_current_block;

char lang_xkbmap[32] = {0};
char lang_onscreen[32] = {0};
char launcher_program[256] = {0};

char *xkb_names[5]={NULL, NULL, NULL, NULL, NULL};
