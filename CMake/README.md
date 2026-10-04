# Visual Studio 2026 build

Requires CMake 4.2+, Visual Studio 2026 with x86 C++ tools and a Windows SDK,
a DirectX SDK containing `d3dx8.h`/`d3dx8.lib`.

```bat
call "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64
set GEN_DIRECTX_ROOT=C:\SDK\DirectX
cmake --preset Generals-Debug
cmake --build --preset Generals-Debug
```

Other presets: `Generals-Release`, `ZeroHour-Debug`, `ZeroHour-Release`.
Binaries: `Binaries/Generals/<configuration>/Generals.exe` or `Binaries/ZeroHour/<configuration>/GeneralsZH.exe`.
Set the working directory to the corresponding game's retail data directory.
Bink playback uses `Vendors/LibBinkDec`.
SDL3, OpenAL Soft, and dr_libs are fetched by CMake.

SDK roots accept environment variables or CMake `-DGEN_<SDK>_ROOT=...`.
`GEN_ENABLE_GAMESPY` defaults to `OFF`; enabling it requires the original SDK.
