// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Clock.h"
#include <SDL3/SDL.h>
#include <cstdint>

std::uint32_t Platform::Milliseconds()
{
    return static_cast<std::uint32_t>(SDL_GetTicks());
}

std::uint64_t Platform::PerformanceCounter() { return SDL_GetPerformanceCounter(); }
std::uint64_t Platform::PerformanceFrequency() { return SDL_GetPerformanceFrequency(); }
void Platform::Delay(std::uint32_t milliseconds) { SDL_Delay(milliseconds); }
