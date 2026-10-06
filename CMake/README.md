# Visual Studio 2026 build

Requires CMake 4.2+, Visual Studio 2026 with x86/x64 C++ tools and a Windows SDK.

```bat
call "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64
cmake --preset Generals-x86-Debug
cmake --build --preset Generals-x86-Debug
```

Other presets: `Generals-x86-Release`, `ZeroHour-x86-Debug`, `ZeroHour-x86-Release`.
Each also has an `x64` preset.
Binaries: `Binaries/Generals/<configuration>/Generals.exe` or `Binaries/ZeroHour/<configuration>/GeneralsZH.exe`.
The x64 presets add an `x64` directory before the configuration.
Set the working directory to the corresponding game's retail data directory.
Bink playback uses `Vendors/LibBinkDec`.
SDL3, OpenAL Soft, dr_libs, DXC, and DirectXTex are fetched by CMake.

SDK roots accept environment variables or CMake `-DGEN_<SDK>_ROOT=...`.
`GEN_ENABLE_GAMESPY` defaults to `OFF`; enabling it requires the original SDK.
