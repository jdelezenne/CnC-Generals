#pragma once
#include <cmath>
#include <cstdint>
#include <cstring>
namespace Platform {
inline unsigned ColorComponent(float value)
{
    const double scaled = static_cast<double>(value) * 255.0;
    if (!std::isfinite(scaled) || scaled >= 2147483648.0 || scaled < -2147483648.0)
        return 0x80000000u;
    return static_cast<unsigned>(static_cast<std::int32_t>(scaled));
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
