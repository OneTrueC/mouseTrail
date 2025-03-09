NAME = mousetrail
VERSION = 0.1

PREFIX = /usr/local
BINDIR = ${PREFIX}/bin
MANDIR = ${PREFIX}/share/man/man1

X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib

INC = -I. -I/usr/include -I${X11INC}
LIB = -L${X11LIB} -lX11 -lXfixes
DEF = "-DVERSION=\"${VERSION}\""

CFLAGS = -Wall -Wextra -std=c99 -pedantic -Os ${INC} ${DEF}
LDFLAGS = -s ${LIB}

SOURCE = mousetrail.c

all: ${NAME}

${NAME}: ${SOURCE}
	${CC} ${CFLAGS} ${LDFLAGS} ${SOURCE} -o $@
	chmod +x $@

install: ${NAME}
	cp "${NAME}" "${BINDIR}/${NAME}"
	sed mousetrail.1 -e "s/VERSION/v${VERSION}/" > "${MANDIR}/${NAME}.1"

uninstall:
	rm -f "${BINDIR}/${NAME}"
	rm -f "${MANDIR}/${NAME}.1"

clean:
	rm -f ${NAME}
