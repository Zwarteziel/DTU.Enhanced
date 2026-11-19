# Undermountain Enhanced

Version 1.0.0

A native Windows version of **Descent to Undermountain** (Interplay, 1997) that
runs without DOSBox, with fixes for long-standing bugs and a number of
improvements. You need your own copy of the game, version 1.3.

Undermountain Enhanced is not an emulator. The game's program, `DTUGAME.EXE`
(a 32-bit DOS/4GW program made with Watcom C), is translated to x64 machine code
with M-HT's static recompiler [SR](https://github.com/M-HT/SR). A runtime written
for this game replaces DOS, DPMI, VESA, the timer, the keyboard, the mouse and
the HMI SOS sound driver. SDL3 handles the window, the CRT shader, sound and
game controllers.

**This repository contains no code or data of the game.** The translation is
made on your own computer, from your own `DTUGAME.EXE`, when you build the
program (see *Building*). There is no ready-made exe to download.

## What works

The whole game: intro movies, menus, character creation, Waterdeep, all levels,
conversations, music and sound, saving and loading.

**Display**
- Window or full screen, 4:3 picture, with the VGA monitor emulation (CRT
  shaders) of DOSBox Staging, or sharp/linear/nearest scaling without it.
- The 3D view uses all 480 lines of the screen instead of every line twice
  (with *Hires Dungeon Graphics* on in the game's options).
- Full drawing quality up to the view distance: walls further away no longer
  switch to a darker, simpler drawing method.
- Textures on objects (tables, beds, creatures) with perspective correction.
- Creatures and your weapon move smoothly between the 15 key frames per second
  of their animations.
- Creatures are drawn with their feet on the floor; in the original they often
  walked in the air.
- Your weapon no longer turns black for a moment during an attack.
- Water splashes at your feet are visible again without the white flash that
  filled the screen in the original.

**Controls**
- Mouse captured by the window, with adjustable speed and vertical speed.
- A mouse pointer in the text menus (main menu, Load Game, Options).
- Conversation answers with the keys 1-9 and 0.
- Keys that open a screen (automap, inventory, statistics) no longer make it
  flash open and closed when held.
- Game controllers (Xbox, PlayStation and others), configurable.

**Game**
- Runs at 60 fps (configurable) with the movement, strafing, falling and jumps
  of the original at about 20 fps, so they do not depend on the frame rate.
- You no longer get stuck at the top of a ladder.
- Footsteps of creatures no longer repeat four times per step.
- The weapon's hit sound is no longer dropped (often on the killing blow) when
  many ambient sounds are playing.
- Music fades out and waits while the game loads, instead of stuttering.
- Several crashes and freezes of the original are caught (division by zero,
  null-pointer reads).

**Other**
- Checks at start-up that the game files are version 1.3, and can install the
  official 1.3 patch from `DTU1-3.EXE` or `dtu1_3.zip`
  ([download](https://www.patches-scrolls.de/patch/1215/7/41518/download)).
- Plays the game's MVE movies itself.
- Shows its version at the bottom of the main menu, in the title bar and in
  the file properties of the exe.

## Installation

1. Install Descent to Undermountain and update it to version 1.3.
2. Build `DTU-Enhanced.exe` (see *Building*) and copy it, with `SDL3.dll`,
   into the game folder (the folder with `DTUGAME.EXE`).
3. The game reads some files from the CD. Either set `cd_path` in
   `dtu-enhanced.ini` to the folder with the contents of the CD (the folder that
   contains `DTU\`), or copy the CD to `_CD\DTU\_CDFILES` next to the exe.
4. Start `DTU-Enhanced.exe`.

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

The build runs on Linux (or WSL on Windows) and makes a Windows exe. You need
clang and lld (tested with 18), nasm (2.16), python3, gcc/g++, unzip and patch,
and three archives in one folder (`TOOLS`):

| File | What |
|---|---|
| `SR-master.zip` | [M-HT/SR](https://github.com/M-HT/SR) as a zip of the repository |
| `SDL3-devel-3.4.20-mingw.tar.gz` | SDL3 for mingw, from the [SDL releases](https://github.com/libsdl-org/SDL/releases) |
| `mingw64.zip` | An MSYS2 mingw-w64 x86_64 tree (`include`, `lib` with the CRT, libgcc and `libz.a`) |

Then:

```sh
TOOLS=/path/to/tools GAME=/path/to/DTU sh build-windows.sh
```

`GAME` is the game folder with `DTUGAME.EXE` version 1.3. The script builds SR
(with the patch in `third_party/SR`), translates `DTUGAME.EXE`, and links the
result with the runtime. The exe and `SDL3.dll` end up in `$HOME/dtu-build/win`
(set `PREFIX` for another place).

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
| `docs/` | The development log (in Dutch) with screenshots and comparisons |

`docs/analyse-en-plan.md` describes, version by version, how the game works
internally where it mattered and how each change was made and tested.

## Licence and credits

Undermountain Enhanced is released under the GNU General Public License,
version 3 or later (see `LICENSE`).

- [SR](https://github.com/M-HT/SR) by Roman Pauer, MIT licence (also GPL-2.0
  or later); the files in `recomp/x64/` come from SR.
- CRT shaders from [DOSBox Staging](https://github.com/dosbox-staging/dosbox-staging)
  0.83, MIT licence (see `third_party/dosbox-staging-shaders`).
- [SDL3](https://www.libsdl.org/) and [zlib](https://zlib.net/), zlib licence.
- The source code of Descent (Parallax Software), released in 1998, was a
  reference for the engine that the game is built on; none of it is included.

Descent to Undermountain is © 1997 Interplay Productions. Dungeons & Dragons
and Forgotten Realms are trademarks of Wizards of the Coast. This project is
not affiliated with or endorsed by them.
