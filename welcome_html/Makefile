DESTDIR= /usr/local
CFLAGS= -O
PROGRAM=	welcome_html
SRCS=		welcome_html.c
OBJS=		welcome_html.o
LIBS=

all:	${PROGRAM}

${PROGRAM}:	${OBJS}
	${CC} ${CFLAGS} -o $@ ${OBJS} ${LIBS}

install: ${PROGRAM}
	install -s -m 755 ${PROGRAM} ${DESTDIR}

tags:
	ctags -tdw *.c

clean:
	rm -f a.out core *.o ${PROGRAM}
