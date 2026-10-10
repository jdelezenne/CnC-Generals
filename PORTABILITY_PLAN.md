# Generals and Zero Hour Linux and macOS support plan

This document records the implementation plan for native Linux and macOS support for both games. The plan below preserves the information presented in the conversation; the checklist tracks completion of its nine phases.

## Progress

Current status (2026-10-10): both native Linux x64 Release targets build successfully on a case-sensitive filesystem. Both games load the retail data, render their live menu scenes with readable fonts, and play menu music. In-game rendering, sound and the control bar are working; the owner has verified that everything looks good. Linux installation packages contain the executable and adjacent libdxcompiler.so and resolve their dependencies independently of the build tree. README.md documents the build, retail data setup, offline font import and installation commands.

The shared implementation retains the existing Windows x64 color conversion, CRC and cleanup behavior with compiler defaults. The Linux memory report now uses the same signed-range limit as Windows, allowing the existing game LOD logic to enable the menu scene and music. Temporary probes, debugger scripts, captures and validation logs have been removed. No additional runtime testing is being requested. Windows builds remain paused at the owner's request.

Remaining work is distribution and regression coverage beyond the verified Linux runtime: clean-checkout packaging, save/load and replay compatibility, and LAN interoperability. AVI capture and the named synchronization service have not received final runtime verification. macOS remains incomplete, including TCP-table discovery and shader compiler support. The nine-phase checklist covers both Linux and macOS and therefore remains open.

System dependencies are installed and upgraded by the repository owner: `apt` on Linux and `brew` on macOS. Agents inspect package availability and provide exact commands; they do not install or upgrade packages. Ask the owner before using a workaround, vendor compatibility patch or diagnostic suppression in place of the required dependency setup. This rule is also recorded in `AGENTS.md`.

Recorded on 2026-10-06. Both Windows x64 Release baseline builds pass with the pending image/preference changes and shared thread/synchronization services. Compiler/platform separation, native shader generation and case-sensitive asset lookup are implemented; Generals and Zero Hour x64 Release builds both pass with the shared clock. The nine phases remain incomplete. The dormant embedded-browser stack has since been removed with owner authorization; no replacement vendor is needed. Runtime testing remains stopped, and the user has authorized resuming WSL after completing the Ubuntu reinstall.

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

Each batch should produce a bounded, reviewable change. The owner's 2026-10-10 request to complete the engine/device so the games can run authorizes native startup checks for this batch, superseding the earlier runtime-testing pause recorded below. No new validation suite, runtime migration, gameplay changes, or assert suppression belong in this work.

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
| ATL/COM embedded browser and generated BrowserDispatch | Removed from both games, including dormant WOL callers, lower renderer hooks, URL parser/factory, ATL module setup, COM dispatch generation and BrowserEngine.dll. Existing localized TOS text handling is retained | Owner-authorized cleanup of an uninitialized subsystem; no new browser dependency |
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
- Enum definitions needed before use are split into lightweight headers. All 83 moved enum definitions across both titles were compared with baseline `c1b857f` and are unchanged, including conditional entries, animation/KindOf/armor/module types, Zero Hour's academy classification and extra AI debug value. Original name-table macros remain in their original owning headers. The new headers are listed explicitly in both CMake source lists. Two missing dependent-type `typename` keywords were added to each sparse-match template; its matching algorithm is unchanged.
- Work continues on top of checkpoints `6411a9c` and `6b1e68f` (`linux`). The earlier full Windows header rebuilds were interrupted and have no successful final result. Subsequent scoped MSVC x64 Release compilation passed for both titles' ModuleFactory, GameState, Recorder and Debug sources, and both complete `gen_platform` libraries passed. These scoped results do not establish complete game builds for this batch. The native Generals ModuleFactory now compiles without errors.

This remains a partial source port. Zero Hour's native full build, Windows checks for this header batch and the remaining nine-phase gates are not complete. Runtime testing remains stopped.

### Save-directory, time and diagnostic boundaries (2026-10-07)

- Both titles enumerate save files and scratch maps through shared SDL directory services. Callbacks retain leaf names; save metadata loads now explicitly use the save-directory path. Enumeration and removal no longer change the process working directory. Retail data stays read-only, and writable paths remain in SDL preferences; no runtime migration was added.
- Shared local calendar time comes from SDL. Native locale formatting preserves Windows short-date and time-without-seconds behavior; POSIX formatting uses the installed locale and SDL's time-format preference. Save field order remains unchanged. Replay timestamps explicitly encode the original 16-byte little-endian SYSTEMTIME layout, including weekday before day. Windows comparison against actual SYSTEMTIME bytes and Linux ASan/UBSan round-trip fixtures passed.
- All five replay text writes and their matching reader now use explicit zero-terminated UTF-16LE. The original 1024-unit read-buffer limit and 1023-unit visible-text limit remain; malformed input cannot write beyond that buffer. Fixtures match both original Windows wide-print formats, including non-ASCII text, surrogate pairs and isolated surrogates. Four-byte Linux wchar_t fixtures also pass ASan/UBSan. Replay `time_t` fields and other network/text formats still require the architecture/serialization audit.
- Decoded game strings retain one character slot per original UTF-16 unit on both two- and four-byte hosts. Surrogate pairs keep their original two-slot lengths and indexes. All 65,536 units round-trip unchanged, and paired/isolated-surrogate length fixtures pass on Windows and under Linux ASan/UBSan. Unicode token copying uses the actual host character-storage width, preserving its existing character count and algorithm. Quoted-printable Unicode conversion likewise operates on the original UTF-16LE bytes; its existing escape algorithm is unchanged.
- Wide formatting translates Windows `%s`/`%S`, character and length-modifier conventions for the POSIX CRT; Windows keeps its native formatter. Ordered UTF-16 output fixtures match for strings, characters, width/precision, signed/unsigned 64-bit values and exact-capacity/overflow results. The exact reserved-capacity boundary returns the original positive count rather than introducing a new error. Linux ASan/UBSan checks pass.
- Both titles' diagnostic dialogs, window hiding and profiler clocks use shared SDL services on Windows and native Unix platforms. Assertion conditions, fullscreen/background-thread ignore rules, Abort/Retry/Ignore actions and title-specific default buttons remain. Localized dialogs convert explicit UTF-16 to UTF-8. Windows debugger output and Windows SEH/context stack reporting remain native boundaries.
- POSIX engine stack capture uses real `backtrace`/`backtrace_symbols`, following Renegade's committed implementation, with native image/symbol lookup through `dladdr`. This does not yet add POSIX signal-context reporting or source-line resolution.
- Clang requires an explicit specialization marker for the disabled-mask name table and explicit conversions of callback pointers into the existing function dictionary's `void*` field. These changes retain the original types, table entries, order and callbacks. No diagnostic was suppressed.
- The memory pool's native system allocation/size/free operations are isolated behind `Platform/Memory.h`. Windows retains GlobalAlloc/GlobalSize; POSIX uses its native heap. Zeroing, filler patterns, allocation failure handling and pool algorithms remain. The existing 16-byte x64 pool alignment now applies to every 64-bit platform. Windows and Linux ASan/UBSan fixtures pass alignment, zeroing, reported-size and filler-write checks. Required `sizeof(type)` syntax and loop-variable scope corrections preserve existing values; QuickTrig only loses the removed C++ `register` keyword.
- LocalFile keeps its descriptor, access flags and parsing interface. Native descriptor services preserve Windows CRT behavior; POSIX translates CRLF, Ctrl-Z, text writes and read/write append handling. The signed 32-bit seek limit includes restoring the prior position on overflow. Ordered fixture output matches the Windows CRT across binary/text reads, boundary-sized reads, writes, append and seek, with Linux ASan/UBSan checks passing. Binary file operations do not hold the text registry lock during I/O.
- Executable-relative MemoryPools.ini discovery uses SDL's base path, preserving its original location and compiled defaults when the file is absent. It does not change cwd or migrate files. Native SDK selectors for the existing offline GameSpy header imports are scoped to those imports and restored afterward; no `_UNIX` definition is added to engine compiler flags, and vendor files are unchanged.
- Zero Hour's AsciiString retains both unsigned-short header fields and its original assertions. Its reference-count operations use typed sequentially consistent atomics through the platform library instead of a Windows `long*` cast; the declared counter is not widened. Valid-range comparisons against actual Windows Interlocked operations preserve the neighboring allocation field. Eight-thread stress and Linux ASan/UBSan checks pass. Other existing shared-state concurrency still needs the runtime-service audit.
- CRC network-byte-order conversion uses SDL3's endian operation behind a common interface, with its 32-bit inputs and CRC algorithm retained. Unused Windows-only imports have been removed from RAM/streaming file sources and common string/CRC headers.
- The rider's original `ObjectStatusType` has no enum definition anywhere in Zero Hour. It remains a distinct opaque enum, with its original MSVC `int` representation stated explicitly for standard C++. Its field, name, parsed status indexes and consumers are unchanged; no alias to the differently named ObjectStatusTypes enum or invented enumerators was added. A direct MSVC comparison verifies equal size, alignment and enclosing-record layout. INI parser scope corrections retain the original terminal-entry callback selection; CRT case comparisons use the same native comparison interface.

Generals' Linux platform library, save metadata/map sources, Recorder and POSIX engine stack source have compiled successfully. Both titles' scoped MSVC x64 Release checks also pass the affected memory, file, recorder, diagnostic and module-header sources, and their complete platform libraries pass. These results do not establish full game builds or runtime behavior for this batch.

Zero Hour Linux Clang x64 Release configuration and generation passed. Both titles' complete Linux platform libraries and scoped native module-header/save/map/recorder/stack-reporting compilation now pass. CRC byte-order fixtures compare 100,000 values against native htonl on Windows and Linux, with Linux ASan/UBSan passing. The latest affected Windows x64 Release source checks also passed for both titles, including INI/water parsing, module headers, save metadata and Recorder; their complete platform libraries passed. Complete engine/executable builds for this batch remain pending. Building from the Windows-mounted source tree also exposes include-path casing warnings; a genuinely case-sensitive filesystem remains a required portability gate.

### Next native engine boundary

The complete Generals Linux engine build has advanced through common system and most INI sources. Its current platform blocker is `INIWebpageURL.cpp` importing `GameNetwork/WOLBrowser/WebBrowser.h`, whose base class, public global and callbacks depend on ATL, generated COM dispatch and Windows BrowserEngine.dll.

The subsequent caller audit corrects the earlier replacement requirement: the high-level browser was intended for the old Westwood Online TermsOfService and MessageBoard screens. `TheWebBrowser` is initialized to null and never assigned a live instance; its creation is commented out in Generals `GameEngine.cpp:378` and Zero Hour `GameEngine.cpp:521`, including baseline `c1b857f`. The login screen explicitly sets `useWebBrowserForTOS = FALSE` and uses its existing text-list path. The ladder/message-board calls check the null browser. The lower DX8 wrapper still attempts browser-engine initialization/update/shutdown, but no compiled game caller creates a page independently of the dormant high-level browser.

A new embedded-browser vendor is therefore not justified by current game behavior.

### Dormant browser removal (2026-10-08)

The owner authorized deleting this legacy stack. Both titles' high-level/W3D browser code, FEB dispatch template, URL parser and memory-pool entry, factories and guarded WOL menu callers have been removed. The existing localized TOS text path is now unconditional; its statements were compared against the original path and are unchanged apart from indentation and its explanatory comment. The lower DX8 initialization/update/render/shutdown hooks and the browser-only ATL module lifetime calls were removed too.

CMake no longer lists browser sources, requires MIDL, generates BrowserDispatch code/type libraries, builds or links eabrowserdispatch, imports BrowserEngine.dll or embeds its type library in executable resources. The vendor DLL and 136 obsolete generated browser artifacts were removed. Historical project files no longer refer to the deleted files/target. External default-browser URL launching is a separate facility and remains unchanged.

Generated compatibility/version headers now have explicit include paths on their actual engine/device/executable consumers; they previously inherited that path from the browser-dispatch target. Zero Hour's existing DamageTypeFlags typedef is in a lightweight owning header required directly by WeaponSet.h; its type and member are unchanged. No game algorithm or assertion was changed to make these checks pass.

Both complete Windows x64 Release builds passed (exit code 0), producing Generals.exe and GeneralsZH.exe. Zero Hour's first final link attempt exhausted memory inside MSBuild's diagnostic parser. An unchanged incremental retry reported success while its executable was absent; explicitly rebuilding the executable target then passed with zero errors and restored the verified output. Source, generated project and executable-resource audits show no embedded-browser dependency. Runtime testing remains stopped.

Both native Linux builds passed the former ATL/browser blocker and stopped on other portability work: Generals reports missing enum definitions in ActionManager.h and MSVC __max calls in Player.cpp; Zero Hour first reported DamageTypeFlags visibility and Module.cpp's CRT string comparison. The damage-flags header dependency is corrected and covered by the successful Windows rebuild; a complete native game build is still pending. Fonts/IME, native file factories, networking, CPU helpers, remaining serialized formats, Metal and distribution gates remain open.

Complete Linux executable linking, Zero Hour native checks, full Windows builds for this batch, macOS configuration/toolchain work and the remaining runtime/file/font/network services are still pending. Runtime game testing remains stopped.

### Complete Linux port in progress (2026-10-08)

The owner resumed native Linux runtime work and requested completion of the port. Both complete Linux Clang x64 Release builds were run with Ninja's keep-going mode to collect independent failures. Neither game executable links yet; there has been no Linux runtime test. Most initial failures repeated through a small set of template and platform headers.

- Dependent template lookup is explicit, with inherited public/protected access retained. Missing specialization markers, implicit-int declarations and loop-variable scopes are corrected without changing their algorithms. Additional enum definitions and existing science/upgrade aliases live in lightweight owning headers; the new moved definitions still need the full baseline comparison.
- Shared SDL cursor loading uses SDL_image's animated-cursor decoder, including hotspots and frame delays. Local file enumeration and metadata now have shared SDL implementations. Both platforms use the same device source list, with native Windows startup/resources and stack reporting selected separately. The native device files retain their historical Win32 names for now.
- RawFile's system open/read/write/seek/date operations use native platform services. Its access modes, error/retry flow, bias logic and DOS timestamp interface remain. File-format validation and full Windows regression checks for this batch remain open.
- CPU instruction queries, memory/OS information, executable paths, diagnostic output and clocks have explicit native services. Linux secure seeding uses the committed Renegade getrandom implementation; the SHA mixing/output algorithm remains. Windows retains the existing seed inputs. macOS's added native code has not been compiled.
- The remaining renderer settings wrapper uses Renegade's committed common INI implementation. Both platforms store this in SDL preferences. Its full compilation and persistence checks remain open.
- The shared font backend uses SDL_ttf/FreeType and the owner's installed system FreeType/HarfBuzz on Linux. The owner approved reproducing the measured Windows mapping of absent Arial Unicode MS to Arial. Forty-eight metric rows were compared with GDI: ordinary Arial and that mapping match across the tested sizes/styles. The custom Generals width now uses an exact FreeType size request before glyph rasterization, with light hinting, rather than scaling the finished bitmap. The complete corrected backend matches all 48 tested GDI metric rows, including 288 glyph advances across both styles and eight point sizes. Pixel placement/coverage and complete game runtime checks remain open. Font discovery uses explicitly registered local fonts, preference Fonts and system font directories, rather than recursively scanning retail data.
- Existing owner Arial files and installation Settings.ini were copied offline into Linux SDL preferences. Installation paths were adapted to the native retail paths. No runtime migration or retail-data writes were added. The owner continues to install/upgrade system dependencies with apt or brew; agents do not do so.
- Socket wrappers preserve the existing game descriptors and packet algorithms while adapting native socket-length parameters and error handling. Native empty nonblocking receives now follow the existing Windows zero-result behavior. The remaining LAN/interface, FTP/thread and networking services still need completion and validation.

Uncompiled library helpers with no game callers were removed from the common source lists, including dormant DirectDraw conversion/surfaces, old modeless-dialog/message-loop helpers, COM dispatch utilities and the embedded-resource file class. Native Windows PE/loaded-image helpers remain selected only for Windows. The common file-identification consumer still needs its portable PE metadata boundary.

AVI capture is separate from Bink playback. The existing capture path writes uncompressed AVI through VfW; it does not invoke the commented-out compression path. A shared replacement is still needed. FFmpeg development packages were inspected and are absent; no dependency was installed or requested on the assumption that an encoder is necessary.

Required remaining gates: complete both executable builds; preserve serialized widths/UTF-16 units and file formats; compare the new enum/header moves against the baseline; validate font/cursor/settings/file/socket boundaries; build from a genuinely case-sensitive Linux source tree; complete Windows regression builds; run both Linux games through intro, full 3D menu and a match with working input/audio. No successful full-build or runtime result is claimed for this batch.
Current checks: the complete gen_platform library linked in ZeroHour-Linux-Shared-Third; WWDebug and Generals WWSaveLoad also linked in the recent scoped passes. Complete engine builds still fail on remaining source portability and standard C++ scope issues. The complete corrected font API matches 48 GDI metric rows (288 advances), including the custom-width transform. These boundary results do not establish game runtime behavior. SDL text/composition events and the remaining IMM manager/native window hooks still require replacement on both platforms.

## Engine/device and executable milestone (2026-10-10)

Both Linux Clang x64 Release executable targets have linked successfully. The actual game targets, rather than a separate validation suite, compiled the completed engine/device sources. This supersedes the compile/link blockers in the preceding historical batches.

- Legacy loop-variable scope and missing/implicit declarations now compile as standard C++. Template allocator and big-integer remainder specializations have explicit storage definitions.
- Display, screenshot writing, clocks, window/input integration, directory enumeration, desktop copies and CRT text helpers use shared platform services. Water-track temporary vectors bind to const references.
- LAN identity, DNS worker lifecycle, result-upload sockets and IPv4 ping use native boundaries while retaining game packet and request algorithms. The dormant unreferenced WWAudio sound-render-object translation unit is excluded from the game source inventories; game audio continues through the existing audio backend.
- Zero Hour diagnostics/profiling retain their checks and output, with native stack capture, console/socket/file output, memory probes, timing and synchronization. Linux command-file lookup appends the required extension to extensionless executable names.
- CMake records WW library dependencies and the static engine/device cycle for ELF archive resolution. SDL headers are exported with the platform window/input interface.
- Generals' first startup initialized an X11 SDL window, then exited from the memory-manager linkage self-check. The self-check now calls allocation functions directly, preserving its counter diagnostic against compiler allocation elision. Application lock-pointer cleanup covers ordinary returns and explicit exit paths, and startup exceptions/window failures have visible logging.
- The next startup passed that self-check and reached engine initialization. Empty logical directory names now enumerate the current directory in the SDL file services, restoring discovery of retail BIG archives. Windows platform text services use the same legacy wchar_t ABI as the game libraries.

Both Windows x64 Release regression builds passed before the latest filename-copy change. Further Windows builds were stopped at the owner's request. Generals subsequently initialized the Vulkan renderer, but did not complete script initialization. No successful 3D menu or match is claimed. Debug/x86, a genuinely case-sensitive Linux source checkout, saves/replays/LAN compatibility and macOS builds remain separate outstanding gates.

## Engine/device change reassessment (2026-10-10)

The working Windows game is the behavioral baseline. A Linux failure establishes a port incompatibility; it does not by itself establish a defect in the game's intended Windows behavior. Earlier explanations did not make that distinction adequately. In particular, `MemoryPoolObject::deleteInstance()` deliberately checks `if (this)` before accessing the object. The isolated caller guard added to `ScriptConditions::reset()` has been withdrawn. The remaining null-call compatibility must be considered across the allocator interface, rather than handled by patching the first crashing caller. No compiler workaround or diagnostic suppression was applied during this audit.

This inventory covers the engine/device completion edits identifiable from this session's edit helpers, diffs and build/runtime logs. The workspace already contained substantial earlier portability changes. Those earlier changes have not been bulk reverted or attributed to this batch. "Retain" below means the change has a concrete compatibility justification; it does not mean its runtime behavior has been fully verified.

| Change | Reassessment and disposition |
| --- | --- |
| Legacy loop-variable scope | Retain declaration moves needed for uses after a loop. They reproduce the existing MSVC `/Zc:forScope-` scope. Loop initialization, conditions and increments must remain identical; unnecessary counterpart moves are not evidence of game defects. |
| Dependent `typename`, explicit types and declarations | Retain compiler compatibility edits. The intended types are supported by existing uses. `WrapHTTP` returns no value and its callers ignore a result. No algorithm change is justified by implicit-int diagnostics alone. |
| `FALSE` versus null in pointer expressions | Retain changes only where the declared value is a pointer. Three accidental `FALSE` to `NULL` replacements in Boolean GUI parser functions were reverted in each title. |
| Const pointers for CRT search results/literals | Retain where the characters are read only. These satisfy standard overloads without changing the data. |
| Water-track const vector references | Retain: the functions read the inputs, and existing callers pass temporary vectors. Both declarations and definitions were changed together. |
| Required owning headers | Retain additions such as `KindOfType` and `AudioAffect` visibility; these resolve incomplete declarations without changing enum values. Earlier enum/header moves require their own baseline comparison. |
| Title-specific matrix and device-description types | Retain each title's original type: Generals uses `Matrix4` and nullable character pointers; Zero Hour uses `Matrix4x4` and the matching `StringClass` constructor. A common replacement across both was inappropriate and was corrected. |
| Pool allocator specialization storage | Retain explicit definitions with initializers. ELF link failures showed missing storage; the change supplies the same default-constructed allocator expected on Windows. |
| Big-integer remainder storage | Retain explicit specialization definition/initialization; this resolves an ELF symbol-definition issue and preserves the existing arithmetic implementation. |
| CMake archive dependency graph | Retain explicit references and the engine/device static-library cycle. ELF archive resolution differs from MSVC's rescanning; completed Linux links provide evidence for this build-boundary change. |
| SDL usage requirements | Retain SDL's public include/link requirement because public platform headers expose SDL types. |
| Windows `wchar_t` ABI | Retain `/Zc:wchar_t-` on the platform C++ target to match the existing game-library ABI. Windows link failures and subsequent successful builds support this change. |
| Windows `std::min` parentheses | Retain the local protection against the Windows `min` macro. This changes parsing, not the selected minimum. |
| Dormant WWAudio `soundrobj.cpp` source removal | Retain the source-inventory correction: there are no active game callers; Zero Hour already disables this code without WWAudio. This is not a replacement of the game's active audio path. |
| Case comparison, lowercasing and string-copy boundaries | Retain the portable APIs where Windows delegates to its existing CRT operations. Locale and non-ASCII behavior remain runtime verification concerns. |
| Narrow/wide formatting boundaries | Retain the need for an adapter and the legacy count/format semantics. Do not treat successful compilation as proof of truncation or localization equivalence. |
| New generic diagnostic integer formatter | Revised: native-long buffers now fit the native width, HRESULT hexadecimal output explicitly uses 32 bits, and short values keep the legacy integer promotion. Both Linux builds pass; diagnostic output equivalence still needs runtime comparison. |
| Clocks and debug-monitor output | Retain native services; game timer algorithms are unchanged. Windows uses the matching native counter/output implementation. |
| Window/render handle | Retain: Windows still returns the native HWND; Linux returns the SDL window expected by the new renderer. Generals' X11 window and Vulkan initialization support the Linux boundary. |
| Minimized/windowed queries | Retain the portable boundary. Focus, fullscreen and device-reset behavior still need comparison with Windows. |
| Directory/path APIs | Retain portable enumeration and existence calls. User-image loading still needs wildcard/path verification against the Windows baseline. |
| Empty logical directory | Retain the mapping to `.` inside the SDL services. Passing an empty path prevented retail archive discovery; startup then advanced through retail INI loading. |
| Archive filename loop condition reorder | Withdrawn. This was incidental bounds hardening, not the cause of the observed INI lookup failure. The original game expression is restored in both titles. |
| Overlapping filename copy | Retain as a reproduced Linux CRT compatibility change under review. The exact `ObjectCreationList.ini` path was corrupted by overlapping `strcpy`; `memmove` preserved it and startup advanced through the previously failing lookup. This does not establish that the Windows game had been failing. |
| Allocator linkage self-check | Retain the direct allocation-function calls as a compiler compatibility change. The original optimized Linux check counted two calls instead of six; the diagnostic and failure condition remain intact. Windows previously passed its check. |
| Script transport-status caller guard | The isolated guard was withdrawn. The owner selected portable cleanup calls with compiler defaults. Both titles now use one shared pointer-checking helper before calling the original pooled cleanup method. The original guard is identical in all 19 game-source revisions in the 20-commit history including reflogs. Generals now passes the previously failing reset. |
| BMP serialization boundary | A native replacement is required, but new row padding/header output and screenshot rectangle selection change observable capture behavior. Keep these marked unverified; compare output with the Windows baseline before claiming equivalence. |
| Extra screenshot `UnlockRect` calls | Withdrawn. They were incidental cleanup edits added alongside the platform adaptation, without a demonstrated port requirement. |
| Desktop replay copies | Retain the need for a native desktop-folder boundary. Failure behavior and desktop availability remain unverified. |
| Replay error messages | Revised: the shared interface again supplies wide messages directly to UnicodeString. Windows uses FormatMessageW, and the POSIX boundary converts its native UTF-8 message to wide text. Both Linux builds pass; localized runtime output remains unverified. |
| LAN user/host identity | Needs revision/verification: `GetComputerName` was replaced by socket `gethostname`; the failure fallback and initialization requirements are not identical. Preserve Windows identity behavior in its native implementation. |
| Socket-length/error adapters | Retain native `socklen_t` conversion and error mapping. Packet layouts and request algorithms are unchanged; LAN/result-upload behavior is not runtime verified. |
| Socket startup in ping/results workers | Needs comparison: the old workers request Winsock 1.1; the shared Windows wrapper requests 2.2. Calling the same wrapper is not exact evidence of preserved Windows behavior. |
| Native IPv4 ping | Retain the need for a native implementation while keeping the existing Windows ICMP implementation. Linux ping-socket permissions/timeout behavior and macOS support remain unverified. |
| Failed DNS-resolution guards in ping/results | Incidental error-path hardening, not a demonstrated startup requirement. These changes need scope review; do not present them as fixes required by a previously broken Windows game. They have not been silently reverted with unrelated networking edits. |
| DNS completion synchronization | Atomics and joining an already completed worker have a portability rationale. The lifecycle must also preserve cancellation and subsequent requests. |
| DNS cancellation | Revised: one shared SDL worker lifecycle gives each request its own reference-counted context. Cancellation detaches the worker and releases the caller reference without waiting for resolution. A canceled worker cannot publish into a subsequent request. Both Linux builds pass; cancellation runtime stress remains unverified. |
| Generals named audio mutex | Retain the native synchronization boundary; Windows maps to its named recursive mutex. Timeout, cross-process identity and cleanup still require runtime verification. |
| Zero Hour diagnostic source selection | Retain native replacements for Windows-only stack/exception/console/pipe sources, rather than omitting diagnostics. Stack symbols, console commands and socket reconnection are not yet runtime validated. |
| Native debug command-file extension | Retain extension handling for Linux's extensionless executable. Windows `.exe` names still select the corresponding `.dbgcmd` file. Command registration order remains a separate issue. |
| Native diagnostic initialization order | Revised: the early instance constructor performs PreStaticInit, and the shared application entry point performs the late stage after static command registration. Native compiler ordering schedules the early instance. Zero Hour builds and reaches its engine draw loop; diagnostic command behavior remains unverified. |
| Diagnostic file output | Needs comparison: the raw-file replacement dropped the explicit write-through flag, and collision handling now tests path existence instead of the precise copy error. These are not proven equivalent. |
| Memory readability probes | Retain a real native diagnostic boundary. Windows now probes via `ReadProcessMemory`; this is not identical to the old `IsBadReadPtr` implementation and needs behavior review. |
| Profile lock implementation | Revised: one shared std::atomic exchange/store path replaces the mixed atomic-storage and Windows assembly accesses. Zero Hour builds; lock contention and Windows regression validation remain unverified. |
| Profile/debug reallocation | Needs comparison: shared allocate-copy-free replaces GlobalReAlloc. It preserves bytes but can change allocation identity. Verify the callers against the working Windows behavior before claiming equivalence. |
| Diagnostic `RepeatChar` parameter | Retain a const/value-compatible interface for existing temporary arguments; no output algorithm changed. |
| Application lock-pointer lifetime | Retain the native early-exit cleanup requirement supported by startup termination failures. Its all-platform placement and the order relative to static caches/allocator shutdown still need review. |
| Application failure logging/return codes | Withdrawn. Temporary startup logging and altered failure exit codes were removed; the original startup failure handling is restored in both titles. |
| Unused language-file result | Retain removal of the unused local only; the `doesFileExist` call remains, preserving NameKey generation. The earlier removal of the Win9x language branch is outside this batch's local-variable edit. |
| Windows file-version timestamp adapter | Needed to compile against the changed raw-file interface. Preserve the original last-write timestamp semantics despite the function's name. The new `Return_File` lifetime change is incidental and needs separate review. |

Audit actions: restored original script cleanup calls, Boolean parser returns, archive-loop condition order and screenshot release sequence in both titles. Other changes were assessed rather than bulk reverted. No new Windows build or game launch was performed during the reassessment. Existing pending changes remain available for review; Linux implementation has resumed with compiler defaults, shared cleanup, DNS and profile paths, and corrected diagnostic initialization. Both current Linux targets build and reach their engine draw loops. Windows builds remain paused at the owner's request.
