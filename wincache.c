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

#include <stdio.h>
#include <stdlib.h>
#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>
#include <xcb/xcb_icccm.h>
#include <string.h>

#include "wincache.h"

void wincache_init_element(struct wincache_element *element) {

	element->next=NULL;
	element->prev=NULL;
	element->window=0;
	element->type=0;
	element->mapped=0;
	element->filled=0;
	element->class_name=NULL;
	element->instance=NULL;

}

void wincache_init() {

	wincache_init_element(&wincache_list);
}

struct wincache_element *wincache_find_element(uint32_t window) {

	struct wincache_element *element;
	for(element=&wincache_list;element;element=element->next) {
		if (element->window==window) {
			break;
		}
	}
	return element;
}

struct wincache_element *wincache_add_element(uint32_t window) {

	struct wincache_element *element;

	element=wincache_find_element(window);

	if (element==NULL) { // that window id doesn't exists in the list
		element=(struct wincache_element *)malloc(sizeof(struct wincache_element));
		if (element==NULL) {
			return NULL;
		}
		wincache_init_element(element);
		element->window=window;
		element->next=wincache_list.next;
		if (element->next) {
			element->next->prev=element;
		}
		element->prev=&wincache_list;
		wincache_list.next=element;
	}
	return (element);
}

void wincache_destroy_element(uint32_t window) {

	struct wincache_element *element;
	
	element=wincache_find_element(window);
	if (element) {
		element->prev->next=element->next;
		if (element->next) {
			element->next->prev=element->prev;
		}
		free(element->instance);
		free(element->class_name);
		free(element);
	}
}

struct wincache_element *wincache_fill_element(uint32_t window) {

	struct wincache_element *element;
	
	element=wincache_add_element(window);
	if (!element) {
		return NULL;
	}
	
	// check if it is already filled
	if (element->filled) {
		printf("Already filled %d\n",window);
		return element;
	}
	printf("Filling %d\n",window);
	
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
	
	transient_for_cookie   = xcb_get_property_unchecked(conn,0,window,atoms[TWM_ATOM_WM_TRANSIENT_FOR],XCB_ATOM_WINDOW,0,1);
	window_type_cookie     = xcb_get_property_unchecked(conn,0,window,atoms[TWM_ATOM__NET_WM_WINDOW_TYPE],XCB_ATOM_ATOM,0,1);
	wmclass_cookie         = xcb_icccm_get_wm_class_unchecked(conn,window);
	normal_hints_cookie    = xcb_get_property_unchecked(conn,0,window,atoms[TWM_ATOM_WM_NORMAL_HINTS],XCB_ATOM_WM_SIZE_HINTS,0,1);
	size_hints_cookie      = xcb_get_property_unchecked(conn,0,window,atoms[TWM_ATOM_WM_NORMAL_HINTS],XCB_ATOM_WM_SIZE_HINTS,5,4);

	xcb_flush(conn);
	
	transient_for = xcb_get_property_reply(conn,transient_for_cookie,0);
	if ((transient_for)&&(transient_for->length!=0)) {
		element->is_transient = 1;
	} else {
		element->is_transient = 0;
	}
	free(transient_for);

	window_type = xcb_get_property_reply(conn,window_type_cookie,0);
	// An atom value never has the three upper bits set, so 0xFFFFFFFF is an invalid atom value, and we can use it as a "not-defined" marker
	if ((window_type==NULL)||(window_type->length==0)) {
		element->type = 0xFFFFFFFF;
	} else {
		element->type= ((uint32_t *)(xcb_get_property_value(window_type)))[0];
	}
	free(window_type);
	
	wmclass = xcb_get_property_reply(conn,wmclass_cookie,0);
	xcb_icccm_get_wm_class_reply_t wmclass_data;
	if ((wmclass)&&(wmclass->length!=0)) {
		xcb_icccm_get_wm_class_from_reply(&wmclass_data,wmclass);
		element->class_name=strdup(wmclass_data.class_name);
		element->instance=strdup(wmclass_data.instance_name);
		xcb_icccm_get_wm_class_reply_wipe(&wmclass_data);
	}

	normal_hints = xcb_get_property_reply(conn,normal_hints_cookie,0);

	element->resizable=1; // by default, all windows are resizable

	if (normal_hints) {
		uint32_t v;
		v=*((uint32_t *)(xcb_get_property_value(normal_hints)));
		if ((v&0x30)!=0) { // MIN and MAX size values are set
			int i;
			uint32_t *v2,j;
			int32_t minw,minh,maxw,maxh;

			size_hints = xcb_get_property_reply(conn,size_hints_cookie,0);
			if (size_hints) {
				v2=((uint32_t *)(xcb_get_property_value(size_hints)));
				minw=v2[0];
				minh=v2[1];
				maxw=v2[2];
				maxh=v2[3];
				if ((size_hints->length!=0)&&(minw!=0)&&(minh!=0)&&(maxw!=0)&&(maxh!=0)&&(minh==maxh)&&(minw==maxw)) {
					element->resizable=0;
				}
				free(size_hints);
			}
		}
	}
	element->filled=1;
	return (element);
}

