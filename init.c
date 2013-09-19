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
#include "shutdown.h"

void init_load_config() {

	char string[300];
	char *text;
	int retval;
	
	FILE *cfile;
	
	sprintf(string,"%s/%s",BASE_CONFIG_DIR,CONFIG_FILE);
	cfile=fopen(string,"r");
	if (cfile==NULL) {
		printf("Can't open configuration file %s\n",string);
		exit(-1);
	}
	strcpy(lang_xkbmap,"us");
	strcpy(lang_onscreen,"us");
	strcpy(launcher_program,"/usr/bin/xterm");
	while(!feof(cfile)) {
		retval=fscanf(cfile,"%s",string);
		if (retval<1) {
			continue;
		}
		text=string;
		while(*text==' ') {
			text++;
		}
		if (text[0]=='#') { // comment
			continue;
		}
		if (text[0]=='\n') { // empty line
			continue;
		}
		if(!strncmp("keyboard_lang:",text,14)) {
			strcpy(lang_xkbmap,text+14);
			sprintf(string,"setxkbmap %s",lang_xkbmap);
			system(string);
			continue;
		}
		if(!strncmp("onscreen_keyboard_lang:",text,23)) {
			strcpy(lang_onscreen,text+23);
			continue;
		}
		if(!strncmp("onscreen_keyboard_map:",text,22)) {
			strcpy(lang_onscreen,text+22);
			continue;
		}
		if(!strncmp("launcher_command:",text,17)) {
			strcpy(launcher_program,text+17);
			continue;
		}
		printf("Unknown command in config file: %s\n",string);
	}
	fclose(cfile);
}

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
		"_XKB_RULES_NAMES",
	};

	init_load_config();

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

	xcb_get_property_cookie_t kbd_info_cookie;
	xcb_get_property_reply_t *kbd_info;
	kbd_info_cookie = xcb_get_property(conn,0,scr->root,atoms[TWM__XKB_RULES_NAMES],XCB_ATOM_STRING,0,1024);

	key_win.window = xcb_generate_id(conn);
	uint32_t values[1] = {XCB_EVENT_MASK_EXPOSURE|XCB_EVENT_MASK_BUTTON_RELEASE|XCB_EVENT_MASK_BUTTON_PRESS|XCB_EVENT_MASK_ENTER_WINDOW|XCB_EVENT_MASK_LEAVE_WINDOW};
	void_cookie=xcb_create_window_checked (conn,XCB_COPY_FROM_PARENT,key_win.window,scr->root,0,height-1,width,1,0,XCB_WINDOW_CLASS_INPUT_OUTPUT,scr->root_visual,XCB_CW_EVENT_MASK,values);

	xcb_flush(conn);

	kbd_info = xcb_get_property_reply(conn,kbd_info_cookie,0);
	if (kbd_info!=NULL) {
		if (kbd_info->length>0) {
			char *p=((char *)(xcb_get_property_value(kbd_info)));
			int counter=0;
			int len,max;
			max=xcb_get_property_value_length(kbd_info);
			printf("Tamano: %d\n",max);
			char *end=p+max;
			do {
				len = strnlen(p,max);
				if (*p!=0) {
					xkb_names[counter++]=strdup(p);
				}
				p+=len+1;
				max-=len+1;
			} while ((p < end) && (counter < 5));
		}
		free(kbd_info);
	}

	if (xcb_request_check(conn,void_cookie)) {
		printf("Can't create the fake window\n");
	} else {
		menuwin_init();
	}
	shutdown_init();
}

void destroy_tabletwm() {

	shutdown_destroy();

	wincache_destroy_element(key_win.window);
	destroy_keycodes();
	xcb_destroy_window(conn,key_win.window);
	xcb_flush(conn);

}
