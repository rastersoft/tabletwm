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

	xcb_unmap_notify_event_t *ee=(xcb_unmap_notify_event_t *)e;

	uint32_t data[]={0,XCB_WINDOW_NONE}; // Withdrawn status
	xcb_change_property(conn,XCB_PROP_MODE_REPLACE,ee->window,atoms[TWM_ATOM_WM_STATE],XCB_ATOM_CARDINAL,32,2,data);
	xcb_flush(conn);

	support_set_focus();
}

void action_map_request(xcb_generic_event_t *e) {

	xcb_map_request_event_t *ee=(xcb_map_request_event_t *)e;

	xcb_map_window(conn,ee->window);

	uint32_t data[]={1,XCB_WINDOW_NONE}; // Normal status
	xcb_change_property(conn,XCB_PROP_MODE_REPLACE,ee->window,atoms[TWM_ATOM_WM_STATE],XCB_ATOM_CARDINAL,32,2,data);
	xcb_flush(conn);

	struct support_new_size sizes;
	memset(&sizes,0,sizeof(struct support_new_size));

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

}

void action_configure_request(xcb_generic_event_t *e) {

	xcb_configure_request_event_t *ee=(xcb_configure_request_event_t *)e;
	
	uint32_t v[7];
	int i;
	int nx,ny,nw,nh;

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

void action_key(xcb_generic_event_t *e) {

	xcb_key_press_event_t *ee=(xcb_key_press_event_t *)e;
	
	// Alt+F4
	if ((ee->detail==70)&&(ee->state&XCB_MOD_MASK_1)) {
		support_close_window();
	}
	
	printf("Captura de tecla: %d %X\n",ee->detail,ee->state);
	
}