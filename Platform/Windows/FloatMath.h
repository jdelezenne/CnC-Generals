#pragma once

#include <intrin.h>
#include <cstdint>
#include <cstring>

namespace Platform {
inline long RoundFloatToLong(float value)
{
    // CVTSS2SI follows the thread's rounding mode, as the original FISTP did.
    return _mm_cvtss_si32(_mm_set_ss(value));
}

inline float TruncateFloatBits(float value)
{
    // Preserve the original shift/mask operation, including its signed-zero
    // and out-of-range exponent behavior.
    std::uint32_t bits;
    std::memcpy(&bits, &value, sizeof(bits));
    unsigned exponent = (bits >> 23) & 255;
    std::uint32_t mask = 0;
    if (exponent >= 127) {
        unsigned shift = (exponent - 127) & 31;
        mask = 0xff800000u >> shift;
        if (shift) mask |= 0xffffffffu << (32 - shift);
    }
    bits &= mask;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}

inline unsigned ColorComponent(float value)
{
    return static_cast<unsigned>(_mm_cvttsd_si32(_mm_set_sd(static_cast<double>(value) * 255.0)));
}

inline float ClampColorBits(float value)
{
    std::uint32_t bits;
    std::memcpy(&bits, &value, sizeof(bits));
    if (bits & 0x80000000u) bits = 0;
    else if (bits > 0x3f800000u) bits = 0x3f800000u;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}
}
