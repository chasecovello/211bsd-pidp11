# Makefile for welcome_html.c: Displays a 'welcome' message like the one
# displayed by 2.11BSD's /usr/local/welcome, but rendered in HTML for use by a
# web server CGI program. Note that this program does not output HTTP response
# headers or a complete HTML page, just the welcome text and <pre> tags.
# Therefore, it must be called from a complete CGI program. See
# /home/user/weather/weather.c for an example.
#
# Original /usr/local/welcome written in 1988 by Jim King
# This version written in 2019 by Chase Covello

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
