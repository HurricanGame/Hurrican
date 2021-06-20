[![CI build](https://github.com/drfiemost/Hurrican/actions/workflows/build.yml/badge.svg)](https://github.com/drfiemost/Hurrican/actions/workflows/build.yml)

A fork of Hurrican, freeware jump and shoot game created by Poke53280, with SDL2 enabled by default, support for libopenmpt and CRT simulation.
Additional userlevels from http://turricanforever.de included.

Original code by Eiswuxe (Poke53280)  
Further work by Pickle and Stefan Schmidt (thrimbor)

![screenshot](https://github.com/drfiemost/Hurrican/wiki/images/level1.png)

---

### Dependencies

The code depends on SDL2 (or the old deprecated SDL) with the image and mixer components, OpenGL and libepoxy.
Optionally libopenmpt can be used for the music.
A compiler with c++17 support is required.

### Building

The code can be built using cmake (tested on Linux and MinGW)

    mkdir build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make

The following build options are available:

OpenGL Options
* -DRENDERER=GL1          : Use the OpenGL 1.X code (fixed pipline)
* -DRENDERER=GLES1        : Use the OpenGL 1.X code with ES compatible
* -DRENDERER=GL2          : Use the OpenGL 2.0 code (programable pipline) [this is the default if not specified]
* -DRENDERER=GLES2        : Use the OpenGL 2.0 code with ES compatible
* -DRENDERER=GL3          : Use the OpenGL 3.0 code (programable pipline)
* -DRENDERER=GLES3        : Use the OpenGL 3.0 code with ES compatible
* -DFBO=ON                : Add FBO support, allow screen to be scaled to arbitrary dimensions, available only with GL2 or GL3 and enabled by default
* -DEGL=ON                : Enable EGL support, available only with GL2 or GL3

Sound
* -DOPENMPT=ON            : Use the libopenmpt code for music (SDL2_mixer uses libmodplug while SDL_mixer uses the lower quality mikmod engine)

