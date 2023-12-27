mouseTrail
==========
A fun little program which uses the X window system
to create a mouse trail, potentially with a rainbow effect.
It was written to be compiled with musl libs, so may not
work with glibc.

Usage
-----
$ **mouseTrail**

Info
----
Cursors in the trail are XPM files and are selected
either by the name given by Xorg, or the location of
the hotspot formatted as *xhot*-*yhot*.xpm

The XPMs do not include transparency by default, and
so one color (set by the emptyColor, in config) is
replaced with transparency.

Included are some examples, which are taken from the
Adwaita cursor theme.

If the current cursor has no name and has its hotspot at
(0, 0), then **mouseTrail** will stop and wait until another
cursor is used.


Exit Status
-----------
| Return Code |                                                Meaning |
| :---------- | -----------------------------------------------------: |
| 0           |                                                Success |
| 2           |              Cannot find the specified initCursor file |
| 9           | Invalid XPM file attempted to be opened during runtime |
| 11          |                         Cannot connect to the X server |

Customization
-------------
**mouseTrail** can be customized by editing the variables
under <ins>/* config */</ins> and recompiling.

### Rainbow Mode
Rainbow mode works by replacing a single color in each copy
with rainbow (set in config).

The included example XPMs use red as the rainbow color.

Dependencies
------------
- libXpm development headers
- libXfixes development headers
- X11 development headers

###### Adapted from mouseTrail.1
