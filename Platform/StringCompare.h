// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstring>
#include <cwchar>
#include <cctype>
#ifndef _WIN32
#include <strings.h>
#endif

namespace Platform {
inline char* CopyString(char* destination, const char* source, int capacity)
{
    if (capacity > 0) {
        const auto count = std::strlen(source) < static_cast<std::size_t>(capacity)
            ? std::strlen(source) : static_cast<std::size_t>(capacity - 1);
        std::memcpy(destination, source, count);
        destination[count] = 0;
    }
    return destination;
}
inline char* DuplicateString(const char* text)
{
#ifdef _WIN32
    return _strdup(text);
#else
    return ::strdup(text);
#endif
}
inline char* LowerCase(char* text)
{
#ifdef _WIN32
    return _strlwr(text);
#else
    for (auto* item = text; *item; ++item) *item = static_cast<char>(std::tolower(static_cast<unsigned char>(*item)));
    return text;
#endif
}
inline char* UpperCase(char* text)
{
#ifdef _WIN32
    return _strupr(text);
#else
    for (auto* item = text; *item; ++item) *item = static_cast<char>(std::toupper(static_cast<unsigned char>(*item)));
    return text;
#endif
}
// Retain the legacy CRT's locale-based comparisons. SDL's Unicode case folding
// would change comparisons of existing game strings.
inline int CompareNoCase(const char* left, const char* right)
{
#ifdef _WIN32
    return _stricmp(left, right);
#else
    return strcasecmp(left, right);
#endif
}

inline int CompareNoCase(const wchar_t* left, const wchar_t* right)
{
#ifdef _WIN32
    return _wcsicmp(left, right);
#else
    return wcscasecmp(left, right);
#endif
}

inline int CompareNoCase(const char* left, const char* right, std::size_t length)
{
#ifdef _WIN32
    return _strnicmp(left, right, length);
#else
    return strncasecmp(left, right, length);
#endif
}
}
