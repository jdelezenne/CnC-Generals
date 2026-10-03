function(gen_legacy_settings name)
    # STLport must precede both the VC6 STL and SDK includes in every C++ TU.
    target_include_directories(${name} BEFORE PRIVATE "${GEN_STLPORT_INCLUDE_DIR}")
    target_include_directories(${name} PRIVATE ${GEN_${name}_INCLUDES}
        "${GEN_DIRECTX_INCLUDE_DIR}" "${GEN_CODE_DIR}/Libraries/Include")
    if(GEN_MILES_INCLUDE_DIR)
        target_include_directories(${name} PRIVATE "${GEN_MILES_INCLUDE_DIR}")
    endif()
    target_compile_definitions(${name} PRIVATE ${GEN_${name}_DEFINES})
    foreach(flag IN LISTS GEN_${name}_OPTIONS)
        # VC6 emits unsuppressible CodeView truncation warnings for some STL
        # types in the game. Keep the original code generation settings.
        if(flag STREQUAL "/WX" AND name MATCHES "^(gameengine|gameenginedevice|generals)$")
            continue()
        endif()
        target_compile_options(${name} PRIVATE "$<$<COMPILE_LANGUAGE:C,CXX>:${flag}>")
    endforeach()
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
if(GEN_CORE_ONLY)
    return()
endif()
if(GEN_GAME STREQUAL "ZeroHour")
    foreach(name profile eadebug wwshade)
        add_library(${name} STATIC ${GEN_${name}_SOURCES})
        gen_legacy_settings(${name})
    endforeach()
    set_target_properties(eadebug PROPERTIES OUTPUT_NAME debug)
    set_target_properties(wwshade PROPERTIES OUTPUT_NAME WWShade)

    # shdpp writes next to its input, so stage the original shaders and headers.
    set(shader_dir "${CMAKE_BINARY_DIR}/generated/WWShade")
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
set(browser_dir "${CMAKE_BINARY_DIR}/generated/EABrowserDispatch")
if(GEN_GAME STREQUAL "ZeroHour" AND GEN_ENABLE_SAFEDISC)
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/generated/Common/SafeDisc")
    file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/generated/Common/SafeDisc/CdaPfn.h"
        CONTENT "#include \"${GEN_SAFEDISC_INCLUDE_DIR}/CdaPfn.h\"\n" @ONLY)
endif()
file(MAKE_DIRECTORY "${browser_dir}")
add_custom_command(
    OUTPUT "${browser_dir}/BrowserDispatch_i.c" "${browser_dir}/BrowserDispatch.h" "${browser_dir}/BrowserDispatch.tlb"
    COMMAND "${GEN_VC6_midl}" /out "${browser_dir}" /tlb BrowserDispatch.tlb
        /h BrowserDispatch.h /iid BrowserDispatch_i.c /mktyplib203 /win32
        "${GEN_CODE_DIR}/Libraries/Source/EABrowserDispatch/BrowserDispatch.idl"
    DEPENDS "${GEN_CODE_DIR}/Libraries/Source/EABrowserDispatch/BrowserDispatch.idl"
    VERBATIM)
add_library(eabrowserdispatch STATIC "${browser_dir}/BrowserDispatch_i.c")
target_include_directories(eabrowserdispatch PUBLIC "${CMAKE_BINARY_DIR}/generated")

if(GEN_ENABLE_GAMESPY)
    foreach(sdk HTTP Patching Peer Presence Stats)
        # SDK project paths retain the original source membership; no directory glob.
        get_filename_component(sdk_dir "${GEN_GAMESPY_${sdk}_PROJECT}" DIRECTORY)
        file(STRINGS "${GEN_GAMESPY_${sdk}_PROJECT}" sdk_sources REGEX "^SOURCE=.*[.][cC]$")
        set(sources "")
        foreach(line IN LISTS sdk_sources)
            string(REGEX REPLACE "^SOURCE=" "" path "${line}")
            string(REPLACE "\\" "/" path "${path}")
            cmake_path(ABSOLUTE_PATH path BASE_DIRECTORY "${sdk_dir}" NORMALIZE OUTPUT_VARIABLE path)
            list(APPEND sources "${path}")
        endforeach()
        add_library(gamespy${sdk} STATIC ${sources})
        target_include_directories(gamespy${sdk} PRIVATE "${GEN_GAMESPY_ROOT}" "${sdk_dir}")
        target_compile_definitions(gamespy${sdk} PRIVATE WIN32 _WINDOWS _MBCS
            "$<$<CONFIG:Debug>:_DEBUG>" "$<$<CONFIG:Release>:NDEBUG>")
    endforeach()
else()
    add_library(gamespyoffline STATIC "${PROJECT_SOURCE_DIR}/cmake/stubs/gamespy-disabled.cpp")
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
        if(GEN_ENABLE_LZH)
            if(source MATCHES "/CompLibSource/")
                get_filename_component(filename "${source}" NAME)
                list(APPEND compression_sources "${GEN_LZH_ROOT}/CompLibSource/${filename}")
            else()
                list(APPEND compression_sources "${source}")
            endif()
        endif()
    else()
        list(APPEND compression_sources "${source}")
    endif()
endforeach()
add_library(compression STATIC ${compression_sources})
gen_legacy_settings(compression)
# EA's sources include ZLib/zlib.h; provide an alias without copying the SDK.
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/generated/ZLib")
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/generated/ZLib/zlib.h" CONTENT "#include \"${GEN_ZLIB_ROOT}/zlib.h\"\n" @ONLY)
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/generated/MSS")
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/generated/MSS/MSS.h" CONTENT "#include \"${GEN_MILES_INCLUDE_DIR}/mss.h\"\n" @ONLY)
target_include_directories(compression PRIVATE "${CMAKE_BINARY_DIR}/generated"
    "${GEN_LZH_ROOT}" "${GEN_LZH_ROOT}/CompLibHeader")
target_compile_definitions(compression PRIVATE GEN_ENABLE_LZH=$<BOOL:${GEN_ENABLE_LZH}>)

if(GEN_ENABLE_BENCHMARK)
    add_library(benchmark STATIC "${GEN_BENCHMARK_ROOT}/emfloat.c" "${GEN_BENCHMARK_ROOT}/misc.c"
        "${GEN_BENCHMARK_ROOT}/nbench0.c" "${GEN_BENCHMARK_ROOT}/nbench1.c" "${GEN_BENCHMARK_ROOT}/sysspec.c")
    target_include_directories(benchmark PUBLIC "${GEN_BENCHMARK_ROOT}")
endif()

if(NOT GEN_ENABLE_BINK)
    list(FILTER GEN_gameenginedevice_SOURCES EXCLUDE REGEX "/VideoDevice/Bink/")
    list(APPEND GEN_gameenginedevice_SOURCES "${PROJECT_SOURCE_DIR}/cmake/stubs/bink-disabled.cpp")
endif()
foreach(name gameengine gameenginedevice)
    add_library(${name} STATIC ${GEN_${name}_SOURCES})
    gen_legacy_settings(${name})
    target_include_directories(${name} PRIVATE "${GEN_CODE_DIR}/GameEngine/Include/Precompiled"
        "${GEN_GAMESPY_ROOT}" "${GEN_CODE_DIR}/Libraries/Source/Compression")
    target_compile_definitions(${name} PUBLIC GEN_ENABLE_BINK=$<BOOL:${GEN_ENABLE_BINK}>
        GEN_ENABLE_GAMESPY=$<BOOL:${GEN_ENABLE_GAMESPY}>
        GEN_ENABLE_BENCHMARK=$<BOOL:${GEN_ENABLE_BENCHMARK}>)
    target_link_libraries(${name} PRIVATE eabrowserdispatch)
    if(GEN_GAME STREQUAL "ZeroHour")
        target_compile_definitions(${name} PUBLIC GEN_ENABLE_SAFEDISC=$<BOOL:${GEN_ENABLE_SAFEDISC}>)
    endif()
endforeach()
set_target_properties(gameengine PROPERTIES OUTPUT_NAME GameEngine)
target_precompile_headers(gameengine PRIVATE "${GEN_CODE_DIR}/GameEngine/Include/Precompiled/PreRTS.h")
set_target_properties(gameenginedevice PROPERTIES OUTPUT_NAME GameEngineDevice)
# Use the DLL ABI, including SDK distributions which also support static Miles.
target_compile_definitions(gameenginedevice PRIVATE A1_NO_STATIC)
if(GEN_MILES_CLEANUP_SOURCE)
    # Import libraries generated from a runtime DLL lack this SDK helper.
    target_sources(gameenginedevice PRIVATE "${GEN_MILES_CLEANUP_SOURCE}")
endif()
if(GEN_ENABLE_BINK)
    target_include_directories(gameenginedevice PUBLIC "${GEN_BINK_INCLUDE_DIR}")
    target_link_libraries(gameenginedevice PRIVATE "${GEN_BINK_LIBRARY}")
endif()
if(GEN_ENABLE_BENCHMARK)
    target_link_libraries(gameenginedevice PRIVATE benchmark)
endif()

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
configure_file("${PROJECT_SOURCE_DIR}/cmake/BuildVersion.h.in" "${CMAKE_BINARY_DIR}/generated/BuildVersion.h" @ONLY)
configure_file("${PROJECT_SOURCE_DIR}/cmake/GeneratedVersion.h.in" "${CMAKE_BINARY_DIR}/generated/GeneratedVersion.h" @ONLY)

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
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/generated/RTS.rc" CONTENT "${game_resources}" @ONLY)
list(REMOVE_ITEM GEN_generals_SOURCES "${GEN_CODE_DIR}/Main/RTS.RC")
add_executable(generals WIN32 ${GEN_generals_SOURCES} "${CMAKE_BINARY_DIR}/generated/RTS.rc")
set_source_files_properties("${CMAKE_BINARY_DIR}/generated/RTS.rc" PROPERTIES OBJECT_DEPENDS "${browser_dir}/BrowserDispatch.tlb")
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
if(TARGET profile)
    target_link_libraries(generals PRIVATE profile eadebug wwshade)
endif()
