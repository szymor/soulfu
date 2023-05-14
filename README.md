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

## Building the game
Clone the repository:
```
git clone --recurse-submodules <repolink> soulfu
```
Build:
```
cd soulfu
make
```

## What has been done till now?
+ Tools for handling **datafile.sdf** (the archive where all game data is stored) have been implemented. Maybe they are not of the highest quality, but they get work done. To be precise:
  + data packer/unpacker,
  + language file converter,
  + script compiler.
+ A few mods have been merged:
  + Saving System by Xuln,
  + Sky Box by MiR 2008,
  + Arena by bravebebe.

## What will be done?
I do not promise anything. The project is done in my free time. I have some ideas, though:
+ fix screen scaling for 16:9 aspect ratio,
+ integrate some higher quality mods,
+ improve data handling tools even more,
+ 64 bit platform support,
+ port to platforms other than PC.

But it's easier said than done.

## Trivia
+ Build with DEVTOOL flag to enable development tools inside the game. Hold down the C key to make a relevant button appear.
+ No 64 bit support at the moment.
+ FileUtil crashes the game. Surely, it needs to be fixed.
