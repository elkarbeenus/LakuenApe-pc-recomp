**LaküenApeRecomp** is a personal fork of [DKB Recomp](https://github.com/Lucas-blip-png/dkb-pc-recomp) for Linux and SteamOS: one AppImage that builds *Donkey Kong Bananza* from your own dump on a desktop PC, the Steam Deck and the Legion Go S. It is not the official project; all of the recompilation work is Lucas-blip-png's and the DKB Recomp contributors'.

**The package holds no game data.**

### Use it

1. Download `LakuenApeRecomp-<version>-x86_64.AppImage`, make it executable (`chmod +x`, or Properties > Executable) and start it; on a Steam Deck or a SteamOS PC from Desktop Mode. No system packages are needed.
2. Pick your `.nup` file (1.0.0, or with the 3.2.0 update inside) or the ExeFS and RomFS folders, and an install folder with plenty of free space. **When done** on the build's page can put the computer in standby or shut it down after the build (a minute's countdown with **Don't do it** first).
3. Start `dkb` in the install folder, or add it to Steam as a non-Steam game.

### What the fork adds

- **Device profiles, recognised at every start:** Steam Deck (1280x800, 40 fps, handheld mode), Legion Go S (1280x800 scaled to its 1920x1200 screen, 40 fps, handheld mode), and a PC with an AMD card of 8 GB or less or 16 GB of RAM or less (the game at 1920x1080 instead of 2880x1620). Your own settings always win; `device_profile = desktop` in `dkb.ini` turns it off. `dkb.log` names the profile ("cfg: device profile").
- **The build at low priority** (nice 10, lowest best-effort disk priority), so the PC stays usable meanwhile (`DKB_NICE=0` keeps the normal one).
- **The desktop's own file chooser** (kdialog on SteamOS/KDE, else zenity; `DKB_TK_DIALOGS=1` keeps Tk's).
- **Update notice** for this fork's newer releases.

Requirements: x86-64 with AVX2, Linux 6.7 or later (SteamOS 3.8 qualifies), a Vulkan 1.3 driver, 16 GB of RAM. A preview: the package and its checks are built in CI; a full install from a dump is not verified yet.
