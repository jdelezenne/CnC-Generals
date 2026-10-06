// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstdint>

namespace Platform {
// Keep legacy millisecond arithmetic at 32 bits, including on LP64 hosts.
std::uint32_t Milliseconds();
}
