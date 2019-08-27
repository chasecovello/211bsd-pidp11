#	
#	Makefile for welcome.  Revised 2019/08/26
#
DESTDIR= /usr/local
BINDIR=	bin
MANDIR= man/man1
CFLAGS= -O
SEPFLAG=
PROGRAM=	welcome
SRCS=		welcome.c
OBJS=		welcome.o
MAN=		welcome.0
LIBS=

all:	${PROGRAM} ${MAN}

${PROGRAM}:	${OBJS}
	${CC} ${SEPFLAG} ${CFLAGS} -o $@ ${OBJS} ${LIBS}

install: ${PROGRAM} welcome.0
	mkdir -p ${DESTDIR}/${BINDIR}
	mkdir -p ${DESTDIR}/${MANDIR}
	install -s -m 755 ${PROGRAM} ${DESTDIR}/${BINDIR}
	install -c -m 444 welcome.1 ${DESTDIR}/${MANDIR}

welcome.0: welcome.1
	#/usr/man/manroff welcome.1 > welcome.0

tags:
	ctags -tdw *.c

clean:
	rm -f a.out core *.o ${PROGRAM}

depend:
	mkdep welcome.c
