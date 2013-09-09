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

void fill_keycodes() {


	struct xkb_keymap *keymap;
	struct xkb_context *context;
	const struct xkb_rule_names rules={
		.rules=NULL,
		.model=NULL,
		.layout=NULL,
		.variant=NULL,
		.options=NULL
	};
	struct xkb_state *state;
	enum xkb_state_component current_state;
	int counter;
	int i,j,k;
	char mods[256];
	char keysym_asc[256];
	char command[15];
	uint32_t max_keys;
	int w,h,retval;
	int jumpto;
	
	context=xkb_context_new(0);
	keymap=xkb_keymap_new_from_names(context,&rules,0);

	state=NULL;

	// Get all the modifier keys
	for(i=8;i<256;i++) {
		state=xkb_state_new(keymap);
		current_state=xkb_state_update_key(state, i,XKB_KEY_DOWN);
		if (current_state!=0) {
			mods[i]=1;
		} else {
			mods[i]=0;
		}
		xkb_state_unref(state);
	}
	mods[7]=1; // fake mod, used for "no mod"

	// Read the keyboard definition files
	FILE *keyboard_file=fopen(KEYBOARD_FILE,"r");
	if (keyboard_file==NULL) {
		printf("Can't open keyboard definition file\n");
	} else {
		retval=fscanf(keyboard_file,"%s %d",command,&keyboard_blocks);
		if (retval!=2) {
			printf("Can't read the number of blocks\n");
		} else {
			max_keys=keyboard_blocks*4*KEYS_PER_ROW;
			keyboard_lowercase=(struct key_element *)malloc(max_keys*sizeof(struct key_element));
			for(counter=0;(!feof(keyboard_file))&&(counter<max_keys);counter++) {
				retval=fscanf(keyboard_file,"%s %d %d",command,&w,&h);
				if(retval!=3) {
					break;
				}
				keyboard_lowercase[counter].size=KEYS_FONT_SIZE;
				keyboard_lowercase[counter].g_element[0]=0;
				keyboard_lowercase[counter].w=w;
				keyboard_lowercase[counter].h=h;
				keyboard_lowercase[counter].keycode=0;
				keyboard_lowercase[counter].modifier=0;
				if (!strcmp(command,"BLANK")) {
					keyboard_lowercase[counter].type=KEY_BLANK;
					keyboard_lowercase[counter].keysym=0;
				} else if (!strcmp(command,"KEY")) {
					keyboard_lowercase[counter].type=KEY_PH;
					retval=fscanf(keyboard_file,"%s",keyboard_lowercase[counter].g_element);
					keyboard_lowercase[counter].keysym=init_utf8_to_keysym(keyboard_lowercase[counter].g_element);
					if (keyboard_lowercase[counter].keysym==0) {
						keyboard_lowercase[counter].type=KEY_BLANK;
					}
				} else if ((!strcmp(command,"KEYSYM"))||(!strcmp(command,"KEYSYMTEXT"))) {
					keyboard_lowercase[counter].type=KEY_PH;
					retval=fscanf(keyboard_file,"%s",keysym_asc);
					keyboard_lowercase[counter].keysym=xkb_keysym_from_name(keysym_asc,0);
					if (keyboard_lowercase[counter].keysym==0) {
						printf("Unknown keysym %s\n",keysym_asc);
						keyboard_lowercase[counter].type=KEY_BLANK;
					} else {
						if (!strcmp(command,"KEYSYMTEXT")) {
							retval=fscanf(keyboard_file,"%s",keyboard_lowercase[counter].g_element);
							keyboard_lowercase[counter].size=KEYS_TEXT_FONT_SIZE;
						} else {
							retval=xkb_keysym_to_utf8(keyboard_lowercase[counter].keysym,keyboard_lowercase[counter].g_element,7);
							if (retval==-1) {
								retval++;
							}
							keyboard_lowercase[counter].g_element[retval]=0;// terminate string
						}
					}
				} else if (!strcmp(command,"TAB")) {
					keyboard_lowercase[counter].type=KEY_TAB;
					keyboard_lowercase[counter].keysym=XK_Tab;
				} else if (!strcmp(command,"SPACE")) {
					keyboard_lowercase[counter].type=KEY_SPACE;
					keyboard_lowercase[counter].keysym=XK_space;
				} else if (!strcmp(command,"RETURN")) {
					keyboard_lowercase[counter].type=KEY_RETURN;
					keyboard_lowercase[counter].keysym=XK_Return;
				} else if (!strcmp(command,"DELETE")) {
					keyboard_lowercase[counter].type=KEY_DELETE;
					keyboard_lowercase[counter].keysym=XK_BackSpace;
				} else if (!strcmp(command,"JUMPTO")) {
					retval=fscanf(keyboard_file,"%d %s",&jumpto,command);
					keyboard_lowercase[counter].type=KEY_JUMPTO;
					keyboard_lowercase[counter].keycode=jumpto;
					keyboard_lowercase[counter].keysym=0;
					if (!strcmp(command,"GEN")) {
						keyboard_lowercase[counter].modifier=0;
					} else if (!strcmp(command,"SHIFT")) {
						keyboard_lowercase[counter].modifier=1;
					} else if (!strcmp(command,"SYMBOLS")) {
						keyboard_lowercase[counter].modifier=2;
					} else if (!strcmp(command,"LETTERS")) {
						keyboard_lowercase[counter].modifier=3;
					}
					if (jumpto>=keyboard_blocks) {
						printf("Ilegal jump to block %d (max. is %d)\n",jumpto,keyboard_blocks);
						keyboard_lowercase[counter].type=KEY_BLANK;
					}
				} else if (!strcmp(command,"UP")) {
					keyboard_lowercase[counter].type=KEY_UP;
					keyboard_lowercase[counter].keysym=XK_Up;
				} else if (!strcmp(command,"DOWN")) {
					keyboard_lowercase[counter].type=KEY_DOWN;
					keyboard_lowercase[counter].keysym=XK_Down;
				} else if (!strcmp(command,"LEFT")) {
					keyboard_lowercase[counter].type=KEY_LEFT;
					keyboard_lowercase[counter].keysym=XK_Left;
				} else if (!strcmp(command,"RIGHT")) {
					keyboard_lowercase[counter].type=KEY_RIGHT;
					keyboard_lowercase[counter].keysym=XK_Right;
				} else {
					printf("Unknown command %s\n",command);
					keyboard_lowercase[counter].type=KEY_BLANK;
					keyboard_lowercase[counter].keysym=0;
				}
			}

			xkb_keysym_t  keysym;
			xkb_keycode_t keycode_mod;
			for(i=7;i<256;i++) { // do a loop on every modifier
				if (!mods[i]) {
					continue; // In this loop we test each modifier with each keycode
				}
				state=xkb_state_new(keymap);
				if (i!=7) {
					xkb_state_update_key(state, i,XKB_KEY_DOWN); // press the modifier key
					keycode_mod=i;
				} else {
					keycode_mod=0;
				}
				for(j=8;j<256;j++) {
					if (mods[j]) {
						continue;  // Don't test modifiers; we want "normal" keys
					}
					keysym=xkb_state_key_get_one_sym(state, j);
					if (keysym==XKB_KEY_NoSymbol) {
						continue;
					}
					for(k=0;k<counter;k++) { // and now we check each desired key with the keysymbol obtained
						if ((keyboard_lowercase[k].type!=KEY_BLANK)&&(keyboard_lowercase[k].keysym==keysym)) {
							keyboard_lowercase[k].keycode=j;
							keyboard_lowercase[k].modifier=keycode_mod;
						}
					}
				}
			}
		}
	}
	
	keyboard_current_block=0;
	xkb_keymap_unref(keymap);
	xkb_context_unref(context);
}

uint32_t init_utf8_to_keysym(unsigned char *data) {

	uint32_t keysym=(uint32_t)(*data++);
	uint32_t tmp;
	if (keysym==0) {
		return 0;
	}
	if (keysym<128) {
		return (keysym);
	}
	if (keysym<192) { // error, not a valid UTF-8 code
		return 0;
	}
	if (keysym<224) { // two-byte element
		keysym&=0x0000001F;
		keysym<<=6;
		tmp=(uint32_t)(*data++);
		keysym|=tmp&0x3F;
		return (keysym);
	}
	if (keysym<240) { // three-byte element
		keysym&=0x0000000F;
		keysym<<=12;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F)<<6;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F);
		return (keysym);
	}
	if (keysym<248) { // four-byte element
		keysym&=0x00000007;
		keysym<<=18;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F)<<12;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F)<<6;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F);
		return (keysym);
	}
	if (keysym<252) { // five-byte element
		keysym&=0x00000003;
		keysym<<=24;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F)<<18;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F)<<12;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F)<<6;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F);
		return (keysym);
	}
	if (keysym<254) { // six-byte element
		keysym&=0x00000001;
		keysym<<=30;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F)<<24;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F)<<18;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F)<<12;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F)<<6;
		tmp=(uint32_t)(*data++);
		keysym|=(tmp&0x3F);
		return (keysym);
	}
	return 0;
}


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

	fill_keycodes();

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
	int counter=4*KEYS_PER_ROW*keyboard_current_block;
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
				case KEY_JUMPTO:
					if (keyboard_lowercase[counter].modifier==1) { // Shift symbol
						cairo_move_to(key_win.cr,0.0,-0.6);
						cairo_line_to(key_win.cr,0.0,0.6);
						cairo_stroke(key_win.cr);
						cairo_move_to(key_win.cr,-0.4,-0.2);
						cairo_line_to(key_win.cr,0.0,-0.6);
						cairo_line_to(key_win.cr,0.4,-0.2);
						cairo_stroke(key_win.cr);
					} else if (keyboard_lowercase[counter].modifier==2) { // Jump to numbers/symbols
						cairo_set_font_size(key_win.cr,KEYS_JUMP_FONT_SIZE);
						cairo_text_extents(key_win.cr,"123", &te);
						cairo_move_to(key_win.cr,-te.x_bearing-(te.width/2.0),0.35);
						cairo_show_text(key_win.cr,"123");
						cairo_set_font_size(key_win.cr,KEYS_FONT_SIZE);
					} else if (keyboard_lowercase[counter].modifier==3) { // Jump to letters
						cairo_set_font_size(key_win.cr,KEYS_JUMP_FONT_SIZE);
						cairo_text_extents(key_win.cr,"abc", &te);
						cairo_move_to(key_win.cr,-te.x_bearing-(te.width/2.0),0.35);
						cairo_show_text(key_win.cr,"abc");
						cairo_set_font_size(key_win.cr,KEYS_FONT_SIZE);
					} else { // Generic symbol
						cairo_set_font_size(key_win.cr,KEYS_JUMP_FONT_SIZE);
						cairo_text_extents(key_win.cr,"...", &te);
						cairo_move_to(key_win.cr,-te.x_bearing-(te.width/2.0),0.35);
						cairo_show_text(key_win.cr,"...");
						cairo_set_font_size(key_win.cr,KEYS_FONT_SIZE);
					}
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
					if (keyboard_lowercase[counter].size!=KEYS_FONT_SIZE) {
						cairo_set_font_size(key_win.cr,keyboard_lowercase[counter].size);
					}
					cairo_text_extents(key_win.cr,keyboard_lowercase[counter].g_element, &te);
					cairo_move_to(key_win.cr,-te.x_bearing-(te.width/2.0),0.35);
					cairo_show_text(key_win.cr,keyboard_lowercase[counter].g_element);
					if (keyboard_lowercase[counter].size!=KEYS_FONT_SIZE) {
						cairo_set_font_size(key_win.cr,KEYS_FONT_SIZE);
					}
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

void menuwin_press_key_at(int x, int y) {

	if (y==0) { // main buttons row
		switch(x) {
		case 0:
			support_close_window();
		break;
		case 5:
			if (key_win.has_keyboard&0x01) {
				key_win.has_keyboard^=0x02;
				menuwin_set_window();
			}
		break;
		case 6:
		case 7:
			support_next_window(1);
		break;
		case 8:
		case 9:
			support_next_window(0);
		break;
		}
		return;
	}

	int lx,ly,i;
	
	i=4*KEYS_PER_ROW*keyboard_current_block;
	for(ly=4;ly>0;ly--) {
		for(lx=0;lx<KEYS_PER_ROW;lx++) {
			if (keyboard_lowercase[i].type!=KEY_BLANK) {
				if ((x>=lx)&&(x<(lx+keyboard_lowercase[i].w))&&(y<=ly)&&(y>(ly-keyboard_lowercase[i].h))) {
					if (keyboard_lowercase[i].type==KEY_JUMPTO) {
						keyboard_current_block=keyboard_lowercase[i].keycode;
						menuwin_expose(NULL);
					} else {
						xcb_keycode_t keycode=keyboard_lowercase[i].keycode;
						xcb_keycode_t keymod =keyboard_lowercase[i].modifier;
						if (keycode!=0) {
							if (keymod!=0) {
								xcb_test_fake_input(conn,XCB_KEY_PRESS,keymod,XCB_CURRENT_TIME,XCB_NONE,0,0,0);
								xcb_flush(conn);
							}
							xcb_test_fake_input(conn,XCB_KEY_PRESS,keycode,XCB_CURRENT_TIME,XCB_NONE,0,0,0);
							xcb_flush(conn);
							xcb_test_fake_input(conn,XCB_KEY_RELEASE,keycode,XCB_CURRENT_TIME,XCB_NONE,0,0,0);
							xcb_flush(conn);
							if (keymod!=0) {
								xcb_test_fake_input(conn,XCB_KEY_RELEASE,keymod,XCB_CURRENT_TIME,XCB_NONE,0,0,0);
								xcb_flush(conn);
							}
						}
					}
					goto key_end;
				}
			}
			i++;
		}
	}
key_end:
	return;
}

void menuwin_expose(xcb_expose_event_t *ee) {

	cairo_set_source_rgb(key_win.cr, 0.75,0.75,0.75);
	cairo_paint(key_win.cr);
	if (key_win.possition) {
		menuwin_paint_buttons();
	}
	xcb_flush(conn);
}
