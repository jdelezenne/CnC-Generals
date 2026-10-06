// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace Platform {
// File and CRC boundaries use the original UTF-16LE code units, regardless of
// the host's wchar_t width. Preserve isolated surrogates from existing files.
template<class Character>
std::size_t UTF16CodeUnits(const Character* text, std::size_t length)
{
    static_assert(sizeof(Character) == 2 || sizeof(Character) == 4);
    std::size_t units = length;
    if constexpr (sizeof(Character) == 4) {
        for (std::size_t i = 0; i < length; ++i) {
            const auto value = static_cast<std::uint32_t>(text[i]);
            if (value > 0x10ffff) throw std::range_error("Invalid Unicode code point");
            if (value > 0xffff) ++units;
        }
    }
    return units;
}

template<class Character>
std::vector<unsigned char> EncodeUTF16LE(const Character* text, std::size_t length)
{
    std::vector<unsigned char> bytes;
    bytes.reserve(UTF16CodeUnits(text, length) * 2);
    const auto append = [&bytes](std::uint32_t unit) {
        bytes.push_back(static_cast<unsigned char>(unit));
        bytes.push_back(static_cast<unsigned char>(unit >> 8));
    };
    for (std::size_t i = 0; i < length; ++i) {
        auto value = static_cast<std::uint32_t>(static_cast<std::make_unsigned_t<Character>>(text[i]));
        if constexpr (sizeof(Character) == 4) {
            if (value > 0xffff) {
                value -= 0x10000;
                append(0xd800 + (value >> 10));
                append(0xdc00 + (value & 0x3ff));
                continue;
            }
        }
        append(value);
    }
    return bytes;
}

template<class Character>
std::basic_string<Character> DecodeUTF16LE(const unsigned char* bytes, std::size_t units)
{
    static_assert(sizeof(Character) == 2 || sizeof(Character) == 4);
    const auto read = [bytes](std::size_t index) -> std::uint32_t {
        return bytes[index * 2] | (static_cast<std::uint32_t>(bytes[index * 2 + 1]) << 8);
    };
    std::basic_string<Character> text;
    text.reserve(units);
    for (std::size_t i = 0; i < units; ++i) {
        auto value = read(i);
        if constexpr (sizeof(Character) == 4) {
            if (value >= 0xd800 && value <= 0xdbff && i + 1 < units) {
                const auto low = read(i + 1);
                if (low >= 0xdc00 && low <= 0xdfff) {
                    value = 0x10000 + ((value - 0xd800) << 10) + (low - 0xdc00);
                    ++i;
                }
            }
        }
        text.push_back(static_cast<Character>(value));
    }
    return text;
}
}
