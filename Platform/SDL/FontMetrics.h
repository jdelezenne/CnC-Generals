#pragma once
#include <vector>
namespace Platform {
struct FontPixelMetrics { int pixels, ascent, descent; };
struct FontMetrics {
    int faceCount = 0;
    int bitmapHeight = 0, bitmapAscent = 0, bitmapCharacterHeight = 0;
    int units = 0;
    int ascent = 0;
    int descent = 0;
    int averageWidth = 0;
    std::vector<FontPixelMetrics> sizes;
};
FontMetrics ReadFontMetrics(const char* path, int faceIndex = 0);
}
