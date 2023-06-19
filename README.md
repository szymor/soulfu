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
  + 3D model converter,
  + language file converter,
  + script compiler.
+ A few mods have been merged:
  + Squire AI by Poobah,
  + Mana Regeneration by Xuln,
  + Saving System by Xuln,
  + Sky Box by MiR,
  + Arena by bravebebe.
+ Background music has been enabled. It was already there, composed by Aaron.
+ Support for resolutions up to 1920x1080. No stretching, no black stripes.
+ Rough translation to French, German, Italian, Polish and Spanish. Window layout needs to be adjusted and the translation needs to be reviewed as it was done mostly with DeepL. Books are left untranslated, too.
+ Elf has been rebalanced. I felt that class was a bit overpowered - 10 points more than other classes make a difference. Its gameplay style was in fact the same as Soldier's, so I guess almost nobody played the latter. That's why I decided to lower Elf's initial HP, strength and intelligence. However, it became problematic if we wanted to charm monsters, so I needed to treat Elf specially in this regard (to be consistent with Aaron's original idea) by making taming easier. Elf was also supposed to use a bow more frequently, so I made it more useful by making it magical. Is it too strong? Maybe, but 10 points more was even stronger as a benefit. Why haven't I rebalanced D'warf then? Strength or HP are not as useful as Dexterity.

## What could be done?
I do not promise anything as the project is done in my free time. I have some ideas, though:
+ avoid recompilation of unchanged scripts by the script compiler,
+ a standalone room editor,
+ conversion of 3D models along with skeletal animation data,
+ proper translation - add language-specific font characters, adjust window layout, translate books and monster names, etc.
+ network game - Aaron wrote a bunch of code for this feature, so I guess it could be made usable with relatively little effort,
+ 64 bit platform support,
+ port to platforms other than PC,
+ a new spell Drain - Aaron left some notes and an image (=ODRAIN.PCX), so maybe it could be finished,
+ new areas - judging from music files Aaron left, he had in mind designing Airship, Desert, Forest and Mountain areas,
+ better AI - Apprentices could use spells in a more helpful way, Kittens/Puppies could stop drowning so easily, etc. Aaron had in mind issuing commands as you can find command-related images in the archive (=CHARGE.PCX, =FOLLOW.PCX, =HELP.PCX, =KEY.PCX, =STAND.PCX, =THROUGH.PCX, =TOWN.PCX), so it could be implemented, too.

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
