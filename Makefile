X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib

INC = -I. -I/usr/include -I$(X11INC)
LIB = -L$(X11LIB) -lX11 -lXfixes

CFLAGS = -Wall -Wextra -std=c99 -pedantic -O3 $(INC)
LDFLAGS = -s $(LIB)

SOURCE = mouseTrail.c

MAKE = $(CC) $(CFLAGS) $(LDFLAGS) $(SOURCE) -o

all: mouseTrail

mouseTrail: $(SOURCE)
	$(MAKE) $@
	chmod +x $@

install: mouseTrail
	cp mouseTrail /usr/bin/
	cp mouseTrail.1 /usr/share/man/man1/mouseTrail.1.gz

clean:
	rm -f mouseTrail
