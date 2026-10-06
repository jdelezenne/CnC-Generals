add_library(gen_graphics_headers INTERFACE)
target_include_directories(gen_graphics_headers INTERFACE
    "${PROJECT_SOURCE_DIR}/Platform/Graphics/Include"
    "${PROJECT_SOURCE_DIR}/Vendors/WineD3D8/Include"
    "${PROJECT_SOURCE_DIR}")
if(NOT WIN32)
    target_link_libraries(gen_graphics_headers INTERFACE Microsoft::DirectX-Headers)
endif()
