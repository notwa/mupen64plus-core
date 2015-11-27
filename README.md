Mupen64Plus, now with Lua 5.3
-----------------------------

This is a fork of Mupen64Plus that adds Lua scripting support.
Right now, it can't do much, but eventually it should be possible to implement
things such as frontend UIs, advanced debugging/cheating/TAS tools, game
enhancement scripts, etc with Lua.

This is the Mupen64Plus-Core library. It needs to be paired with the modified
Mupen64Plus-UI-Console program (or other frontend, if you feel like adapting it)
that adds the `--lua` command line option to actually load a script. You'll also
need the rest of Mupen64Plus; so far I haven't forked the other modules since I
haven't changed them, so the ones from https://github.com/mupen64plus will work
just fine.

Mind that I've only modified the Unix makefile. I have neither the experience
nor interest to mess with the Windows makefiles at the moment, so it probably
won't build on Windows until someone gets around to fixing those.

Also please keep in mind that this is still in early development. APIs are not
documented and will likely change.


Original README follows...


Mupen64Plus-Core README
-----------------------

The most current version of this README and more documentation can be found on
the Mupen64Plus wiki:

http://code.google.com/p/mupen64plus/wiki/README

Mupen64Plus is based off of mupen64, originally created by Hacktarux. This
package contains the only the Mupen64Plus core library.  For a fully functional
emulator, the user must also install graphics, sound, input, and RSP plugins,
as well as a user interface program (called a front-end).

README Sections
  1. Requirements for building or running Mupen64Plus
  2. Building From Source
  3. Installation
  4. Key Commands In Emulator

1. Requirements and Pre-requisites
----------------------------------

*Binary Package Requirements*

  - SDL 1.2 or 2.0
  - libpng
  - freetype 2
  - zlib

*Source Build Requirements*

In addition to the binary libraries, the following packages are required if you
build Mupen64Plus from source:

  - GNU C and C++ compiler, libraries, and headers
  - GNU make
  - Development packages for all the libraries above
  - `binutils-dev` package, if building with debugger functions.

2. Building From Source
-----------------------

If you downloaded the binary distribution of Mupen64Plus, skip to the
Installation section. To build the source distribution, unzip and cd into the
`projects/unix directory`, then build using `make`:

```
 $ unzip mupen64plus-core-x-y-z-src.zip
 $ cd mupen64plus-core-x-y-z-src/projects/unix
 $ make all
```

Type `make` by itself to view all available build options:

```
 $ make
 Mupen64Plus makefile.
   Targets:
     all           == Build Mupen64Plus and all plugins
     clean         == remove object files
     install       == Install Mupen64Plus and all plugins
     uninstall     == Uninstall Mupen64Plus and all plugins
   Options:
     BITS=32       == build 32-bit binaries on 64-bit machine
     LIRC=1        == enable LIRC support
     NO_ASM=1      == build without assembly (no dynamic recompiler or MMX/SSE code)
     USE_GLES=1    == build against GLESv2 instead of OpenGL
     VC=1          == build against Broadcom Videocore GLESv2
     NEON=1        == (ARM only) build for hard floating point environments
     VFP_HARD=1    == (ARM only) full hardware floating point ABI
     SHAREDIR=path == extra path to search for shared data files
     WARNFLAGS=flag == compiler warning levels (default: -Wall)
     OPTFLAGS=flag == compiler optimization (default: -O3)
     PIC=(1|0)     == Force enable/disable of position independent code
     OSD=(1|0)     == Enable/disable build of OpenGL On-screen display
     NEW_DYNAREC=1 == Replace dynamic recompiler with Ari64's experimental dynarec
     POSTFIX=name  == String added to the name of the the build (default: '')
   Install Options:
     PREFIX=path   == install/uninstall prefix (default: /usr/local/)
     SHAREDIR=path == path to install shared data (default: PREFIX/share/mupen64plus/)
     LIBDIR=path   == path to install plugin libs (default: PREFIX/lib)
     INCDIR=path   == path to install core header files (default: PREFIX/include/mupen64plus)
     DESTDIR=path  == path to prepend to all installation paths (only for packagers)
   Debugging Options:
     PROFILE=1     == build gprof instrumentation into binaries for profiling
     DEBUG=1       == add debugging symbols to binaries
     DEBUGGER=1    == build graphical debugger
     DBG_CORE=1    == print debugging info in r4300 core
     DBG_COUNT=1   == print R4300 instruction count totals (64-bit dynarec only)
     DBG_COMPARE=1 == enable core-synchronized r4300 debugging
     DBG_TIMING=1  == print timing data
     DBG_PROFILE=1 == dump profiling data for r4300 dynarec to data file
     V=1           == show verbose compiler output
```

3. Installation
---------------

*Binary Distribution*

To install the binary distribution of Mupen64Plus, `su` to root and run the
provided `install.sh` script:

```
 $ su
 # ./install.sh
 # exit
 $
```

The install script will copy the executable to `/usr/local/bin` and a directory
called `/usr/local/share/mupen64plus` will be created to hold plugins and other
files used by mupen64plus.

NOTE: By default, `install.sh` uses `/usr/local` for the install prefix. Although
the user can specify an alternate prefix to `install.sh` at the commandline, the
mupen64plus binary was compiled to look for the install directory in `/usr/local`,
so specifying an alternate prefix to `install.sh` will cause problems (the
mupen64plus front-end application will not find the directory containing the
core library) unless the directory to which you install it is known by your
dynamic library loader (ie, included in `/etc/ld.conf.so`)

If you want to use a prefix other than `/usr/local`, you may also download the
source code package and build with the PREFIX option (see below).

*Source Distribution*

After building mupen64plus and all plugins, `su` to root and type `make install`
to install Mupen64Plus. The install process will copy the executable to
`$PREFIX/bin` and a directory called `$PREFIX/share/mupen64plus` will be created
to hold plugins and other files used by mupen64plus. By default, `PREFIX` is set
to `/usr/local`. This can be changed by passing the `PREFIX` option to `make`. NOTE:
you must pass the prefix, when building AND installing. For example, to install
mupen64plus to `/usr`, do this:

```
 $ make PREFIX=/usr all
 $ sudo make PREFIX=/usr install
 $
```

4. Key Commands In Emulator
---------------------------
The keys or joystick/mouse inputs which will be mapped to the N64 controller
for playing the games are determined by the input plugin.  The emulator core
also supports several key commands during emulation, which may be configured by
editing the `~/.config/mupen64plus/mupen64plus.cfg` file.  They are:

```
   Escape == Quit the emulator
      0-9 == Select virtual 'slot' for save/load state (F5 and F7) commands
       F5 == Save emulator state
       F7 == Load emulator state
       F9 == Reset emulator
      F10 == slow down emulator by 5%
      F11 == speed up emulator by 5%
      F12 == take screenshot
Alt-Enter == Toggle between windowed and fullscreen (may not be supported by all video plugins)
   p or P == Pause on/off
   m or M == Mute/unmute sound
   g or G == Press "Game Shark" button (only if cheats are enabled)
   / or ? == single frame advance while paused
        F == Fast Forward (playback at 250% normal speed while F key is pressed)
        [ == Decrease volume
        ] == Increase volume
```
