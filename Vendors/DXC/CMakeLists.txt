include(FetchContent)
FetchContent_Declare(DXC
    URL https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.9.2609/dxc_2026_09_29.zip
    URL_HASH SHA256=ad31b1fc8443175d204f77a611fdb3ef2ec42759bdc2f1167368de24a4a7e7f1
    SOURCE_SUBDIR Generals)
FetchContent_MakeAvailable(DXC)
add_executable(Vendor::DXC IMPORTED GLOBAL)
set_target_properties(Vendor::DXC PROPERTIES IMPORTED_LOCATION "${dxc_SOURCE_DIR}/bin/x64/dxc.exe")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(dxc_runtime_arch x64)
else()
    set(dxc_runtime_arch x86)
endif()
add_library(Vendor::DXCompiler SHARED IMPORTED GLOBAL)
set_target_properties(Vendor::DXCompiler PROPERTIES
    IMPORTED_IMPLIB "${dxc_SOURCE_DIR}/lib/${dxc_runtime_arch}/dxcompiler.lib"
    IMPORTED_LOCATION "${dxc_SOURCE_DIR}/bin/${dxc_runtime_arch}/dxcompiler.dll"
    INTERFACE_INCLUDE_DIRECTORIES "${dxc_SOURCE_DIR}/inc")
set(GEN_DXC_RUNTIME_DIRECTORY "${dxc_SOURCE_DIR}/bin/${dxc_runtime_arch}" CACHE INTERNAL "")
