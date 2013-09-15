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
#include <xcb/randr.h>
#include <string.h>

#include "globals.h"
#include "actions.h"
#include "support.h"
#include "wincache.h"
#include "menuwin.h"

void action_xrandr_screen_change_notify(xcb_generic_event_t *e) {

	/*
	 * if screen is changed:
	 * modify global width/height
	 * maximize all currently mapped windows to new size
	 */

	xcb_randr_screen_change_notify_event_t *ee=(xcb_randr_screen_change_notify_event_t *)e;
	width=ee->width;
	height=ee->height;
	xcb_query_tree_reply_t *r=xcb_query_tree_reply(conn,xcb_query_tree(conn,scr->root),0);
	xcb_window_t *wp=xcb_query_tree_children(r);
	uint16_t i=r->children_len;
	while(i) {
		i--;
		xcb_window_t w=wp[i];
		xcb_get_window_attributes_reply_t *r=xcb_get_window_attributes_reply(conn,xcb_get_window_attributes_unchecked(conn,w),0);
		if (r->map_state==XCB_MAP_STATE_VIEWABLE) {
			xcb_get_property_reply_t *r=xcb_get_property_reply(conn,xcb_get_property_unchecked(conn,0,w,atoms[TWM_ATOM_WM_TRANSIENT_FOR],XCB_ATOM_WINDOW,0,1),0);
			if (!r->length) {
				uint32_t v[4]={0,0,width,height};
				xcb_configure_window(conn,w,XCB_CONFIG_WINDOW_X|XCB_CONFIG_WINDOW_Y|XCB_CONFIG_WINDOW_WIDTH|XCB_CONFIG_WINDOW_HEIGHT,v);
			}
			free(r);
		}
		free(r);
	}
	xcb_flush(conn);
	free(r);
}

void action_unmap_notify(xcb_generic_event_t *e) {

	struct wincache_element *element;

	xcb_unmap_notify_event_t *ee=(xcb_unmap_notify_event_t *)e;

	element=wincache_find_element(ee->window);
	if (element) {
		element->mapped=0;
	}

	uint32_t data[]={0,XCB_WINDOW_NONE}; // Withdrawn status
	xcb_change_property(conn,XCB_PROP_MODE_REPLACE,ee->window,atoms[TWM_ATOM_WM_STATE],XCB_ATOM_CARDINAL,32,2,data);
	xcb_flush(conn);

	support_send_dock_up(NULL,NULL);
	support_set_focus();
}

void action_destroy_notify(xcb_generic_event_t *e) {

	struct wincache_element *element;

	xcb_destroy_notify_event_t *ee=(xcb_destroy_notify_event_t *)e;

	wincache_destroy_element(ee->window);
}

void action_map_request(xcb_generic_event_t *e) {

	struct wincache_element *element;

	xcb_map_request_event_t *ee=(xcb_map_request_event_t *)e;
#ifdef DEBUG
	printf("Mapping %d\n",ee->window);
#endif
	element=wincache_fill_element(ee->window);
	if (element) {
		element->mapped=1;
	}
	xcb_map_window(conn,ee->window);

	uint32_t data[]={1,XCB_WINDOW_NONE}; // Normal status
	xcb_change_property(conn,XCB_PROP_MODE_REPLACE,ee->window,atoms[TWM_ATOM_WM_STATE],XCB_ATOM_CARDINAL,32,2,data);
	xcb_flush(conn);

	struct support_new_size sizes;
	if (element->asked_for_new_size==1) {
		memcpy(&sizes,&element->new_size,sizeof(struct support_new_size));
		element->asked_for_new_size=0;
	} else {
		memset(&sizes,0,sizeof(struct support_new_size));
	}

	support_calculate_new_size(ee->window,&sizes);

	/* set window size and position if needed */
	uint32_t v[4];
	uint32_t flags=0;
	int loop=0;
	if (sizes.new_x) {
		v[loop++]=sizes.x;
		flags|=XCB_CONFIG_WINDOW_X;
	}
	if (sizes.new_y) {
		v[loop++]=sizes.y;
		flags|=XCB_CONFIG_WINDOW_Y;
	}
	if (sizes.new_w) {
		v[loop++]=sizes.w;
		flags|=XCB_CONFIG_WINDOW_WIDTH;
	}
	if (sizes.new_h) {
		v[loop++]=sizes.h;
		flags|=XCB_CONFIG_WINDOW_HEIGHT;
	}
	if ((flags)&&(sizes.force_change)) {
		xcb_configure_window(conn,ee->window,flags,v);
	}

	support_set_focus();
	support_send_dock_up(NULL,NULL);
}

void action_configure_notify(xcb_generic_event_t *e) {

	struct wincache_element *element;

	xcb_configure_notify_event_t *ee=(xcb_configure_notify_event_t *)e;

#ifdef DEBUG
	printf("Configure notify %d\n",ee->window);
#endif

	element=wincache_fill_element(ee->window);
	if ((element)&&(element->type!=atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DOCK])) {
		support_send_dock_up(NULL,NULL); // after a configure notify for a non-dock window, ensure that the docks are always on top
	}
}

void action_configure_request(xcb_generic_event_t *e) {

	xcb_configure_request_event_t *ee=(xcb_configure_request_event_t *)e;
	
	uint32_t v[7];
	int i;
	int nx,ny,nw,nh;
#ifdef DEBUG
	printf("Configure request for window %d\n",ee->window);
#endif

	struct support_new_size sizes;
	memset(&sizes,0,sizeof(struct support_new_size));

	i=0;
	if (ee->value_mask&XCB_CONFIG_WINDOW_X) {
		sizes.new_x=1;
		sizes.x=ee->x;
	}
	if (ee->value_mask&XCB_CONFIG_WINDOW_Y) {
		sizes.new_y=1;
		sizes.y=ee->y;
	}
	if (ee->value_mask&XCB_CONFIG_WINDOW_WIDTH) {
		sizes.new_w=1;
		sizes.w=ee->width;
	}
	if (ee->value_mask&XCB_CONFIG_WINDOW_HEIGHT) {
		sizes.new_h=1;
		sizes.h=ee->height;
	}
	
	support_calculate_new_size(ee->window,&sizes);

	i=0;
	if (sizes.new_x) {
		ee->value_mask|=XCB_CONFIG_WINDOW_X;
		v[i++]=sizes.x;
	}
	if (sizes.new_y) {
		ee->value_mask|=XCB_CONFIG_WINDOW_Y;
		v[i++]=sizes.y;
	}
	if (sizes.new_w) {
		ee->value_mask|=XCB_CONFIG_WINDOW_WIDTH;
		v[i++]=sizes.w;
	}
	if (sizes.new_h) {
		ee->value_mask|=XCB_CONFIG_WINDOW_HEIGHT;
		v[i++]=sizes.h;
	}
	if (ee->value_mask&XCB_CONFIG_WINDOW_BORDER_WIDTH) {
		v[i++]=ee->border_width;
	}
	if (ee->value_mask&XCB_CONFIG_WINDOW_SIBLING) {
		v[i++]=ee->sibling;
	}
	if (ee->value_mask&XCB_CONFIG_WINDOW_STACK_MODE) {
		v[i]=ee->stack_mode;
	}
	xcb_configure_window(conn,ee->window,ee->value_mask,v);
	xcb_flush(conn);
	if (ee->value_mask&XCB_CONFIG_WINDOW_STACK_MODE) {
		support_set_focus();
	}
}

void action_expose(xcb_generic_event_t *e) {

	xcb_expose_event_t *ee=(xcb_expose_event_t *)e;
	if (ee->window==key_win.window) {
		menuwin_expose(ee);
	} else if (ee->window==shutdown_win.window) {
		shutdown_expose();
	}
}

void action_mouse_enter(xcb_generic_event_t *e) {

	// Expand the menu (without keyboard) if the mouse enters the bottom part of the screen
	if (key_win.possition==0) {
		key_win.has_keyboard&=2; // disable keyboard, but keep where it was before
		key_win.possition=1; // enable the menu
		key_win.enabled_by_mouse=1;
		menuwin_set_window();
	}
}

void action_mouse_leave(xcb_generic_event_t *e) {

	// Collapse the menu if the mouse leaves it, but only if it was expanded because the mouse entered the bottom part of the screen
	// Never collapse it if it was expanded with a key
	if ((key_win.possition==1)&&(key_win.enabled_by_mouse==1)) {
		key_win.possition=0; // disable the menu
		key_win.enabled_by_mouse=0;
		menuwin_set_window();
	}
}

void action_mouse_click(xcb_generic_event_t *e) {

	struct xcb_button_press_event_t *ee=(struct xcb_button_press_event_t *)e;
	
	if (ee->event==key_win.window) {
		int x=(ee->event_x*KEYS_PER_ROW)/width;
		int y=((key_win.height-ee->event_y)*10)/height;
	
		menuwin_press_key_at(x,y);
	}
	if (ee->event==shutdown_win.window) {
		shutdown_press(ee->event_x,ee->event_y);
	}
}

void action_key(xcb_generic_event_t *e) {

	xcb_key_press_event_t *ee=(xcb_key_press_event_t *)e;
	
	// Alt+F4
	if ((ee->detail==70)&&(ee->state&XCB_MOD_MASK_1)) {
		support_close_window();
		return;
	}
	
	// Ctrl+TAB
	if ((ee->detail==23)&&(ee->state&XCB_MOD_MASK_CONTROL)) {
		support_next_window(0);
		return;
	}

	// Alt+TAB
	if ((ee->detail==23)&&(ee->state&XCB_MOD_MASK_1)) {
		support_next_window(1);
		return;
	}

	// MENU with keyboard
	if ((ee->detail==135)&&(ee->state&XCB_MOD_MASK_CONTROL)&&(!(ee->state&XCB_MOD_MASK_1))) {
		key_win.enabled_by_mouse=0;
		key_win.possition=1-key_win.possition; // enable/disable the menu
		if (key_win.possition) {
			key_win.has_keyboard|=1; // enable keyboard, and put it where it was before
		} else {
			key_win.has_keyboard&=2; // disable keyboard, but keep it where it was before
		}
		support_send_dock_up(NULL,NULL);
		menuwin_set_window();
	}

	// MENU alone
	if ((ee->detail==135)&&(!(ee->state&XCB_MOD_MASK_CONTROL))&&(!(ee->state&XCB_MOD_MASK_1))) {
		key_win.has_keyboard&=2; // disable keyboard, but remember where it was before
		key_win.possition=1-key_win.possition; // enable/disable the menu
		key_win.enabled_by_mouse=0;
		support_send_dock_up(NULL,NULL);
		menuwin_set_window();
	}

	// SHUTDOWN
	if ((ee->detail==135)&&(!(ee->state&XCB_MOD_MASK_CONTROL))&&((ee->state&XCB_MOD_MASK_1))) {
		if(shutdown_win.cache->mapped==1) {
			shutdown_hide();
		} else {
			shutdown_show();
		}
	}

#ifdef DEBUG
	// Alt+F5
	if ((ee->detail==71)&&(ee->state&XCB_MOD_MASK_1)) {
		keep_running=0;
		return;
	}

	printf("Key capture: %d %X\n",ee->detail,ee->state);
#endif
}
