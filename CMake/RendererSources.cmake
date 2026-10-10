list(APPEND GEN_ww3d2_SOURCES
    "${GEN_CODE_DIR}/Libraries/Source/WWVegas/WW3D2/dx8caps.cpp"
    "${GEN_CODE_DIR}/Libraries/Source/WWVegas/WW3D2/dx8fvf.cpp"
    "${GEN_CODE_DIR}/Libraries/Source/WWVegas/WW3D2/dx8indexbuffer.cpp"
    "${GEN_CODE_DIR}/Libraries/Source/WWVegas/WW3D2/dx8polygonrenderer.cpp"
    "${GEN_CODE_DIR}/Libraries/Source/WWVegas/WW3D2/dx8renderer.cpp"
    "${GEN_CODE_DIR}/Libraries/Source/WWVegas/WW3D2/dx8texman.cpp"
    "${GEN_CODE_DIR}/Libraries/Source/WWVegas/WW3D2/dx8vertexbuffer.cpp"
    "${GEN_CODE_DIR}/Libraries/Source/WWVegas/WW3D2/dx8wrapper.cpp")
if(GEN_GAME STREQUAL "ZeroHour")
    list(APPEND GEN_ww3d2_SOURCES
        "${GEN_CODE_DIR}/Libraries/Source/WWVegas/WW3D2/dx8rendererdebugger.cpp")
endif()
