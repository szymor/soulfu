# SoulFu: Back From The Dead

## Description
SoulFu is a 3D action role-playing hack and slash dungeon crawler made by Aaron Bishop, the creator of Egoboo.

## Why is the subtitle so gloomy?
Well, the game seems to be forgotten among developers. I decided to make some effort to change status quo and wanted to show it by adding the unofficial "Back From The Dead" subtitle. I hope my work will be a seed of further development of the game.

## How to contribute?
1. Fork the repository.
2. Implement your idea. If you implement more than one feature, it is a good practice to create a separate branch for each feature.
3. Create a new pull request.
4. If I like the changes, I will merge them. If not... well, don't worry. You can develop your fork of SoulFu on your own :)

Having said that, I need to say that any help is really appreciated. Programmers, level designers, 2D/3D artists, translators, testers, etc. If you do not know where to start, just ask...

If you want to talk about development of SoulFu, you can reach me out at #soulfu:matrix.org ([click to join Matrix](https://matrix.to/#/#soulfu:matrix.org)).

## Building the game
Clone the repository:
```
git clone --recurse-submodules <repolink> soulfu
```

Install the following tools and libraries:

+ GCC,
+ Make,
+ SDL 1.2,
+ SDL_net,
+ libogg,
+ libjpeg,
+ vorbis,
+ OpenGL development files.

Note that the libraries need to be in the 32-bit version.

Example for Fedora:
```
sudo dnf install gcc make glibc-devel.i686 sdl12-compat-devel.i686 SDL_net-devel.i686 libogg-devel.i686 libjpeg-turbo-devel.i686 libvorbis-devel.i686 mesa-libGL-devel.i686
```

Example for Ubuntu/Debian:
```
sudo apt update
sudo apt install gcc-multilib libc6-dev:i386 libsdl-net1.2-dev:i386 libvorbis-dev:i386 libjpeg-dev:i386 libgcc-11-dev:i386 libsdl1.2-dev:i386
```

Build:
```
cd soulfu
make
```

If pkg-config doesn't find the libraries, you can tell it to look in a different folder like this:
```
# Fedora
PKG_CONFIG_PATH=/usr/lib/pkgconfig/ make

# Ubuntu/Debian
PKG_CONFIG_PATH=/usr/lib/i386-linux-gnu/pkgconfig/ make
```

For Arch Linux there exists a package in AUR: [link](https://aur.archlinux.org/packages/soulfu-git).

## What has been done till now?
+ Tools for handling **datafile.sdf** (the archive where all game data is stored) have been implemented. Maybe they are not of the highest quality, but they get work done. To be precise:
  + data packer/unpacker,
  + 3D model converter,
  + language file converter,
  + script compiler.
+ A few mods have been merged:
  + Squire AI by Poobah *[note: AI has been largely modified since the original release of Poobah's AI]*,
  + Mana Regeneration by Xuln *[note: it has been extended to Apprentices]*,
  + Saving System by Xuln *[note: some issues leading to loss of game progress have been fixed]*,
  + Sky Box by MiR,
  + Arena by bravebebe.
+ As noted above, AI has been improved significantly. Your helpers can now fight a bit better against bumpy enemies (like rats or slimes). Apprentices can cast spells in an intelligent manner. Some TALK commands are implemented, e.g. hold your ground, charge, follow, go to the nearest door. The work is in progress, so expect even more changes.
+ Background music has been enabled. It was already there, composed by Aaron.
+ Support for resolutions up to 1920x1080. No stretching, no black stripes.
+ Rough translation to French, German, Italian and Spanish made with DeepL. Polish translation is much better as I speak Polish natively. Polish language-specific font characters have been added. Not all text has been translated, though.
+ Elf has been rebalanced. The initial HP, strength and intelligence have been lowered. Taming, especially mountable monsters, is easier. The bow equipped initially by Elf is magical.
+ Some bug fixing has been done around G'nome Copter. Now it can be built by G'nome at intelligence of 30.

## What could be done?
I do not promise anything as the project is done in my free time. I have some ideas, though:
+ avoid recompilation of unchanged scripts by the script compiler,
+ a standalone room editor,
+ conversion of 3D models along with skeletal animation data,
+ proper translation - add language-specific font characters, translate books and monster names, etc.
+ network game - Aaron wrote a bunch of code for this feature, so I guess it could be made usable with relatively little effort,
+ 64 bit platform support,
+ port to platforms other than PC,
+ a new spell Drain - Aaron left some notes and an image (=ODRAIN.PCX), so maybe it could be finished,
+ new areas - judging from music files Aaron left, he had in mind designing Airship, Desert, Forest and Mountain areas,
+ even better AI - Apprentices could use a Gonne (G'nome's gun) and open chests or doors on demand, Kittens/Puppies could stop drowning so easily, heart collection AI could be refactored and extended to other characters, etc.
+ port to SDL2 (as a compilation switch),
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
