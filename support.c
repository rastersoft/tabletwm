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

void support_calculate_new_size(xcb_window_t window, struct support_new_size *size) {

	/* The caller must fill the SIZE structure with the current known values, and this function will change them, if needed */

	xcb_get_property_cookie_t window_type_cookie;
	xcb_get_property_cookie_t transient_for_cookie;
	xcb_get_property_cookie_t normal_hints_cookie;
	xcb_get_property_cookie_t size_hints_cookie;
	xcb_get_property_cookie_t wmclass_cookie;

	xcb_get_property_reply_t *window_type;
	xcb_get_property_reply_t *transient_for;
	xcb_get_property_reply_t *normal_hints;
	xcb_get_property_reply_t *size_hints;
	xcb_get_property_reply_t *wmclass;
	
	uint32_t window_type_i;
	uint8_t is_transient;
	uint8_t what_to_do; // 0: maximize always; 1: maximize if resizable, center if not; 2: reduce to screen size if bigger, center if not; 3: leave as-is
	uint8_t is_resizable;
	
	window_type_cookie     = xcb_get_property(conn,0,window,atoms[TWM_ATOM__NET_WM_WINDOW_TYPE],XCB_ATOM_ATOM,0,1);
	transient_for_cookie   = xcb_get_property_unchecked(conn,0,window,atoms[TWM_ATOM_WM_TRANSIENT_FOR],XCB_ATOM_WINDOW,0,1);
	wmclass_cookie         = xcb_icccm_get_wm_class_unchecked(conn,window);
	normal_hints_cookie    = xcb_get_property_unchecked(conn,0,window,atoms[TWM_ATOM_WM_NORMAL_HINTS],XCB_ATOM_WM_SIZE_HINTS,0,1);

	xcb_flush(conn);
	
	window_type = xcb_get_property_reply(conn,window_type_cookie,0);
	// An atom value never has the three upper bits set, so 0xFFFFFFFF is an invalid atom value, and we can use it as a "not-defined" marker
	window_type_i = window_type->length==0 ? 0xFFFFFFFF : *((uint32_t *)(xcb_get_property_value(window_type)));
	free(window_type);
	
	transient_for = xcb_get_property_reply(conn,transient_for_cookie,0);
	is_transient = transient_for->length==0 ? 0 : 1;
	free(transient_for);

	wmclass = xcb_get_property_reply(conn,wmclass_cookie,0);
	xcb_icccm_get_wm_class_reply_t wmclass_data;
	if (wmclass->length!=0) {
		xcb_icccm_get_wm_class_from_reply(&wmclass_data,wmclass);
		printf("CLASS: %s : %s\n",wmclass_data.instance_name,wmclass_data.class_name);
	} else {
		printf("No existe WMCLASS\n");
	}

	normal_hints = xcb_get_property_reply(conn,normal_hints_cookie,0);

	is_resizable=1; // by default, all windows are resizable

	uint32_t v;
	v=*((uint32_t *)(xcb_get_property_value(normal_hints)));
	if ((v&0x30)!=0) { // MIN and MAX size values are set
		int i;
		uint32_t *v2,j;
		int32_t minw,minh,maxw,maxh;

		size_hints_cookie = xcb_get_property_unchecked(conn,0,window,atoms[TWM_ATOM_WM_NORMAL_HINTS],XCB_ATOM_WM_SIZE_HINTS,5,4);
		xcb_flush(conn);
		size_hints = xcb_get_property_reply(conn,size_hints_cookie,0);
		v2=((uint32_t *)(xcb_get_property_value(size_hints)));
		minw=v2[0];
		minh=v2[1];
		maxw=v2[2];
		maxh=v2[3];
		if ((size_hints->length!=0)&&(minw!=0)&&(minh!=0)&&(maxw!=0)&&(maxh!=0)&&(minh==maxh)&&(minw==maxw)) {
			is_resizable=0;
		}
	}

	if ((window_type_i==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DESKTOP])||
		(window_type_i==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_UTILITY])) {
			what_to_do=0;
	} else if (
		(window_type_i==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DIALOG])||
		(window_type_i==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_NORMAL])) {
			what_to_do=1;
	} else if (window_type_i==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_SPLASH]) {
			what_to_do=2;
	} else if (
		(window_type_i==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DOCK])||
		(window_type_i==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_TOOLBAR])||
		(window_type_i==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_MENU])||
		(window_type_i==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DROPDOWN_MENU])||
		(window_type_i==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_POPUP_MENU])||
		(window_type_i==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_TOOLTIP])||
		(window_type_i==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_NOTIFICATION])||
		(window_type_i==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_COMBO])||
		(window_type_i==atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DND])) {
			what_to_do=3;
	} else {
		if (is_transient) {
			what_to_do=2;
		} else {
			what_to_do=1;
		}
	}

	switch(what_to_do) {
	case 1:
		if (is_resizable==0) {
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
		if ((size->new_x==1)&&(size->x!=0)) {
			size->new_x=1;
			size->x=0;
		}
		if ((size->new_y==1)&&(size->y!=0)) {
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

	/* delete window */
	xcb_query_tree_reply_t *reply=xcb_query_tree_reply(conn,xcb_query_tree(conn,scr->root),0);
	xcb_window_t *wp=xcb_query_tree_children(reply);
	uint16_t i=reply->children_len;
	while(i) {
		/* find top-most mapped top-level window, and WM_DELETE_WINDOW it */
		i--;
		xcb_window_t w=wp[i];
		xcb_get_window_attributes_reply_t *r_wattr=xcb_get_window_attributes_reply(conn,xcb_get_window_attributes_unchecked(conn,w),0);
		if(r_wattr->map_state==XCB_MAP_STATE_VIEWABLE){
			xcb_client_message_event_t e;
			e.response_type=XCB_CLIENT_MESSAGE;
			e.format=32;
			e.sequence=0;
			e.window=w;
			e.type=atoms[TWM_ATOM_WM_PROTOCOLS];
			e.data.data32[0]=atoms[TWM_ATOM_WM_DELETE_WINDOW];
			e.data.data32[1]=XCB_TIME_CURRENT_TIME;
			e.data.data32[2]=0;
			e.data.data32[3]=0;
			e.data.data32[4]=0;
			xcb_send_event(conn,0,w,XCB_EVENT_MASK_NO_EVENT,(const char *)&e);
			xcb_flush(conn);
			free(r_wattr);
			break;
		}
		free(r_wattr);
	}
	free(reply);
}

