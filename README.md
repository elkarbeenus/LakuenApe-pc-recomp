# LaküenApeRecomp

[![Latest release](https://img.shields.io/github/v/release/elkarbeenus/LakuenApe-pc-recomp?label=release)](https://github.com/elkarbeenus/LakuenApe-pc-recomp/releases/latest)
[![License: GPL-3.0-or-later](https://img.shields.io/badge/license-GPL--3.0--or--later-blue)](LICENSE)

A native Linux build of a Switch 2 static recompilation, made for SteamOS. One AppImage builds the game for your PC from
your own dump and then works as its launcher: Play, play time and updates in one window. It runs on a desktop PC first
(Ryzen 7 5800X3D, RX 6600 XT, 16 GB) and also on the Steam Deck and the Legion Go S, each with settings made for it.

LaküenApeRecomp is a fork. The recompilation itself is the upstream project's work and its contributors' (see
[Credits](#credits)); this fork adds the Linux packaging, the launcher, the device profiles and the German texts, and
follows upstream's releases. Problems you only see here belong in this repository's
[issues](https://github.com/elkarbeenus/LakuenApe-pc-recomp/issues).

> [!IMPORTANT]
> **No game files are included, and none ever will be.** You need your own copy of the game, dumped from your own
> console. Instructions for obtaining those files will not be provided. Everything the build creates from your copy
> stays on your PC; never share those files.

## Features

- **One AppImage, nothing to install.** It brings its own Python, compiler and runtime libraries, so SteamOS's read-only
  system is no obstacle.
- **Recognises your device at every start** and picks fitting settings: the start screen shows what it found and what
  the game starts with. Your own settings always win.
- **One window for everything after the build:** Play, your play time, and one question when a newer release is out.
  **Update** downloads it, checks it, and brings your game up to date as well.
- **Steam Gaming Mode:** add the AppImage as a non-Steam game with `--play`, and it starts straight into the game.
- **When done:** standby or shut down after the build, for a build overnight (with a minute to cancel).
- **The build runs at low priority**, so the PC stays usable meanwhile.
- **German, English and Russian** in the setup and in the in-game settings menu.
- Everything upstream brings: AMD FSR 3.1 and FSR 4, frame generation, window filters, Anti-Lag 2, co-op, 16:10 and
  ultrawide, controller support and more.

## Install and play

1. Download `LakuenApeRecomp-<version>-x86_64.AppImage` from the
   [latest release](https://github.com/elkarbeenus/LakuenApe-pc-recomp/releases/latest).
2. Make it executable (right-click > Properties > Executable, or `chmod +x`) and start it. On a Steam Deck or a SteamOS
   PC, do this in Desktop Mode.
3. Pick your `.nup` file (or ExeFS and RomFS folders) and an install folder with plenty of free space, then start the
   build. It takes a while once; **When done** can put the PC to sleep afterwards.
4. Press **Play**. From then on the AppImage opens on the game.

**Requirements:** an x86-64 CPU with AVX2, Linux 6.7 or later (SteamOS 3.8 qualifies), a Vulkan 1.3 driver and 16 GB
of RAM.

### Steam (Gaming Mode)

In Desktop Mode, add the AppImage to Steam as a non-Steam game and set its launch option to `--play`. It then starts the
game without the setup window, and opens the window only to ask about a newer release. Steam counts the game as running
until it ends.

### Updates

When a newer release is out, the start screen asks once: **Update** downloads the new AppImage next to the old one,
checks it against the release's `SHA256SUMS.txt`, starts it and lets it update your game. **Play** keeps the current
version for now. Your saves and settings stay where they are.

## Devices

The device is recognised at every start from its firmware, CPU, RAM and graphics card.

| Device | Recognised by | Settings |
|---|---|---|
| PC with an AMD card of 8 GB or less, or 16 GB of RAM or less (such as Ryzen 7 5800X3D + RX 6600 XT) | RAM and graphics memory | Full HD 1920×1080, AMD FSR 4 (FSR 3.1 where FSR 4 cannot run), 60 fps shown as 120 by frame generation 2x |
| Lenovo Legion Go S | firmware or Ryzen Z2 Go | 1280×800 scaled to 1920×1200 by FSR, 40 fps shown as 80 by frame generation 2x, handheld mode |
| Steam Deck (LCD and OLED) | firmware or APU | 1280×800, 40 fps, handheld mode |
| Any other PC | | the game's own defaults |

A profile only fills in settings you have not set yourself, so anything you change in the in-game menu wins and
resetting it returns to the profile's value. `device_profile` in the settings file (`totk.ini` in the install folder)
picks a profile by hand or turns it off with `desktop`; `totk.ini.example` lists every value.

### Tips

- **The picture looks grainy:** in the menu under Display, try the upscaler *AMD FSR 3.1* or *AMD FSR 4*, scene
  resolution *full*, and adjust the sharpness. FSR 4 needs AMD's library and an RX 9000 card; anywhere else the game
  runs FSR 3.1 in its place.
- **High refresh monitors:** frame generation needs a display of at least twice the game's frame rate. On 180 Hz or
  more, frame generation 3x shows 60 fps as 180.
- **Frame generation and FSR are experimental upstream.** If something looks wrong, switch them off in the menu.
- **AMD under Linux:** upstream is still tracking GPU hangs on RADV, so the parallel recording threads stay off on AMD
  cards before the RX 9070.

## For developers

| | |
|---|---|
| Branch | `linux-appimage`: one branch and one build for every device |
| Build | `.github/workflows/fork-appimage-release.yml`, on every push to that branch or by hand; it runs the fork's tests, builds the AppImage in upstream's pinned container and publishes the release |
| Versions | upstream's `VERSION` plus the build number, such as `v0.4.2.11` |
| Upstream | merged in regularly; each change of this fork stays a separate commit on top, so it can be read, reverted or sent upstream on its own |

Internal names (folders, files, the game's program and its settings variables) stay upstream's, so settings, saves and
upstream updates carry over. Where the fork's changes live:

- Linux AppImage: `tools/make_release_linux.py`, [docs/installer.md](docs/installer.md)
- Device profiles: `runtime/cfg.c`; the launcher's copy of the rules in `installer/device.py`; both checked by
  `tests/device_profile_test.py`
- Launcher, play time and updates: `installer/launch.py`, `installer/update.py`, `installer/gui.py`
- When done: `installer/power.py`
- German texts: `installer/text/de.json`, `runtime/ui_text.c`
- Name: `runtime/port_name.h`

Building from source, the toolchain and the tests are described in [docs/building.md](docs/building.md); the upstream
documentation is in [docs/upstream-readme.md](docs/upstream-readme.md).

## Credits

LaküenApeRecomp would not exist without the people whose work it is built on:

- **The upstream recompilation project by [Lucas-blip-png](https://github.com/Lucas-blip-png) and its contributors**:
  the lifter, the runtime, the Switch 2 graphics front end, the shader lowering and the movie decoder are all theirs.
- **[TotK Recomp](https://github.com/valentderah/totk-pc-recomp) by valentderah**, the base the upstream project grew
  from, and the projects it builds on: [LAS Recomp](https://github.com/peiikai/LASRecomp),
  [MacNativeGames](https://github.com/ly05010419/MacNativeGames), [nxnative](https://github.com/seedds/nxnative),
  [SavageWindRecomp](https://github.com/Revivedx/SavageWindRecomp), [Ryujinx](https://github.com/Ryujinx/Ryujinx),
  [deko3d](https://github.com/devkitPro/deko3d) and [hactool](https://github.com/SciresM/hactool).
- **[Eglantine](https://git.eden-emu.dev/Franseven/PokeDinner-a-Eglantine-Fork)** (Eden Emulator Project) for the
  Ampere-to-Maxwell shader lowering and its Switch 2 notes, and [Mesa NAK](https://gitlab.freedesktop.org/mesa/mesa) for
  the encodings both build on.
- **[GRAnimated](https://github.com/GRAnimated)** for the game's symbols and research.
- **[libvpx](https://chromium.googlesource.com/webm/libvpx)** for the movies, **[Switchbrew](https://switchbrew.org)** and
  the Switch homebrew community.
- **AMD FidelityFX**, Hookshot and Longshot for the FSR 3.1, FSR 4 and Anti-Lag paths.

Every project whose code is used here, with its license and the exact commit, is listed in
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md); the upstream credits are in
[docs/upstream-readme.md](docs/upstream-readme.md#credits-and-origin).

## License

LaküenApeRecomp as a whole is under the [GNU GPL v3 or later](LICENSE), as upstream, because it includes Eglantine's
GPL shader lowering. The files that came from TotK Recomp keep their [MIT License](LICENSE-MIT), and third-party code
keeps its own license ([THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)).

This project is not affiliated with or endorsed by Nintendo. It contains no game code or assets.
