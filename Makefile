CC=gcc -c
LN=gcc
CFLAGS= -g -O0

tabletwm: tabletwm.o globals.o init.o actions.o support.o wincache.o menuwin.o
	$(LN) -o tabletwm tabletwm.o globals.o init.o actions.o support.o wincache.o menuwin.o `pkg-config --libs xcb xcb-util xcb-randr xcb-icccm cairo`

tabletwm.o: tabletwm.c globals.h init.h actions.h
	$(CC) $(CFLAGS) -o tabletwm.o tabletwm.c `pkg-config --cflags xcb xcb-util xcb-randr cairo`

globals.o: globals.c globals.h
	$(CC) $(CFLAGS) -o globals.o globals.c `pkg-config --cflags xcb cairo`

init.o: init.c init.h globals.h wincache.h
	$(CC) $(CFLAGS) -o init.o init.c `pkg-config --cflags xcb cairo`

actions.o: actions.c actions.h globals.h support.h wincache.h menuwin.h
	$(CC) $(CFLAGS) -o actions.o actions.c `pkg-config --cflags xcb cairo`

support.o: support.c support.h globals.h wincache.h
	$(CC) $(CFLAGS) -o support.o support.c `pkg-config --cflags xcb xcb-icccm cairo`
	
wincache.o: wincache.c wincache.h globals.h
	$(CC) $(CFLAGS) -o wincache.o wincache.c `pkg-config --cflags xcb cairo`

menuwin.o: menuwin.c menuwin.h globals.h
	$(CC) $(CFLAGS) -o menuwin.o menuwin.c `pkg-config --cflags xcb cairo`

clean:
	rm -f *.o tabletwm
