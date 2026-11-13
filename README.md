# LaküenApeRecomp

A personal fork of a static recompilation project, tuned for Linux and SteamOS on a desktop PC (RX 6600 XT), the Steam
Deck and the Legion Go S. It is not the official project and is not run by its developers: all of the recompilation
work is the upstream project's and its contributors' ([upstream documentation](docs/upstream-readme.md)). Problems you
only see in this fork belong in this fork's issues.

> [!IMPORTANT]
> **No game files are included, and none ever will be.** You need your own copy of the game, dumped from your own
> console. Instructions for obtaining those files will not be provided. Everything the build creates from your copy
> stays on your PC; never share those files.

## Using it

1. Download `LakuenApeRecomp-<version>-x86_64.AppImage` from this fork's latest
   [release](https://github.com/elkarbeenus/LakuenApe-pc-recomp/releases/latest).
2. Make it executable (`chmod +x`, or Properties > Executable) and start it; on a Steam Deck or a SteamOS PC from
   Desktop Mode. No system packages are needed.
3. Pick your `.nup` file or ExeFS and RomFS folders and an install folder with plenty of free space. **When done** on the
   build's page can put the computer in standby or shut it down after the build.
4. Press **Play**. From then on the AppImage opens on the game: **Play** starts it, and when a newer release is out it asks
   first whether to update or to play the current version.
5. For Steam (Gaming Mode), add the AppImage as a non-Steam game with the launch option `--play`: it starts the game at
   once and shows a window only to ask about a newer release.

Requirements: x86-64 with AVX2, Linux 6.7 or later (SteamOS 3.8 qualifies), a Vulkan 1.3 driver, 16 GB of RAM.

## About this fork

| | |
|---|---|
| Fork branch | `linux-appimage` (one branch and one build for every device) |
| Output | `LakuenApeRecomp-<version>-x86_64.AppImage`, published as a release by `.github/workflows/fork-appimage-release.yml` |
| Versions | upstream's `VERSION` plus the build number: `v0.3.0.2` |
| License | GPL-3.0-or-later, as upstream ([LICENSE](LICENSE)); files from its own upstream keep MIT ([LICENSE-MIT](LICENSE-MIT)) |

Each change is a separate commit on top of upstream, so it can be read, reverted or sent upstream on its own. Internal
names (folders, files, the game's program and its settings variables) stay upstream's, so settings, saves and upstream
updates carry over.

- **Linux AppImage.** The setup with its own Python, LLVM 23 and the Steam Runtime sniper sysroot: no system packages,
  so SteamOS's read-only system is no obstacle. (`tools/make_release_linux.py`, docs/installer.md)
- **Device profiles, recognised at every start** (Linux): the Steam Deck (firmware or APU), the Legion Go S (firmware or
  Ryzen Z2 Go) and a small PC (an AMD card of 8 GB or less, or 16 GB of RAM or less). A profile only sets defaults for
  the keys the settings file does not set, so your own settings and the menu's always win; `device_profile = desktop`
  turns it off, and the log names the profile and why. The values are in `totk.ini.example` ([display],
  device_profile). (`runtime/cfg.c`, `tests/device_profile_test.py`)
- **When done:** standby or shut down when the build ends (a minute's countdown with **Don't do it** first; never after
  Cancel), for a build overnight. (`installer/power.py`)
- **The build at low priority** on Linux (nice 10, the lowest best-effort disk priority), so the machine stays usable;
  an environment switch keeps the normal priority. (`installer/`, lower_priority)
- **The desktop's own file chooser** (kdialog on SteamOS/KDE, else zenity), with Tk's as the fallback.
  (`installer/guilogic.py`)
- **The setup as the launcher:** after an install it remembers the folder; started again it opens on the game with
  **Play** and asks about a newer release of this fork (update now or play the current version). An update downloads the
  release's AppImage next to the running one, checks it against `SHA256SUMS.txt` and updates the install with it.
  `--play` starts the game without the window, for Steam; the window waits hidden while the game runs, so Steam counts
  it as running. (`installer/launch.py`, `installer/gui.py`)
- **German** in the setup window and the settings menu, picked by the system language or chosen by hand (the setup's
  DE button, the menu's Settings language). The setup's texts no longer name the game. (`installer/text/de.json`,
  `runtime/ui_text.c`)
- **Name:** window titles, the settings menu's banner, the setup window and menu entry, and the AppImage are named
  LaküenApeRecomp (`runtime/port_name.h`).
