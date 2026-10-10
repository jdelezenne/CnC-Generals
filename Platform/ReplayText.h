// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "Platform/UTF16.h"
#include <cstdio>

namespace Platform {
template<class Character>
std::size_t WriteReplayText(std::FILE* file, const Character* text, std::size_t length)
{
    auto bytes = EncodeUTF16LE(text, length);
    bytes.push_back(0);
    bytes.push_back(0);
    return std::fwrite(bytes.data(), 1, bytes.size(), file);
}

template<class Character>
std::basic_string<Character> ReadReplayText(std::FILE* file)
{
    // Replay names originally use a 1024-unit buffer with its last unit zeroed.
    std::vector<unsigned char> bytes;
    for (std::size_t i = 0; i < 1024; ++i) {
        unsigned char unit[2];
        if (std::fread(unit, 1, 2, file) != 2 || (unit[0] == 0 && unit[1] == 0)) break;
        if (i < 1023) { bytes.push_back(unit[0]); bytes.push_back(unit[1]); }
    }
    return DecodeUTF16LE<Character>(bytes.data(), bytes.size() / 2);
}
}
