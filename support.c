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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "support.h"
#include "globals.h"
#include "wincache.h"
#include "menuwin.h"

void support_capture_key(uint32_t mods,uint32_t key) {

	xcb_grab_key(conn,0,scr->root,mods,key,XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC); // key itself
	if (mods!=XCB_MOD_MASK_ANY) {
		xcb_grab_key(conn,0,scr->root,mods|XCB_MOD_MASK_2,key,XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC); // key + NumLock
		xcb_grab_key(conn,0,scr->root,mods|XCB_MOD_MASK_LOCK,key,XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC); // key + CapsLock
		xcb_grab_key(conn,0,scr->root,mods|XCB_MOD_MASK_2|XCB_MOD_MASK_LOCK,key,XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC); // key + NumLock + CapsLock
	}
}

void support_calculate_new_size(xcb_window_t window, struct support_new_size *size) {

	/* The caller must fill the SIZE structure with the current known values, and this function will change them, if needed */

	uint8_t what_to_do; // 0: maximize always; 1: maximize if resizable, center if not; 2: reduce to screen size if bigger, center if not; 3: leave as-is

	struct wincache_element *element;

	element=wincache_add_element(window);
	if (element==NULL) {
		return;
	}

	if (element->mapped==0) {
		if (size->new_w) {
			element->cur_width=size->w;
		}
		if (size->new_h) {
			element->cur_height=size->h;
		}
	}

	if ((element->filled==0)||(element->mapped==0)) {
		return;
	}

	if ((element->type==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DESKTOP])||
		(element->type==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_UTILITY])) {
			what_to_do=0;
	} else if (
		(element->type==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DIALOG])||
		(element->type==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_NORMAL])) {
			what_to_do=1;
	} else if (element->type==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_SPLASH]) {
			what_to_do=2;
	} else if (
		(element->type==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DOCK])||
		(element->type==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_TOOLBAR])||
		(element->type==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_MENU])||
		(element->type==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DROPDOWN_MENU])||
		(element->type==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_POPUP_MENU])||
		(element->type==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_TOOLTIP])||
		(element->type==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_NOTIFICATION])||
		(element->type==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_COMBO])||
		(element->type==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DND])) {
			what_to_do=3;
	} else {
		if (element->is_transient) {
			what_to_do=2;
		} else {
			what_to_do=1;
		}
	}

	switch(what_to_do) {
	case 1:
#ifdef DEBUG
		printf("Maximize if resizable; center if not\n");
#endif
		if (element->resizable==0) {
			int nx,ny;
			if (size->new_w) {
				nx=(width-size->w)/2;
			} else {
				nx=(width-element->cur_width)/2;
			}
			if ((size->new_x==0)||(size->x!=nx)) {
				size->new_x=1;
				size->x=nx;
				size->force_change=1;
			}

			if (size->new_h) {
				ny=(height-size->h)/2;
			} else {
				ny=(height-element->cur_height)/2;
			}
			if ((size->new_y==0)||(size->y!=ny)) {
				size->new_y=1;
				size->y=ny;
				size->force_change=1;
			}
			break;
		}
		// if we reach here, we want to maximize it
	case 0:
#ifdef DEBUG
		printf("Maximize always\n");
#endif
		if ((size->new_x==0)||((size->new_x==1)&&(size->x!=0))) {
			size->new_x=1;
			size->x=0;
		}
		if ((size->new_y==0)||((size->new_y==1)&&(size->y!=0))) {
			size->new_y=1;
			size->y=0;
		}
		size->new_w=1;
		size->new_h=1;
		size->w=width;
		size->h=height;
		element->cur_width=size->w;
		element->cur_height=size->h;
		size->force_change=1;
	break;
	case 2:
#ifdef DEBUG
		printf("Reduce if bigger; center if not\n");
#endif
		if (!size->new_w) {
			size->w=element->cur_width;
		}

		if (size->w>width) { // if size is bigger than the screen, resize to the screen
			size->new_x=1;
			size->x=0;
			size->w=width;
			element->cur_width=width;
			size->force_change=1;
		} else { // if size is smaller, center in the screen
			int nx;
			nx=(width-size->w)/2;
			element->cur_width=size->w;
			if ((size->new_x==0)||(size->x!=nx)) {
				size->new_x=1;
				size->x=nx;
				size->force_change=1;
			}
		}

		if (!size->new_h) {
			size->h=element->cur_height;
		}
		if (size->h>height) {
			size->new_y=1;
			size->y=0;
			size->h=height;
			element->cur_height=height;
			size->force_change=1;
		} else {
			int ny;
			ny=(height-size->h)/2;
			element->cur_height=size->h;
			if ((size->new_y==0)||(size->y!=ny)) {
				size->new_y=1;
				size->y=ny;
				size->force_change=1;
			}
		}
	break;
	case 3:
#ifdef DEBUG
		printf("Keep as-is\n");
#endif
	break;
	default:
	break;
	}
#ifdef DEBUG
	printf("New position: %d,%d; new size: %dx%d\n",size->x,size->y,size->w,size->h);
#endif
}

void support_send_dock_up(xcb_query_tree_reply_t *r2,xcb_window_t *wp2) {

	uint16_t i;
	struct wincache_element *element;
	xcb_window_t *wp;
	xcb_query_tree_reply_t *r;

	uint32_t value;

#ifdef DEBUG
	printf("Sending DOCK windows up\n");
#endif

	if (r2==NULL) {
		r=xcb_query_tree_reply(conn,xcb_query_tree(conn,scr->root),0);
		if(!r) {
			return;
		}
		wp=xcb_query_tree_children(r);
	} else {
		wp=wp2;
		r=r2;
	}

	// Move all the DOCK windows, to ensure that the are always on top
	for(i=0;i<r->children_len;i++) {
		element=wincache_find_element(wp[i]);
		if (element==NULL) {
			continue;
		}
		if ((element->mapped)&&(element->type==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DOCK])) {
			value=XCB_STACK_MODE_ABOVE;
			xcb_configure_window(conn,element->window,XCB_CONFIG_WINDOW_STACK_MODE,&value);
		}
	}
	xcb_flush(conn);
	if (r2==NULL) {
		free(r);
	}
}

void support_next_window(int next_app) {

	uint16_t i;
	uint32_t *data;
	uint32_t window_type_i;
	xcb_window_t *wp;
	uint32_t final_window=XCB_WINDOW_NONE;

	struct wincache_element *element,*current_window;

	xcb_query_tree_reply_t *r=xcb_query_tree_reply(conn,xcb_query_tree(conn,scr->root),0);
	if(!r) {
		return;
	}
	wp=xcb_query_tree_children(r);
	i = r->children_len;
	current_window=NULL;
	while(i) {
		i--;
		element=wincache_find_element(wp[i]);
		if (element==NULL) {
			continue;
		}

		if ((element->mapped)&&(element->type!=atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DOCK])) {
			current_window=element;
			break; // this is the current top window
		}
	}

	if (current_window==NULL) {
		free(r);
		return;
	}

	for(i=0;i<r->children_len;i++) {
		element=wincache_find_element(wp[i]);
		if (element==NULL) {
			continue;
		}
		if ((element->mapped==0)||(element->type==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DOCK])) {
			element=NULL;
			continue;
		}
		if (next_app) { // show the next window of other app class in the list
			if (0!=strcmp(current_window->class_name,element->class_name)) { // found a valid window of the same class; move to top
				break;
			}
		} else { // show the next window of an app with the same class
			if (0==strcmp(current_window->class_name,element->class_name)) { // found a valid window of the same class; move to top
				break;
			}
		}
		element=NULL;
	}

	if (element!=NULL) {
		const static uint32_t value[] = { XCB_STACK_MODE_ABOVE };
		/* Move the window to the top of the stack */
		xcb_configure_window (conn, element->window, XCB_CONFIG_WINDOW_STACK_MODE, value);
		support_set_focus();
		support_send_dock_up(r,wp);
		// xcb_flush(conn); // not needed because support_send_dock_up() and support_set_focus() already do it
	} else { // no window found
		if (next_app==1) { // there is only one type of app, but there can be several windows. Let's try to swap between them...
			support_next_window(0);
		}
	}
	free(r);
}

void support_close_window() {

	struct wincache_element *element;
	xcb_window_t window;

	/* delete window */
	xcb_query_tree_reply_t *reply=xcb_query_tree_reply(conn,xcb_query_tree(conn,scr->root),0);
	if(!reply) {
		return;
	}
	xcb_window_t *wp=xcb_query_tree_children(reply);

	uint16_t i=reply->children_len;
	while(i) {
		/* find top-most mapped top-level window, and WM_DELETE_WINDOW it */
		i--;
		window=wp[i];

		element=wincache_find_element(window);
		if (element==NULL) {
			continue;
		}

		// DOCK windows must not be close
		if ((element->mapped)&&(element->type!=atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DOCK])) {
			xcb_client_message_event_t e;
			e.response_type=XCB_CLIENT_MESSAGE;
			e.format=32;
			e.sequence=0;
			e.window=window;
			e.type=atoms[TWM_ATOM_WM_PROTOCOLS];
			e.data.data32[0]=atoms[TWM_ATOM_WM_DELETE_WINDOW];
			e.data.data32[1]=XCB_TIME_CURRENT_TIME;
			e.data.data32[2]=0;
			e.data.data32[3]=0;
			e.data.data32[4]=0;
			xcb_send_event(conn,0,window,XCB_EVENT_MASK_NO_EVENT,(const char *)&e);
			xcb_flush(conn);
			break;
		}
	}
	free(reply);
}

void support_set_focus() {

	uint32_t *data;
	uint32_t window_type_i;
	xcb_window_t window;
	uint32_t final_window=XCB_WINDOW_NONE;

	struct wincache_element *element;

#ifdef DEBUG
	printf("Set focus\n");
#endif

	/* set the new input focus */
	xcb_query_tree_reply_t *r=xcb_query_tree_reply(conn,xcb_query_tree(conn,scr->root),0);
	if (!r) {
		return;
	}
	xcb_window_t *wp=xcb_query_tree_children(r);
	uint16_t i=r->children_len;
	while(i) {
		i--;
		window=wp[i];

		element=wincache_find_element(window);
		if (element==NULL) {
			continue;
		}
#ifdef DEBUG
		printf("Searching: %d, input flag: %d, type: %d (%d %d %d %d)\n",element->mapped,element->input_flag,element->type,atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DESKTOP],atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DOCK],atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_TOOLBAR],atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_MENU]);
#endif

		if ((element->mapped)&&(element->input_flag)&&(element->type!=atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DOCK])) {
#ifdef DEBUG
			printf("Focus for window %d\n",window);
#endif
			xcb_set_input_focus(conn,XCB_INPUT_FOCUS_PARENT,window,XCB_TIME_CURRENT_TIME);
			final_window=window;
			break;
		}
	}
	free(r);
	xcb_change_property(conn,XCB_PROP_MODE_REPLACE,scr->root,atoms[TWM_ATOM__NET_ACTIVE_WINDOW],XCB_ATOM_WINDOW,32,1,&final_window);
	xcb_flush(conn);
}

void support_launch_manager() {

	int f;

	uint32_t window;
	uint32_t value;

	window=wincache_find_launcher_window();

	if (window==0) { // the launcher has not been launched yet; launch it
		f=fork();
		if (f==0) {
			system(launcher_program);
			exit(0);
		}
	} else { // the launcher has been launched. Show its window
		value=XCB_STACK_MODE_ABOVE;
		xcb_configure_window (conn,window, XCB_CONFIG_WINDOW_STACK_MODE, &value);
	}
	key_win.possition=0;
	support_send_dock_up(NULL,NULL);
	menuwin_set_window(); // hide the keyboard when showing the launcher
}
