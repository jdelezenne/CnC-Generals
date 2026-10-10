// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Fonts.h"
#include "Platform/Paths.h"
#include "Platform/SDL/FontMetrics.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <algorithm>
#include <cmath>
#include <limits>
#include <filesystem>
#include <string>
#include <vector>

namespace {
struct FontFile { std::string path, family; bool bold, italic; int faceIndex; Platform::FontMetrics metrics; };
std::vector<FontFile>& Files() { static std::vector<FontFile> files; return files; }
bool StartFonts() { return TTF_WasInit() || TTF_Init(); }
bool Equal(const std::string& left, const char* right) { return right && SDL_strcasecmp(left.c_str(), right) == 0; }
void DiscoverDirectory(const std::filesystem::path& directory)
{
    std::error_code error;
    for (std::filesystem::recursive_directory_iterator item(directory, error), end;
        !error && item != end; item.increment(error)) {
        if (!item->is_regular_file(error)) continue;
        const auto extension = item->path().extension().string();
        if (Equal(extension, ".ttf") || Equal(extension, ".otf") || Equal(extension, ".ttc") || Equal(extension, ".fon") || Equal(extension, ".fnt"))
            Platform::RegisterFontFile(item->path().string().c_str());
    }
}
void DiscoverFonts()
{
    static bool discovered = false;
    if (discovered) return;
    discovered = true;
    DiscoverDirectory(std::filesystem::path(Platform::PreferenceDirectory(Platform::CurrentGame())) / "Fonts");
#ifdef _WIN32
    const char* windows = SDL_getenv("WINDIR");
    if (windows) DiscoverDirectory(std::filesystem::path(windows) / "Fonts");
#elif defined(__APPLE__)
    DiscoverDirectory("/System/Library/Fonts");
    DiscoverDirectory("/Library/Fonts");
#else
    DiscoverDirectory("/usr/share/fonts");
    DiscoverDirectory("/usr/local/share/fonts");
#endif
}
}

struct Platform::Font { TTF_Font* handle; int height, ascent, overhang; FT_Library library; FT_Face face; int scale; };

bool Platform::RegisterFontFile(const char* path)
{
    if (!path || !StartFonts()) return false;
    const std::string resolved = ReadPath(path);
    for (const auto& file : Files()) if (file.path == resolved) return true;
    const auto first = ReadFontMetrics(resolved.c_str());
    for (int index = 0; index < first.faceCount; ++index) {
        const auto properties = SDL_CreateProperties();
        SDL_SetStringProperty(properties, TTF_PROP_FONT_CREATE_FILENAME_STRING, resolved.c_str());
        SDL_SetFloatProperty(properties, TTF_PROP_FONT_CREATE_SIZE_FLOAT, 12);
        SDL_SetNumberProperty(properties, TTF_PROP_FONT_CREATE_FACE_NUMBER, index);
        TTF_Font* font = TTF_OpenFontWithProperties(properties);
        SDL_DestroyProperties(properties);
        if (!font) continue;
        const char* family = TTF_GetFontFamilyName(font);
        const char* style = TTF_GetFontStyleName(font);
        Files().push_back({resolved, family ? family : "", style && SDL_strcasestr(style, "bold"),
            style && (SDL_strcasestr(style, "italic") || SDL_strcasestr(style, "oblique")), index,
            index ? ReadFontMetrics(resolved.c_str(), index) : first});
        TTF_CloseFont(font);
    }
    if (!first.faceCount) return false;
    return true;
}
void Platform::RemoveFontFile(const char* path)
{
    const std::string resolved = ReadPath(path);
    std::erase_if(Files(), [&](const FontFile& font) { return font.path == resolved; });
}
Platform::Font* Platform::OpenFont(const char* family, int pointSize, bool bold, int averageWidth, bool italic)
{
    if (!family || pointSize <= 0 || !StartFonts()) return nullptr;
    DiscoverFonts();
    const int pixels = (pointSize * 96 + 36) / 72;
    const FontFile* selected = nullptr;
    int selectedScale = 1;
    int bestScore = std::numeric_limits<int>::max();
    const auto select = [&](const char* requested) {
        for (const auto& file : Files()) {
            if (!Equal(file.family, requested) || file.italic != italic) continue;
            const auto& metrics = file.metrics;
            if (file.bold != bold && !(metrics.bitmapHeight && bold && !file.bold)) continue;
            int scale = 1;
            int score = 0;
            if (metrics.bitmapHeight) {
                const int characterHeight = std::max(1, metrics.bitmapCharacterHeight);
                scale = std::max(1, pixels / characterHeight);
                const int realized = characterHeight * scale;
                score = std::abs(realized - pixels) + (realized > pixels ? pixels * 2 : 0);
                if (file.bold != bold) ++score;
            }
            if (score < bestScore) {
                selected = &file;
                selectedScale = scale;
                bestScore = score;
            }
        }
    };
    select(family);
    if (!selected && SDL_strcasecmp(family, "Arial Unicode MS") == 0) select("Arial");
    if (!selected) {
        return nullptr;
    }
    const auto properties = SDL_CreateProperties();
    SDL_SetStringProperty(properties, TTF_PROP_FONT_CREATE_FILENAME_STRING, selected->path.c_str());
    SDL_SetFloatProperty(properties, TTF_PROP_FONT_CREATE_SIZE_FLOAT, selected->metrics.bitmapHeight ? 1.0f : static_cast<float>(pixels));
    SDL_SetNumberProperty(properties, TTF_PROP_FONT_CREATE_FACE_NUMBER, selected->faceIndex);
    TTF_Font* handle = TTF_OpenFontWithProperties(properties);
    SDL_DestroyProperties(properties);
    if (!handle) return nullptr;
    TTF_SetFontHinting(handle, TTF_HINTING_MONO);
    const bool syntheticBold = selected->metrics.bitmapHeight && bold && !selected->bold;
    int height = TTF_GetFontHeight(handle), ascent = TTF_GetFontAscent(handle);
    const auto& metrics = selected->metrics;
    if (metrics.bitmapHeight) {
        height = metrics.bitmapHeight * selectedScale;
        ascent = metrics.bitmapAscent * selectedScale;
    }
    if (metrics.units) {
        ascent = (metrics.ascent * pixels + metrics.units / 2) / metrics.units;
        height = ascent + (metrics.descent * pixels + metrics.units / 2) / metrics.units;
    }
    for (const auto& size : metrics.sizes) if (size.pixels == pixels) { ascent = size.ascent; height = ascent + size.descent; break; }
    FT_Library library = nullptr;
    FT_Face face = nullptr;
    if (averageWidth && metrics.units && metrics.averageWidth) {
        if (FT_Init_FreeType(&library) || FT_New_Face(library, selected->path.c_str(), selected->faceIndex, &face)) {
            if (library) FT_Done_FreeType(library);
            TTF_CloseFont(handle);
            return nullptr;
        }
        FT_Size_RequestRec request{};
        request.type = FT_SIZE_REQUEST_TYPE_NOMINAL;
        request.width = static_cast<FT_Long>(std::lround(64.0 * averageWidth * metrics.units / metrics.averageWidth));
        request.height = pixels * 64;
        if (FT_Request_Size(face, &request)) {
            FT_Done_Face(face);
            FT_Done_FreeType(library);
            TTF_CloseFont(handle);
            return nullptr;
        }
    }
    return new Font{handle, height, ascent, syntheticBold ? 1 : 0, library, face, selectedScale};
}
void Platform::CloseFont(Font* font)
{
    if (!font) return;
    if (font->face) FT_Done_Face(font->face);
    if (font->library) FT_Done_FreeType(font->library);
    TTF_CloseFont(font->handle);
    delete font;
}
int Platform::FontHeight(const Font* font) { return font ? font->height : 0; }
int Platform::FontAscent(const Font* font) { return font ? font->ascent : 0; }
int Platform::FontOverhang(const Font* font) { return font ? font->overhang : 0; }
Platform::FontGlyph Platform::RasterizeGlyph(Font* font, std::uint32_t character, int width, int height, int originX)
{
    FontGlyph glyph;
    if (!font || width <= 0 || height <= 0) return glyph;
    glyph.intensity.resize(static_cast<std::size_t>(width) * height);
    if (font->face) {
        if (FT_Load_Char(font->face, character, FT_LOAD_TARGET_LIGHT) ||
            FT_Render_Glyph(font->face->glyph, FT_RENDER_MODE_NORMAL)) return glyph;
        const auto* slot = font->face->glyph;
        glyph.advance = static_cast<int>((slot->advance.x + 32) / 64);
        const auto& bitmap = slot->bitmap;
        for (unsigned y = 0; y < bitmap.rows; ++y) {
            const int destinationY = static_cast<int>(y) + font->ascent - slot->bitmap_top;
            if (destinationY < 0 || destinationY >= height) continue;
            const auto* row = bitmap.buffer + (bitmap.pitch >= 0 ? y : bitmap.rows - y - 1) * std::abs(bitmap.pitch);
            for (unsigned x = 0; x < bitmap.width; ++x) {
                const int destinationX = static_cast<int>(x) + originX + slot->bitmap_left;
                if (destinationX < 0 || destinationX >= width) continue;
                glyph.intensity[static_cast<std::size_t>(destinationY) * width + destinationX] = row[x];
            }
        }
        return glyph;
    }
    int minx, maxx, miny, maxy, advance;
    if (!TTF_GetGlyphMetrics(font->handle, character, &minx, &maxx, &miny, &maxy, &advance)) return glyph;
    glyph.advance = advance * font->scale + font->overhang;
    SDL_Surface* rendered = TTF_RenderGlyph_Blended(font->handle, character, {255,255,255,255});
    if (!rendered) return glyph;
    SDL_Surface* surface = SDL_ConvertSurface(rendered, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(rendered);
    if (!surface) return glyph;
    const int offsetY = font->ascent - TTF_GetFontAscent(font->handle) * font->scale;
    for (int y = 0; y < surface->h * font->scale; ++y) {
        if (y + offsetY < 0 || y + offsetY >= height) continue;
        const auto* row = static_cast<const std::uint8_t*>(surface->pixels) + (y / font->scale) * surface->pitch;
        for (int x = 0; x < surface->w * font->scale + font->overhang; ++x) {
            const int destinationX = x + originX;
            if (destinationX < 0 || destinationX >= width) continue;
            const int column = x / font->scale;
            auto alpha = column < surface->w ? row[column * 4 + 3] : std::uint8_t{0};
            if (font->overhang && x > 0) alpha = std::max(alpha, row[((x - 1) / font->scale) * 4 + 3]);
            glyph.intensity[static_cast<std::size_t>(y + offsetY) * width + destinationX] = alpha;
        }
    }
    SDL_DestroySurface(surface);
    return glyph;
}
