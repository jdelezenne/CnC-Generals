#include "Platform/Text.h"
#include "Platform/UTF16.h"
#include <SDL3/SDL_stdinc.h>

bool Platform::NarrowFromWide(const wchar_t* source, std::string& result, bool& unmapped)
{
    auto units = EncodeUTF16LE(source, std::wcslen(source));
    units.push_back(0); units.push_back(0);
    char* converted = SDL_iconv_string("UTF-8", "UTF-16LE",
        reinterpret_cast<const char*>(units.data()), units.size());
    if (!converted) return false;
    result.assign(converted);
    SDL_free(converted);
    unmapped = false;
    return true;
}

bool Platform::WideFromNarrow(const char* source, std::wstring& result)
{
    char* converted = SDL_iconv_string("UTF-16LE", "UTF-8", source, std::strlen(source) + 1);
    if (!converted) return false;
    const auto* units = reinterpret_cast<const unsigned char*>(converted);
    std::size_t length = 0;
    while (units[length * 2] || units[length * 2 + 1]) ++length;
    result = DecodeUTF16LE<wchar_t>(units, length);
    SDL_free(converted);
    return true;
}
