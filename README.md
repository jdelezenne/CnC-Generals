
# Command & Conquer Generals (inc. Zero Hour) Source Code

This repository includes source code for Command & Conquer Generals, and its expansion pack Zero Hour. This release provides support to the Steam Workshop for both games ([C&C Generals](https://steamcommunity.com/workshop/browse/?appid=2229870) and [C&C Generals - Zero Hour](https://steamcommunity.com/workshop/browse/?appid=2732960)).


## Dependencies

If you wish to rebuild the source code and tools successfully you will need to find or write new replacements (or remove the code using them entirely) for the following libraries;

- DirectX SDK (Version 9.0 or higher) (expected path `\Code\Libraries\DirectX\`)
- 3DSMax 4 SDK - (expected path `\Code\Libraries\Max4SDK\`)
- NVASM - (expected path `\Code\Tools\NVASM\`)
- BYTEmark - (expected path `\Code\Libraries\Source\Benchmark`)
- RAD Miles Sound System SDK - (expected path `\Code\Libraries\Source\WWVegas\Miles6\`)
- RAD Bink SDK - (expected path `\Code\GameEngineDevice\Include\VideoDevice\Bink`)
- SafeDisk API - (expected path `\Code\GameEngine\Include\Common\SafeDisk` and `\Code\Tools\Launcher\SafeDisk\`)
- Miles Sound System "Asimp3" - (expected path `\Code\Libraries\WPAudio\Asimp3`)
- GameSpy SDK - (expected path `\Code\Libraries\Source\GameSpy\`)
- ZLib (1.1.4) - (expected path `\Code\Libraries\Source\Compression\ZLib\`)
- LZH-Light (1.0) - (expected path `\Code\Libraries\Source\Compression\LZHCompress\CompLibSource` and `CompLibHeader`)


## Linux x64

Use CMake 4.2 or newer, Ninja, Clang 21 with matching libc++ and libc++abi development packages, Python 3, Git, FreeType and HarfBuzz development packages. SDL's Linux desktop backends use the system ALSA, PulseAudio, X11, Wayland, xkbcommon, DRM and GBM development packages. Install dependencies through your distribution's package manager. Rendering requires a Vulkan driver.

Build from a native Linux filesystem:

```sh
cmake --preset Generals-Linux-Clang-x64-Release
cmake --build --preset Generals-Linux-Clang-x64-Release --parallel
cmake --preset ZeroHour-Linux-Clang-x64-Release
cmake --build --preset ZeroHour-Linux-Clang-x64-Release --parallel
```

The presets use `clang` and `clang++`; ensure those commands select the installed compiler and matching C++ library. CMake fetches pinned dependencies. Linux uses system FreeType and HarfBuzz.

Copy your existing retail game installations to Linux. Keep the archive files and loose asset directories together. Zero Hour also needs the Generals installation. Set `[Installation]` and `InstallPath=/absolute/path/to/game/` in each game's `Settings.ini`, under `${XDG_DATA_HOME:-$HOME/.local/share}/Electronic Arts/Generals/` and `Electronic Arts/ZeroHour/`. Set `Language` in that section to the language of your retail data; it defaults to `english`.

Import the original font files from your existing Windows installation before running either game:

```sh
python3 Tools/ImportFonts.py /path/to/Windows/Fonts "$HOME/.local/share/Electronic Arts/Generals"
python3 Tools/ImportFonts.py /path/to/Windows/Fonts "$HOME/.local/share/Electronic Arts/ZeroHour"
```

Use your configured XDG data directory instead when it differs from the default. The importer copies Arial, Times New Roman, Courier New, FixedSys and Courier files, and Arial Unicode MS when present.

Run `Binaries/Generals/Linux-Clang-x64/Release/Generals -win` from the Generals retail directory, or `Binaries/ZeroHour/Linux-Clang-x64/Release/GeneralsZH -win` from the Zero Hour retail directory, using the executable's absolute path. Add `-quickstart` to skip the sizzle video and use a static menu background.

To package either build, run `cmake --install Build/ZeroHour/Linux-Clang-x64/Release --component Game --prefix /path/to/output`, substituting the Generals build directory as needed. Keep the executable and `libdxcompiler.so` together. Retail assets and fonts are imported separately.

## Compiling the original Win32 projects

To use the compiled binaries, you must own the game. The C&C Ultimate Collection is available for purchase on [EA App](https://www.ea.com/en-gb/games/command-and-conquer/command-and-conquer-the-ultimate-collection/buy/pc) or [Steam](https://store.steampowered.com/bundle/39394/Command__Conquer_The_Ultimate_Collection/).

The quickest way to build all configurations in the project is to open `rts.dsw` in Microsoft Visual Studio C++ 6.0 (SP6 recommended for binary matching to Generals patch 1.08 and Zero Hour patch 1.04) and select Build -> Batch Build, then hit the “Rebuild All” button.

If you wish to compile the code under a modern version of Microsoft Visual Studio, you can convert the legacy project file to a modern MSVC solution by opening `rts.dsw` in Microsoft Visual Studio .NET 2003, and then opening the newly created project and solution file in MSVC 2015 or newer.

NOTE: As modern versions of MSVC enforce newer revisions of the C++ standard, you will need to make extensive changes to the codebase before it successfully compiles, even more so if you plan on compiling for the Win64 platform.

When the workspace has finished building, the compiled binaries will be copied to the folder called `/Run/` found in the root of each games directory. 


## Known Issues

Windows has a policy where executables that contain words “version”, “update” or “install” in their filename will require UAC Elevation to run. This will affect “versionUpdate” and “buildVersionUpdate” projects from running as post-build events. Renaming the output binary name for these projects to not include these words should resolve the issue for you.


## Contributing

This repository will not be accepting contributions (pull requests, issues, etc). If you wish to create changes to the source code and encourage collaboration, please create a fork of the repository under your GitHub user/organization space.


## Support

This repository is for preservation purposes only and is archived without support. 


## License

This repository and its contents are licensed under the GPL v3 license, with additional terms applied. Please see [LICENSE.md](LICENSE.md) for details.
