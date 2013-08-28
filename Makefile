CC=gcc -c
LN=gcc
CFLAGS=-O2

swm: swm.o
	$(LN) -o swm swm.o `pkg-config --libs xcb xcb-util xcb-randr`

swm.o: swm.c
	$(CC) $(CFLAGS) -o swm.o swm.c `pkg-config --cflags xcb xcb-util xcb-randr`
	
clean:
	rm -f *.o swm
