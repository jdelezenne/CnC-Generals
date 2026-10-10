// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstdint>
#include <vector>
namespace Platform {
struct Font;
struct FontGlyph { int advance = 0; std::vector<std::uint8_t> intensity; };
bool RegisterFontFile(const char* path);
void RemoveFontFile(const char* path);
Font* OpenFont(const char* family, int pointSize, bool bold, int averageWidth = 0, bool italic = false);
void CloseFont(Font* font);
int FontHeight(const Font* font);
int FontAscent(const Font* font);
int FontOverhang(const Font* font);
FontGlyph RasterizeGlyph(Font* font, std::uint32_t character, int width, int height, int originX);
}
