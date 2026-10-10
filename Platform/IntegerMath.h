// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstdint>
#include <limits>

namespace Platform {
// Win32 MulDiv's signed 32-bit result, nearest rounding and failure value.
inline int RoundedMultiplyDivide(int number, int numerator, int denominator)
{
    if (!denominator) return -1;
    std::int64_t product = static_cast<std::int64_t>(number) * numerator;
    const bool negative = (product < 0) != (denominator < 0);
    if (product < 0) product = -product;
    const std::int64_t divisor = denominator < 0 ? -static_cast<std::int64_t>(denominator) : denominator;
    std::int64_t result = (product + divisor / 2) / divisor;
    if (negative) result = -result;
    if (result < std::numeric_limits<int>::min() || result > std::numeric_limits<int>::max()) return -1;
    return static_cast<int>(result);
}
}
