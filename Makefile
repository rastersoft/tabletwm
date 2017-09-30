CC=gcc -c
LN=gcc
CFLAGS= -O2

PREFIX=/usr/local
BINDIR=$(PREFIX)/bin
DESTDIR=

tabletwm: actions.o globals.o init.o menuwin.o shutdown.o support.o tabletwm.o wincache.o 
	$(LN) -o tabletwm actions.o globals.o init.o menuwin.o shutdown.o support.o tabletwm.o wincache.o  `pkg-config --libs xcb xcb-util xcb-randr xcb-icccm xcb-keysyms xcb-xtest cairo xkbcommon`
	strip tabletwm

clean:
	rm -f *.o tabletwm

install:
	install -d $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)/etc/tabletwm.d
	install -m 755 tabletwm $(DESTDIR)$(BINDIR)
	install tabletwm.cfg.example $(DESTDIR)/etc/tabletwm.d/
	install *.keymap $(DESTDIR)/etc/tabletwm.d/

install-init:
	install -d $(DESTDIR)/etc/init.d
	install -d $(DESTDIR)/home/debian
	install -m 755 tabletwm.init $(DESTDIR)/etc/init.d/tabletwm
	install xinitrc $(DESTDIR)/home/debian/.xinitrc
	update-rc.d tabletwm defaults

uninstall:
	rm -rf $(DESTDIR)$(BINDIR)/tabletwm


actions.o: actions.c shutdown.h menuwin.h wincache.h globals.h support.h actions.h
	$(CC) -o actions.o actions.c

globals.o: globals.c globals.h
	$(CC) -o globals.o globals.c

init.o: init.c shutdown.h menuwin.h wincache.h globals.h init.h
	$(CC) -o init.o init.c

menuwin.o: menuwin.c support.h globals.h wincache.h menuwin.h
	$(CC) -o menuwin.o menuwin.c

shutdown.o: shutdown.c shutdown.h menuwin.h wincache.h globals.h support.h actions.h
	$(CC) -o shutdown.o shutdown.c

support.o: support.c menuwin.h wincache.h globals.h support.h
	$(CC) -o support.o support.c

tabletwm.o: tabletwm.c menuwin.h actions.h support.h globals.h init.h
	$(CC) -o tabletwm.o tabletwm.c

wincache.o: wincache.c wincache.h globals.h
	$(CC) -o wincache.o wincache.c

