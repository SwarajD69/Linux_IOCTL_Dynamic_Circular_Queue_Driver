CC=gcc
CFLAGS=-Wall

all: configurator filler reader

configurator: configurator.c queue_ioctl.h
        $(CC) $(CFLAGS) configurator.c -o configurator

filler: filler.c queue_ioctl.h
        $(CC) $(CFLAGS) filler.c -o filler

reader: reader.c queue_ioctl.h
        $(CC) $(CFLAGS) reader.c -o reader

clean:
        rm -f configurator filler reader

