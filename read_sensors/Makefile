# Makefile for read_sensors.c: read and output values of temperature and
# pressure sensors from the BMP180 memory-mapped sensor device. See:
#
# https://obsolescence.wixsite.com/obsolescence/pidp-11-temp-barometer-hack
#
# for details on how to set this up on a PiDP-11.
#
# This program requires access to /dev/kmem to read the sensor data, so it
# must be run setgid kmem, or by a user with kmem group membership.

DESTDIR= /usr/local
CFLAGS= -O
PROGRAM=	read_sensors
SRCS=		read_sensors.c
OBJS=		read_sensors.o
LIBS=

all:	${PROGRAM}

${PROGRAM}:	${OBJS}
	${CC} ${CFLAGS} -o $@ ${OBJS} ${LIBS}

install: ${PROGRAM}
	install -s -m 2755 -g kmem ${PROGRAM} ${DESTDIR}

tags:
	ctags -tdw *.c

clean:
	rm -f a.out core *.o ${PROGRAM}
