CC=gcc -c
LN=gcc
CFLAGS= -g -O0

tabletwm: tabletwm.o globals.o init.o actions.o support.o wincache.o
	$(LN) -o tabletwm tabletwm.o globals.o init.o actions.o support.o wincache.o `pkg-config --libs xcb xcb-util xcb-randr xcb-icccm`

tabletwm.o: tabletwm.c globals.h init.h actions.h
	$(CC) $(CFLAGS) -o tabletwm.o tabletwm.c `pkg-config --cflags xcb xcb-util xcb-randr`

globals.o: globals.c globals.h
	$(CC) $(CFLAGS) -o globals.o globals.c `pkg-config --cflags xcb`

init.o: init.c init.h globals.h wincache.h
	$(CC) $(CFLAGS) -o init.o init.c `pkg-config --cflags xcb`

actions.o: actions.c actions.h globals.h support.h wincache.h
	$(CC) $(CFLAGS) -o actions.o actions.c `pkg-config --cflags xcb`

support.o: support.c support.h globals.h wincache.h
	$(CC) $(CFLAGS) -o support.o support.c `pkg-config --cflags xcb xcb-icccm`
	
wincache.o: wincache.c wincache.h globals.h
	$(CC) $(CFLAGS) -o wincache.o wincache.c `pkg-config --cflags xcb`

clean:
	rm -f *.o tabletwm
