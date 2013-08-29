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

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "globals.h"
#include "init.h"

void init_tabletwm() {

	xcb_void_cookie_t w_cookie;
	xcb_window_t win;

	conn=xcb_connect(0,0);
	assert(conn);

	scr=xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
	width=scr->width_in_pixels;
	height=scr->height_in_pixels;

	uint32_t v[]={XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY|XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT};
	xcb_change_window_attributes(conn,scr->root,XCB_CW_EVENT_MASK,v);

	/* get atoms */

	char *a_atoms[TWM_ATOM_LAST_VALUE]={
		"WM_SIZE_HINTS",
		"WM_NORMAL_HINTS",
		"WM_PROTOCOLS",
		"WM_DELETE_WINDOW",
		"WM_TRANSIENT_FOR",
		"_NET_WM_WINDOW_TYPE"
	};

	xcb_intern_atom_cookie_t atom_cookie[TWM_ATOM_LAST_VALUE];
	int i;
	for(i=0;i<TWM_ATOM_LAST_VALUE;i++) {
		atom_cookie[i]=xcb_intern_atom_unchecked(conn,0,strlen(a_atoms[i]),a_atoms[i]);
	};

	win = xcb_generate_id(conn);

	// fake window to ensure that the window manager is recognized as an Extended Window Manager Hints WM
	w_cookie=xcb_create_window_checked (conn,XCB_COPY_FROM_PARENT,win,scr->root,0,0,1,1,1,XCB_WINDOW_CLASS_INPUT_OUTPUT,scr->root_visual,0,NULL);

	xcb_flush(conn);

	xcb_intern_atom_reply_t *atom_reply;
	for(i=0;i<TWM_ATOM_LAST_VALUE;i++) {
		atom_reply=xcb_intern_atom_reply(conn,atom_cookie[i],0),
		atoms[i]=atom_reply->atom;
		free(atom_reply);
	};

	if (xcb_request_check(conn,w_cookie)) {
		printf("Can't create the fake window\n");
	}
}
