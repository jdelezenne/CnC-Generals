list(APPEND GEN_gameenginedevice_SOURCES
    "${GEN_CODE_DIR}/GameEngineDevice/Source/Win32Device/Common/Win32BIGFile.cpp"
    "${GEN_CODE_DIR}/GameEngineDevice/Source/Win32Device/Common/Win32BIGFileSystem.cpp"
    "${GEN_CODE_DIR}/GameEngineDevice/Source/Win32Device/Common/Win32GameEngine.cpp"
    "${GEN_CODE_DIR}/GameEngineDevice/Source/Win32Device/Common/Win32LocalFile.cpp"
    "${GEN_CODE_DIR}/GameEngineDevice/Source/Win32Device/Common/Win32LocalFileSystem.cpp"
    "${GEN_CODE_DIR}/GameEngineDevice/Source/Win32Device/Common/Win32OSDisplay.cpp"
    "${GEN_CODE_DIR}/GameEngineDevice/Source/Win32Device/GameClient/Win32Mouse.cpp")
list(APPEND GEN_generals_SOURCES
    "${GEN_CODE_DIR}/Main/WinMain.cpp"
    "${GEN_CODE_DIR}/Main/RTS.RC")
list(APPEND GEN_ww3d2_SOURCES
    "${GEN_CODE_DIR}/Libraries/Source/WWVegas/WW3D2/dx8caps.cpp"
    "${GEN_CODE_DIR}/Libraries/Source/WWVegas/WW3D2/dx8fvf.cpp"
    "${GEN_CODE_DIR}/Libraries/Source/WWVegas/WW3D2/dx8indexbuffer.cpp"
    "${GEN_CODE_DIR}/Libraries/Source/WWVegas/WW3D2/dx8polygonrenderer.cpp"
    "${GEN_CODE_DIR}/Libraries/Source/WWVegas/WW3D2/dx8renderer.cpp"
    "${GEN_CODE_DIR}/Libraries/Source/WWVegas/WW3D2/dx8texman.cpp"
    "${GEN_CODE_DIR}/Libraries/Source/WWVegas/WW3D2/dx8vertexbuffer.cpp"
    "${GEN_CODE_DIR}/Libraries/Source/WWVegas/WW3D2/dx8webbrowser.cpp"
    "${GEN_CODE_DIR}/Libraries/Source/WWVegas/WW3D2/dx8wrapper.cpp")
if(GEN_GAME STREQUAL "ZeroHour")
    list(APPEND GEN_ww3d2_SOURCES
        "${GEN_CODE_DIR}/Libraries/Source/WWVegas/WW3D2/dx8rendererdebugger.cpp")
endif()
