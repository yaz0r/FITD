# FITD

Free in the Dark, a Alone in the Dark engine reimplementation.

The goal of this project is to make playable on modern machines the "Alone in the Dark" (AITD) games released from 1992 to 1995 by Infograms.
The focus is on the versions currently available on Steam and GOG (some other versions like Alone in the Dark 3 windows versions are incompatible for now).

Steam:
* [Alone in the Dark 1](https://store.steampowered.com/app/548090/Alone_in_the_Dark_1/)
* [Alone in the Dark 2](https://store.steampowered.com/app/548890/Alone_in_the_Dark_2/)
* [Alone in the Dark 3](https://store.steampowered.com/app/548900/Alone_in_the_Dark_3/)

[GOG](https://www.gog.com/en/game/alone_in_the_dark_the_trilogy_123) version available as a trilogy.

Jack in the Dark that was included in some releases is also supported.

Additionaly, there is (very) early support for "Timegate: Knights Chase" released by Infogram in 1995 using a slightly upgraded engine.

[Steam](https://store.steampowered.com/app/781280/Time_Gate_Knights_Chase/) version, and [GOG](https://www.gog.com/en/game/time_gate_knights_chase) versions.

## Status
* Graphical issues and missing features in all games
* AITD1 was completable at some point (with some missing features), but might have regressed
* Jack in the Dark was also completable at some point.
* Other games are considered non-completable due to a variety of missing features or bugs.
* Current focus is to make games completable and fixing some of the graphics shortcomings.

## Build instructions
* Requires VS2022 with CMake module installed (you can also provide your own cmake but vs2022.bat won't work out of the box).
* Clone the repo with recursive submodules enabled
* Run build/vs2022.bat this should detect vs2022/cmake and generate the solution
* Open solution build/vs2022/FITD.sln
* Set fitd as startup project
* Set fitd working directory to where the game files live
* Hit F5

## Screenshots

![image](https://github.com/user-attachments/assets/d12b7c66-6c57-4507-b2b3-540cb2cc6806)
![image](https://github.com/user-attachments/assets/ba4508ba-1b7f-4ac1-b573-169e3f8e7bab)
![image](https://github.com/user-attachments/assets/653d3dc5-4a35-4a7f-95bc-76e7b42899d9)
![image](https://github.com/user-attachments/assets/310c3151-18cf-4914-8992-7de1dd763653)

