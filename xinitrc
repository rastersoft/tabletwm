#!/bin/bash

# Launch DBUS user session
export `dbus-launch`
# Launch ConsoleKit session
ck-launch-session
# Launch Tablet Window Manager
tabletwm
# If the window manager dies, kill the dbus user session
killall dbus-daemon
