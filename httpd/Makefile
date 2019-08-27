DESTDIR= /usr/libexec
CFLAGS= -O
PROGRAM=	httpd
SRCS=		httpd.c
OBJS=		httpd.o
LIBS=
LOGFILE=	/usr/adm/httpd.log

all:	${PROGRAM}

${PROGRAM}:	${OBJS}
	${CC} ${CFLAGS} -o $@ ${OBJS} ${LIBS}

install: ${PROGRAM}
	install -s -m 755 ${PROGRAM} ${DESTDIR}
	touch ${LOGFILE}
	chown www ${LOGFILE}
	chgrp staff ${LOGFILE}
	chmod 644 ${LOGFILE}

tags:
	ctags -tdw *.c

clean:
	rm -f a.out core *.o ${PROGRAM}
