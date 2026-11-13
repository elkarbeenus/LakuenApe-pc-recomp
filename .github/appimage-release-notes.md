**DKB Recomp Linux AppImage (fork preview).** `DKB-Setup-<version>-x86_64.AppImage` builds Donkey Kong Bananza on Linux and SteamOS from your own dump without any system packages: it carries its own Python, LLVM 23 and the Steam Runtime sniper sysroot.

1. Make it executable (`chmod +x`, or Properties > Executable) and start it; on a Steam Deck or a SteamOS desktop from Desktop Mode.
2. Pick your `.nup` file (1.0.0, or with the 3.2.0 update inside) or the ExeFS and RomFS folders, and an install folder with plenty of free space.
3. When the setup is done, start `dkb` in the install folder (or add it to Steam as a non-Steam game).

Requirements: x86-64 with AVX2, Linux 6.7 or later (SteamOS 3.8 qualifies), a Vulkan 1.3 driver, 16 GB of RAM (32 GB is comfortable).

**The package holds no game data.** It is a preview: the package and its checks are built in CI; a full install from a dump is not verified yet. Windows and macOS packages are on the upstream project, Lucas-blip-png/dkb-pc-recomp. Details: docs/installer.md, "Bananza AppImage".
