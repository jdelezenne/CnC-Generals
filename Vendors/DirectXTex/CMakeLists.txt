include(FetchContent)
if(NOT WIN32)
    FetchContent_Declare(SAL
        URL https://raw.githubusercontent.com/dotnet/runtime/5c4e2a301ec1dd8ef75a3ba98e5be1ff2cea0130/src/coreclr/pal/inc/rt/sal.h
        URL_HASH SHA256=e26ee611231a859b5a800eb2e1606e7ca03b8e3981d7c98cf1ca20c89b658b68
        DOWNLOAD_NO_EXTRACT TRUE
        SOURCE_SUBDIR DownloadOnly)
    FetchContent_MakeAvailable(SAL)
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
    set(DXHEADERS_BUILD_TEST OFF)
    set(DXHEADERS_BUILD_GOOGLE_TEST OFF)
    set(DXHEADERS_INSTALL ON)
    FetchContent_Declare(directx-headers
        URL https://github.com/microsoft/DirectX-Headers/archive/refs/tags/v1.619.5.tar.gz
        URL_HASH SHA256=24a0b7d8079a2dbbc90753c0d8bc812040d052acce2302e69a97c5d873b313b8
        OVERRIDE_FIND_PACKAGE)
    FetchContent_Declare(directxmath
        URL https://github.com/microsoft/DirectXMath/archive/refs/tags/jun2026.tar.gz
        URL_HASH SHA256=cb46002679b16a25fd11d0a2a13f6a7c7bf9c51bf04258fb5485e5b839da8474
        OVERRIDE_FIND_PACKAGE)
    FetchContent_MakeAvailable(directx-headers directxmath)
    add_library(Microsoft::DirectXMath ALIAS DirectXMath)
    target_include_directories(DirectXMath SYSTEM INTERFACE "$<BUILD_INTERFACE:${sal_SOURCE_DIR}>")
    target_link_libraries(DirectXMath INTERFACE Microsoft::DirectX-Headers)
endif()
set(BUILD_TOOLS OFF)
set(BUILD_SAMPLE OFF)
set(BUILD_DX11 OFF)
set(BUILD_DX12 OFF)
set(BC_USE_OPENMP OFF)
FetchContent_Declare(DirectXTex
    URL https://github.com/microsoft/DirectXTex/archive/refs/tags/may2026.tar.gz
    URL_HASH SHA256=204ae68a66fb15d87867d836efbac9ae5aa799ca2a1867024d96a8168fa3651f
    SOURCE_SUBDIR DownloadOnly)
FetchContent_MakeAvailable(DirectXTex)
find_package(Git REQUIRED)
set(patch "${CMAKE_CURRENT_SOURCE_DIR}/RemoveWIC.patch")
execute_process(COMMAND "${GIT_EXECUTABLE}" apply --reverse --check --unsafe-paths
    "--directory=${directxtex_SOURCE_DIR}" "${patch}"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    RESULT_VARIABLE already_patched OUTPUT_QUIET ERROR_QUIET)
if(NOT already_patched EQUAL 0)
    execute_process(COMMAND "${GIT_EXECUTABLE}" apply --unsafe-paths
        "--directory=${directxtex_SOURCE_DIR}" "${patch}"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMAND_ERROR_IS_FATAL ANY)
endif()
add_subdirectory("${directxtex_SOURCE_DIR}" "${directxtex_BINARY_DIR}")
