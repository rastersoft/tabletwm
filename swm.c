/* Simple Window Manager
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

#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include<assert.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<pthread.h>
#include<xcb/xcb.h>
#include<xcb/xcb_atom.h>
#include<xcb/randr.h>

int argc;
char **argv;
int rcf;
struct stat rcs;
char *rcb;
xcb_connection_t *conn;
xcb_screen_t *scr;
uint16_t width;
uint16_t height;
xcb_keycode_t grab;
int grabi;
xcb_atom_t atom_wm_size_hints;
xcb_atom_t atom_wm_normal_hints;
xcb_atom_t atom_wm_protocols;
xcb_atom_t atom_wm_delete_window;
xcb_atom_t atom_wm_transient_for;
xcb_atom_t atom_net_wm_window_type;

uint8_t xrandr;

struct key {
	int a;
	char *e;
	xcb_window_t w;
} key[256];

xcb_keycode_t keya;

static void *in(void *v) {

	while(1) {
		/* open/read/close (hopefully) a pipe */
		int f=open(".swmin",O_RDONLY);
		int32_t b;
		read(f,&b,sizeof(b));
		close(f);
		/* switch case doesn't work with dereferenced string literals!? */
		/* TODO: is behavior same regardless of endianness? */
		if (b==*(int32_t *)"quit") {
			xcb_disconnect(conn);
			exit(0);
		} else if (b==*(int32_t *)"dele") {
			/* delete window */
			xcb_query_tree_reply_t *r=xcb_query_tree_reply(conn,xcb_query_tree(conn,scr->root),0);
			xcb_window_t *wp=xcb_query_tree_children(r);
			uint16_t i=r->children_len;
			while(i) {
				/* find top-most mapped top-level window, and WM_DELETE_WINDOW it */
				i--;
				xcb_window_t w=wp[i];
				xcb_get_window_attributes_reply_t *r=xcb_get_window_attributes_reply(conn,xcb_get_window_attributes_unchecked(conn,w),0);
				if (r->map_state==XCB_MAP_STATE_VIEWABLE) {
					xcb_client_message_event_t e;
					e.response_type=XCB_CLIENT_MESSAGE;
					e.format=32;
					e.sequence=0;
					e.window=w;
					e.type=atom_wm_protocols;
					e.data.data32[0]=atom_wm_delete_window;
					e.data.data32[1]=XCB_TIME_CURRENT_TIME;
					e.data.data32[2]=0;
					e.data.data32[3]=0;
					e.data.data32[4]=0;
					xcb_send_event(conn,0,w,XCB_EVENT_MASK_NO_EVENT,(const char *)&e);
					xcb_flush(conn);
					free(r);
					break;
				}
				free(r);
			}
			free(r);
		} else if (b==*(int32_t *)"alte") {
			/* alternate between first and second top-most mapped top-level windows */
			xcb_query_tree_reply_t *r=xcb_query_tree_reply(conn,xcb_query_tree(conn,scr->root),0);
			xcb_window_t *wp=xcb_query_tree_children(r);
			uint16_t i=r->children_len;
			int j=0;
			
			while(i) {
				i--;
				xcb_window_t w=wp[i];
				xcb_get_window_attributes_reply_t *r=xcb_get_window_attributes_reply(conn,xcb_get_window_attributes_unchecked(conn,w),0);
				if (r->map_state==XCB_MAP_STATE_VIEWABLE) {
					if (!j) {
						j=1;
					} else {
						uint32_t v[]={XCB_STACK_MODE_ABOVE};
						xcb_configure_window(conn,w,XCB_CONFIG_WINDOW_STACK_MODE,v);
						xcb_set_input_focus(conn,XCB_INPUT_FOCUS_PARENT,w,XCB_TIME_CURRENT_TIME);
						xcb_flush(conn);
						free(r);
						break;
					}
				}
				free(r);
			}
			free(r);
		} else if (b==*(int32_t *)"circ") {
			/* circulate all mapped top-level windows */
			xcb_circulate_window(conn,XCB_CIRCULATE_LOWER_HIGHEST,scr->root);
			xcb_flush(conn);
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
		} else if (b==*(int32_t *)"kill") {
			/* forcefully kill top-most mapped top-level window */
			xcb_query_tree_reply_t *r=xcb_query_tree_reply(conn,xcb_query_tree(conn,scr->root),0);
			xcb_window_t *wp=xcb_query_tree_children(r);
			uint16_t i=r->children_len;
			
			while(i) {
				i--;
				xcb_window_t w=wp[i];
				xcb_get_window_attributes_reply_t *r=xcb_get_window_attributes_reply(conn,xcb_get_window_attributes_unchecked(conn,w),0);
				if (r->map_state==XCB_MAP_STATE_VIEWABLE) {
					xcb_kill_client(conn,w);
					xcb_flush(conn);
					free(r);
					break;
				}
				free(r);
			}
			free(r);
		}
	}
}

int main(int ac,char *av[]) {
	argc=ac;
	argv=av;
	chdir(getenv("HOME"));

#if 0
	{
		/* parse .swmrc: modify key[256] */
		rcf=open(".swmrc",O_RDONLY);
		fstat(rcf,&rcs);
		rcb=mmap(0,rcs.st_size,PROT_READ,MAP_PRIVATE,rcf,0);
		char *rcbp=rcb;
		grab=(*rcbp++-'0')*10*10;
		grab+=(*rcbp++-'0')*10;
		grab+=(*rcbp-'0');
		rcbp+=2;
		while(*rcbp){
			xcb_keycode_t i=(*rcbp++-'0')*10*10;
			i+=(*rcbp++-'0')*10;
			i+=(*rcbp-'0');
			rcbp+=2;
			key[i].a=*rcbp=='0'?0:1;
			rcbp+=2;
			key[i].e=rcbp;
			while(*rcbp)rcbp++;
			rcbp+=2;
		}
	}
#endif

	conn=xcb_connect(0,0);
	assert(conn);
	scr=xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
	width=scr->width_in_pixels;
	height=scr->height_in_pixels;

	{
		/* grab events from root window; passive grab on grabkey */
		uint32_t v[]={XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY|XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT};
		xcb_change_window_attributes(conn,scr->root,XCB_CW_EVENT_MASK,v);
		//xcb_grab_key(conn,0,scr->root,0,grab,XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC);
	}
	{
		/* get atoms */
		xcb_intern_atom_cookie_t c[]={
			xcb_intern_atom_unchecked(conn,0,13,"WM_SIZE_HINTS"),
			xcb_intern_atom_unchecked(conn,0,15,"WM_NORMAL_HINTS"),
			xcb_intern_atom_unchecked(conn,0,12,"WM_PROTOCOLS"),
			xcb_intern_atom_unchecked(conn,0,16,"WM_DELETE_WINDOW"),
			xcb_intern_atom_unchecked(conn,0,16,"WM_TRANSIENT_FOR"),
			xcb_intern_atom_unchecked(conn,0,19,"_NET_WM_WINDOW_TYPE"),
		};
		xcb_intern_atom_reply_t *r[]={
			xcb_intern_atom_reply(conn,c[0],0),
			xcb_intern_atom_reply(conn,c[1],0),
			xcb_intern_atom_reply(conn,c[2],0),
			xcb_intern_atom_reply(conn,c[3],0),
			xcb_intern_atom_reply(conn,c[4],0),
			xcb_intern_atom_reply(conn,c[5],0),
		};
		atom_wm_size_hints=r[0]->atom;free(r[0]);
		atom_wm_normal_hints=r[1]->atom;free(r[1]);
		atom_wm_protocols=r[2]->atom;free(r[2]);
		atom_wm_delete_window=r[3]->atom;free(r[3]);
		atom_wm_transient_for=r[4]->atom;free(r[4]);
		atom_net_wm_window_type=r[5]->atom;free(r[5]);
	}
	
	/* detect changes in screen size with xrandr */
	xcb_randr_query_version_reply_t *r=xcb_randr_query_version_reply(conn,xcb_randr_query_version(conn,1,1),0);
	if (r) {
		free(r);
		xcb_randr_select_input(conn,scr->root,XCB_RANDR_NOTIFY_MASK_SCREEN_CHANGE);

		const xcb_query_extension_reply_t *r=xcb_get_extension_data(conn,&xcb_randr_id);
		xrandr=r->first_event;
		free((void *)r);
	}

	xcb_flush(conn);
	/* create pipe reading thread */
	//pthread_t t;
	//pthread_create(&t,0,in,0);
	xcb_generic_event_t *e;
	
	while((e=xcb_wait_for_event(conn))) {
		uint8_t r=e->response_type&~0x80;
		
		if (r>=xrandr) {
			switch(r-xrandr) {
				case(XCB_RANDR_SCREEN_CHANGE_NOTIFY):{
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
							xcb_get_property_reply_t *r=xcb_get_property_reply(conn,xcb_get_property_unchecked(conn,0,w,atom_wm_transient_for,XCB_ATOM_WINDOW,0,1),0);
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
					break;
				}
			}
		} else {
			switch(r) {
				case (XCB_KEY_PRESS): {
					xcb_key_press_event_t *ee=(xcb_key_press_event_t *)e;
					if (!grabi++) {
						/* the grabkey was pressed. now actively grab keyboard */
						xcb_grab_keyboard_unchecked(conn,0,scr->root,XCB_TIME_CURRENT_TIME,XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC);
						xcb_flush(conn);
					} else {
						/*
						 * a key is pressed in active grab mode
						 */
						xcb_keycode_t d=ee->detail;
						uint16_t s=ee->state;
						if (s&XCB_MOD_MASK_1) {
							xcb_query_tree_reply_t *r=xcb_query_tree_reply(conn,xcb_query_tree(conn,scr->root),0);
							xcb_window_t *wp=xcb_query_tree_children(r);
							uint16_t i=r->children_len;
							while(i) {
								i--;
								xcb_window_t w=wp[i];
								xcb_get_window_attributes_reply_t *r=xcb_get_window_attributes_reply(conn,xcb_get_window_attributes_unchecked(conn,w),0);
								if (r->map_state==XCB_MAP_STATE_VIEWABLE) {
									key[d].w=w;
									free(r);
									break;
								}
								free(r);
							}
							free(r);
						} else if (key[d].w&&!(s&XCB_MOD_MASK_CONTROL)) {
							uint32_t v[]={XCB_STACK_MODE_ABOVE};
							xcb_configure_window(conn,key[d].w,XCB_CONFIG_WINDOW_STACK_MODE,v);
							xcb_set_input_focus(conn,XCB_INPUT_FOCUS_PARENT,key[d].w,XCB_TIME_CURRENT_TIME);
							xcb_flush(conn);
						} else if (!(s&XCB_MOD_MASK_SHIFT)) {
							if (key[d].e) {
								if (!fork()) {
									char *av[]={"sh","-c",key[d].e,0};
									execv("/bin/sh",av);
									exit(EXIT_FAILURE);
								}
								if (!key[d].a&&!(s&XCB_MOD_MASK_CONTROL)) {
									keya=d;
								}
							}
						}
					}
				}
				break;
				
				case (XCB_KEY_RELEASE):
					if (!--grabi) {
						xcb_ungrab_keyboard(conn,XCB_TIME_CURRENT_TIME);
						xcb_flush(conn);
					}
				break;
				
				case (XCB_CREATE_NOTIFY):
					/*xcb_create_notify_event_t *ee=(xcb_create_notify_event_t *)e;*/
				break;

				
				case(XCB_UNMAP_NOTIFY):{
					xcb_unmap_notify_event_t *ee=(xcb_unmap_notify_event_t *)e;
					{
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

						/* unassign any window assignment */
						int j=256;
						while(j) {
							j--;
							if (key[j].w==ee->window) {
								key[j].w=0;
								break;
							}
						}
					}
				}
				break;

				case(XCB_MAP_REQUEST): {
				
					xcb_map_request_event_t *ee=(xcb_map_request_event_t *)e;
					
									xcb_get_property_reply_t *r2=xcb_get_property_reply(conn,xcb_get_property_unchecked(conn,0,ee->window,atom_net_wm_window_type,XCB_ATOM_ATOM,0,1),0);
					
					uint32_t tipo;
					tipo=*((uint32_t *)(xcb_get_property_value(r2)));
					
					printf("Window type: l: %d f: %d t: %d  valor: %s\n",r2->length,r2->format,r2->type,xcb_get_atom_name_name(xcb_get_atom_name_reply(conn,xcb_get_atom_name_unchecked(conn,tipo),NULL)));
					free(r2);
					
					{
						/* maximize the window if it is not transient */
						xcb_get_property_reply_t *r=xcb_get_property_reply(conn,xcb_get_property_unchecked(conn,0,ee->window,atom_wm_transient_for,XCB_ATOM_WINDOW,0,1),0);
						if (!r->length) {
							uint32_t v[4]={0,0,width,height};
							xcb_configure_window(conn,ee->window,XCB_CONFIG_WINDOW_X|XCB_CONFIG_WINDOW_Y|XCB_CONFIG_WINDOW_WIDTH|XCB_CONFIG_WINDOW_HEIGHT,v);
						}
						free(r);
					}
					xcb_map_window(conn,ee->window);
					xcb_flush(conn);
					{
						/* set the new input focus */
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
					key[keya].w=ee->window;
					keya=0;
				}
				break;

				case(XCB_CONFIGURE_REQUEST):{
					xcb_configure_request_event_t *ee=(xcb_configure_request_event_t *)e;
					
					uint32_t v[7];
					int i=0;
					/* only modify the request if it is not transient */
					xcb_get_property_reply_t *r=xcb_get_property_reply(conn,xcb_get_property_unchecked(conn,0,ee->window,atom_wm_transient_for,XCB_ATOM_WINDOW,0,1),0);
					ee->value_mask|=XCB_CONFIG_WINDOW_X|XCB_CONFIG_WINDOW_Y|XCB_CONFIG_WINDOW_WIDTH|XCB_CONFIG_WINDOW_HEIGHT;
					printf("%dx%d  %dx%d\n",ee->x,ee->y,ee->width,ee->height);
					if (r->length) {
						int nx,ny,nw,nh;
						nx=ee->x>=0 ? ee->x : (width-ee->width)/2;
						ny=ee->y>=0 ? ee->y : (height-ee->height)/2;
						
						if(ee->width>width) {
							nx=0;
							nw=width;
						} else {
							nw=ee->width;
						}
						if(ee->height>height) {
							ny=0;
							nh=height;
						} else {
							nh=ee->height;
						}
						printf("2: %dx%d  %dx%d\n",nx,ny,nw,nh);
						v[i++]=nx;
						v[i++]=ny;
						v[i++]=nw;
						v[i++]=nh;
					} else {
						v[i++]=0;
						v[i++]=0;
						v[i++]=width;
						v[i++]=height;
					}
					free(r);

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
				break;
				case(XCB_CIRCULATE_REQUEST):
					/*xcb_circulate_request_event_t *ee=(xcb_circulate_request_event_t *)e;*/
				break;

				case(0): {
					xcb_generic_error_t *ee=(xcb_generic_error_t *)e;
					printf("error event type %d\n",ee->error_code);
				}
				break;
				default:
					printf("unhandled event type %d\n",e->response_type);
				break;
			}
		}
		free(e);
	}
	xcb_disconnect(conn);
	return(-1);
}
