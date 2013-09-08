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
#include "wincache.h"

void init_tabletwm() {

	xcb_void_cookie_t void_cookie;
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
		
		"WM_CLASS",
		"WM_STATE",
		"_NET_ACTIVE_WINDOW",
		"WM_HINTS",
	};

	wincache_init();

	conn=xcb_connect(0,0);
	assert(conn);

	scr=xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
	width=scr->width_in_pixels;
	height=scr->height_in_pixels; // reserve the bottom part for the 1pixel-height, collapsed menu bar

	xcb_change_window_attributes(conn,scr->root,XCB_CW_EVENT_MASK,v);

	xcb_depth_iterator_t depth_iter;
	xcb_visualid_t    root_visual = { 0 };
	visual_type = NULL;
	
	for (depth_iter = xcb_screen_allowed_depths_iterator (scr); depth_iter.rem; xcb_depth_next (&depth_iter)) {
		xcb_visualtype_iterator_t visual_iter;

		visual_iter = xcb_depth_visuals_iterator (depth_iter.data);
		for (; visual_iter.rem; xcb_visualtype_next (&visual_iter)) {
			if (scr->root_visual == visual_iter.data->visual_id) {
				visual_type = visual_iter.data;
				break;
			}
		}
	}

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

	xcb_change_property(conn,XCB_PROP_MODE_REPLACE,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM_WM_DELETE_WINDOW]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DESKTOP]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DOCK]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_TOOLBAR]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_MENU]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_UTILITY]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_SPLASH]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DIALOG]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DROPDOWN_MENU]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_POPUP_MENU]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_TOOLTIP]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_NOTIFICATION]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_COMBO]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DND]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_NORMAL]);

	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ALLOWED_ACTIONS]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ACTION_MOVE]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ACTION_RESIZE]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ACTION_MINIMIZE]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ACTION_SHADE]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ACTION_STICK]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ACTION_MAXIMIZE_HORZ]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ACTION_MAXIMIZE_VERT]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ACTION_FULLSCREEN]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ACTION_CHANGE_DESKTOP]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ACTION_CLOSE]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ACTION_ABOVE]);
	xcb_change_property(conn,XCB_PROP_MODE_APPEND,scr->root,atoms[TWM_ATOM__NET_SUPPORTED],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_ACTION_BELOW]);

	// This window contains the menu and the virtual keyboard (when showed)
	// It is also used to ensure that the window manager is recognized as an Extended Window Manager Hints WM
	// By default it occupies one pixel at the bottom of the screen, to allow to detect when the mouse is moved to the bottom
	key_win.window = xcb_generate_id(conn);
	uint32_t values[1] = {XCB_EVENT_MASK_EXPOSURE|XCB_EVENT_MASK_BUTTON_RELEASE|XCB_EVENT_MASK_ENTER_WINDOW|XCB_EVENT_MASK_LEAVE_WINDOW};
	void_cookie=xcb_create_window_checked (conn,XCB_COPY_FROM_PARENT,key_win.window,scr->root,0,height-1,width,1,0,XCB_WINDOW_CLASS_INPUT_OUTPUT,scr->root_visual,XCB_CW_EVENT_MASK,values);

	xcb_flush(conn);

	if (xcb_request_check(conn,void_cookie)) {
		printf("Can't create the fake window\n");
	} else {
		key_win.surface=cairo_xcb_surface_create(conn,key_win.window,visual_type,width,10);
		printf("Error: %s\n",cairo_status_to_string(cairo_surface_status(key_win.surface)));
		key_win.cr=cairo_create(key_win.surface);
		// Set the _NET_SUPPORTING_WM_CHECK property pointing to the window ID in both the root and fake windows
		// Also set the WM_NAME property in both windows to TWM_NAME
		xcb_change_property(conn,XCB_PROP_MODE_REPLACE,scr->root,atoms[TWM_ATOM__NET_SUPPORTING_WM_CHECK],XCB_ATOM_WINDOW,32,1,&key_win.window);
		xcb_change_property(conn,XCB_PROP_MODE_REPLACE,scr->root,atoms[TWM_ATOM__NET_WM_NAME],XCB_ATOM_STRING,8,strlen(TWM_NAME),TWM_NAME);
		xcb_change_property(conn,XCB_PROP_MODE_REPLACE,key_win.window,atoms[TWM_ATOM__NET_SUPPORTING_WM_CHECK],XCB_ATOM_WINDOW,32,1,&key_win.window);
		xcb_change_property(conn,XCB_PROP_MODE_REPLACE,key_win.window,atoms[TWM_ATOM__NET_WM_NAME],XCB_ATOM_STRING,8,strlen(TWM_NAME),TWM_NAME);
		xcb_change_property(conn,XCB_PROP_MODE_REPLACE,key_win.window,atoms[TWM_ATOM__NET_WM_WINDOW_TYPE],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DOCK]);
		xcb_map_window(conn,key_win.window);
		xcb_flush(conn);
		key_win.cache=wincache_fill_element(key_win.window);
		key_win.cache->mapped=1;
		key_win.possition=0;
		key_win.has_keyboard=0;
		key_win.width=width;
		key_win.height=1;
		key_win.enabled_by_mouse=0;
	}
	
	// Read the keyboard definition files
	FILE *keyboard_file=fopen(KEYBOARD_FILE,"r");
	if (keyboard_file==NULL) {
		printf("Can't open keyboard definition file\n");
	} else {
		int counter;
		uint32_t keysym;
		for(counter=0;!feof(keyboard_file);counter++) {
			char command[15];
			int w,h,retval;
			retval=fscanf(keyboard_file,"%s %d %d",command,&w,&h);
			if(retval!=3) {
				break;
			}
			keyboard_lowercase[counter].g_element[0]=0;
			keyboard_lowercase[counter].w=w;
			keyboard_lowercase[counter].h=h;
			if (!strcmp(command,"BLANK")) {
				keyboard_lowercase[counter].type=KEY_BLANK;
				keysym=0;
			} else if (!strcmp(command,"KEY")) {
				keyboard_lowercase[counter].type=KEY_PH;
				retval=fscanf(keyboard_file,"%s",keyboard_lowercase[counter].g_element);
				keysym=init_utf8_to_keysym(keyboard_lowercase[counter].g_element);
				if (keysym==0) {
					keyboard_lowercase[counter].type=KEY_BLANK;
				}
			} else if (!strcmp(command,"TAB")) {
				keyboard_lowercase[counter].type=KEY_TAB;
				keysym=XK_Tab;
			} else if (!strcmp(command,"SPACE")) {
				keyboard_lowercase[counter].type=KEY_SPACE;
				keysym=XK_space;
			} else if (!strcmp(command,"RETURN")) {
				keyboard_lowercase[counter].type=KEY_RETURN;
				keysym=XK_Return;
			} else if (!strcmp(command,"DELETE")) {
				keyboard_lowercase[counter].type=KEY_DELETE;
				keysym=XK_BackSpace;
			} else if (!strcmp(command,"SHIFT")) {
				keyboard_lowercase[counter].type=KEY_SHIFT;
				keysym=0;
			} else if (!strcmp(command,"UP")) {
				keyboard_lowercase[counter].type=KEY_UP;
				keysym=XK_Up;
			} else if (!strcmp(command,"DOWN")) {
				keyboard_lowercase[counter].type=KEY_DOWN;
				keysym=XK_Down;
			} else if (!strcmp(command,"LEFT")) {
				keyboard_lowercase[counter].type=KEY_LEFT;
				keysym=XK_Left;
			} else if (!strcmp(command,"RIGHT")) {
				keyboard_lowercase[counter].type=KEY_RIGHT;
				keysym=XK_Right;
			} else if (!strcmp(command,"SYMBOL")) {
				keyboard_lowercase[counter].type=KEY_SYMBOL;
				keysym=0;
			} else {
				printf("Unknown command %s\n",command);
				keyboard_lowercase[counter].type=KEY_BLANK;
				keysym=0;
			}
		}
	}
}

uint32_t init_utf8_to_keysym(unsigned char *data) {

	uint32_t keysym=(uint32_t)(*data++);
	uint32_t tmp;
	if (keysym==0) {
		return 0;
	}
	if (keysym<128) {
		return (keysym);
	}
	if (keysym<192) { // error, not a valid UTF-8 code
		return 0;
	}
	if (keysym<224) { // two-byte element
		keysym&=0x0000001F;
		keysym<<=6;
		tmp=(uint32_t)(*data++);
		keysym|=tmp&0x3F;
		return (keysym);
	}
	if (keysym<240) { // three-byte element
		keysym&=0x0000000F;
		keysym<<=12;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F)<<6;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F);
		return (keysym);
	}
	if (keysym<248) { // four-byte element
		keysym&=0x00000007;
		keysym<<=18;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F)<<12;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F)<<6;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F);
		return (keysym);
	}
	if (keysym<252) { // five-byte element
		keysym&=0x00000003;
		keysym<<=24;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F)<<18;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F)<<12;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F)<<6;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F);
		return (keysym);
	}
	if (keysym<254) { // six-byte element
		keysym&=0x00000001;
		keysym<<=30;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F)<<24;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F)<<18;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F)<<12;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F)<<6;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F);
		return (keysym);
	}
	return 0;
}

