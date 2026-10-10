// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/DateTime.h"
#include <SDL3/SDL.h>
#include <stdexcept>

Platform::CalendarTime Platform::LocalCalendarTime()
{
    SDL_Time now;
    SDL_DateTime time;
    if (!SDL_GetCurrentTime(&now) || !SDL_TimeToDateTime(now, &time, true))
        throw std::runtime_error(SDL_GetError());
    return {static_cast<std::uint16_t>(time.year), static_cast<std::uint16_t>(time.month),
        static_cast<std::uint16_t>(time.day_of_week), static_cast<std::uint16_t>(time.day),
        static_cast<std::uint16_t>(time.hour), static_cast<std::uint16_t>(time.minute),
        static_cast<std::uint16_t>(time.second), static_cast<std::uint16_t>(time.nanosecond / 1000000)};
}

