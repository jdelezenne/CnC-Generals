// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/DateTime.h"
#include "Platform/UTF16.h"
#include <SDL3/SDL_time.h>
#include <ctime>
#include <iomanip>
#include <locale>
#include <sstream>
#include <stdexcept>

namespace {
std::vector<unsigned char> Format(const Platform::CalendarTime& value, const wchar_t* pattern)
{
    std::tm time{};
    time.tm_year = value.year - 1900;
    time.tm_mon = value.month - 1;
    time.tm_mday = value.day;
    time.tm_wday = value.dayOfWeek;
    time.tm_hour = value.hour;
    time.tm_min = value.minute;
    time.tm_sec = value.second;
    std::wostringstream stream;
    stream.imbue(std::locale(""));
    stream << std::put_time(&time, pattern);
    const auto text = stream.str();
    return Platform::EncodeUTF16LE(text.data(), text.size());
}
}

std::vector<unsigned char> Platform::FormatShortDate(const CalendarTime& time)
{
    return Format(time, L"%x");
}

std::vector<unsigned char> Platform::FormatShortTime(const CalendarTime& time)
{
    SDL_TimeFormat preference;
    if (!SDL_GetDateTimeLocalePreferences(nullptr, &preference))
        throw std::runtime_error(SDL_GetError());
    return Format(time, preference == SDL_TIME_FORMAT_12HR ? L"%I:%M %p" : L"%H:%M");
}
