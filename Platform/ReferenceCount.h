// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstdint>
namespace Platform {
void IncrementReferenceCount(std::uint16_t& count);
void DecrementReferenceCount(std::uint16_t& count);
}
