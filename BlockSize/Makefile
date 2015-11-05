CC = gcc
CFLAGS = -O2 -g -static

all: block_size

block_size: block_size.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

clean:
	rm -f *.o block_size
