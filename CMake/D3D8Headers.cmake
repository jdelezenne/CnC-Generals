# Stage the graphics headers without the obsolete platform headers in the SDK.
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
