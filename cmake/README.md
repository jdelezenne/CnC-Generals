# Visual Studio 2026 build

Requires CMake 4.2+, Visual Studio 2026 with x86 C++ tools and a Windows SDK,
a DirectX SDK containing `d3dx8.h`/`d3dx8.lib`, and Miles 6.5.

```bat
call "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64
set GEN_DIRECTX_ROOT=C:\SDK\DirectX
set GEN_MILES_ROOT=C:\SDK\Miles65
cmake --preset generals-debug
cmake --build --preset generals-debug
```

Other presets: `generals-release`, `zerohour-debug`, `zerohour-release`.
Binaries: `out/build/msvc2026/<preset>/bin/RTSD.exe` (Debug) or `RTS.exe` (Release).
Run alongside the corresponding game's retail data and Miles runtime files.
Bink playback is enabled by default through `vendors/libbinkdec`.

SDK roots accept environment variables or CMake `-DGEN_<SDK>_ROOT=...`.
Miles expects `include/mss.h` and `lib/mss32.lib` (or `lib/win/mss32.lib`).
An import library can be generated from the matching retail DLL:

```powershell
./cmake/make-import-library.ps1 -Dll "C:/Games/Generals/mss32.dll" -Output "C:/SDK/Miles65/lib/mss32.lib"
```

Keep the SDK's `win/cleanup.c` or `cleanup.c` alongside it when using a generated
import library. The library must match the runtime DLL.

`GEN_ENABLE_GAMESPY`, `GEN_ENABLE_LZH`, `GEN_ENABLE_BENCHMARK` and
`GEN_ENABLE_SAFEDISC` default to `OFF`; enabling them requires their original SDKs.
`GEN_ENABLE_BINK=OFF` skips movies. `GEN_CORE_ONLY=ON` builds the standalone
Westwood libraries.
