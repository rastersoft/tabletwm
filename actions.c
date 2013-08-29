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

#include "globals.h"
#include "actions.h"

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

	/* set the new input focus */
	xcb_query_tree_reply_t *r=xcb_query_tree_reply(conn,xcb_query_tree(conn,scr->root),0);
	xcb_window_t *wp=xcb_query_tree_children(r);
	uint16_t i=r->children_len;
	while(i) {
		i--;
		xcb_window_t w=wp[i];
		xcb_get_window_attributes_reply_t *r=xcb_get_window_attributes_reply(conn,xcb_get_window_attributes_unchecked(conn,w),0);
		if (r) {
			if (r->map_state==XCB_MAP_STATE_VIEWABLE) {
				xcb_set_input_focus(conn,XCB_INPUT_FOCUS_PARENT,w,XCB_TIME_CURRENT_TIME);
				xcb_flush(conn);
				free(r);
				break;
			}
			free(r);
		}
	}
	free(r);
}

void action_map_request(xcb_generic_event_t *e) {

	xcb_map_request_event_t *ee=(xcb_map_request_event_t *)e;
					
	xcb_get_property_reply_t *r2=xcb_get_property_reply(conn,xcb_get_property_unchecked(conn,0,ee->window,atoms[TWM_ATOM__NET_WM_WINDOW_TYPE],XCB_ATOM_ATOM,0,1),0);
	
	uint32_t tipo;
	tipo=*((uint32_t *)(xcb_get_property_value(r2)));
	
	printf("Window type: l: %d f: %d t: %d  valor: %s\n",r2->length,r2->format,r2->type,xcb_get_atom_name_name(xcb_get_atom_name_reply(conn,xcb_get_atom_name_unchecked(conn,tipo),NULL)));
	free(r2);
	

		/* maximize the window if it is not transient */
	xcb_get_property_reply_t *r=xcb_get_property_reply(conn,xcb_get_property_unchecked(conn,0,ee->window,atoms[TWM_ATOM_WM_TRANSIENT_FOR],XCB_ATOM_WINDOW,0,1),0);
	if (!r->length) {
		uint32_t v[4]={0,0,width,height};
		xcb_configure_window(conn,ee->window,XCB_CONFIG_WINDOW_X|XCB_CONFIG_WINDOW_Y|XCB_CONFIG_WINDOW_WIDTH|XCB_CONFIG_WINDOW_HEIGHT,v);
	}
	free(r);

	xcb_map_window(conn,ee->window);
	xcb_flush(conn);

	/* set the new input focus */
	xcb_query_tree_reply_t *qtr=xcb_query_tree_reply(conn,xcb_query_tree(conn,scr->root),0);
	xcb_window_t *wp=xcb_query_tree_children(qtr);
	uint16_t i=qtr->children_len;
	while(i) {
		i--;
		xcb_window_t w=wp[i];
		xcb_get_window_attributes_reply_t *war=xcb_get_window_attributes_reply(conn,xcb_get_window_attributes_unchecked(conn,w),0);
		if (war->map_state==XCB_MAP_STATE_VIEWABLE) {
			xcb_set_input_focus(conn,XCB_INPUT_FOCUS_PARENT,w,XCB_TIME_CURRENT_TIME);
			xcb_flush(conn);
			free(war);
			break;
		}
		free(war);
	}
	free(qtr);
}

void action_configure_request(xcb_generic_event_t *e) {

	xcb_configure_request_event_t *ee=(xcb_configure_request_event_t *)e;
	
	uint32_t v[7];
	int i=0;
	int nx,ny,nw,nh;

	/* only modify the request if it is not transient */
	xcb_get_property_reply_t *r=xcb_get_property_reply(conn,xcb_get_property_unchecked(conn,0,ee->window,atoms[TWM_ATOM_WM_TRANSIENT_FOR],XCB_ATOM_WINDOW,0,1),0);
	printf("%dx%d  %dx%d\n",ee->x,ee->y,ee->width,ee->height);

	if (r->length) {
		if (ee->x>=0) {
			nx=ee->x;
		} else {
			nx=(width-ee->width)/2;
			ee->value_mask|=XCB_CONFIG_WINDOW_X;
		}
		if (ee->y>=0) {
			ny=ee->y;
		} else {
			ny=(height-ee->height)/2;
			ee->value_mask|=XCB_CONFIG_WINDOW_Y;
		}
		
		if(ee->width>width) {
			nx=0;
			nw=width;
			ee->value_mask|=XCB_CONFIG_WINDOW_X|XCB_CONFIG_WINDOW_WIDTH;
		} else {
			nw=ee->width;
		}
		if(ee->height>height) {
			ny=0;
			nh=height;
			ee->value_mask|=XCB_CONFIG_WINDOW_Y|XCB_CONFIG_WINDOW_HEIGHT;
		} else {
			nh=ee->height;
		}
		printf("2: %dx%d  %dx%d\n",nx,ny,nw,nh);
		v[i++]=nx;
		v[i++]=ny;
		v[i++]=nw;
		v[i++]=nh;
	} else {
		ee->value_mask|=XCB_CONFIG_WINDOW_X|XCB_CONFIG_WINDOW_Y|XCB_CONFIG_WINDOW_WIDTH|XCB_CONFIG_WINDOW_HEIGHT;
		nx=0;
		ny=0;
		nw=width;
		nh=height;
	}
	free(r);

	if (ee->value_mask&XCB_CONFIG_WINDOW_X) {
		v[i++]=nx;
	}
	if (ee->value_mask&XCB_CONFIG_WINDOW_Y) {
		v[i++]=ny;
	}
	if (ee->value_mask&XCB_CONFIG_WINDOW_WIDTH) {
		v[i++]=nw;
	}
	if (ee->value_mask&XCB_CONFIG_WINDOW_HEIGHT) {
		v[i++]=nh;
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
		/* it modifies the window stack; set the new input focus */
		xcb_query_tree_reply_t *r=xcb_query_tree_reply(conn,xcb_query_tree(conn,scr->root),0);
		xcb_window_t *wp=xcb_query_tree_children(r);
		uint16_t i=r->children_len;
		while(i) {
			i--;
			xcb_window_t w=wp[i];
			xcb_get_window_attributes_reply_t *r=xcb_get_window_attributes_reply(conn,xcb_get_window_attributes_unchecked(conn,w),0);
			if (r->map_state==XCB_MAP_STATE_VIEWABLE) {
				xcb_set_input_focus(conn,XCB_INPUT_FOCUS_PARENT,w,XCB_TIME_CURRENT_TIME);
				xcb_flush(conn);
				free(r);
				break;
			}
			free(r);
		}
		free(r);
	}
}


