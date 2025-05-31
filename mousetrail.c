/* fun little program to create a mousetrail using the X window system    *\
|* Copyright (C) 2025 Andrew Charles Marino                               *|
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
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>

/* config */
	/* number of copies in the trail */
	const int numCopies = 8;
	/* amount of time between clones in miliseconds */
	const int copyTiming = 30;

	/* whether or not rainbow mode is enabled */
	const int rainbow = 0;
	/* rainbowify either all (0), only lighter (1), or only darker (2) pixels */
	const int rainType = 0;

	/* name of the copy windows */
	const char* name = "mousetrail";
/* end config */

const char* shortopts = "v";

#define alpha(color) (color & 0xFF000000)

int
main(void)
{
	Display *dpy;
	GC gc;
	int i, x, y, scr;
	struct timespec ts;
	unsigned long pixel, color;
	Window root, copies[numCopies];
	XFixesCursorImage* cursor;
	XRectangle rect;
	XserverRegion region;
	XSetWindowAttributes wa;
	XVisualInfo vinfo;

	if ((dpy = XOpenDisplay(NULL)) == NULL)
		return 1;

	scr = DefaultScreen(dpy);
	root = RootWindow(dpy, scr);
	XMatchVisualInfo(dpy, scr, 32, TrueColor, &vinfo);

	ts.tv_sec = copyTiming / 1000;
	ts.tv_nsec = (copyTiming % 1000) * 1000000;

	cursor = XFixesGetCursorImage(dpy);

	for (i = 0; i < numCopies; i++) {
		wa.override_redirect = 1;
		wa.colormap = XCreateColormap(dpy, root, vinfo.visual, AllocNone);
		wa.background_pixel = 0;
		wa.border_pixel = 0;

		copies[i] = XCreateWindow(dpy, root, 0, 0, cursor->width,
		                        cursor->height, 0, vinfo.depth, InputOutput,
		                        vinfo.visual, CWOverrideRedirect | CWColormap |
		                        CWBackPixel | CWBorderPixel, &wa);

		region = XFixesCreateRegion(dpy, &rect, 1);
		XFixesSetWindowShapeRegion(dpy, copies[i], ShapeInput, 0, 0, region);
		XFixesDestroyRegion(dpy, region);

		XChangeProperty(dpy, copies[i], XInternAtom(dpy, "_NET_WM_NAME", 0),
		                XInternAtom(dpy, "UTF8_STRING", 0), 8, PropModeReplace,
		                (unsigned char*)name, strlen(name));

		XMapWindow(dpy, copies[i]);
		XFlush(dpy);
	}

	XFree(cursor);

	gc = XCreateGC(dpy, copies[0], 0, NULL);

	while (1) {
		for (i = 0; i < numCopies; i++) {
			nanosleep(&ts, NULL);

			XClearWindow(dpy, copies[i]);

			cursor = XFixesGetCursorImage(dpy);

			if (cursor->height == 1 && cursor->width == 1) {
				XFree(cursor);
				continue;
			}

			XResizeWindow(dpy, copies[i], cursor->width, cursor->height);
			XMoveWindow(dpy, copies[i], cursor->x - cursor->xhot,
			            cursor->y - cursor->yhot);
			XRaiseWindow(dpy, copies[i]);

			if (rainbow) color = rand() % 0xFFFFFF;

			for (x = 0; x < cursor->width; x++)
				for (y = 0; y < cursor->height; y++) {
					pixel = cursor->pixels[x + (y * cursor->width)];

					if (pixel != 0) {
						if (rainbow) {
							switch (rainType) {
							case 0: /* all pixels */
								if (alpha(pixel) > 0xAA000000)
									pixel = color + alpha(pixel);
								break;
							case 1: /* only light */
								pixel &= color + 0xFF000000;
								break;
							case 2: /* only dark */
								if (alpha(pixel) > 0xAA000000)
									pixel |= color;
								break;
							}
						}

						XSetForeground(dpy, gc, pixel);
						XDrawPoint(dpy, copies[i], gc, x, y);
					}
				}

			XFree(cursor);
		}
	}

	for (i = 0; i < numCopies; i++)
		XDestroyWindow(dpy, copies[i]);

	XFreeGC(dpy, gc);
	XCloseDisplay(dpy);

	return 0;
}
