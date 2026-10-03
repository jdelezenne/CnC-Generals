# VC6 build

Use a VC6 developer command prompt, CMake 3.25+, and the x86 DirectX SDK
containing `d3dx8.h`/`d3dx8.lib`, Miles 6.5, and a Platform SDK containing
`DbgHelp.lib`. STLport 4.5.3 (with EA's patch) and zlib 1.1.4 are in `vendors/`.

```bat
call "C:\Program Files (x86)\Microsoft Visual Studio\VC98\Bin\VCVARS32.BAT"
set GEN_DIRECTX_ROOT=C:\SDK\DirectX
set GEN_MILES_ROOT=C:\SDK\Miles65
set GEN_PLATFORM_SDK_ROOT=C:\SDK\PlatformSDK
cmake --preset generals-release -DGEN_ENABLE_GAMESPY=OFF -DGEN_ENABLE_BINK=OFF -DGEN_ENABLE_LZH=OFF -DGEN_ENABLE_BENCHMARK=OFF
cmake --build --preset generals-release
```

Repeat with `generals-debug`, `zerohour-release`, or `zerohour-debug`.
Binaries: `out/build/<preset>/bin/RTS.exe` (Release), `RTSD.exe` (Debug).
Run with the corresponding game's retail data and Miles runtime files.

SDK roots accept CMake `-DGEN_<SDK>_ROOT=...` or environment variables.
Miles expects `include/mss.h` and `lib/mss32.lib` (or `lib/win/mss32.lib`).
To generate the import library from the installed game's matching DLL:

```powershell
./cmake/make-import-library.ps1 -Dll "C:/Games/Generals/mss32.dll" -Output "C:/SDK/Miles65/lib/mss32.lib"
```

Keep the SDK's `win/cleanup.c` (or `cleanup.c`) alongside it; CMake compiles
this helper when present. The generated import library must match the runtime DLL.

`GEN_DBGHELP_LIBRARY` can select an individual x86 import library.

| Option (default ON) | Required when enabled | Disabled behavior |
| --- | --- | --- |
| `GEN_ENABLE_GAMESPY` | Matching original GameSpy SDK, including its five DSPs | Online button and service workers disabled |
| `GEN_ENABLE_BINK` | `bink.h`, `binkw32.lib` | Movies skipped |
| `GEN_ENABLE_LZH` | LZH-Light `CompLibHeader/` and `CompLibSource/` | LZH operations fail; RefPack, EAC and zlib remain available |
| `GEN_ENABLE_BENCHMARK` | EA's BYTEmark integration with `RunBenchmark` | BYTEmark indices are zero |

Zero Hour defaults to `GEN_ENABLE_SAFEDISC=OFF`. Enabling it requires the original
SafeDisc SDK's `CdaPfn.h`, selected with `GEN_SAFEDISC_ROOT`.

`GEN_CORE_ONLY=ON` builds the seven standalone Westwood libraries.
`GEN_VERSION` defaults to `1.8` for Generals and `1.4` for Zero Hour;
`GEN_BUILD_NUMBER` defaults to `0`.

Source manifests come from the original DSPs. Regenerate with
`python cmake/extract-dsp.py`.
