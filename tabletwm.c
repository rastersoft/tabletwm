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
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>
#include <xcb/randr.h>

#include <signal.h>

#include "globals.h"
#include "init.h"
#include "actions.h"


int main() {

	uint8_t xrandr;

	printf("TabletWM version 0.5\n");

	init_tabletwm();

	uint32_t v[]={XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY|XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT};
	xcb_change_window_attributes(conn,scr->root,XCB_CW_EVENT_MASK,v);
	support_capture_key(XCB_MOD_MASK_CONTROL,23); // Ctrl+TAB
	support_capture_key(XCB_MOD_MASK_1,23); // Alt+TAB
	support_capture_key(XCB_MOD_MASK_1,70); // Alt+F4
	support_capture_key(XCB_MOD_MASK_ANY,135); // MENU key (between the right WINDOWS key and the right Ctrl key)
	//xcb_grab_key(conn,0,scr->root,XCB_MOD_MASK_ANY,XCB_GRAB_ANY,XCB_GRAB_MODE_ASYNC,XCB_GRAB_MODE_ASYNC);

#ifdef DEBUG
	support_capture_key(XCB_MOD_MASK_1,71); // Alt+F5 for VALGRIND tests
#endif

	/* detect changes in screen size with xrandr */
	xcb_randr_query_version_reply_t *r=xcb_randr_query_version_reply(conn,xcb_randr_query_version(conn,1,1),0);
	if (r) {
		xcb_randr_select_input(conn,scr->root,XCB_RANDR_NOTIFY_MASK_SCREEN_CHANGE);

		const xcb_query_extension_reply_t *r2=xcb_get_extension_data(conn,&xcb_randr_id);
		xrandr=r2->first_event;
	}
	free(r);

	xcb_flush(conn);
	xcb_generic_event_t *e;
	
	keep_running=1;
	
	while((keep_running)&&(e=xcb_wait_for_event(conn))) {
		uint8_t r=e->response_type&~0x80;
		
		if (r>=xrandr) {
			switch(r-xrandr) {
				case(XCB_RANDR_SCREEN_CHANGE_NOTIFY):
					action_xrandr_screen_change_notify(e);
				break;
			}
		} else {
			switch(r) {
				case(XCB_KEY_RELEASE):
					action_key(e);
				break;
				case (XCB_CREATE_NOTIFY):
					//xcb_create_notify_event_t *ee=(xcb_create_notify_event_t *)e;
				break;
				case(XCB_UNMAP_NOTIFY):
					action_unmap_notify(e);
				break;
				case(XCB_DESTROY_NOTIFY):
					action_destroy_notify(e);
				break;
				case(XCB_MAP_REQUEST):
					action_map_request(e);
				break;
				case(XCB_CONFIGURE_REQUEST):
					action_configure_request(e);
				break;
				case(XCB_CIRCULATE_REQUEST):
					//xcb_circulate_request_event_t *ee=(xcb_circulate_request_event_t *)e;
				break;
				case(XCB_EXPOSE):
					action_expose(e);
				break;
				case(XCB_ENTER_NOTIFY):
					action_mouse_enter(e);
				break;
				case(XCB_LEAVE_NOTIFY):
					action_mouse_leave(e);
				break;
				case(XCB_BUTTON_RELEASE):
					action_mouse_click(e);
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
	wincache_destroy_element(key_win.window);
	destroy_keycodes();
	xcb_destroy_window(conn,key_win.window);
	xcb_flush(conn);
	xcb_disconnect(conn);
	return(0);
}
