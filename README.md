# SoulFu: Back From The Dead

> A 3D action role-playing hack and slash dungeon crawler made by Aaron Bishop, the creator of Egoboo.

<div align="center">

![logo](soulfu_bftd.png)

[![Matrix](https://img.shields.io/matrix/soulfu%3Amatrix.org?label=Matrix)](https://matrix.to/#/#soulfu:matrix.org)

</div>

---

## Why is the subtitle so gloomy?
Well, the game seems to be forgotten among developers. I decided to make some effort to change status quo and wanted to show it by adding the unofficial "Back From The Dead" subtitle. I hope my work will be a seed of further development of the game.

## How to contribute?
1. Fork the repository.
2. Implement your idea. If you implement more than one feature, it is a good practice to create a separate branch for each feature.
3. Create a new pull request.
4. If I like the changes, I will merge them. If not... well, don't worry. You can develop your fork of SoulFu on your own :)

Having said that, I need to say that any help is really appreciated. Programmers, level designers, 2D/3D artists, translators, testers, etc. If you do not know where to start, just ask...

> [!IMPORTANT]
> If you REALLY want to contribute, create a pull request related to a **SINGLE feature** with **NO side effects**. Do not work on the branch requested to be pulled before resolving the pull request! Preferably, just create a separate feature branch for your implementation. Please read [the manual](https://git-scm.com/book/en/v2/Git-Branching-Basic-Branching-and-Merging) if in doubt.

> [!CAUTION]
> **Non-compliance** to the rule above may lead to **REJECTION of the pull request**!

## Building the game
Clone the repository:
```
git clone --recurse-submodules <repolink> soulfu
```

Install the following tools and libraries:

+ gcc,
+ make,
+ SDL 2.0,
+ SDL2_net,
+ libogg,
+ libjpeg,
+ vorbis,
+ OpenGL development files.

### Examples
<details>
<summary>Arch Linux</summary>

For Arch Linux there exists a package in AUR: [link](https://aur.archlinux.org/packages/soulfu-git).

</details>

<details>
<summary>Debian / Ubuntu</summary>

1. Update package lists and install the required libraries:
```sh
sudo apt update
sudo apt install \
    build-essential \
    libsdl2-dev \
    libsdl2-net-dev \
    libogg-dev \
    libvorbis-dev \
    libjpeg-dev \
    libgl-dev
```
2. Navigate to the project directory and build **soulfu**.
```sh
cd soulfu
make
```

`make` compiles the game binary and builds `datafile.sdf` (the game data archive) in one step. The data tools (`sdp`, `slc`, `ssc`) are compiled automatically as needed.

To build with in-game development tools enabled:
```sh
make debug
```
</details>

<details>
<summary>Fedora (outdated, for SDL 1.2 version)</summary>

Run the following:
```sh
sudo dnf install gcc make glibc-devel.i686 sdl12-compat-devel.i686 SDL_net-devel.i686 libogg-devel.i686 libjpeg-turbo-devel.i686 libvorbis-devel.i686 mesa-libGL-devel.i686
```

Navigate to the project directory and build soulfu
```sh
cd soulfu
make
```

**Note:** If pkg-config doesn't find the libraries, you can tell it to look in a different folder like this: 
```sh
PKG_CONFIG_PATH=/usr/lib/i386-linux-gnu/pkgconfig/ make
```

</details>

<details>
<summary>Windows with MSYS2 (outdated, for 32 bit)</summary>

1. Install MSYS2 from the [official homepage](https://www.msys2.org/) or use a package manager like [scoop](https://scoop.sh/) `scoop install msys2`.

2. Open MSYS2 and install git and make.

```
pacman -S git make
```

3. It is recommended to clone the repo within MSYS2, because SRC files need EOLs in Unix style.
```
git clone --recurse-submodules https://github.com/szymor/soulfu soulfu
```

4. Update package lists and install the required libraries:
```
pacman -Su
pacman -F mingw-w64-i686-binutils
pacman -S mingw-w64-i686-gcc mingw-w64-i686-pkgconf mingw-w64-i686-SDL2 mingw-w64-i686-SDL2_net mingw-w64-i686-libjpeg-turbo mingw-w64-i686-libvorbis mingw-w64-i686-libogg
```

5. Export $PATH with binaries. In this case the directory is in `C:\Users\user\scoop\apps\msys2\current\mingw32\bin\`
```
export PATH=/c/Users/user/scoop/apps/msys2/current/mingw32/bin:$PATH
```

6. Check Makefile.mingw if the variable WINDRES is set correctly and build:
```
make -f Makefile.mingw
```

7. To play the game you might need DLLs. You can find them in their official repositories: [SDL2.dll](https://github.com/libsdl-org/SDL "Repo of SDL"), [SDL_net.dll](https://github.com/libsdl-org/SDL_net "Repo of SDL_net") 

</details>

<details>
<summary>Cross-compiling for Windows (MinGW-w64)</summary>

Cross-compilation is done on Linux using MinGW-w64. You will need:

- `mingw-w64`
- `cmake`
- `wget`

Set up the sysroot by running `packaging/setup_win64_sysroot.sh` from the repository root. This downloads SDL2 and SDL2_net from their official releases and cross-compiles libogg, libvorbis, and libjpeg-turbo from source into `/tmp/mingw64-sysroot`.

Once the sysroot is in place, build with:

```sh
make -f Makefile.mingw64
```
</details>

<details>
<summary>Haiku (outdated, for 32 bit)</summary>

```
setarch x86
pkgman install libsdl2_devel sdl2_net_devel libogg_devel libvorbis_devel libsdl2_x86 sdl2_net_x86
make
```

</details>


## What has been done till now?
+ Tools for handling **datafile.sdf** (the archive where all game data is stored) have been implemented. Maybe they are not of the highest quality, but they get work done. To be precise:
  + data packer/unpacker,
  + 3D model converter,
  + language file converter,
  + music converter,
  + bytecode analyser,
  + script compiler.
+ A few mods have been merged:
  + Squire AI by Poobah *[note: AI has been largely modified since the original release of Poobah's AI]*,
  + Mana Regeneration by Xuln *[note: it has been extended to Apprentices]*,
  + Saving System by Xuln *[note: some issues leading to loss of game progress have been fixed]*,
  + Sky Box by MiR,
  + Arena by bravebebe.
+ 64 bit platform support by xmeadow.
+ As noted above, AI has been improved significantly. Your helpers can now fight a bit better against bumpy enemies (like rats or slimes). Apprentices can cast spells in an intelligent manner. Some TALK commands are implemented, e.g. hold your ground, charge, follow, go to the nearest door. The work is in progress, so expect even more changes.
+ Background music has been enabled. It was already there, composed by Aaron.
+ Support for resolutions up to 1920x1080. No stretching, no black stripes.
+ Rough translation to French, German, Italian and Spanish made with DeepL. Polish translation is much better as I speak Polish natively. Polish language-specific font characters have been added. Not all text has been translated, though.
+ Elf has been rebalanced. The initial HP, strength and intelligence have been lowered. Taming, especially mountable monsters, is easier. The bow equipped initially by Elf is magical.
+ Some bug fixing has been done around G'nome Copter. Now it can be built by G'nome at intelligence of 30.
+ Port to SDL2.
+ Joystick support up to 16 buttons. The limit was 8 in the original and it made impossible to map controls to some buttons on modern gamepads.
+ Syntax definition for Sublime to add syntax highlighting for SoulFu scripting language (by Sky).

## What could be done?
I do not promise anything as the project is done in my free time. I have some ideas, though:
+ avoid recompilation of unchanged scripts by the script compiler,
+ a standalone room editor *(in progress by xmeadow)*,
+ conversion of 3D models along with skeletal animation data *(in progress by xmeadow)*,
+ proper translation - add language-specific font characters, translate books and monster names, etc.
+ network game - Aaron wrote a bunch of code for this feature, so I guess it could be made usable with relatively little effort,
+ port to platforms other than PC *(xmeadow confirms that build is possible for RG351 handheld console)*,
+ a new spell Drain - Aaron left some notes and an image (=ODRAIN.PCX), so maybe it could be finished,
+ new areas - judging from music files Aaron left, he had in mind designing Airship, Desert, Forest and Mountain areas,
+ even better AI - Apprentices could use a Gonne (G'nome's gun) and open chests or doors on demand, Kittens/Puppies could stop drowning so easily, heart collection AI could be refactored and extended to other characters, etc.
+ new weapons - maybe a spear, a trident?

But it's easier said than done.

Take a look at **notes/Master To Do List.txt** for even more ideas by Aaron himself.

## Trivia
+ Build with DEVTOOL flag to enable development tools inside the game. Hold down the C key to make a relevant button appear.
+ Luck gives you bonus to money and damage. You can find better stuff in chests, too.
+ Fortune cookies bring you luck.
+ Soldier can have one extra companion.
+ Mystic can turn undead by praying.
+ D'warf is immune to poison and petrification,
+ Successful taming requires three things - Tripe, high max HP of tamer and low HP of monster. Hit the monster a few times to soften it.
+ Unholy weapons make you hungry faster.
+ The spell Levitate can be used to create flying weapons.
