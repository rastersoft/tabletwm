#!/bin/bash
#Edit and uncomment this to set your preferred language
#export LANG=en_US.UTF-8

#Edit and uncomment this to set your timezone
#export TZ="Europe/Madrid"

# Launch DBUS user session
export `dbus-launch`
# Launch Tablet Window Manager
tabletwm
# If the window manager dies, kill the dbus user session
killall dbus-daemon
