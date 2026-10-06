// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Input.h"
#include "Platform/SDL/InputInternal.h"
#include <windows.h>
#include <string>
#include <cstdint>

bool Platform::FrenchKeyboardLayout()
{
    const auto language = reinterpret_cast<std::uintptr_t>(GetKeyboardLayout(0)) & 0xffff;
    return language == 0x040c || language == 0x080c || language == 0x0c0c ||
        language == 0x100c || language == 0x140c;
}
unsigned int Platform::DoubleClickTime() { return GetDoubleClickTime(); }

void Platform::ConfigureNativeInput()
{
    SDL_SetHint("SDL_WINDOWS_DPI_AWARENESS", "unaware");
    const std::string doubleClickTime = std::to_string(GetDoubleClickTime());
    SDL_SetHint(SDL_HINT_MOUSE_DOUBLE_CLICK_TIME, doubleClickTime.c_str());
    const int width = GetSystemMetrics(SM_CXDOUBLECLK), height = GetSystemMetrics(SM_CYDOUBLECLK);
    const int radius = (width < height ? width : height) / 2;
    const std::string doubleClickRadius = std::to_string(radius > 0 ? radius : 1);
    SDL_SetHint(SDL_HINT_MOUSE_DOUBLE_CLICK_RADIUS, doubleClickRadius.c_str());
}
