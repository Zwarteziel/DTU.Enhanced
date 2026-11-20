Undermountain Enhanced
======================

A native Windows version of Descent to Undermountain (Interplay, 1997) that
runs without DOSBox, with fixes for long-standing bugs and a number of
improvements. You need your own copy of the game, version 1.3.

https://github.com/Zwarteziel/DTU.Enhanced


Installation
------------

1. Install Descent to Undermountain and update it to version 1.3. If your
   copy is older, Undermountain Enhanced says so at its first start and can
   install the official 1.3 patch for you (from DTU1-3.EXE or dtu1_3.zip in
   the game folder or in your Downloads folder).

2. Unpack this zip file into the game folder, the folder with DTUGAME.EXE.
   You get DTU-Enhanced.exe, SDL3.dll and a folder called "enhanced".

3. The game reads some files from its CD. Either copy the contents of the CD
   to the folder _CD\DTU\_CDFILES next to DTU-Enhanced.exe, or set cd_path in
   dtu-enhanced.ini (written at the first start) to the folder that contains
   the DTU folder of the CD.

4. Start DTU-Enhanced.exe.


The first start
---------------

This download contains no code of the game. At its first start,
Undermountain Enhanced makes its own version of the game from your
DTUGAME.EXE: it translates it with the tools in the folder "enhanced" (SR,
a static recompiler, and nasm, an assembler). A small window shows the
progress; it takes less than a minute. The result is kept in
enhanced\cache, so later starts are immediate. It is made again when
DTUGAME.EXE or Undermountain Enhanced changes.

If it goes wrong, enhanced\cache\build.log and dtu-enhanced.log say why.


Settings and keys
-----------------

All settings are in dtu-enhanced.ini, each with a short explanation. Every
improvement can be switched back to the original behaviour there.

  Alt+Enter or F11   full screen on/off
  Ctrl+F10           release the mouse (click in the window to capture it)
  Ctrl+F12           screenshot
  1-9, 0             choose an answer in a conversation

Game controllers work too; the buttons can be changed in the [controller]
section of the ini file.


Licences
--------

Undermountain Enhanced is free software under the GNU General Public
License, version 3 or later (LICENSE.txt); its source code is at the address
above. The tools in the folder "enhanced" and the libraries it uses have
their own licences, in enhanced\licenses.

Descent to Undermountain is (c) 1997 Interplay Productions. Dungeons &
Dragons and Forgotten Realms are trademarks of Wizards of the Coast. This
project is not affiliated with or endorsed by them.
