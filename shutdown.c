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
#include "shutdown.h"

void shutdown_init() {

	xcb_void_cookie_t void_cookie;

	shutdown_win.surface=NULL;
	shutdown_win.cr=NULL;
	
	shutdown_win.window = xcb_generate_id(conn);
	uint32_t values[1] = {XCB_EVENT_MASK_EXPOSURE|XCB_EVENT_MASK_BUTTON_RELEASE};

	void_cookie=xcb_create_window_checked (conn,XCB_COPY_FROM_PARENT,shutdown_win.window,scr->root,0,height-1,width,1,0,XCB_WINDOW_CLASS_INPUT_OUTPUT,scr->root_visual,XCB_CW_EVENT_MASK,values);

	xcb_flush(conn);

	if (xcb_request_check(conn,void_cookie)) {
		printf("Can't create the shutdown window\n");
		shutdown_win.window = 0;
	} else {
		uint32_t v[4];
		uint32_t value_mask;

		value_mask=XCB_CONFIG_WINDOW_X|XCB_CONFIG_WINDOW_Y|XCB_CONFIG_WINDOW_WIDTH|XCB_CONFIG_WINDOW_HEIGHT;
		v[0]=width/4;;
		v[1]=height/4;
		v[2]=width/2;
		v[3]=height/2;
		shutdown_win.width=v[2];
		shutdown_win.height=v[3];
		xcb_configure_window(conn,shutdown_win.window,value_mask,v);
		xcb_change_property(conn,XCB_PROP_MODE_REPLACE,shutdown_win.window,atoms[TWM_ATOM__NET_WM_NAME],XCB_ATOM_STRING,8,strlen(TWM_NAME),TWM_NAME);
		xcb_change_property(conn,XCB_PROP_MODE_REPLACE,shutdown_win.window,atoms[TWM_ATOM__NET_WM_WINDOW_TYPE],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DOCK]);
		shutdown_win.cache=wincache_fill_element(shutdown_win.window);
		shutdown_win.cache->mapped=0;
		xcb_flush(conn);
		shutdown_win.surface=cairo_xcb_surface_create(conn,shutdown_win.window,visual_type,width/2,height/2);
		cairo_xcb_surface_set_size(shutdown_win.surface,v[2],v[3]);
		xcb_flush(conn);
	}
}

void shutdown_destroy() {

	shutdown_hide();
	cairo_surface_destroy(shutdown_win.surface);
	xcb_destroy_window(conn,shutdown_win.window);
	wincache_destroy_element(shutdown_win.window);
}

void shutdown_show() {

	if (shutdown_win.cache->mapped==0) {
		xcb_map_window(conn,shutdown_win.window);
		xcb_flush(conn);
		shutdown_win.cache->mapped=1;
		printf("Show\n");
	}

}

void shutdown_hide() {

	if (shutdown_win.cache->mapped==1) {
		xcb_unmap_window(conn,shutdown_win.window);
		xcb_flush(conn);
		shutdown_win.cache->mapped=0;
		printf("Hide\n");
	}
}

void shutdown_paint_button(struct key_win_s* win,int x, int y, int w, int h, int rows, int columns, float r, float g, float b) {

	x=(x*win->width)/columns;
	y=(y*win->height)/rows;
	w=(w*win->width)/columns;
	h=(h*win->height)/rows;
	float scale;
	
	cairo_set_source_rgb(win->cr,r,g,b);
	cairo_move_to(win->cr,x+BUTTON_E_RADIUS,y+BUTTON_MARGIN);
	cairo_arc(win->cr,x+w-BUTTON_E_RADIUS,y+BUTTON_E_RADIUS,BUTTON_RADIUS,M_PI32,0);
	cairo_arc(win->cr,x+w-BUTTON_E_RADIUS,y+h-BUTTON_E_RADIUS,BUTTON_RADIUS,0,M_PI2);
	cairo_arc(win->cr,x+BUTTON_E_RADIUS,y+h-BUTTON_E_RADIUS,BUTTON_RADIUS,M_PI2,M_PI);
	cairo_arc(win->cr,x+BUTTON_E_RADIUS,y+BUTTON_E_RADIUS,BUTTON_RADIUS,M_PI,M_PI32);
	cairo_fill(win->cr);

	cairo_save(win->cr);
	cairo_translate(win->cr,x+w/2,y+h/2);
	w=(win->width)/columns;
	h=(win->height)/rows;
	if (w>h) {
		scale=(float)h;
	} else {
		scale=(float)w;
	}
	scale/=2.0;
	cairo_scale(win->cr,scale,scale);
}

void shutdown_paint_cancel() {

	shutdown_paint_button(&shutdown_win,0,2,1,1,3,2,1.0,0.0,0.0);
	cairo_set_source_rgb(shutdown_win.cr,0.0,0.0,0.0);
	cairo_set_line_width(shutdown_win.cr,0.2);
	cairo_move_to(shutdown_win.cr,-0.8,-0.8);
	cairo_line_to(shutdown_win.cr,0.8,0.8);
	cairo_stroke(shutdown_win.cr);
	cairo_move_to(shutdown_win.cr,0.8,-0.8);
	cairo_line_to(shutdown_win.cr,-0.8,0.8);
	cairo_stroke(shutdown_win.cr);
	cairo_restore(shutdown_win.cr);

}

void shutdown_paint_ok() {

	shutdown_paint_button(&shutdown_win,1,2,1,1,3,2,0.0,1.0,0.0);
	cairo_set_source_rgb(shutdown_win.cr,0.0,0.0,0.0);
	cairo_set_line_width(shutdown_win.cr,0.2);
	cairo_move_to(shutdown_win.cr,0.0,0.8);
	cairo_arc_negative(shutdown_win.cr,-1.0,0.8,1.0,0,M_PI*1.7);
	cairo_stroke(shutdown_win.cr);
	cairo_move_to(shutdown_win.cr,0.0,0.8);
	cairo_arc(shutdown_win.cr,3.0,0.8,3.0,M_PI,M_PI*1.18);
	cairo_stroke(shutdown_win.cr);
	cairo_restore(shutdown_win.cr);

}

void shutdown_expose() {

	if (shutdown_win.cache->mapped==1) {
	
		cairo_text_extents_t te;
	
		shutdown_win.cr=cairo_create(shutdown_win.surface);
		cairo_select_font_face(shutdown_win.cr,"sans-serif",CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size(shutdown_win.cr,18);

		cairo_set_source_rgb(shutdown_win.cr, 0.75,0.75,0.75);
		cairo_paint(shutdown_win.cr);
		shutdown_paint_cancel();
		shutdown_paint_ok();
		cairo_set_source_rgb(shutdown_win.cr,0.0,0.0,0.0);
		cairo_text_extents(shutdown_win.cr,"Shutdown the device?", &te);
		cairo_move_to(shutdown_win.cr,(shutdown_win.width/2)+(-te.x_bearing-(te.width/2.0)),(shutdown_win.height/3)+(-te.y_bearing-(te.height/2.0)));
		cairo_show_text(shutdown_win.cr,"Shutdown the device?");

		xcb_flush(conn);
		cairo_destroy(shutdown_win.cr);
	}
}

void shutdown_press(uint16_t x,uint16_t y) {

	if(y<(height/3)) {
		return;
	}
	if (x<(width/4)) {
		shutdown_hide();
	} else {
		system("dbus-send --system --print-reply --dest=\"org.freedesktop.ConsoleKit\" /org/freedesktop/ConsoleKit/Manager org.freedesktop.ConsoleKit.Manager.Stop");
	}

}
