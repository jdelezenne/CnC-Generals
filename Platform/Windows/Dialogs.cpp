// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Dialogs.h"
#include <windows.h>
#include <stdexcept>
#include <vector>

void Platform::DebuggerOutput(const char* text) { OutputDebugStringA(text); }
std::string Platform::LegacyTextToUTF8(const char* text)
{
    const int size = MultiByteToWideChar(CP_ACP, 0, text, -1, nullptr, 0);
    if (!size) throw std::runtime_error("Cannot decode native dialog text");
    std::vector<wchar_t> wide(size);
    if (!MultiByteToWideChar(CP_ACP, 0, text, -1, wide.data(), size))
        throw std::runtime_error("Cannot decode native dialog text");
    const int bytes = WideCharToMultiByte(CP_UTF8, 0, wide.data(), size, nullptr, 0, nullptr, nullptr);
    if (!bytes) throw std::runtime_error("Cannot encode native dialog text");
    std::string result(bytes, '\0');
    if (!WideCharToMultiByte(CP_UTF8, 0, wide.data(), size, result.data(), bytes, nullptr, nullptr))
        throw std::runtime_error("Cannot encode native dialog text");
    result.pop_back();
    return result;
}
