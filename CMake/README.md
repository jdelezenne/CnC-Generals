# Visual Studio 2026 build

Requires CMake 4.2+, Visual Studio 2026 with x86 C++ tools and a Windows SDK,
a DirectX SDK containing `d3dx8.h`/`d3dx8.lib`. Miles 6.5c is in `Vendors/Miles65`.

```bat
call "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64
set GEN_DIRECTX_ROOT=C:\SDK\DirectX
cmake --preset Generals-Debug
cmake --build --preset Generals-Debug
```

Other presets: `Generals-Release`, `ZeroHour-Debug`, `ZeroHour-Release`.
Binaries: `Binaries/<game>/Debug/RTSD.exe` (Debug) or `Binaries/<game>/Release/RTS.exe` (Release).
Run alongside the corresponding game's retail data and Miles runtime files.
Bink playback uses `Vendors/LibBinkDec`.

SDK roots accept environment variables or CMake `-DGEN_<SDK>_ROOT=...`.
Miles expects `include/mss.h` and `lib/mss32.lib` (or `lib/win/mss32.lib`).
An import library can be generated from the matching retail DLL:

```powershell
./CMake/MakeImportLibrary.ps1 -Dll "C:/Games/Generals/mss32.dll" -Output "C:/SDK/Miles65/lib/mss32.lib"
```

Keep the SDK's `win/cleanup.c` or `cleanup.c` alongside it when using a generated
import library. The library must match the runtime DLL.

`GEN_ENABLE_GAMESPY` defaults to `OFF`; enabling it requires the original SDK.
