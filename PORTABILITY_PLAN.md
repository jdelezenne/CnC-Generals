# Generals and Zero Hour Linux and macOS support plan

This document records the implementation plan for native Linux and macOS support for both games. The plan below preserves the information presented in the conversation; the checklist tracks completion of its nine phases.

## Progress

System dependencies are installed and upgraded by the repository owner: `apt` on Linux and `brew` on macOS. Agents inspect package availability and provide exact commands; they do not install or upgrade packages. Ask the owner before using a workaround, vendor compatibility patch or diagnostic suppression in place of the required dependency setup. This rule is also recorded in `AGENTS.md`.

Recorded on 2026-10-06. Both Windows x64 Release baseline builds pass with the pending image/preference changes and shared thread/synchronization services. Compiler/platform separation, native shader generation and case-sensitive asset lookup are implemented; Generals and Zero Hour x64 Release builds both pass with the shared clock. The nine phases remain incomplete, including the embedded-browser replacement decision. Runtime testing remains stopped, and the user has authorized resuming WSL after completing the Ubuntu reinstall.

- [ ] 1. Establish the baseline and exact scope.
- [ ] 2. Make CMake portable.
- [ ] 3. Complete shared runtime services.
- [ ] 4. Complete portable file handling.
- [ ] 5. Replace GDI fonts and finish text input.
- [ ] 6. Port networking and remaining application integration.
- [ ] 7. Finish the shared renderer for Vulkan and Metal.
- [ ] 8. Finish architecture and binary-format compatibility.
- [ ] 9. Finish native distribution.

## Implementation plan

I’d use Renegade’s committed work in **`890b02dd35bb518aee13bff5c7229c2bfa4f5c35` (`linux-gameplay-checkpoint`)** as the reference. They already provide SDL threads, fonts and IME, directory handling, file interfaces, networking boundaries, and portable runtime helpers.

Some parts need adaptation: Renegade’s POSIX file code uses Linux-specific `renameat2`, its DXC configuration excludes macOS, and our renderer needs runtime compilation of Generals’ legacy shaders.

The implementation should proceed in this order:

1. **Establish the baseline and exact scope.**
   Finish reviewing the pending image-loader and preference-folder changes. Inventory the sources actually compiled by CMake, their Windows dependencies, and matching Renegade replacements. Keep the reference pinned to those commits.
   **Completion:** every dependency has a concrete replacement or a justified native platform boundary.

2. **Make CMake portable.**
   Separate compiler flags from platform source selection. Add Linux Clang and macOS AppleClang presets, including macOS arm64. Preserve Windows x86/x64 builds. Make build and binary paths identify game, platform, compiler, architecture, and configuration. Replace Zero Hour’s `shdpp.exe` with a native build tool; resolve Windows resource and COM/MIDL generation.
   **Completion:** native configurations select explicit sources and appropriate host tools, without new feature options.

3. **Complete shared runtime services.**
   Adapt Renegade’s thread, synchronization, clock, CPU-information, and diagnostic interfaces. Use shared SDL implementations where suitable on **Windows, Linux, and macOS**. Preserve timer rollover, worker shutdown, priorities, and floating-point rounding behavior. Keep native exception reporting behind a small platform boundary.
   **Completion:** common engine headers no longer pull in Windows headers for these services.

4. **Complete portable file handling.**
   Build on our SDL preference routing and Renegade’s file/directory interfaces. Replace native handles, `_open`, `FindFirstFile`, and remaining direct file operations. Preserve seek, timestamp, overwrite, and error semantics. Handle case-insensitive asset lookup on case-sensitive disks without renaming retail assets.
   **Completion:** both games can consume read-only retail data, with writable data in SDL preferences. Migration remains exclusively offline.

5. **Replace GDI fonts and finish text input.**
   Adapt Renegade’s SDL_ttf/FreeType backend and font-metric handling. Extend its interface for Generals’ required styles, widths, spacing, and glyph formats. Use the original font files and preserve layout metrics. Complete SDL text input, composition, keyboard-layout handling, and clipboard integration. Keep legacy UTF-16 formats explicit at text, save, and network boundaries.
   **Completion:** menus and text entry have one implementation across platforms, with preserved layout and string-format semantics.

6. **Port networking and remaining application integration.**
   Adapt the reusable socket interfaces for Generals’ LAN transport, preserving packet bytes, nonblocking behavior, timeouts, and error handling. Introduce the native application entry point and replace remaining Windows audio initialization calls. Resolve embedded-browser callers before removing COM dependencies or choosing a replacement.
   **Completion:** the complete executable links without accidental Win32 dependencies.

7. **Finish the shared renderer for Vulkan and Metal.**
   Complete Linux Vulkan integration using the existing SPIR-V path. Establish a pinned shader toolchain for macOS, with SDL_shadercross as the proposed translation layer. Cover both embedded shaders and runtime-generated legacy vertex/pixel shaders. Preserve resource bindings, matrix layout, texture formats, blending, depth, and render-target behavior.
   **Completion:** the same renderer implementation supports Direct3D12, Vulkan, and Metal, including terrain, water, boats, effects, and video.

8. **Finish architecture and binary-format compatibility.**
   Replace remaining compiler-specific assembly and x86-only helpers with implementations that preserve their semantics. Audit LP64 assumptions, serialized structures, UTF-16 data, floating-point state, and iteration order where simulation depends on it. Keep changes confined to the affected operations and format boundaries.
   **Completion:** Windows x86/x64, Linux x64, and macOS arm64 build with compatible save, replay, and network formats.

9. **Finish native distribution.**
   Package runtime libraries, shaders, icons, fonts, and executable resources appropriately; configure Linux runtime library paths and macOS bundles. Keep retail-data discovery separate from writable preferences. Final macOS completion requires an actual Mac for native builds and Metal execution.

**The next implementation batch should be portable CMake plus runtime/file services.** Fonts/text follow, then a complete Linux executable, then Metal and Apple Silicon. This gives us reusable infrastructure before tackling platform-specific rendering.

Each batch should produce a bounded, reviewable change. Runtime testing stays stopped as requested; no validation scripts, runtime migration, gameplay changes, or assert suppression belong in this work.

## Compiled dependency inventory

The initial Windows x64 CMake inventory contained 1,843 game translation units across Generals and Zero Hour, excluding vendor sources and tools. The inventory uses generated project compile items, rather than the legacy project files. Shared service changes must apply to both game trees.

| Compiled dependency | Replacement or boundary | Pinned Renegade reference / adaptation |
| --- | --- | --- |
| D3D8/D3DX, WIC, Windows SDK graphics declarations | Existing common SDL GPU shim, SDL_image decoder and portable graphics declarations; complete Metal shader translation | `890b02d:Platform/SDL/GPU`; our portable graphics declarations and legacy runtime shader compiler remain required |
| Thread handles, CRT threads, critical sections, mutexes | Shared SDL threads and recursive synchronization; native boundary only for genuine interprocess synchronization | `890b02d:Platform/SDL/Thread.cpp`, `Platform/Synchronization.h`; retain Generals worker interface |
| Multimedia/performance timers, sleep and CPU/FP helpers | SDL clocks/delays and portable CPU/rounding interfaces; preserve 32-bit clock rollover | `890b02d:Platform/SDL/Runtime.cpp`, `Platform/Processor.h`; Renegade's POSIX processor implementation is still x86-specific |
| File handles, CRT descriptors, directory enumeration, DOS timestamps | Common file/directory services with preference routing and case-insensitive asset resolution | `890b02d:Platform/Files.h`, `Platform/SDL/Directory.cpp`; replace Linux-only rename operation with cross-platform no-overwrite semantics |
| Registry settings and writable installation paths | Common preference-folder INI settings and explicit read/write routing already pending review | `890b02d:Platform/SDL/Paths.cpp`, `Platform/SDL/UISettings.cpp`; migration stays offline |
| GDI fonts, clipboard, IMM/text conversion | SDL_ttf/FreeType metrics, SDL clipboard and text/composition events | `890b02d:Platform/SDL/Fonts.cpp`, `FontMetrics.cpp`, `IME.cpp`; extend for italic/fixed-width styles and explicit UTF-16 |
| Winsock LAN/transport and GameSpy socket wrappers | Shared socket interface with native socket/error boundary, unchanged wire formats | `890b02d:Platform/Network/Sockets.h`, `Transport.cpp`; adapt to Generals transport |
| Native entry, dialogs, URL launch and audio driver initialization | Common SDL application integration and existing shared audio backend; native crash-reporting boundary | `890b02d:Platform/SDL/Runtime.cpp`, `Debug.cpp`, `Platform/Executable.cpp` |
| ATL/COM embedded browser and generated BrowserDispatch | Active W3D browser/window callers require a shared embedded-browser replacement before removing COM; ordinary URL launches can use SDL | No equivalent committed Renegade implementation; do not remove MIDL while these callers remain |
| Zero Hour shader text generation | Native ShaderText build tool plus the configured C compiler's preprocessor; same DWORD-array interface | Generals-specific: replaces binary-only `shdpp.exe`; original shader sources remain authoritative |
| MSVC forced includes, assembly, resources and linker flags | Compiler-specific CMake settings; native resource packaging and explicit architecture/format boundaries | `890b02d:CMake/Targets.cmake`; native presets must not imply a completed native executable |

Generals and Zero Hour x64 Release baseline builds both succeeded. All 14 native-generated WWShade headers were checked against their complete preprocessed shader text, including terminators and padding. Generals and Zero Hour x64 Release shared-clock builds both succeeded (exit code 0). Runtime testing remains stopped; x86/Debug and native Linux/macOS build checks remain pending for this batch.
### Current implementation batch

- MSVC flags, runtime checks and forced includes are compiler-specific; Windows resource/MIDL generation and system libraries are selected only for Windows. The common D3D8 renderer sources now have their own explicit source list.
- Linux Clang x64 Debug/Release presets have been added for both games. Windows path/name unification and macOS presets remain pending. No native executable has been claimed working.
- Zero Hour's shader headers now use a native ShaderText tool and the configured C preprocessor. The old binary tool's generated text contained truncated shaders and unrelated instructions; the replacement embeds the complete original source after preprocessing.
- Both WWLib thread implementations have moved into one SDL implementation, with Windows SEH/Zero Hour thread registration isolated in a native file. Shutdown follows the pinned Renegade signal-and-join lifecycle, reporting a timeout and joining rather than forcibly terminating the worker. Thread flags/priorities are atomic implementation state; gameplay types remain unchanged. Invalid legacy priority values retain their original ineffective behavior.
- Recursive critical sections use SDL on every platform. Unnamed timed mutexes share the standard recursive timed-mutex implementation. Windows named interprocess mutex behavior is preserved behind a native boundary; no compiled game callers use names. Native named-mutex support is not claimed for Linux/macOS.
- Both engine `Common/CriticalSection.h` wrappers now use the same SDL recursive critical sections as WWLib. Scoped locking and performance-timer instrumentation are unchanged. Both Windows x64 Release builds passed this change (exit code 0).
- Both game lifecycles now use an SDL `main` in `Main/Application.cpp`, selected explicitly on every platform. SDL supplies arguments; the original 20-entry argument limit, null game `argv[0]`, and early `-win` selection remain. Windows SEH translation, CRT heap tracking, `-DX` stack diagnostics and native IME/cursor callbacks moved to `Main/WindowsApplication.cpp`. The engine factory moved into its device implementation. Title-specific close behavior, localized Zero Hour debug splash selection and memory/version shutdown order remain. Mouse cursors, IME, the device factory and their other platform dependencies still need their planned shared replacements.
- Single-instance handling is behind `Platform/Application.h`: the original Windows mutex/window activation is retained, and Linux/macOS use a POSIX lock in SDL preferences. Both titles retain the shared Generals identifier and use the same preference lock location. The native lock is released at application exit. Splash loading uses the common case-aware read-path resolver.
- All eight Unicode Xfer save/load/CRC boundaries now encode explicit UTF-16LE bytes and count UTF-16 code units without changing `WideChar` or gameplay types. The existing one-byte save length, 255-unit guard/assert/exception and DeepCRC signed-byte payload behavior remain. Focused fixture and exhaustive code-unit round-trip checks passed with two-byte Windows `wchar_t` and four-byte Linux `wchar_t`; Linux AddressSanitizer/UndefinedBehaviorSanitizer reported no errors. Remaining text/network/LP64 formats still require the wider audit.
- All 450 direct legacy millisecond-clock calls in 97 compiled game sources now use the shared SDL clock; both WWLib system-timer headers use it too. SDL event timestamps share the same origin, so the WinMM input offset is removed. The new clock and system-timer rollover remain explicitly 32-bit on LP64 hosts, with existing timer interfaces and game state types preserved. SDL manages the original one-millisecond timer resolution. Performance-counter/CPU helpers remain pending.
- Common path resolution now follows Renegade's Linux checkpoint: exact paths first, cached case-insensitive directory indexes refreshed when directories change, and rejection of ambiguous case matches. Both file factories recognize POSIX rooted paths and strip either slash style. Preference routing, read-only retail use and offline-only migration remain unchanged.
- Raw-file portability must retain Zero Hour's compiled creation-time consumer (`verchk.cpp`) and its opaque native-handle contract. Read/write sharing, DOS timestamps, error codes and atomic no-overwrite rename remain required; the POSIX backend cannot inherit Renegade's Linux-only rename operation unchanged.
- Generals and Zero Hour x64 Release shared-clock builds both passed (exit code 0). macOS cannot select the Linux DXC archive; its native shader toolchain remains pending. Runtime testing remains stopped. The user completed the Ubuntu reinstall and authorized WSL. Ubuntu 26.04.1, CMake 4.2.3, Clang 21.1.8 and Ninja 1.13.2 are available; no tooling fallback is authorized.

## Renegade Linux checkpoint adoption

The current reference is `890b02dd35bb518aee13bff5c7229c2bfa4f5c35`, tag `linux-gameplay-checkpoint`. Read committed files with `git show`; never take the reference from its dirty worktree.

Its commit records a successful native Linux Clang Release build and confirmed WSL gameplay graphics/audio. Local input, LAN interoperability, Linux Debug and native hardware validation are still pending in that reference. Its POSIX processor implementation is x86-specific, and its Linux media/network/system services still require separate macOS boundaries.

| Reference change | Generals / Zero Hour adoption |
| --- | --- |
| Rooted POSIX paths and slash-aware file names | Applied to both WWLib file factories through common path helpers; Generals and Zero Hour Windows x64 Release builds passed (exit code 0) |
| Cached case-insensitive asset lookup | Applied to the common SDL path service on every platform; preserves exact matches and rejects ambiguous case collisions |
| Explicit static-library dependencies | Audit actual Generals library references before adapting the dependency graph; required for ELF link ordering and cycles |
| POSIX raw-file modes and directory rejection | Adapt the native file boundaries while retaining creation-time, sharing, DOS-date, error and no-overwrite semantics; include macOS rename support |
| UTF-16 chunk serialization | Explicit UTF-16LE conversion is applied to the base Xfer and save/load/DeepCRC overrides in both titles, retaining their length/guard/CRC behavior. Host-width and exhaustive byte round-trip checks pass; the Renegade WWLib chunk macros have no Generals C++ call sites |
| Shared SDL application, desktop and process services | Shared-platform SDL startup and native diagnostics/instance boundaries are extracted for both titles. Device, font/IME and browser dependencies remain incomplete |
| Portable network transport | Adapt socket operations to Generals packet and LAN interfaces; retain packet layout and error/nonblocking behavior |
| Renegade browser change | Uses an external browser via SDL; does not replace Generals' embedded D3D/COM browser and dispatch API |
| Buffering and overlapping string operations | Review affected Generals callers and establish the same root cause before taking these changes; do not copy unrelated fixes |

The user confirmed Ubuntu is ready and authorized WSL again. Verified Ubuntu 26.04.1 (x86_64), CMake 4.2.3, Clang 21.1.8, Ninja 1.13.2 and pkg-config 2.5.1. Native Generals Linux configuration/build work has resumed; Windows build checks continue. No tooling fallback, runtime migration or runtime game testing is authorized.

### First native configure result

Generals Linux Clang x64 Release completed vendor configuration with native SDL Vulkan GPU, X11/Wayland video, ALSA/PulseAudio, and the same WIC-free SDL_image backend. CMake generation stops at "No SOURCES given to target: generals": the executable source list is populated only by the Windows entry/resource list. The next source batch must extract the existing startup lifecycle into a shared SDL application entry point used on Windows too, retaining title-specific close events, argument handling, splash loading, memory/debug initialization and single-instance behavior. Windows SEH/CRT diagnostics belong behind a native boundary; no dummy main, feature stubs or core-only build option is acceptable.

The startup batch resolves the empty executable source list with the real game lifecycle. Both engine critical-section headers use SDL; unused editor Windows message declarations are confined to Windows in the mouse header, while focus and SDL input interfaces remain available. Native cursor/IME implementations and other legacy header dependencies remain pending.

### Native configuration after startup extraction

Generals Linux Clang x64 Release configuration and generation now pass (exit code 0). SDL_image's vendor setting now uses the actual `BUILD_SHARED_LIBS` input, selecting its static target consistently. DirectX-Headers exports use Renegade's `DXHEADERS_INSTALL ON` / CMP0077 setup so the DirectXMath/DirectXTex dependency exports are valid. These are vendor build settings, not new game feature options.

The first full native Generals build stopped in OpenAL Soft 1.25.2: Clang 21's function-effects checker rejects `std::variant::emplace` from `PrepareResampler` with Ubuntu's libstdc++ 15. No checker, warning or annotation has been disabled, and the vendor pin is unchanged. Renegade's pinned OpenAL revision was acquired and compared; it limits this checker to libc++, but its resampler calls are unchanged. The owner installed `libc++-21-dev` and `libc++abi-21-dev`; both are verified as version `1:21.1.8-6ubuntu1`. Linux presets explicitly select `-stdlib=libc++` with Clang 21. A fresh configuration passed and detected libc++; the complete OpenAL static-library build then passed (exit code 0), including the previously failing `alc/alu.cpp`. Its compile command retains both `-Wfunction-effects` and `-Werror=function-effects`. Installing the matching system library resolved this blocker without vendor changes or diagnostic suppression.

Generals and Zero Hour Windows x64 Release startup/UTF-16 rebuilds both passed (exit code 0), including the new SDL entry and native Windows bindings. Successful Linux CMake generation does not establish a working native executable. Zero Hour native configuration/build and macOS validation remain pending. Runtime testing stays stopped.

### Full native build after the owner-installed libc++ packages

The resumed Generals Linux Clang x64 Release build passed SDL3, the common `gen_platform` static library (paths/settings, window/input, clock, synchronization and POSIX application instance handling), LibBinkDec and DirectX-Headers. OpenAL remains built successfully with its original diagnostics enabled.

The complete game build then stopped on two concrete remaining dependencies:

- DirectXMath includes `sal.h`, which is not supplied by our current dependency setup. Renegade supplies Microsoft's pinned public SAL header as a DirectXMath include dependency. Resolve this as a proper SDK dependency; do not create local empty annotation macros.
- Both engines' shared `PreRTS.h` still imports Windows ATL/COM and other native headers; Generals first fails on `atlbase.h` at line 40. This requires the planned platform separation and embedded-browser work. Installing Linux packages cannot replace that Windows application code.

The native executable is still incomplete. The next code batch addresses these SDK/header boundaries without vendor compatibility patches, diagnostic suppression, runtime migration or game testing.

### SDK and common-header separation (2026-10-07)

- Microsoft SAL is now a hash-pinned public SDK header dependency, matching Renegade's committed setup. No substitute annotation macros or vendor compatibility patch was introduced. DirectXTex's complete Linux static-library build passed.
- Both `PreRTS.h` headers select their existing Windows SDK/ATL imports only on Windows. Windows import ordering remains unchanged; Linux's common Generals precompiled header now builds successfully. Clang's Microsoft-extension mode enables existing language declarations such as `__int64` without substituting gameplay types.
- String comparisons use an explicit native CRT interface, preserving the existing locale-based comparison behavior. SDL's Unicode folding was inspected and deliberately not substituted for the original CRT operation. The standard C++ allocation header supplies standard/placement operators while retaining the game's file/line overload declarations and existing allocation macros.
- Enum definitions needed before use are being split into lightweight headers. All 30 moved enum definitions across both titles were compared with baseline `c1b857f` and are unchanged, including Zero Hour's extra AI debug value. Original name-table macros remain in their original owning headers. The new headers are listed explicitly in both CMake source lists. Two missing dependent-type `typename` keywords were added to each sparse-match template; its matching algorithm is unchanged.
- The repository now contains checkpoint `6411a9c` (`linux`); remaining changes continue on top of it. Both Windows x64 Release header rebuilds are running. The native engine build has advanced into ordinary module sources and still reports incomplete enum dependencies in Object/Team/Player/UpdateModule/View and related headers. Their definitions must be available before use without changing enum values, declaring replacement types, suppressing compiler errors or removing game macros/asserts.

This remains a partial source port. Zero Hour's native full build, Windows checks for this header batch and the remaining nine-phase gates are not complete. Runtime testing remains stopped.
