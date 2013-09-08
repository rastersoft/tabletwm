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
#include "wincache.h"

void menuwin_init() {

	key_win.surface=cairo_xcb_surface_create(conn,key_win.window,visual_type,width,10);
	key_win.cr=cairo_create(key_win.surface);

	// Set the _NET_SUPPORTING_WM_CHECK property pointing to the window ID in both the root and fake windows
	// Also set the WM_NAME property in both windows to TWM_NAME
	xcb_change_property(conn,XCB_PROP_MODE_REPLACE,scr->root,atoms[TWM_ATOM__NET_SUPPORTING_WM_CHECK],XCB_ATOM_WINDOW,32,1,&key_win.window);
	xcb_change_property(conn,XCB_PROP_MODE_REPLACE,scr->root,atoms[TWM_ATOM__NET_WM_NAME],XCB_ATOM_STRING,8,strlen(TWM_NAME),TWM_NAME);
	xcb_change_property(conn,XCB_PROP_MODE_REPLACE,key_win.window,atoms[TWM_ATOM__NET_SUPPORTING_WM_CHECK],XCB_ATOM_WINDOW,32,1,&key_win.window);
	xcb_change_property(conn,XCB_PROP_MODE_REPLACE,key_win.window,atoms[TWM_ATOM__NET_WM_NAME],XCB_ATOM_STRING,8,strlen(TWM_NAME),TWM_NAME);
	xcb_change_property(conn,XCB_PROP_MODE_REPLACE,key_win.window,atoms[TWM_ATOM__NET_WM_WINDOW_TYPE],XCB_ATOM_ATOM,32,1,&atoms[TWM_ATOM__NET_WM_WINDOW_TYPE_DOCK]);
	xcb_map_window(conn,key_win.window);
	xcb_flush(conn);

	key_win.cache=wincache_fill_element(key_win.window);
	key_win.cache->mapped=1;
	key_win.possition=0;
	key_win.has_keyboard=0;
	key_win.width=width;
	key_win.height=1;
	key_win.enabled_by_mouse=0;

	// Read the keyboard definition files
	FILE *keyboard_file=fopen(KEYBOARD_FILE,"r");
	if (keyboard_file==NULL) {
		printf("Can't open keyboard definition file\n");
	} else {
		int counter;
		uint32_t keysym;
		for(counter=0;!feof(keyboard_file);counter++) {
			char command[15];
			int w,h,retval;
			retval=fscanf(keyboard_file,"%s %d %d",command,&w,&h);
			if(retval!=3) {
				break;
			}
			keyboard_lowercase[counter].g_element[0]=0;
			keyboard_lowercase[counter].w=w;
			keyboard_lowercase[counter].h=h;
			if (!strcmp(command,"BLANK")) {
				keyboard_lowercase[counter].type=KEY_BLANK;
				keysym=0;
			} else if (!strcmp(command,"KEY")) {
				keyboard_lowercase[counter].type=KEY_PH;
				retval=fscanf(keyboard_file,"%s",keyboard_lowercase[counter].g_element);
				keysym=init_utf8_to_keysym(keyboard_lowercase[counter].g_element);
				if (keysym==0) {
					keyboard_lowercase[counter].type=KEY_BLANK;
				}
			} else if (!strcmp(command,"TAB")) {
				keyboard_lowercase[counter].type=KEY_TAB;
				keysym=XK_Tab;
			} else if (!strcmp(command,"SPACE")) {
				keyboard_lowercase[counter].type=KEY_SPACE;
				keysym=XK_space;
			} else if (!strcmp(command,"RETURN")) {
				keyboard_lowercase[counter].type=KEY_RETURN;
				keysym=XK_Return;
			} else if (!strcmp(command,"DELETE")) {
				keyboard_lowercase[counter].type=KEY_DELETE;
				keysym=XK_BackSpace;
			} else if (!strcmp(command,"SHIFT")) {
				keyboard_lowercase[counter].type=KEY_SHIFT;
				keysym=0;
			} else if (!strcmp(command,"UP")) {
				keyboard_lowercase[counter].type=KEY_UP;
				keysym=XK_Up;
			} else if (!strcmp(command,"DOWN")) {
				keyboard_lowercase[counter].type=KEY_DOWN;
				keysym=XK_Down;
			} else if (!strcmp(command,"LEFT")) {
				keyboard_lowercase[counter].type=KEY_LEFT;
				keysym=XK_Left;
			} else if (!strcmp(command,"RIGHT")) {
				keyboard_lowercase[counter].type=KEY_RIGHT;
				keysym=XK_Right;
			} else if (!strcmp(command,"SYMBOL")) {
				keyboard_lowercase[counter].type=KEY_SYMBOL;
				keysym=0;
			} else {
				printf("Unknown command %s\n",command);
				keyboard_lowercase[counter].type=KEY_BLANK;
				keysym=0;
			}
		}
	}
	cairo_select_font_face(key_win.cr,"sans-serif",CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(key_win.cr,1.2);
}

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
	cairo_text_extents_t te;
	for(y=4;y>0;y--) {
		for(x=0;x<KEYS_PER_ROW;x++) {
			if (keyboard_lowercase[counter].type!=KEY_BLANK) {
				menuwin_paint_button(x,y,keyboard_lowercase[counter].w,keyboard_lowercase[counter].h,0.9,0.9,0.9);
				cairo_set_line_width(key_win.cr,0.12);
				cairo_set_source_rgb(key_win.cr,0.0,0.0,0.0);
				switch(keyboard_lowercase[counter].type) {
				case KEY_TAB:
					cairo_move_to(key_win.cr,-0.8,-0.8);
					cairo_line_to(key_win.cr,-0.8,0.0);
					cairo_stroke(key_win.cr);
					cairo_move_to(key_win.cr,0.8,0.8);
					cairo_line_to(key_win.cr,0.8,0.0);
					cairo_stroke(key_win.cr);
					cairo_move_to(key_win.cr,-0.7,-0.4);
					cairo_line_to(key_win.cr,0.8,-0.4);
					cairo_stroke(key_win.cr);
					cairo_move_to(key_win.cr,-0.7,0.4);
					cairo_line_to(key_win.cr,0.8,0.4);
					cairo_stroke(key_win.cr);
					cairo_move_to(key_win.cr,-0.3,-0.8);
					cairo_line_to(key_win.cr,-0.7,-0.4);
					cairo_line_to(key_win.cr,-0.3,0.0);
					cairo_stroke(key_win.cr);
					cairo_move_to(key_win.cr,0.3,0.8);
					cairo_line_to(key_win.cr,0.7,0.4);
					cairo_line_to(key_win.cr,0.3,0.0);
					cairo_stroke(key_win.cr);
				break;
				case KEY_RETURN:
					cairo_move_to(key_win.cr,0.8,-1.6);
					cairo_line_to(key_win.cr,0.8,1.2);
					cairo_line_to(key_win.cr,-0.8,1.2);
					cairo_stroke(key_win.cr);
					cairo_move_to(key_win.cr,-0.4,0.8);
					cairo_line_to(key_win.cr,-0.8,1.2);
					cairo_line_to(key_win.cr,-0.4,1.6);
					cairo_stroke(key_win.cr);
				break;
				case KEY_DELETE:
					cairo_move_to(key_win.cr,0.8,0.0);
					cairo_line_to(key_win.cr,-0.8,0.0);
					cairo_stroke(key_win.cr);
					cairo_move_to(key_win.cr,-0.4,-0.4);
					cairo_line_to(key_win.cr,-0.8,0.0);
					cairo_line_to(key_win.cr,-0.4,0.4);
					cairo_stroke(key_win.cr);
				break;
				case KEY_SHIFT:
					cairo_move_to(key_win.cr,0.0,-0.6);
					cairo_line_to(key_win.cr,0.0,0.6);
					cairo_stroke(key_win.cr);
					cairo_move_to(key_win.cr,-0.4,-0.2);
					cairo_line_to(key_win.cr,0.0,-0.6);
					cairo_line_to(key_win.cr,0.4,-0.2);
					cairo_stroke(key_win.cr);
				break;
				case KEY_UP:
					cairo_move_to(key_win.cr,-0.6,0.3);
					cairo_line_to(key_win.cr,0.0,-0.3);
					cairo_line_to(key_win.cr,0.6,0.3);
					cairo_line_to(key_win.cr,-0.6,0.3);
					cairo_fill(key_win.cr);
				break;
				case KEY_DOWN:
					cairo_move_to(key_win.cr,-0.6,-0.3);
					cairo_line_to(key_win.cr,0.0,0.3);
					cairo_line_to(key_win.cr,0.6,-0.3);
					cairo_line_to(key_win.cr,-0.6,-0.3);
					cairo_fill(key_win.cr);
				break;
				case KEY_LEFT:
					cairo_move_to(key_win.cr,0.3,-0.6);
					cairo_line_to(key_win.cr,-0.3,0.0);
					cairo_line_to(key_win.cr,0.3,0.6);
					cairo_line_to(key_win.cr,0.3,-0.6);
					cairo_fill(key_win.cr);
				break;
				case KEY_RIGHT:
					cairo_move_to(key_win.cr,-0.3,-0.6);
					cairo_line_to(key_win.cr,0.3,0.0);
					cairo_line_to(key_win.cr,-0.3,0.6);
					cairo_line_to(key_win.cr,-0.3,-0.6);
					cairo_fill(key_win.cr);
				break;
				case KEY_PH:
					cairo_text_extents(key_win.cr,keyboard_lowercase[counter].g_element, &te);
					cairo_move_to(key_win.cr,-te.x_bearing-(te.width/2.0),0.35);
					cairo_show_text(key_win.cr,keyboard_lowercase[counter].g_element);
				break;

				// KEY_SYMBOL

				}
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
	w=(width)/KEYS_PER_ROW;
	h=(height)/10;
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
