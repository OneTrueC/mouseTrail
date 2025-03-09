# NAME

**mousetrail** - fun little mouse trail

# SYNOPSIS

**mousetrail**

# DESCRIPTION

**mousetrail** is a fun little program which uses the X window system
to create a mouse trail, optionally with a rainbow effect.

# EXIT STATUS

| Return Code |                        Meaning |
| :---------- | -----------------------------: |
| 1           | Cannot connect to the X server |

# CUSTOMIZATION

**mousetrail** can be customized be editing the variables under
*/\* config  \*/* near the top of the source, and recompiling. The
behavior of rainbow mode is contained between lines 127 and 141.

# RAINBOW MODE

Rainbow mode works by replacing either all, only lighter, or only darker
(set in config) pixels with a random color in each copy with rainbow.

# DEPENDENCIES

- libXfixes development headers
- X11 development headers
