**LaküenApeRecomp** is a personal fork of a static recompilation project for Linux and SteamOS: one AppImage that builds the game from your own dump on a desktop PC, the Steam Deck and the Legion Go S. It is not the official project; all of the recompilation work is the upstream project's and its contributors'.

**The package holds no game data.**

### Use it

1. Download `LakuenApeRecomp-<version>-x86_64.AppImage`, make it executable (`chmod +x`, or Properties > Executable) and start it; on a Steam Deck or a SteamOS PC from Desktop Mode. No system packages are needed.
2. Pick your `.nup` file or the ExeFS and RomFS folders, and an install folder with plenty of free space. **When done** on the build's page can put the computer in standby or shut it down after the build (a minute's countdown with **Don't do it** first).
3. Press **Play**. From then on the AppImage opens on the game and asks about newer releases. For Steam, add the AppImage as a non-Steam game with the launch option `--play`.

### What the fork adds

- **Device profiles, recognised at every start:** Steam Deck (1280x800, 40 fps, handheld mode), Legion Go S (1280x800 scaled to its 1920x1200 screen, 40 fps, handheld mode), and a PC with an AMD card of 8 GB or less or 16 GB of RAM or less (the game at 1920x1080). Your own settings always win; `device_profile = desktop` in the settings file turns it off.
- **The build at low priority**, so the PC stays usable meanwhile.
- **The desktop's own file chooser** (kdialog on SteamOS/KDE, else zenity).
- **Launcher with updates:** Play in the setup, and at start a newer release asks whether to update (downloaded and checked automatically) or to play the current version.
- **German** in the setup and the settings menu.

Requirements: x86-64 with AVX2, Linux 6.7 or later (SteamOS 3.8 qualifies), a Vulkan 1.3 driver, 16 GB of RAM. A preview: a full install from a dump is not verified yet.
