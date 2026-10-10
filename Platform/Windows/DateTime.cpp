// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/DateTime.h"
#include "Platform/UTF16.h"
#include <windows.h>

namespace {
SYSTEMTIME NativeTime(const Platform::CalendarTime& time)
{
    return {time.year, time.month, time.dayOfWeek, time.day, time.hour,
        time.minute, time.second, time.milliseconds};
}
}

std::vector<unsigned char> Platform::FormatShortDate(const CalendarTime& time)
{
    const SYSTEMTIME native = NativeTime(time);
    wchar_t text[256];
    const int count = GetDateFormatW(LOCALE_SYSTEM_DEFAULT, DATE_SHORTDATE,
        &native, nullptr, text, 256);
    if (!count) return {};
    return EncodeUTF16LE(text, count - 1);
}

std::vector<unsigned char> Platform::FormatShortTime(const CalendarTime& time)
{
    const SYSTEMTIME native = NativeTime(time);
    wchar_t text[256];
    const int count = GetTimeFormatW(LOCALE_SYSTEM_DEFAULT, TIME_NOSECONDS,
        &native, nullptr, text, 256);
    if (!count) return {};
    return EncodeUTF16LE(text, count - 1);
}
