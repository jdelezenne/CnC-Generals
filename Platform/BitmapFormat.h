#pragma once
#include <array>
#include <cstdint>
#include "Platform/Paths.h"
#include <limits>

namespace Platform {
template<std::size_t Size>
inline void BitmapNumber(std::array<unsigned char, Size>& bytes, std::size_t offset, std::uint32_t value)
{
    for (unsigned i = 0; i < 4; ++i) bytes[offset + i] = static_cast<unsigned char>(value >> (8 * i));
}
inline std::array<unsigned char, 14> BitmapFileHeader(std::uint32_t fileSize)
{
    std::array<unsigned char, 14> bytes{};
    bytes[0] = 'B'; bytes[1] = 'M';
    BitmapNumber(bytes, 2, fileSize);
    BitmapNumber(bytes, 10, 54);
    return bytes;
}
inline std::array<unsigned char, 40> BitmapInfoHeader(int width, int height)
{
    std::array<unsigned char, 40> bytes{};
    BitmapNumber(bytes, 0, 40);
    BitmapNumber(bytes, 4, static_cast<std::uint32_t>(width));
    BitmapNumber(bytes, 8, static_cast<std::uint32_t>(height));
    BitmapNumber(bytes, 12, 1 | (24 << 16));
    BitmapNumber(bytes, 24, 0xB12);
    BitmapNumber(bytes, 28, 0xB12);
    return bytes;
}
inline bool WriteBitmap24(const char* filename, const char* pixels, int width, int height)
{
    if (!pixels || width <= 0 || height <= 0) return false;
    const std::uint64_t rowBytes = std::uint64_t(width) * 3;
    const std::uint64_t stride = (rowBytes + 3) & ~std::uint64_t(3);
    const std::uint64_t size = stride * std::uint64_t(height);
    if (size > std::numeric_limits<std::uint32_t>::max() - 54) return false;
    auto info = BitmapInfoHeader(width, height);
    BitmapNumber(info, 20, static_cast<std::uint32_t>(size));
    const auto header = BitmapFileHeader(static_cast<std::uint32_t>(size + 54));
    auto* file = OpenStream(filename, "wb");
    if (!file) return false;
    bool ok = std::fwrite(header.data(), 1, header.size(), file) == header.size() &&
        std::fwrite(info.data(), 1, info.size(), file) == info.size();
    const unsigned char padding[3]{};
    for (int row = 0; ok && row < height; ++row) {
        ok = std::fwrite(pixels + row * rowBytes, 1, rowBytes, file) == rowBytes &&
            std::fwrite(padding, 1, stride - rowBytes, file) == stride - rowBytes;
    }
    return std::fclose(file) == 0 && ok;
}
}
