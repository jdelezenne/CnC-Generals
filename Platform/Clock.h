// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstdint>

namespace Platform {
// Keep legacy millisecond arithmetic at 32 bits, including on LP64 hosts.
std::uint32_t Milliseconds();
std::uint64_t PerformanceCounter();
std::uint64_t PerformanceFrequency();
void Delay(std::uint32_t milliseconds);
int ConfigureMillisecondTiming(bool enabled);
}
