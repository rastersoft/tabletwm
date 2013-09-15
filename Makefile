CC=gcc -c
LN=gcc
CFLAGS= -O2

tabletwm: tabletwm.o globals.o init.o actions.o support.o wincache.o menuwin.o shutdown.o
	$(LN) -o tabletwm tabletwm.o globals.o init.o actions.o support.o wincache.o menuwin.o shutdown.o `pkg-config --libs xcb xcb-util xcb-randr xcb-icccm xcb-keysyms xcb-xtest cairo xkbcommon`
	strip tabletwm

tabletwm.o: tabletwm.c globals.h init.h actions.h
	$(CC) $(CFLAGS) -o tabletwm.o tabletwm.c `pkg-config --cflags xcb xcb-util xcb-randr cairo`

globals.o: globals.c globals.h
	$(CC) $(CFLAGS) -o globals.o globals.c `pkg-config --cflags xcb cairo`

init.o: init.c init.h globals.h wincache.h shutdown.h
	$(CC) $(CFLAGS) -o init.o init.c `pkg-config --cflags xcb cairo`

actions.o: actions.c actions.h globals.h support.h wincache.h menuwin.h
	$(CC) $(CFLAGS) -o actions.o actions.c `pkg-config --cflags xcb cairo`

support.o: support.c support.h globals.h wincache.h
	$(CC) $(CFLAGS) -o support.o support.c `pkg-config --cflags xcb xcb-icccm cairo`
	
wincache.o: wincache.c wincache.h globals.h
	$(CC) $(CFLAGS) -o wincache.o wincache.c `pkg-config --cflags xcb cairo`

menuwin.o: menuwin.c menuwin.h globals.h
	$(CC) $(CFLAGS) -o menuwin.o menuwin.c `pkg-config --cflags xcb xcb-keysyms xcb-xtest cairo xkbcommon`

shutdown.o: shutdown.c shutdown.h globals.h
	$(CC) $(CFLAGS) -o shutdown.o shutdown.c `pkg-config --cflags xcb cairo`

clean:
	rm -f *.o tabletwm

install:
	install -d /etc/tabletwm.d
	install tabletwm /usr/local/bin
	install tabletwm.cfg.example /etc/tabletwm.d/
	install *.keymap /etc/tabletwm.d/

uninstall:
	rm -rf /usr/local/bin/tabletwm
