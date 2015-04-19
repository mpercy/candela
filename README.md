# candela
Simple Glyph Bitmap Distribution Format (BDF) font renderer for the console

This has only been tested on one 6x4 font, and will surely not work on large
fonts. Needs additional word to support fonts wider than 8 pixels, since it
assumes that the bitmaps for each line are only one byte each. It uses a hash
(#) character to represent a font pixel.

This implementation was not written for efficiency; it was written in a few
hours for fun.

This is intended to be cross-platform C++ requiring only C++98. It has not been
tested on anything other than Linux (g++ and clang++), but there is no reason
to expect it not to work on any platform, including MS Visual Studio.

To build (on a Unix-like platform):

    $ make

To test:

    $ make print

To run:

    $ ./bdf tom-thumb.bdf "Message to render"

Example outout:

    ./bdf tom-thumb.bdf "Hello, world!"
    # #     ##  ##                          ##    #  #
    # #  ##  #   #   #          # #  #   ##  #   ##  #
    ### # #  #   #  # #         ### # # #    #  # #  #
    # # ##   #   #  # #  #      ### # # #    #  # #
    # #  ## ### ###  #  #       ###  #  #   ###  ##  #


This work is licensed under the Apache Software License (ASL) version 2.0.
Please see LICENSE.txt for details.
