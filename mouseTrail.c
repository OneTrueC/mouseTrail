/* fun little program to create a mousetrail using the X window system *\
|*                                                                     *|
|* writen for musl libc, so dont get mad at my function usage          *|
\* Copyright (C) 2024  Andrew Charles Marino                           */
#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#ifdef __GLIBC__
#include <stdio.h>
#endif
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/xpm.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>

/* config */
	const int numCopies = 8; /* number of copies in the trail */
	const int copyTiming = 30; /* amount of time between clones in miliseconds */

	const int rainbow = 1; /* whether or not rainbow mode is enabled */
	/* which color to replace with rainbow */
	const long unsigned replaceColor = 0xFF0000;
	const long unsigned emptyColor = 0x000000; /* the color to make transparent */

	/* directory containing xpm files for each cursor */
	const char* library = "~/.mouseTrail/";
	const char* initCursor = "3-1"; /* initial cursor xpm to use */

	const char* class = "mouseTrail"; /* class to set copy windows to */
/* end config */

int
main()
{
	char *file, cursorName[256];
	Display *dpy;
	GC gc;
	int i, x, y, scr, xhot, yhot;
	unsigned long j, pixel, color;
	struct timespec ts;
	Window root, copies[numCopies];
	XFixesCursorImage *cursor;
	XImage *pointer;
	XRectangle rect;
	XserverRegion region;
	XSetWindowAttributes wa;
	XVisualInfo vinfo;

	/* connect to display, current screen, root window, and vinfo */
	if (!(dpy = XOpenDisplay(NULL)))
		return 11;
	scr = DefaultScreen(dpy);
	root = RootWindow(dpy, scr);
	XMatchVisualInfo(dpy, scr, 32, TrueColor, &vinfo);

	/* set file to the initCursor, and other related variables */
	file = malloc(sizeof(char) * (256 + strlen(library)));
	xhot = yhot = 0;
	sprintf(file, "%s%s.xpm", library, initCursor);

	/* set up the timespec */
	ts.tv_sec = copyTiming / 1000;
	ts.tv_nsec = (copyTiming % 1000) * 1000000;

	if (access(file, F_OK)) {
		free(file);
		return 2;
	}
	XpmReadFileToImage(dpy, file, &pointer, NULL, NULL);

	/* initialize list of copies */
	for (i = 0; i < numCopies; i++) {
		wa.override_redirect = 1;
		wa.colormap = XCreateColormap(dpy, root, vinfo.visual, AllocNone);
		wa.background_pixel = 0;

		copies[i] = XCreateWindow(dpy, root, 0, 0, pointer->width,
		                        pointer->height, 0, vinfo.depth,
		                        InputOutput, vinfo.visual,
		                        CWOverrideRedirect | CWColormap | CWBackPixel |
		                        CWBorderPixel, &wa);

		region = XFixesCreateRegion(dpy, &rect, 1);
		XFixesSetWindowShapeRegion(dpy, copies[i], ShapeInput, 0, 0, region);
		XFixesDestroyRegion(dpy, region);

		XChangeProperty(dpy, copies[i], XInternAtom(dpy, "_NET_WM_NAME", 0),
		                XInternAtom(dpy, "UTF8_STRING", 0), 8, PropModeReplace,
		                (unsigned char*)class, strlen(class));

		XMapWindow(dpy, copies[i]);
		XFlush(dpy);
	}

	gc = XCreateGC(dpy, copies[0], 0, NULL);

	/* move copy to current mouse position every copyTiming miliseconds */
	while (1) {
		for (i = 0; i < numCopies; i++) {
			nanosleep(&ts, &ts);

			XClearWindow(dpy, copies[i]);
			cursor = XFixesGetCursorImage(dpy);

			if (!(cursor->xhot == 0 && cursor->yhot == 0 &&
			    strlen(cursor->name) == 0)) {
				if (cursor->name != cursorName || (strlen(cursor->name) == 0 &&
				    (cursor->xhot != xhot || cursor->yhot != yhot))) {
					XDestroyImage(pointer);

					sprintf(file, "%s%s.xpm", library, cursor->name);

					if (access(file, F_OK))
						sprintf(file, "%s%i-%i.xpm", library, cursor->xhot,
					                                      cursor->yhot);
					if (access(file, F_OK))
						sprintf(file, "%s%s.xpm", library, initCursor);

					if (XpmReadFileToImage(dpy, file, &pointer, NULL, NULL) ==
					    XpmFileInvalid)
						return 2;

					for (j = 0; j < strlen(cursor->name)+1; j++)
						cursorName[j] = cursor->name[j];
					xhot = cursor->xhot;
					yhot = cursor->yhot;
				}
				if (rainbow) color = rand() % 0xFFFFFF;

				for (x = 0; x < pointer->width; x++)
					for (y = 0; y < pointer->height; y++) {
						pixel = XGetPixel(pointer, x, y);

						if (pixel != emptyColor) {
							if (pixel == replaceColor && rainbow)
								XSetForeground(dpy, gc, color + 0xFF000000);
							else
								XSetForeground(dpy, gc, pixel + 0xFF000000);
							XDrawPoint(dpy, copies[i], gc, x, y);
						}
					}
				XMoveWindow(dpy, copies[i], cursor->x - cursor->xhot,
				            cursor->y - cursor->yhot);
				XRaiseWindow(dpy, copies[i]);
				}
			XFree(cursor);
			}
	}
	free(file);
	XCloseDisplay(dpy);

	return 0;
}
