CC=gcc -c
LN=gcc -O2
CFLAGS= -O2

tabletwm: tabletwm.o globals.o init.o actions.o
	$(LN) -o tabletwm tabletwm.o globals.o init.o actions.o `pkg-config --libs xcb xcb-util xcb-randr`

tabletwm.o: tabletwm.c globals.h init.h actions.h
	$(CC) $(CFLAGS) -o tabletwm.o tabletwm.c `pkg-config --cflags xcb xcb-util xcb-randr`

globals.o: globals.c globals.h
	$(CC) $(CFLAGS) -o globals.o globals.c `pkg-config --cflags xcb xcb-util`

init.o: init.c init.h globals.h
	$(CC) $(CFLAGS) -o init.o init.c `pkg-config --cflags xcb xcb-util`

actions.o: actions.c actions.h globals.h
	$(CC) $(CFLAGS) -o actions.o actions.c `pkg-config --cflags xcb xcb-util`

clean:
	rm -f *.o tabletwm
