// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <array>
#include <cstdint>
#include <vector>

namespace Platform {
struct CalendarTime {
    std::uint16_t year, month, dayOfWeek, day, hour, minute, second, milliseconds;
};
CalendarTime LocalCalendarTime();
std::vector<unsigned char> FormatShortDate(const CalendarTime& time);
std::vector<unsigned char> FormatShortTime(const CalendarTime& time);
// Original replay order: year, month, weekday, day, hour, minute, second, ms.
inline std::array<unsigned char, 16> EncodeReplayTime(const CalendarTime& time)
{
    const std::uint16_t fields[] = {time.year, time.month, time.dayOfWeek, time.day,
        time.hour, time.minute, time.second, time.milliseconds};
    std::array<unsigned char, 16> bytes;
    for (std::size_t i = 0; i < 8; ++i) {
        bytes[i * 2] = static_cast<unsigned char>(fields[i]);
        bytes[i * 2 + 1] = static_cast<unsigned char>(fields[i] >> 8);
    }
    return bytes;
}

inline CalendarTime DecodeReplayTime(const unsigned char* bytes)
{
    const auto read = [bytes](std::size_t index) -> std::uint16_t {
        return static_cast<std::uint16_t>(bytes[index * 2] | (bytes[index * 2 + 1] << 8));
    };
    return {read(0), read(1), read(2), read(3), read(4), read(5), read(6), read(7)};
}
}
