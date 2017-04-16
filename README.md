# ahrs-visualizer

**ahrs-visualizer** reads orientation data on its standard input and uses that
  to display a 3D representation of the orientation.

To see what the visualization looks like, check out this
[video showing the MinIMU-9 v2 with a Raspberry Pi][video].

Currently, the presence of GPU-specific code means that it only works on the
[Raspberry Pi] but it can probably be ported to other boards.  It uses OpenGL ES
2.0.

## Getting started

### Get minimu9-ahrs working

This program is meant to be used with [minimu9-ahrs], a separate
utility that reads data from the Pololu MinIMU-9 and computes its
orientation.  To get that working, follow the instructions at the
aforementioned link.

### Building ahrs-visualizer from source

Navigate to the top-level source directory and run these commands:

    sudo apt-get install libpng-dev libboost-program-options-dev
    make

If you want to try out the visualizer before installing it, you can run:

    ./mock_ahrs.py | ASSET_DIR=assets ./ahrs-visualizer

To install the visualizer, run:

    sudo make install

### Invoking

To pipe direction cosine matrix (DCM) data from `minimu9-ahrs` to
`ahrs-visualizer`, simply run the following command:

    minimu9-ahrs | ahrs-visualizer

The visualizer runs fine from the console; you do not need to run X.

Run `man ahrs-visualizer` for more help and details about the options
the program accepts.

## Understanding the visualization:

- The 3D model is a [Pololu MinIMU-9 v2].
- The X axis is drawn as a red line.
- The Y axis is drawn as a green line.
- The Z axis is drawn as a blue line.
- The acceleration vector is drawn as a cyan line.
- The magnetic field vector is drawn as a yellow line.

## Error about opening a vchiq instance

If the program cannot open `/dev/vchiq` because of a permissions
problem, you will see the following error:

    Warning: Could not open /dev/vchiq: Permission denied.
        * failed to open vchiq instance

On the Raspberry Pi, the right way to fix this error is to add your
user to the `video` group.  Just run the following command and then
reboot:

    sudo usermod -a -G video $(whoami)

## Version history

- 1.1.0 (2017-04-16):
  - Fixed the "DSO missing from command line" linker error (thanks pskora).
  - Added a warning if `/dev/vchiq` cannot be opened, because the error message
    from `bcm_host_init` is not helpful.
  - Changed the default installation prefix from /usr to /usr/local.
  - Added support for the ASSET_DIR environment variable so you can easily
    try the program out before installing it.
- 1.0.1 (2014-07-09): Added an extra include directory to the Makefile (thanks Shantanu Bhadoria).
- 1.0.0 (2012-11-10): Initial release.

[Raspberry Pi]: https://www.raspberrypi.org/
[minimu9-ahrs]: https://github.com/DavidEGrayson/minimu9-ahrs
[Pololu MinIMU-9 v2]: https://www.pololu.com/product/1268
[video]: https://www.youtube.com/watch?v=8daR-wtQIx0
