/* fun little program to create a mousetrail using the X window system    *\
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
	const int rainSection = 0;

	/* class to set copy windows to */
	const char* class = "mouseTrail";
/* end config */

#define red(color) (color & 0xFF)
#define green(color) (color & 0xFF00 >> 8)
#define blue(color) (color & 0xFF0000 >> 16)
#define alpha(color) (color & 0xFF000000)
#define bright(color) ((red(color) * 0.21) + (green(color) * 0.72)       \
                       + (blue(color) * 0.07))

int
main()
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
		return 11;

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
		                        cursor->height, 0, vinfo.depth,
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

	XFree(cursor);

	gc = XCreateGC(dpy, copies[0], 0, NULL);

	while (1) {
		for (i = 0; i < numCopies; i++) {
			nanosleep(&ts, &ts);

			XClearWindow(dpy, copies[i]);

			cursor = XFixesGetCursorImage(dpy);

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
							if (rainSection) {
								if (rainSection == 1)
									pixel &= color + 0xFF000000;
								else
									if (alpha(pixel) > 0xAA000000)
										pixel |= color;
							} else
								if (alpha(pixel) > 0xAA000000)
									pixel = color + 0xFF000000;
								else
									pixel = 0;
						}

						XSetForeground(dpy, gc, pixel);
						XDrawPoint(dpy, copies[i], gc, x, y);
					}
				}

			XFree(cursor);
		}
	}

	XCloseDisplay(dpy);
	XFreeGC(dpy, gc);

	return 0;
}
