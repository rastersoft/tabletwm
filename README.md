TabletWM
========

Version 0.5

TabletWM is a minimalistic Window Manager, oriented to tablet PCs and devices with small screens. It tries to keep all windows maximized. No window has decorations.

It also has a virtual keyboard, to be able to manage apps without a physical keyboard.

It is based on SWM, from CINOLT, available at http://lists.freedesktop.org/archives/xcb/2011-November/007375.html


## Using TabletWM

After launching TabletWM in a XWindows session without other window manager, it will become the manager. Every new window will be maximized if it is non transient, or if its type is DESKTOP, UTILITY, DIALOG or NORMAL. The last two types will be maximized only if they are resizable. If not, they will be put centered.

DOCK type windows are kept as-is, and maintained on top. They also won't receive the keyboard focus. This is designed this way to allow to use them to implement launchers and other kind of programs.

There are three hot keys:

* Alt+F4:   closes the current window
* Alt+TAB:  shows the next application
* Ctrl+TAB: shows the next window of the same application group

Ctrl+TAB allows to work easily with applications that uses several windows (like GIMP), because it will jump between the windows of the same app. It is quite limited, because it will also jump between windows of two different instances of the same app (let's say: two GEdit windows, or two different XTerm).

Pressing the MENU key (the one at the right, between WINDOWS and CTRL) will expand a little options bar with several icons. The first one, red and with an X, will close the current window (like Alt+F4). The next one, with two **applications** and an arrow, allows to switch to the next app (like Alt+TAB). Finally, the next one with two **windows** allows to switch to the next window of the same application group. The last icon launchs an **application launcher**, configurable in the **/etc/tabletwm/tabletwm.cfg** file. By default it launches an XTERM. This bar can also be expanded by moving the cursor to the bottom of the screen.

By pressing Ctrl+MENU, both the options bar and a virtual keyboard will be expanded. A new icon will be available in the options bar, that allows to move the keyboard to the top or bottom part of the screen. This is useful when using the GSL1680 user-space driver, because it generates this keypress when touching with three fingers.

By default, TabletWM uses the currently active keyboard layout. But is possible to edit the file /etc/tabletwm/tabletwm.cfg and set a desired keyboard layout (example: es). This layout will be loaded by TabletWM at startup with setxkbmap.

Also is possible to change the layout for the on-screen keyboard. Currently only the US, QWERTZ and AZERTY layouts are available, but copying the /etc/tabletwm/us.keymap to a new file, and modifying it, allows to change the possition of the codes (of course, you must modify /etc/tabletwm/tabletwm.cfg to specify the language for the on-screen keyboard).

The format for the /etc/tabletwm/tabletwm.cfg file is this:

    keyboard_lang:us
    onscreen_keyboard_map:us
    launcher_command:/usr/bin/xterm

The first entry specifies that the keyboard layout must be US.

The second entry specifies that the on-screen keyboard layout must be US (so the layout will be taken from the file /etc/tabletwm/us.keymap).

The third entry still doesn't work, but will contain the command to launch when the user touches the green button.

The lines must not contain blank spaces.

## Known bugs

* Currently is not possible to assign non-US characters to the keys. In theory it shoud work, but it seems that xkb_state_key_get_one_sym() doesn't work fine with that keycodes.


## License


TabletWM is distributed under the terms of the GPLv2 or (at your option) any later version. The terms of this license are available in the COPYING file.


## Author

TabletWM is developed by Raster Software Vigo (Sergio Costas)

http://www.rastersoft.com
raster@rastersoft.com
