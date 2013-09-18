TabletWM
========

Version 0.16

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

Pressing Alt+MENU (or the power button), TabletWM will ask to power down the device. It calls **/sbin/halt**, so you must first give it SUID priviledges with **sudo chmod 4755 /sbin/halt**.

Is important to remember that TabletWM grabs the mouse when the on-screen keyboard is visible. This is a must to avoid problems with popups and other kind of windows. This means that, if you have the on-screen keyboard visible and want to do a click in the screen, you must hide the keyboard first.

By default, TabletWM uses the currently active layout for the physical keyboard. But is possible to edit the file /etc/tabletwm/tabletwm.cfg and set a desired keyboard layout (example: es). This layout will be loaded by TabletWM at startup with setxkbmap.

Also is possible to change the layout for the on-screen keyboard. Currently only the US, ES, QWERTZ and AZERTY layouts are available, but copying the /etc/tabletwm/us.keymap to a new file, and modifying it, allows to change the possition of the codes (of course, you must modify /etc/tabletwm/tabletwm.cfg to specify the language for the on-screen keyboard).

The format for the /etc/tabletwm/tabletwm.cfg file is this:

    keyboard_lang:us
    onscreen_keyboard_map:us
    launcher_command:/usr/bin/xterm

The first entry specifies that the physical keyboard layout must be US. This is set by calling to **setxkbmap**.

The second entry specifies that the on-screen keyboard layout must be US (so the layout will be taken from the file /etc/tabletwm/us.keymap).

The third entry still doesn't work, but will contain the command to launch when the user touches the green button.

The lines must not contain blank spaces.


## KEYMAP file format

The KEYMAP files allows to specify the layout of the on-screen keyboard. This file has to contain several lines, each one describing one key.

The first line in the file must contain a **BLOCKS** command, followed by the number of *layouts* or *keyboards* that will be defined in this file. Ussually this number is 3: one lowercase layout, one uppercase layout, and one numbers/symbols layout. But it can be changed to allow to put more symbols.

After that, there must come as many blocks of 48 key definitions (because each layout contains 48 keys). The format is *command W H [params]*.
The W and H parameters specify the width and the height of the key. The default size is 1 1. Large keys like *return* are 1 2, or the *space bar* which are 3 1.

*Command* allows to specify the keys themselves. It can be:

* BLANK 1 1: when a key is bigger that one possition, the other possitions that it occupies must be filled with BLANK 1 1 commands.
* KEYSYM W H keysym: *keysym* is a keysym definition, availables in /usr/include/xkbcommon/xkbcommon-keysyms.h, but removing the XKB_KEY_ part.
* KEY W H letter: *letter* is an UTF-8 character. Since a lot of keysym numbers are the same than its UTF-8 character, this can work for them (but be careful, it doesn't work for all, only for very common characters).
* KEYSYMTEXT W H keysym text: first, a keysym definition (like in KEYSYM), followed by a text that will be displayed in the key. Useful for non-graphic keys like function ones (F1 to F12).
* TAB W H: has no params. Creates a TAB key.
* SPACE W H: has no params. Creates a SPACE key.
* RETURN W H: has no params. Creates a RETURN key.
* DELETE W H: has no params. Creates a DELETE key.
* UP W H: has no params. Creates a CURSOR UP key.
* DOWN W H: has no params. Creates a CURSOR DOWN key.
* LEFT W H: has no params. Creates a CURSOR LEFT key.
* RIGHT W H: has no params. Creates a CURSOR RIGHT key.
* JUMPTO W H block content: *block* contains the layout number that will be shown when the user touches this key. *content* specifies what text/symbol is shown in this key. It can be *GEN* (shows *...*), *SHIFT* (shows a shift arrow), *LETTERS* (shows *abc*) or *symbols* (shows *123*).

The name of the file must be *XXXXXX.keymap*, being *XXXXXX* the text that the user must put in the *onscreen_keyboard_map* entry in the configuration file.

If TabletWM is unable to map a character to a keycode, that key will be shown in red color.


## Compiling from source code and installing system-wide

TabletWM needs XCB, Cairo and XKBCommon libraries.

Just run as a normal user

	make
	sudo make install

in your device, and will compile and install the window manager. You can also type

	sudo make install-init

and an *init.d* will be installed that will launch TabletWM automatically during startup using the *debian* user. This, of course, only works for Debian systems with a user called *debian*. It also will install a *.xinitrc* file at */home/debian* that will launch the *dbus user session*, and the *ConsoleKit* daemon.


## Known bugs

* Windows existing before launching TabletWM are not recognized by it.
* Sometimes the app seem to be locked. Just using the *shows the next window of the same application group* button (or Ctrl+TAB) fixes it. It seems to have to do when the app really needs a window bigger than the screen.


## License

TabletWM is distributed under the terms of the GPLv2 or (at your option) any later version. The terms of this license are available in the COPYING file.


## Author

TabletWM is developed by Raster Software Vigo (Sergio Costas)

http://www.rastersoft.com
raster@rastersoft.com
