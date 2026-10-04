# Avoid the obsolete Windows platform headers bundled with the DX8 SDK.
set(directx_headers "${CMAKE_BINARY_DIR}/Generated/DirectX8")
file(MAKE_DIRECTORY "${directx_headers}")
file(GLOB dx8_headers "${GEN_DIRECTX_INCLUDE_DIR}/d3d8*.h" "${GEN_DIRECTX_INCLUDE_DIR}/dxfile.h"
    "${GEN_DIRECTX_INCLUDE_DIR}/d3dx8*.h" "${GEN_DIRECTX_INCLUDE_DIR}/d3dx8*.inl")
foreach(header IN LISTS dx8_headers)
    get_filename_component(filename "${header}" NAME)
    configure_file("${header}" "${directx_headers}/${filename}" COPYONLY)
endforeach()
set(GEN_DIRECTX_INCLUDE_DIR "${directx_headers}")
file(CONFIGURE OUTPUT "${directx_headers}/D3DXMath.h" CONTENT "#include <d3dx8math.h>\n" @ONLY)

function(gen_legacy_settings name)
    target_include_directories(${name} BEFORE PRIVATE
        "${PROJECT_SOURCE_DIR}/Vendors/STLport-4.5.3/stlport" "${stlport_support}")
    target_include_directories(${name} PRIVATE ${GEN_${name}_INCLUDES}
        "${GEN_DIRECTX_INCLUDE_DIR}" "${GEN_CODE_DIR}/Libraries/Include")
    if(GEN_MILES_INCLUDE_DIR)
        target_include_directories(${name} PRIVATE "${GEN_MILES_INCLUDE_DIR}")
    endif()
    target_include_directories(${name} PRIVATE "${PROJECT_SOURCE_DIR}/Platform/Bink")
    foreach(config DEBUG RELEASE)
        string(TOLOWER "${config}" config_lower)
        target_compile_definitions(${name} PRIVATE "$<$<CONFIG:${config_lower}>:${GEN_${name}_DEFINES_${config}}>")
        target_compile_options(${name} PRIVATE "$<$<CONFIG:${config_lower}>:${GEN_${name}_OPTIONS_${config}}>")
    endforeach()
    target_compile_definitions(${name} PRIVATE _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS
        WINVER=0x0A00 _WIN32_WINNT=0x0A00 NOMINMAX _USE_32BIT_TIME_T _STLP_NO_IOSTREAMS _CONST_RETURN=)
    # Preserve VC6 x87 code generation and avoid EBX stack alignment in legacy assembly.
    target_compile_options(${name} PRIVATE "$<$<COMPILE_LANGUAGE:C,CXX>:/W3;/MP4;/arch:IA32>"
        "$<$<COMPILE_LANGUAGE:CXX>:/EHsc;/Zc:forScope-;/Zc:wchar_t-;/Zc:twoPhase-;/wd4430;/FI${PROJECT_SOURCE_DIR}/CMake/MSVC2026Compat.h>")
    target_compile_features(${name} PRIVATE cxx_std_17)
    if(name MATCHES "^(wwdownload|compression|gameengine|gameenginedevice|generals|profile|eadebug|wwshade)$")
        set_property(TARGET ${name} PROPERTY MSVC_RUNTIME_CHECKS
            "$<$<CONFIG:Debug>:StackFrameErrorCheck;UninitializedVariable>")
    endif()
    set_target_properties(${name} PROPERTIES DEBUG_POSTFIX Debug)
endfunction()

set(GEN_CORE_LIBRARIES wwdebug wwlib wwmath wwutil wwsaveload wwdownload ww3d2)
foreach(name IN LISTS GEN_CORE_LIBRARIES)
    add_library(${name} STATIC ${GEN_${name}_SOURCES})
    gen_legacy_settings(${name})
endforeach()
set_target_properties(wwdebug PROPERTIES OUTPUT_NAME WWDebug)
set_target_properties(wwlib PROPERTIES OUTPUT_NAME WWLib)
set_target_properties(wwmath PROPERTIES OUTPUT_NAME WWMath)
set_target_properties(wwutil PROPERTIES OUTPUT_NAME WWUtil)
set_target_properties(wwsaveload PROPERTIES OUTPUT_NAME WWSaveLoad)
set_target_properties(wwdownload PROPERTIES OUTPUT_NAME WWDownload)
target_include_directories(ww3d2 PRIVATE "${PROJECT_SOURCE_DIR}/Vendors/BrowserEngine")
set_source_files_properties("${GEN_CODE_DIR}/Libraries/Source/WWVegas/WW3D2/dx8webbrowser.cpp"
    PROPERTIES VS_SETTINGS "MultiProcessorCompilation=false")
if(GEN_GAME STREQUAL "ZeroHour")
    foreach(name profile eadebug wwshade)
        add_library(${name} STATIC ${GEN_${name}_SOURCES})
        gen_legacy_settings(${name})
    endforeach()
    set_target_properties(eadebug PROPERTIES OUTPUT_NAME debug)
    set_target_properties(wwshade PROPERTIES OUTPUT_NAME WWShade)

    # shdpp writes next to its input, so stage the original shaders and headers.
    set(shader_dir "${CMAKE_BINARY_DIR}/Generated/WWShade")
    set(shader_tool "${GEN_CODE_DIR}/Libraries/Source/WWVegas/wwshade/shdpp.exe")
    file(MAKE_DIRECTORY "${shader_dir}")
    foreach(header IN LISTS GEN_wwshade_HEADERS)
        get_filename_component(filename "${header}" NAME)
        configure_file("${header}" "${shader_dir}/${filename}" COPYONLY)
    endforeach()
    foreach(shader IN LISTS GEN_wwshade_SHADERS)
        get_filename_component(filename "${shader}" NAME)
        set(output "${shader_dir}/${filename}_code.h")
        add_custom_command(OUTPUT "${output}"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${shader}" "${shader_dir}/${filename}"
            COMMAND "${shader_tool}" "${filename}"
            WORKING_DIRECTORY "${shader_dir}"
            DEPENDS "${shader}" "${shader_tool}" ${GEN_wwshade_HEADERS}
            VERBATIM)
        target_sources(wwshade PRIVATE "${output}")
    endforeach()
    target_include_directories(wwshade PRIVATE "${shader_dir}")
endif()

# Keep generated COM files in the build directory, leaving the source tree clean.
set(browser_dir "${CMAKE_BINARY_DIR}/Generated/EABrowserDispatch")
find_program(GEN_MIDL NAMES midl REQUIRED)
file(MAKE_DIRECTORY "${browser_dir}")
add_custom_command(
    OUTPUT "${browser_dir}/BrowserDispatch_i.c" "${browser_dir}/BrowserDispatch.h" "${browser_dir}/BrowserDispatch.tlb"
    COMMAND "${GEN_MIDL}" /out "${browser_dir}" /tlb BrowserDispatch.tlb
        /h BrowserDispatch.h /iid BrowserDispatch_i.c /mktyplib203 /win32
        "${GEN_CODE_DIR}/Libraries/Source/EABrowserDispatch/BrowserDispatch.idl"
    DEPENDS "${GEN_CODE_DIR}/Libraries/Source/EABrowserDispatch/BrowserDispatch.idl"
    VERBATIM)
add_library(eabrowserdispatch STATIC "${browser_dir}/BrowserDispatch_i.c")
target_include_directories(eabrowserdispatch PUBLIC "${CMAKE_BINARY_DIR}/Generated")

if(GEN_ENABLE_GAMESPY)
    include("${PROJECT_SOURCE_DIR}/CMake/GameSpySources.cmake")
    add_library(gamespy STATIC ${GEN_gamespy_SOURCES})
    target_include_directories(gamespy PRIVATE "${GEN_GAMESPY_ROOT}")
    target_compile_definitions(gamespy PRIVATE WIN32 _WINDOWS _MBCS
        "$<$<CONFIG:Debug>:_DEBUG>" "$<$<CONFIG:Release>:NDEBUG>")
    foreach(sdk HTTP Patching Peer Presence Stats)
        add_library(gamespy${sdk} ALIAS gamespy)
    endforeach()
else()
    add_library(gamespyoffline STATIC "${PROJECT_SOURCE_DIR}/CMake/Stubs/GameSpyDisabled.cpp")
    target_include_directories(gamespyoffline PRIVATE "${GEN_GAMESPY_ROOT}")
    foreach(sdk HTTP Patching Peer Presence Stats)
        add_library(gamespy${sdk} ALIAS gamespyoffline)
    endforeach()
endif()

set(compression_sources "")
foreach(source IN LISTS GEN_compression_SOURCES)
    if(source MATCHES "/ZLib/")
        get_filename_component(filename "${source}" NAME)
        # maketree.c is a zlib maintainer utility, not part of its runtime archive.
        if(NOT filename STREQUAL "maketree.c")
            list(APPEND compression_sources "${GEN_ZLIB_ROOT}/${filename}")
        endif()
    elseif(source MATCHES "/LZHCompress/")
        continue()
    else()
        list(APPEND compression_sources "${source}")
    endif()
endforeach()
add_library(compression STATIC ${compression_sources})
gen_legacy_settings(compression)
# EA's sources include ZLib/zlib.h; provide an alias without copying the SDK.
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/Generated/ZLib")
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/Generated/ZLib/zlib.h" CONTENT "#include \"${GEN_ZLIB_ROOT}/zlib.h\"\n" @ONLY)
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/Generated/MSS")
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/Generated/MSS/MSS.h" CONTENT "#include \"${GEN_MILES_INCLUDE_DIR}/mss.h\"\n" @ONLY)
target_include_directories(compression PRIVATE "${CMAKE_BINARY_DIR}/Generated")
target_compile_definitions(compression PRIVATE GEN_ENABLE_LZH=0)

foreach(name gameengine gameenginedevice)
    add_library(${name} STATIC ${GEN_${name}_SOURCES})
    gen_legacy_settings(${name})
    target_include_directories(${name} PRIVATE "${GEN_CODE_DIR}/GameEngine/Include/Precompiled"
        "${GEN_GAMESPY_ROOT}" "${GEN_CODE_DIR}/Libraries/Source/Compression")
    target_compile_definitions(${name} PUBLIC GEN_ENABLE_BINK=1
        GEN_ENABLE_GAMESPY=$<BOOL:${GEN_ENABLE_GAMESPY}>
        GEN_ENABLE_BENCHMARK=0)
    target_link_libraries(${name} PRIVATE eabrowserdispatch)
endforeach()
set_target_properties(gameengine PROPERTIES OUTPUT_NAME GameEngine)
target_precompile_headers(gameengine PRIVATE "${PROJECT_SOURCE_DIR}/CMake/MSVC2026Compat.h"
    "${GEN_CODE_DIR}/GameEngine/Include/Precompiled/PreRTS.h")
set_target_properties(gameenginedevice PROPERTIES OUTPUT_NAME GameEngineDevice)
# Use the DLL ABI, including SDK distributions which also support static Miles.
target_compile_definitions(gameenginedevice PRIVATE A1_NO_STATIC)
if(GEN_MILES_CLEANUP_SOURCE)
    # Import libraries generated from a runtime DLL lack this SDK helper.
    target_sources(gameenginedevice PRIVATE "${GEN_MILES_CLEANUP_SOURCE}")
endif()
target_link_libraries(gameenginedevice PRIVATE binkcompat)

if(GEN_GAME STREQUAL "Generals")
    set(default_version "1.8")
else()
    set(default_version "1.4")
endif()
set(GEN_VERSION "${default_version}" CACHE STRING "Major.minor game/replay version")
set(GEN_BUILD_NUMBER "0" CACHE STRING "Game build number")
if(NOT GEN_VERSION MATCHES "^([0-9]+)[.]([0-9]+)$" OR NOT GEN_BUILD_NUMBER MATCHES "^[0-9]+$")
    message(FATAL_ERROR "GEN_VERSION must be major.minor and GEN_BUILD_NUMBER an integer.")
endif()
string(REPLACE "." ";" version_parts "${GEN_VERSION}")
list(GET version_parts 0 GEN_VERSION_MAJOR)
list(GET version_parts 1 GEN_VERSION_MINOR)
configure_file("${PROJECT_SOURCE_DIR}/CMake/BuildVersion.h.in" "${CMAKE_BINARY_DIR}/Generated/BuildVersion.h" @ONLY)
configure_file("${PROJECT_SOURCE_DIR}/CMake/GeneratedVersion.h.in" "${CMAKE_BINARY_DIR}/Generated/GeneratedVersion.h" @ONLY)

# The source release omits the splash bitmap and generated type library.
file(READ "${GEN_CODE_DIR}/Main/RTS.rc" game_resources)
string(REPLACE "\"Generals.ico\"" "\"${GEN_CODE_DIR}/Main/Generals.ico\"" game_resources "${game_resources}")
string(REPLACE "\"../Libraries/Include/EABrowserDispatch/BrowserDispatch.tlb\""
    "\"${browser_dir}/BrowserDispatch.tlb\"" game_resources "${game_resources}")
if(EXISTS "${GEN_CODE_DIR}/Main/Install_Final.bmp")
    string(REPLACE "\"Install_Final.bmp\"" "\"${GEN_CODE_DIR}/Main/Install_Final.bmp\"" game_resources "${game_resources}")
else()
    string(REGEX REPLACE "IDB_LOAD_SCREEN[^\n]*\n" "" game_resources "${game_resources}")
endif()
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/Generated/RTS.rc" CONTENT "${game_resources}" @ONLY)
list(REMOVE_ITEM GEN_generals_SOURCES "${GEN_CODE_DIR}/Main/RTS.RC")
add_executable(generals WIN32 ${GEN_generals_SOURCES} "${CMAKE_BINARY_DIR}/Generated/RTS.rc")
set_source_files_properties("${CMAKE_BINARY_DIR}/Generated/RTS.rc" PROPERTIES OBJECT_DEPENDS "${browser_dir}/BrowserDispatch.tlb")
gen_legacy_settings(generals)
set_target_properties(generals PROPERTIES OUTPUT_NAME RTS DEBUG_POSTFIX D)
target_link_directories(generals PRIVATE "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
target_include_directories(generals PRIVATE "${GEN_CODE_DIR}/Main")
target_link_options(generals PRIVATE /DEBUG /MACHINE:I386)
target_link_libraries(generals PRIVATE gameengine gameenginedevice compression ${GEN_CORE_LIBRARIES}
    gamespyHTTP gamespyPatching gamespyPeer gamespyPresence gamespyStats eabrowserdispatch
    "${GEN_MILES_LIBRARY}" "${GEN_DBGHELP_LIBRARY}" "${GEN_DIRECTX_d3dx8_LIBRARY}" "${GEN_DIRECTX_d3d8_LIBRARY}"
    "${GEN_DIRECTX_dinput8_LIBRARY}" "${GEN_DIRECTX_dxguid_LIBRARY}" "${GEN_DIRECTX_dsound_LIBRARY}"
    kernel32 user32 gdi32 winspool comdlg32 advapi32 shell32 ole32 oleaut32 uuid
    odbc32 odbccp32 winmm vfw32 wsock32 imm32 wininet)
target_link_options(generals PRIVATE /NODEFAULTLIB:libci /NODEFAULTLIB:libc)
target_link_libraries(generals PRIVATE legacy_stdio_definitions)
if(TARGET profile)
    target_link_libraries(generals PRIVATE profile eadebug wwshade)
endif()
