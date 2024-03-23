/* fun little program to create a mousetrail using the X window system *\
|* Copyright (C) 2024 Andrew Charles Marino                               *|
|*                                                                        *|
|* This program is free software: you can redistribute it and/or modify   *|
|* it under the terms of the GNU General Public License as published by   *|
|* the Free Software Foundation, either version 3 of the License, or      *|
|* (at your option) any later version..                                   *|
|*                                                                        *|
|* This program is distributed in the hope that it will be useful,        *|
|* but WITHOUT ANY WARRANTY; without even the implied warranty of         *|
|* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *|
|* GNU General Public License for more details.                           *|
|*                                                                        *|
|* You should have received a copy of the GNU General Public License      *|
\* along with this program.  If not, see <https://www.gnu.org/licenses/>. */
#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdio.h>
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

	/* directory containing xpm files for each cursor, must end in slash */
	const char* library = "/home/drew/Documents/mouseTrail/";

	/* initial cursor xpm to use, not including .xpm,
	 * program assumes that the length is less than 252 bytes */
	const char* initCursor = "3-1";

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
	XImage *image;
	XRectangle rect;
	XserverRegion region;
	XSetWindowAttributes wa;
	XVisualInfo vinfo;

	if (!(dpy = XOpenDisplay(NULL)))
		return 11;
	scr = DefaultScreen(dpy);
	root = RootWindow(dpy, scr);
	XMatchVisualInfo(dpy, scr, 32, TrueColor, &vinfo);

	file = malloc(sizeof(char) * (256 + strlen(library)));
	xhot = yhot = 0;
	snprintf(file, 256, "%s%s.xpm", library, initCursor);

	ts.tv_sec = copyTiming / 1000;
	ts.tv_nsec = (copyTiming % 1000) * 1000000;

	if (access(file, F_OK)) {
		free(file);
		return 2;
	}
	XpmReadFileToImage(dpy, file, &image, NULL, NULL);

	for (i = 0; i < numCopies; i++) {
		wa.override_redirect = 1;
		wa.colormap = XCreateColormap(dpy, root, vinfo.visual, AllocNone);
		wa.background_pixel = 0;

		copies[i] = XCreateWindow(dpy, root, 0, 0, image->width,
		                        image->height, 0, vinfo.depth,
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

	while (1) {
		for (i = 0; i < numCopies; i++) {
			nanosleep(&ts, &ts);

			XClearWindow(dpy, copies[i]);
			cursor = XFixesGetCursorImage(dpy);

			if (cursor->width == 1 && cursor->height == 1) { /* hidden cursor */
				XFree(cursor);
				continue;
			}

			if (cursor->name != cursorName
			|| (strlen(cursor->name) == 0 &&
			   (cursor->xhot != xhot || cursor->yhot != yhot))) {

				XDestroyImage(image);

				snprintf(file, 256, "%s%s.xpm", library, cursor->name);

				if (access(file, F_OK))
					snprintf(file, 256, "%s%i-%i.xpm", library, cursor->xhot,
					                                      cursor->yhot);
				if (access(file, F_OK))
					snprintf(file, 256, "%s%s.xpm", library, initCursor);

				if (access(file, F_OK))
					return 9;

				XpmReadFileToImage(dpy, file, &image, NULL, NULL);

				for (j = 0; j < strlen(cursor->name)+1; j++)
					cursorName[j] = cursor->name[j];
				xhot = cursor->xhot;
				yhot = cursor->yhot;
			}

			if (rainbow) color = rand() % 0xFFFFFF;

			for (x = 0; x < image->width; x++)
				for (y = 0; y < image->height; y++) {
					pixel = XGetPixel(image, x, y);

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

			XFree(cursor);
		}
	}
	free(file);
	XCloseDisplay(dpy);

	return 0;
}
