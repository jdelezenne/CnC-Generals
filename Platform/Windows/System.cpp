// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/System.h"
#include <windows.h>
#include <vector>
#include <stdexcept>
int Platform::LastSystemError() { return GetLastError(); }
std::string Platform::UserName()
{
    char name[257];
    DWORD length = sizeof(name);
    return GetUserNameA(name, &length) ? name : "unknown";
}
void Platform::FormatSystemError(int error, char* buffer, int length)
{
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, error, 0, buffer, length, nullptr);
}
void Platform::FormatSystemError(int error, wchar_t* buffer, int length)
{
    if (length <= 0) return;
    buffer[0] = 0;
    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, error, 0, buffer, length, nullptr);
}
bool Platform::FormatSystemErrorMessage(int error, char* buffer, int length)
{
    return FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, error, 0, buffer, length, nullptr) != 0;
}
std::uint32_t Platform::CurrentThreadIdentifier() { return GetCurrentThreadId(); }
std::string Platform::ExecutablePath()
{
    std::vector<char> path(512);
    for (;;) {
        const auto length = GetModuleFileNameA(nullptr, path.data(), static_cast<DWORD>(path.size()));
        if (!length) throw std::runtime_error("Cannot read executable path");
        if (length < path.size()) return {path.data(), length};
        path.resize(path.size() * 2);
    }
}
