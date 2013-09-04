TabletWM
========

Version 0.1

TabletWM is a minimalistic Window Manager, oriented to tablet PCs and devices with small screens. It tries to keep all windows maximized. No window has decorations.

It is based on SWM, from CINOLT, available at http://lists.freedesktop.org/archives/xcb/2011-November/007375.html


## Using TabletWM

After launching TabletWM in a XWindows session without other window manager, it will become the manager. Every new window will be maximized if it is non transient, or if its type is DESKTOP, UTILITY, DIALOG or NORMAL. The last two types will be maximized only if they are resizable. If not, they will be put centered.

DOCK type windows are kept as-is, and maintained on top. They also won't receive the keyboard focus. This is designed this way to allow to use them to implement launchers and other kind of programs.

There are three hot keys:

* Alt+F4:   closes the current window
* Alt+TAB:  shows the next application
* Ctrl+TAB: shows the next window of the same application group

Ctrl+TAB allows to work easily with applications that uses several windows (like GIMP), because it will jump between the windows of the same app. It is quite limited, because it will also jump between windows of two different instances of the same app (let's say: two GEdit windows, or two different XTerm).

## License


TabletWM is distributed under the terms of the GPLv2 or (at your option) any later version. The terms of this license are available in the COPYING file.


## Author

TabletWM is developed by Raster Software Vigo (Sergio Costas)

http://www.rastersoft.com
raster@rastersoft.com
