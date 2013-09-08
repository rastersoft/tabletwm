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

#include "menuwin.h"
#include "globals.h"

void menuwin_paint_close_button() {

	menuwin_paint_button(0,0,1,1,1.0,0.1,0.1);
	cairo_set_source_rgb(key_win.cr,0.0,0.0,0.0);
	cairo_set_line_width(key_win.cr,0.2);
	cairo_move_to(key_win.cr,-0.8,-0.8);
	cairo_line_to(key_win.cr,0.8,0.8);
	cairo_stroke(key_win.cr);
	cairo_move_to(key_win.cr,0.8,-0.8);
	cairo_line_to(key_win.cr,-0.8,0.8);
	cairo_stroke(key_win.cr);
	cairo_restore(key_win.cr);
}

void menuwin_paint_swap_button() {

	menuwin_paint_button(5,0,1,1,1.0,1.0,1.0);
	cairo_set_source_rgb(key_win.cr,0.0,0.0,0.0);
	cairo_set_line_width(key_win.cr,0.2);
	cairo_move_to(key_win.cr,-0.8,-0.2);
	cairo_line_to(key_win.cr,0.0,-0.8);
	cairo_line_to(key_win.cr,0.8,-0.2);
	cairo_stroke(key_win.cr);
	cairo_move_to(key_win.cr,-0.8,0.2);
	cairo_line_to(key_win.cr,0.0,0.8);
	cairo_line_to(key_win.cr,0.8,0.2);
	cairo_stroke(key_win.cr);
	cairo_move_to(key_win.cr,0.0,-0.8);
	cairo_line_to(key_win.cr,0.0,0.8);
	cairo_stroke(key_win.cr);
	cairo_restore(key_win.cr);
}

void menuwin_paint_change_app_button() {

	menuwin_paint_button(6,0,2,1,1.0,1.0,1.0);
	cairo_set_line_width(key_win.cr,0.2);
	cairo_set_source_rgb(key_win.cr,1.0,1.0,1.0);
	cairo_rectangle(key_win.cr,-1.0,-0.8,-1.2,1.6);
	cairo_fill(key_win.cr);
	cairo_set_source_rgb(key_win.cr,0.0,0.0,0.0);
	cairo_rectangle(key_win.cr,-1.0,-0.8,-1.2,1.6);
	cairo_stroke(key_win.cr);
	cairo_set_source_rgb(key_win.cr,1.0,1.0,1.0);
	cairo_rectangle(key_win.cr,1.0,-0.8,1.2,1.6);
	cairo_fill(key_win.cr);
	cairo_set_source_rgb(key_win.cr,0.0,0.0,0.0);
	cairo_rectangle(key_win.cr,1.0,-0.8,1.2,1.6);
	cairo_stroke(key_win.cr);

	cairo_move_to(key_win.cr,-0.7,0.0);
	cairo_line_to(key_win.cr,0.7,0.0);
	cairo_stroke(key_win.cr);
	cairo_move_to(key_win.cr,0.3,-0.4);
	cairo_line_to(key_win.cr,0.7,0.0);
	cairo_line_to(key_win.cr,0.3,0.4);
	cairo_stroke(key_win.cr);
	
	cairo_set_line_width(key_win.cr,0.12);
	cairo_move_to(key_win.cr,-1.2,-0.50);
	cairo_line_to(key_win.cr,-2.0,-0.50);
	cairo_stroke(key_win.cr);
	cairo_move_to(key_win.cr,-1.2,-0.25);
	cairo_line_to(key_win.cr,-2.0,-0.25);
	cairo_stroke(key_win.cr);
	cairo_move_to(key_win.cr,-1.2,0.0);
	cairo_line_to(key_win.cr,-2.0,0.0);
	cairo_stroke(key_win.cr);
	cairo_move_to(key_win.cr,-1.2,0.25);
	cairo_line_to(key_win.cr,-2.0,0.25);
	cairo_stroke(key_win.cr);
	cairo_move_to(key_win.cr,-1.2,0.50);
	cairo_line_to(key_win.cr,-2.0,0.50);
	cairo_stroke(key_win.cr);
	
	cairo_move_to(key_win.cr,1.2,-0.50);
	cairo_line_to(key_win.cr,2.0,-0.50);
	cairo_stroke(key_win.cr);
	cairo_move_to(key_win.cr,1.2,-0.25);
	cairo_line_to(key_win.cr,2.0,-0.25);
	cairo_stroke(key_win.cr);
	cairo_move_to(key_win.cr,1.2,0.0);
	cairo_line_to(key_win.cr,2.0,0.0);
	cairo_stroke(key_win.cr);
	cairo_move_to(key_win.cr,1.2,0.25);
	cairo_line_to(key_win.cr,2.0,0.25);
	cairo_stroke(key_win.cr);
	cairo_move_to(key_win.cr,1.2,0.50);
	cairo_line_to(key_win.cr,2.0,0.50);
	cairo_stroke(key_win.cr);
	
	cairo_restore(key_win.cr);
}

void menuwin_paint_change_window_button() {

	menuwin_paint_button(8,0,2,1,1.0,1.0,1.0);
	
	cairo_set_line_width(key_win.cr,0.2);
	cairo_set_source_rgb(key_win.cr,1.0,1.0,1.0);
	cairo_rectangle(key_win.cr,-0.2,-0.8,1.2,1.3);
	cairo_fill(key_win.cr);
	cairo_set_source_rgb(key_win.cr,0.0,0.0,0.0);
	cairo_rectangle(key_win.cr,-0.2,-0.8,1.2,1.3);
	cairo_stroke(key_win.cr);

	cairo_set_source_rgb(key_win.cr,1.0,1.0,1.0);
	cairo_rectangle(key_win.cr,-0.8,-0.5,1.2,1.3);
	cairo_fill(key_win.cr);
	cairo_set_source_rgb(key_win.cr,0.0,0.0,0.0);
	cairo_rectangle(key_win.cr,-0.8,-0.5,1.2,1.3);
	cairo_stroke(key_win.cr);
	cairo_restore(key_win.cr);
}

void menuwin_paint_launcher() {

	menuwin_paint_button(10,0,2,1,0.1,1.0,0.2);
	cairo_set_line_width(key_win.cr,0.12);
	cairo_set_source_rgb(key_win.cr,1.0,1.0,1.0);
	cairo_arc(key_win.cr,0.0,0.0,0.8,0,M_PI42);
	cairo_fill(key_win.cr);
	cairo_set_source_rgb(key_win.cr,0.0,0.0,0.0);
	cairo_arc(key_win.cr,0.0,0.0,0.8,0,M_PI42);
	cairo_stroke(key_win.cr);
	
	
	cairo_rectangle(key_win.cr,-0.5,-0.5,0.25,0.25);
	cairo_fill(key_win.cr);
	cairo_rectangle(key_win.cr,-0.5,-0.15,0.25,0.25);
	cairo_fill(key_win.cr);
	cairo_rectangle(key_win.cr,-0.5,0.2,0.25,0.25);
	cairo_fill(key_win.cr);
	cairo_rectangle(key_win.cr,-0.15,-0.5,0.25,0.25);
	cairo_fill(key_win.cr);
	cairo_rectangle(key_win.cr,-0.15,-0.15,0.25,0.25);
	cairo_fill(key_win.cr);
	cairo_rectangle(key_win.cr,-0.15,0.2,0.25,0.25);
	cairo_fill(key_win.cr);
	cairo_rectangle(key_win.cr,0.2,-0.5,0.25,0.25);
	cairo_fill(key_win.cr);
	cairo_rectangle(key_win.cr,0.2,-0.15,0.25,0.25);
	cairo_fill(key_win.cr);
	cairo_rectangle(key_win.cr,0.2,0.2,0.25,0.25);
	cairo_fill(key_win.cr);
	cairo_restore(key_win.cr);
}

void menuwin_paint_keyboard() {

	int x,y;
	int counter=0;
	for(y=4;y>0;y--) {
		for(x=0;x<KEYS_PER_ROW;x++) {
			if (keyboard_lowercase[counter].type!=KEY_BLANK) {
				menuwin_paint_button(x,y,keyboard_lowercase[counter].w,keyboard_lowercase[counter].h,0.9,0.9,0.9);
				cairo_restore(key_win.cr);
			}
			counter++;
		}
	}
}

void menuwin_paint_buttons() {

	menuwin_paint_close_button();
	if (key_win.has_keyboard&0x01) {
		menuwin_paint_swap_button();
		menuwin_paint_keyboard();
	}
	menuwin_paint_change_app_button();
	menuwin_paint_change_window_button();
	menuwin_paint_launcher();
}

void menuwin_paint_button(int x, int y, int w, int h, float r, float g, float b) {

	// coordinates and size are in width/10 and height/10 units
	// 0,0 is bottom-left
	
	x=(x*width)/KEYS_PER_ROW;
	y=key_win.height-(((y+1)*height)/10);
	w=(w*width)/KEYS_PER_ROW;
	h=(h*height)/10;
	float scale;
	
	cairo_set_source_rgb(key_win.cr,r,g,b);
	cairo_move_to(key_win.cr,x+BUTTON_E_RADIUS,y+BUTTON_MARGIN);
	cairo_arc(key_win.cr,x+w-BUTTON_E_RADIUS,y+BUTTON_E_RADIUS,BUTTON_RADIUS,M_PI32,0);
	cairo_arc(key_win.cr,x+w-BUTTON_E_RADIUS,y+h-BUTTON_E_RADIUS,BUTTON_RADIUS,0,M_PI2);
	cairo_arc(key_win.cr,x+BUTTON_E_RADIUS,y+h-BUTTON_E_RADIUS,BUTTON_RADIUS,M_PI2,M_PI);
	cairo_arc(key_win.cr,x+BUTTON_E_RADIUS,y+BUTTON_E_RADIUS,BUTTON_RADIUS,M_PI,M_PI32);
	cairo_fill(key_win.cr);

	cairo_save(key_win.cr);
	cairo_translate(key_win.cr,x+w/2,y+h/2);
	if (w>h) {
		scale=(float)h;
	} else {
		scale=(float)w;
	}
	scale/=2.0;
	cairo_scale(key_win.cr,scale,scale);
}

void menuwin_set_window() {

	uint32_t v[4];
	uint32_t value_mask;

	value_mask=XCB_CONFIG_WINDOW_X|XCB_CONFIG_WINDOW_Y|XCB_CONFIG_WINDOW_WIDTH|XCB_CONFIG_WINDOW_HEIGHT;
	if (key_win.possition==1) {
		if (key_win.has_keyboard&0x01) { // keyboard enabled?
			v[2]=width;
			v[3]=height/2;
			if (key_win.has_keyboard&0x02) { // keyboard on top?
				v[0]=0;
				v[1]=0;
			} else {
				v[0]=0;
				v[1]=height/2;
			}
		} else {
			v[0]=0;
			v[1]=9*height/10;
			v[2]=width;
			v[3]=height/10;
		}
	} else {
		v[0]=0;
		v[1]=height-1;
		v[2]=width;
		v[3]=1;
	}
	key_win.width=v[2];
	key_win.height=v[3];
	xcb_configure_window(conn,key_win.window,value_mask,v);
	cairo_xcb_surface_set_size(key_win.surface,v[2],v[3]);
	menuwin_expose(NULL);
}

void menuwin_expose(xcb_expose_event_t *ee) {

	cairo_set_source_rgb(key_win.cr, 0.75,0.75,0.75);
	cairo_paint(key_win.cr);
	if (key_win.possition) {
		menuwin_paint_buttons();
	}
	xcb_flush(conn);
}
