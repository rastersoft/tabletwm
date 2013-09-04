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

#include "support.h"
#include "globals.h"
#include "wincache.h"

void support_capture_key(uint32_t mods,uint32_t key) {

	xcb_grab_key(conn,0,scr->root,mods,key,XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC); // key itself
	xcb_grab_key(conn,0,scr->root,mods|XCB_MOD_MASK_2,key,XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC); // key + NumLock
	xcb_grab_key(conn,0,scr->root,mods|XCB_MOD_MASK_LOCK,key,XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC); // key + CapsLock
	xcb_grab_key(conn,0,scr->root,mods|XCB_MOD_MASK_2|XCB_MOD_MASK_LOCK,key,XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC); // key + NumLock + CapsLock

}

void support_calculate_new_size(xcb_window_t window, struct support_new_size *size) {

	/* The caller must fill the SIZE structure with the current known values, and this function will change them, if needed */
	
	uint8_t what_to_do; // 0: maximize always; 1: maximize if resizable, center if not; 2: reduce to screen size if bigger, center if not; 3: leave as-is
	
	struct wincache_element *element;
	
	element=wincache_find_element(window);
	if ((element==NULL)||(element->filled==0)||(element->mapped==0)) {
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
		if (element->resizable==0) {
			int nx,ny;
			if (size->new_w) {
				nx=(width-size->w)/2;
				if ((size->new_x==0)||(size->x!=nx)) {
					size->new_x=1;
					size->x=nx;
					size->force_change=1;
				}
			}
			if (size->new_h) {
				ny=(height-size->h)/2;
				if ((size->new_y==0)||(size->y!=ny)) {
					size->new_y=1;
					size->y=ny;
					size->force_change=1;
				}
			}
			break;
		}
	case 0:
		if (((size->new_x==1)&&(size->x!=0))||(size->force_change==1)) {
			size->new_x=1;
			size->x=0;
		}
		if (((size->new_y==1)&&(size->y!=0))||(size->force_change==1)) {
			size->new_y=1;
			size->y=0;
		}
		size->new_w=1;
		size->new_h=1;
		size->w=width;
		size->h=height;
		size->force_change=1;
	break;
	case 2:
		if (size->new_w) {
			if (size->w>width) { // if size is bigger than the screen, resize to the screen
				size->new_x=1;
				size->x=0;
				size->w=width;
				size->force_change=1;
			} else { // if size is smaller, center in the screen
				int nx;
				nx=(width-size->w)/2;
				if ((size->new_x==0)||(size->x!=nx)) {
					size->new_x=1;
					size->x=nx;
					size->force_change=1;
				}
			}
		}
		if (size->new_h) {
			if (size->h>height) {
				size->new_y=1;
				size->y=0;
				size->h=height;
				size->force_change=1;
			} else {
				int ny;
				ny=(height-size->h)/2;
				if ((size->new_y==0)||(size->y!=ny)) {
					size->new_y=1;
					size->y=ny;
					size->force_change=1;
				}
			}
		}
	break;
	case 3:
	break;
	default:
	break;
	}
}

void support_next_app() {


}

void support_next_window() {



}

void support_close_window() {

	xcb_get_window_attributes_cookie_t attr_cookie;
	xcb_get_property_cookie_t          window_type_cookie;
	xcb_get_window_attributes_reply_t *w_attr;
	xcb_get_property_reply_t          *window_type;
	uint32_t window_type_i;
	xcb_window_t window;
	
	/* delete window */
	xcb_query_tree_reply_t *reply=xcb_query_tree_reply(conn,xcb_query_tree(conn,scr->root),0);
	xcb_window_t *wp=xcb_query_tree_children(reply);
	uint16_t i=reply->children_len;
	while(i) {
		/* find top-most mapped top-level window, and WM_DELETE_WINDOW it */
		i--;
		window=wp[i];
		attr_cookie=xcb_get_window_attributes_unchecked(conn,window);
		window_type_cookie = xcb_get_property(conn,0,window,atoms[TWM_ATOM__NET_WM_WINDOW_TYPE],XCB_ATOM_ATOM,0,1);
		xcb_flush(conn);
		xcb_get_window_attributes_reply_t *w_attr=xcb_get_window_attributes_reply(conn,attr_cookie,0);
		window_type = xcb_get_property_reply(conn,window_type_cookie,0);
		if (window_type) {
			// An atom value never has the three upper bits set, so 0xFFFFFFFF is an invalid atom value, and we can use it as a "not-defined" marker
			window_type_i = window_type->length==0 ? 0xFFFFFFFF : *((uint32_t *)(xcb_get_property_value(window_type)));
		} else {
			window_type_i = 0xFFFFFFFF;
		}
		free(window_type);

		// DOCK windows must not be close
		if ((w_attr->map_state==XCB_MAP_STATE_VIEWABLE)&&(window_type_i!=atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DOCK])) {
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
			free(w_attr);
			break;
		}
		free(w_attr);
	}
	free(reply);
	
	support_set_focus();
}

void support_set_focus() {

	xcb_get_window_attributes_cookie_t attr_cookie;
	xcb_get_property_cookie_t          hints_cookie;
	xcb_get_property_cookie_t          window_type_cookie;
	xcb_get_window_attributes_reply_t *w_attr;
	xcb_get_property_reply_t          *window_hints;
	xcb_get_property_reply_t          *window_type;
	uint32_t *data;
	uint32_t window_type_i;
	xcb_window_t window;
	uint32_t final_window=XCB_WINDOW_NONE;

	/* set the new input focus */
	xcb_query_tree_reply_t *r=xcb_query_tree_reply(conn,xcb_query_tree(conn,scr->root),0);
	xcb_window_t *wp=xcb_query_tree_children(r);
	uint16_t i=r->children_len;
	while(i) {
		i--;
		window=wp[i];
		attr_cookie        = xcb_get_window_attributes_unchecked(conn,window);
		hints_cookie       = xcb_get_property_unchecked(conn,0,window,atoms[TWM_ATOM_WM_HINTS],XCB_ATOM_WM_HINTS,0,2);
		window_type_cookie = xcb_get_property_unchecked(conn,0,window,atoms[TWM_ATOM__NET_WM_WINDOW_TYPE],XCB_ATOM_ATOM,0,1);
		xcb_flush(conn);
		w_attr       = xcb_get_window_attributes_reply(conn,attr_cookie,0);
		window_hints = xcb_get_property_reply(conn,hints_cookie,0);
		window_type = xcb_get_property_reply(conn,window_type_cookie,0);
		if (window_type) {
			// An atom value never has the three upper bits set, so 0xFFFFFFFF is an invalid atom value, and we can use it as a "not-defined" marker
			window_type_i = window_type->length<1 ? 0xFFFFFFFF : *((uint32_t *)(xcb_get_property_value(window_type)));
		} else {
			window_type_i = 0xFFFFFFFF;
		}
		free(window_type);

		if ((w_attr)&&(window_hints)&&(window_hints->length>1)) {
			data=((uint32_t *)(xcb_get_property_value(window_hints)));
			// to receive focus, a window must be viewable, have the INPUT flag and not be a DOCK
			if ((w_attr->map_state==XCB_MAP_STATE_VIEWABLE)&&(data[0]&0x01)&&(data[1])&&(window_type_i!=atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DOCK])) {
				xcb_set_input_focus(conn,XCB_INPUT_FOCUS_PARENT,window,XCB_TIME_CURRENT_TIME);
				final_window=window;
				free(w_attr);
				free(window_hints);
				break;
			}
		}
		free(w_attr);
		free(window_hints);
	}
	free(r);
	xcb_change_property(conn,XCB_PROP_MODE_REPLACE,scr->root,atoms[TWM_ATOM__NET_ACTIVE_WINDOW],XCB_ATOM_WINDOW,32,1,&final_window);
	xcb_flush(conn);
}
