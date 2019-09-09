DESTDIR= /home/www/cgi-bin
CFLAGS= -O
PROGRAM=	weather
SRCS=		weather.c
OBJS=		weather.o
LIBS=

all:	${PROGRAM}

${PROGRAM}:	${OBJS}
	${CC} ${CFLAGS} -o $@ ${OBJS} ${LIBS}

install: ${PROGRAM}
	install -s -m 111 -o www ${PROGRAM} ${DESTDIR}

tags:
	ctags -tdw *.c

clean:
	rm -f a.out core *.o ${PROGRAM}
