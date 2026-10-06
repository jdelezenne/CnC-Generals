// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstring>
#include <cwchar>
#ifndef _WIN32
#include <strings.h>
#endif

namespace Platform {
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
}
