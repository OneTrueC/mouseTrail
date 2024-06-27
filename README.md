mouseTrail
==========
A fun little program which uses the X window system
to create a mouse trail, optionally with a rainbow effect.

Usage
-----
$ **mouseTrail**

Info
----
**mouseTrail** takes the current mouse cursor and creates
`numCopies` copies that follow the cursor by moving the
last in the trail to the current mouse position every
`copyTiming` milliseconds.

Exit Status
-----------
| Return Code |                                                Meaning |
| :---------- | -----------------------------------------------------: |
| 11          |                         Cannot connect to the X server |

Customization
-------------
**mouseTrail** can be customized by editing the variables
under <ins>/* config */</ins> and recompiling.

The behavior of rainbow mode is contained between lines
128 and 138.

### Rainbow Mode
Rainbow mode works by replacing either the all, only lighter,
or only darker (set in config) pixels with a random color in
each copy with rainbow.

Dependencies
------------
- libXfixes development headers
- X11 development headers

###### Adapted from mouseTrail.1
