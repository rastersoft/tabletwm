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
#include <string.h>

#include "globals.h"
#include "init.h"

void init_tabletwm() {

	xcb_void_cookie_t void_cookie[30];
	xcb_window_t win;
	xcb_intern_atom_cookie_t atom_cookie[TWM_ATOM_LAST_VALUE];
	uint32_t v[]={XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY|XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT};
	int i;

	char *a_atoms[TWM_ATOM_LAST_VALUE]={
		"WM_SIZE_HINTS",
		"WM_NORMAL_HINTS",
		"WM_PROTOCOLS",
		"WM_DELETE_WINDOW",
		"WM_TRANSIENT_FOR",
		"_NET_SUPPORTING_WM_CHECK",
		"_NET_WM_NAME",
		"_NET_SUPPORTED",

		"_NET_WM_WINDOW_TYPE",
		"_NET_WM_WINDOW_TYPE_DESKTOP",
		"_NET_WM_WINDOW_TYPE_DOCK",
		"_NET_WM_WINDOW_TYPE_TOOLBAR",
		"_NET_WM_WINDOW_TYPE_MENU",
		"_NET_WM_WINDOW_TYPE_UTILITY",
		"_NET_WM_WINDOW_TYPE_SPLASH",
		"_NET_WM_WINDOW_TYPE_DIALOG",
		"_NET_WM_WINDOW_TYPE_DROPDOWN_MENU",
		"_NET_WM_WINDOW_TYPE_POPUP_MENU",
		"_NET_WM_WINDOW_TYPE_TOOLTIP",
		"_NET_WM_WINDOW_TYPE_NOTIFICATION",
		"_NET_WM_WINDOW_TYPE_COMBO",
		"_NET_WM_WINDOW_TYPE_DND",
		"_NET_WM_WINDOW_TYPE_NORMAL",

		"_NET_WM_ALLOWED_ACTIONS",
		"_NET_WM_ACTION_MOVE",
		"_NET_WM_ACTION_RESIZE",
		"_NET_WM_ACTION_MINIMIZE",
		"_NET_WM_ACTION_SHADE",
		"_NET_WM_ACTION_STICK",
		"_NET_WM_ACTION_MAXIMIZE_HORZ",
		"_NET_WM_ACTION_MAXIMIZE_VERT",
		"_NET_WM_ACTION_FULLSCREEN",
		"_NET_WM_ACTION_CHANGE_DESKTOP",
		"_NET_WM_ACTION_CLOSE",
		"_NET_WM_ACTION_ABOVE",
		"_NET_WM_ACTION_BELOW",
	};

	conn=xcb_connect(0,0);
	assert(conn);

	scr=xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
	width=scr->width_in_pixels;
	height=scr->height_in_pixels;

	xcb_change_window_attributes(conn,scr->root,XCB_CW_EVENT_MASK,v);

	/* get atoms */
	for(i=0;i<TWM_ATOM_LAST_VALUE;i++) {
		atom_cookie[i]=xcb_intern_atom_unchecked(conn,0,strlen(a_atoms[i]),a_atoms[i]);
	};

	xcb_flush(conn);

	xcb_intern_atom_reply_t *atom_reply;
	for(i=0;i<TWM_ATOM_LAST_VALUE;i++) {
		atom_reply=xcb_intern_atom_reply(conn,atom_cookie[i],0),
		atoms[i]=atom_reply->atom;
		free(atom_reply);
	};

	i=1;
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_REPLACE,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM_WM_DELETE_WINDOW]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DESKTOP]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DOCK]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_TOOLBAR]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_MENU]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_UTILITY]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_SPLASH]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DIALOG]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DROPDOWN_MENU]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_POPUP_MENU]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_TOOLTIP]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_NOTIFICATION]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_COMBO]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DND]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_NORMAL]);

	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ALLOWED_ACTIONS]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ACTION_MOVE]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ACTION_RESIZE]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ACTION_MINIMIZE]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ACTION_SHADE]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ACTION_STICK]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ACTION_MAXIMIZE_HORZ]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ACTION_MAXIMIZE_VERT]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ACTION_FULLSCREEN]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ACTION_CHANGE_DESKTOP]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ACTION_CLOSE]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ACTION_ABOVE]);
	void_cookie[i++]=xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ACTION_BELOW]);


	win = xcb_generate_id(conn);

	// fake window to ensure that the window manager is recognized as an Extended Window Manager Hints WM
	void_cookie[0]=xcb_create_window_checked (conn,XCB_COPY_FROM_PARENT,win,scr->root,0,0,1,1,1,XCB_WINDOW_CLASS_INPUT_OUTPUT,scr->root_visual,0,NULL);

	xcb_flush(conn);

	// check if the SUPPORTED property has been inserted fine
	for(i=1;i<17;i++) {
		if(xcb_request_check(conn,void_cookie[i])) {
			printf("Error when inserting element %d in _NET_SUPPORTED property\n",i);
		}
	}

	if (xcb_request_check(conn,void_cookie[0])) {
		printf("Can't create the fake window\n");
	} else {
		// Set the _NET_SUPPORTING_WM_CHECK property pointing to the window ID in both the root and fake windows
		// Also set the WM_NAME property in both windows to TWM_NAME
		void_cookie[0]=xcb_change_property(conn,XCB_PROP_MODE_REPLACE,scr->root,atoms[TWM_ATOM__NET_SUPPORTING_WM_CHECK],XCB_ATOM_WINDOW,32,1,&win);
		void_cookie[1]=xcb_change_property(conn,XCB_PROP_MODE_REPLACE,scr->root,atoms[TWM_ATOM__NET_WM_NAME],XCB_ATOM_STRING,8,strlen(TWM_NAME),TWM_NAME);
		void_cookie[2]=xcb_change_property(conn,XCB_PROP_MODE_REPLACE,win,atoms[TWM_ATOM__NET_SUPPORTING_WM_CHECK],XCB_ATOM_WINDOW,32,1,&win);
		void_cookie[3]=xcb_change_property(conn,XCB_PROP_MODE_REPLACE,win,atoms[TWM_ATOM__NET_WM_NAME],XCB_ATOM_STRING,8,strlen(TWM_NAME),TWM_NAME);
		xcb_flush(conn);
		if (xcb_request_check(conn,void_cookie[0])) {
			printf("Can't set _NET_SUPPORTING_WM_CHECK in root window\n");
		}
		if (xcb_request_check(conn,void_cookie[1])) {
			printf("Can't set _NET_WM_NAME in root window\n");
		}
		if (xcb_request_check(conn,void_cookie[2])) {
			printf("Can't set _NET_SUPPORTING_WM_CHECK in fake window\n");
		}
		if (xcb_request_check(conn,void_cookie[3])) {
			printf("Can't set _NET_WM_NAME in fake window\n");
		}
	}
}
