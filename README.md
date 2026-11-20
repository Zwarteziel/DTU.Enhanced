# Undermountain Enhanced

Version 1.1.0

DTU Enhanced is a native Windows version of **Descent to Undermountain** 
a 1998 MS-DOS game made by Interplay. It fixes a number of long-standing
bugs and contains a number of improvements. You need your own copy of the game, 
preferably patched to version 1.3.

Undermountain Enhanced is not an emulator. The game's program, `DTUGAME.EXE`
(a 32-bit DOS/4GW program made with Watcom C), is translated to x64 machine code
with M-HT's static recompiler [SR](https://github.com/M-HT/SR). A runtime written
for this game replaces DOS, DPMI, VESA, the timer, the keyboard, the mouse and
the HMI SOS sound driver. SDL3 handles the window, the CRT shader, sound and
game controllers.

**Neither this repository nor the download contains code or data of the
game.** The translation is made on your own computer, from your own
`DTUGAME.EXE`: the release zip comes with Windows builds of SR and nasm, and
the program prepares the game with them at its first start (in less than a
minute). You can also build everything yourself (see *Building*).

## What works

Highlights:

_Gameplay:
_
- Frame-rate independent movement;
- Fixed original bug where ground-based creatures float in the air;
- Fixed original bug where using ladders got you stuck;
- Dialogs can be selected with number keys;
- Game controller support;
- Detection of the game's version and patch support if game is pre-v1.3.


_Graphics:
_
- VGA CRT-shaders and sharp/linear/nearest neighbour scaling;
- Full 480-line 3D view in HR-mode, instead of original 240-lines drawn two times;
- Full drawing quality up to view distance;
- Smoother weapon animations;
- Fixed original weapon texture turning black for a moment;
- Fixed original textures on beds and tables being drawn in incorrect perspective;
- Fixed original water-sprites creating white flashes in viewport while wading;
- Added a mouse pointer for menu's;

_Sound:
_
- Fixed weapon-impact samples not playing when an enemy is slain;
- Fixed footstep sounds being repeated to quicly;
- Music will wait while the game loads to reduce stuttering.


## Installation

1. Install Descent to Undermountain and update it to version 1.3. (An older
   version is detected at start-up; the program can install the 1.3 patch.)
2. Download the zip of the [latest release](https://github.com/Zwarteziel/DTU.Enhanced/releases)
   and unpack it into the game folder (the folder with `DTUGAME.EXE`). You get
   `DTU-Enhanced.exe`, `SDL3.dll` and a folder `enhanced`.
3. The game reads some files from the CD. Either set `cd_path` in
   `dtu-enhanced.ini` to the folder with the contents of the CD (the folder that
   contains `DTU\`), or copy the CD to `_CD\DTU\_CDFILES` next to the exe.
4. Start `DTU-Enhanced.exe`. The first time, a small window shows how the game
   is prepared from your `DTUGAME.EXE`; the result is kept in
   `enhanced\cache` and made again only when `DTUGAME.EXE` or the program
   changes. `enhanced\cache\build.log` tells what happened.

On first start the program writes `dtu-enhanced.ini` next to the exe. All
settings are in that file, each with a short explanation; every improvement can
be switched back to the original behaviour there. Settings added by a newer
version are appended to an existing file. A log is written to
`dtu-enhanced.log`.

## Keys added by Undermountain Enhanced

| Key | Action |
|---|---|
| Alt+Enter or F11 | Full screen on/off |
| Ctrl+F10 | Release the mouse (click in the window to capture it again) |
| Ctrl+F12 | Screenshot |
| 1-9, 0 | Choose an answer in a conversation |

All other keys go to the game; F2 in the game changes its key bindings.

## Default controller layout

| Controller | Game |
|---|---|
| Left stick | Walk and strafe |
| Right stick | Turn and look (in menus: the pointer) |
| RT / LT | Left / right mouse button (attack, talk) |
| A / B | Enter / Escape |
| X | Immersive mode on/off (V) |
| Y | Inventory |
| LB / RB | Automap / auto-walk |
| D-pad | Arrow keys |
| Start / Back | F2 / F3 |

Every button can be changed in the `[controller]` section of the ini file.

## Building

The build runs on Linux (or WSL on Windows) and makes Windows programs. You
need clang and lld (tested with 18), nasm (2.16), python3, gcc/g++, unzip and
patch, and these archives in one folder (`TOOLS`):

| File | What |
|---|---|
| `SR-master.zip` | [M-HT/SR](https://github.com/M-HT/SR) as a zip of the repository |
| `SDL3-devel-3.4.20-mingw.tar.gz` | SDL3 for mingw, from the [SDL releases](https://github.com/libsdl-org/SDL/releases) |
| `mingw64.zip` | An MSYS2 mingw-w64 x86_64 tree (`include`, `lib` with the CRT, libgcc, libstdc++ and `libz.a`) |
| `nasm-2.16.03.tar.xz` | The [nasm](https://www.nasm.us/) source (only for the release zip) |

**The release zip** (no game code, prepares the game at its first start):

```sh
TOOLS=/path/to/tools sh build-release.sh
```

This builds SR and nasm for Windows, and `DTU-Enhanced.exe` with the loader for
the prepared game (`runtime/selfbuild.c`, `runtime/coffload.c`). The zip ends up
in `$HOME/dtu-build/release` (set `PREFIX` for another place). It needs perl,
autoconf, make and zip as well.

**An exe with the game linked in**, from your own copy of the game:

```sh
TOOLS=/path/to/tools GAME=/path/to/DTU sh build-windows.sh
```

`GAME` is the game folder with `DTUGAME.EXE` version 1.3. The script builds SR
(with the patch in `third_party/SR`), translates `DTUGAME.EXE`, and links the
result with the runtime. The exe and `SDL3.dll` end up in `$HOME/dtu-build/win`.
This exe contains the translated game: keep it to yourself.

`build-headless.sh` builds `dtu-headless`, a Linux version without a window
that runs the game from a script (keys, mouse, timed screenshots, a WAV file of
the sound) and logs every DOS, DPMI and VESA call. It is used to test changes.

## Repository layout

| Folder | Contents |
|---|---|
| `recomp/` | Input for SR: entry points, code/data hints and about 1,700 instruction replacements (most generated for the segment registers) that hook the translated code into the runtime |
| `runtime/` | The platform layer (DOS, DPMI, video, timer, input, sound mixer, MVE player, patch installer, the game fixes) |
| `runtime/sdl/` | The Windows front end: window, CRT shader, controller, settings |
| `tools/` | Scripts used to find code, check the translation and test levels |
| `third_party/` | The SR patch and the DOSBox Staging shaders |
| `release/` | The README and licence notice that go into the release zip |
| `docs/` | The development log (in Dutch) with screenshots and comparisons |

`docs/analyse-en-plan.md` describes, version by version, how the game works
internally where it mattered and how each change was made and tested.

## Licence and credits

Undermountain Enhanced is released under the GNU General Public License,
version 3 or later (see `LICENSE`).

- [SR](https://github.com/M-HT/SR) by Roman Pauer, MIT licence (also GPL-2.0
  or later); the files in `recomp/x64/` come from SR. SR contains udis86
  (BSD licence).
- [nasm](https://www.nasm.us/), BSD licence (in the release zip).
- CRT shaders from [DOSBox Staging](https://github.com/dosbox-staging/dosbox-staging)
  0.83, MIT licence (see `third_party/dosbox-staging-shaders`).
- [SDL3](https://www.libsdl.org/) and [zlib](https://zlib.net/), zlib licence.
- The source code of Descent (Parallax Software), released in 1998, was a
  reference for the engine that the game is built on; none of it is included.

Descent to Undermountain is © 1997 Interplay Productions. Dungeons & Dragons
and Forgotten Realms are trademarks of Wizards of the Coast. This project is
not affiliated with or endorsed by them.
