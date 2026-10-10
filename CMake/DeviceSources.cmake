if(WIN32)
    list(APPEND GEN_gameengine_SOURCES
        "${GEN_CODE_DIR}/GameEngine/Source/Common/System/StackDump.cpp")
endif()
list(APPEND GEN_gameenginedevice_SOURCES
    "${GEN_CODE_DIR}/GameEngineDevice/Include/W3DDevice/GameClient/WaterTrackTypes.h"
    "${GEN_CODE_DIR}/GameEngineDevice/Source/Win32Device/Common/Win32BIGFile.cpp"
    "${GEN_CODE_DIR}/GameEngineDevice/Source/Win32Device/Common/Win32BIGFileSystem.cpp"
    "${GEN_CODE_DIR}/GameEngineDevice/Source/Win32Device/Common/Win32GameEngine.cpp"
    "${GEN_CODE_DIR}/GameEngineDevice/Source/Win32Device/Common/Win32LocalFile.cpp"
    "${GEN_CODE_DIR}/GameEngineDevice/Source/Win32Device/Common/Win32LocalFileSystem.cpp"
    "${GEN_CODE_DIR}/GameEngineDevice/Source/Win32Device/Common/Win32OSDisplay.cpp"
    "${GEN_CODE_DIR}/GameEngineDevice/Source/Win32Device/GameClient/Win32Mouse.cpp")
list(APPEND GEN_gameengine_SOURCES "${GEN_CODE_DIR}/GameEngine/Include/Common/HardwareTypes.h")
if(GEN_GAME STREQUAL "ZeroHour")
    list(APPEND GEN_gameenginedevice_SOURCES
        "${GEN_CODE_DIR}/GameEngineDevice/Include/W3DDevice/GameClient/GraphicsVenderID.h")
endif()
if(WIN32)
list(APPEND GEN_generals_SOURCES
    "${GEN_CODE_DIR}/Main/WindowsApplication.cpp"
    "${GEN_CODE_DIR}/Main/RTS.RC")

endif()
