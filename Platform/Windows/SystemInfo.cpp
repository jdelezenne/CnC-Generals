// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/SystemInfo.h"
#include <windows.h>
#include <climits>
bool Platform::QueryMemoryInfo(MemoryInfo& info)
{
    MEMORYSTATUS memory{};
    GlobalMemoryStatus(&memory);
    info = {MemorySize(memory.dwTotalPhys), MemorySize(memory.dwAvailPhys), MemorySize(memory.dwTotalPageFile),
        MemorySize(memory.dwAvailPageFile), MemorySize(memory.dwTotalVirtual), MemorySize(memory.dwAvailVirtual)};
    return true;
}
bool Platform::QueryOSInfo(OSInfo& info)
{
    OSVERSIONINFOA os{};
    os.dwOSVersionInfoSize = sizeof(os);
    if (!GetVersionExA(&os)) return false;
    info = {os.dwMajorVersion, os.dwMinorVersion, os.dwBuildNumber, os.dwPlatformId, os.szCSDVersion};
    return true;
}
const char* Platform::OperatingSystemName(unsigned platform)
{
    switch (platform) {
    case VER_PLATFORM_WIN32s: return "Windows 3.1";
    case VER_PLATFORM_WIN32_WINDOWS: return "Windows 9x";
    case VER_PLATFORM_WIN32_NT: return "Windows NT";
    default: return "";
    }
}
int Platform::TimeZoneBiasMinutes()
{
    TIME_ZONE_INFORMATION zone{};
    GetTimeZoneInformation(&zone);
    return zone.Bias;
}
