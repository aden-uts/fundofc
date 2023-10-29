CC = gcc
CFLAGS = -Wall -ansi -Werror
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:.c=.o)

huff: $(OBJECTS)
	$(CC) $(CFLAGS) -o huff $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTS) huff