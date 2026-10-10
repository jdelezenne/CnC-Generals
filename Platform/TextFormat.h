// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cwchar>
#include <string>
#include <type_traits>
#include <vector>
#include <cstring>
#include <algorithm>

namespace Platform {
template<class Arguments>
inline void CopyFormatArguments(va_list& copy, const Arguments& arguments)
{
#ifdef _WIN32
    va_copy(copy, arguments);
#else
    // va_copy reads its source; the native builtin requires a mutable pointer.
    using Element = std::remove_const_t<std::remove_pointer_t<std::decay_t<Arguments>>>;
    va_copy(copy, const_cast<Element*>(arguments));
#endif
}
template<class Arguments>
inline int FormatBytes(char* buffer, std::size_t capacity, const char* format, const Arguments& arguments)
{
    va_list copy;
    CopyFormatArguments(copy, arguments);
#ifdef _WIN32
    const int result = _vsnprintf(buffer, capacity, format, copy);
#else
    std::vector<char> text(capacity + 1);
    const int result = std::vsnprintf(text.data(), text.size(), format, copy);
    if (result >= 0) std::memcpy(buffer, text.data(), std::min<std::size_t>(static_cast<std::size_t>(result) + 1, capacity));
#endif
    va_end(copy);
    return result >= 0 && static_cast<std::size_t>(result) > capacity ? -1 : result;
}
inline int PrintBytes(char* buffer, std::size_t capacity, const char* format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    const int result = FormatBytes(buffer, capacity, format, arguments);
    va_end(arguments);
    return result;
}
// Legacy callers reserve one character and reject output beyond that count.
template<class Arguments>
inline int FormatText(char* buffer, std::size_t capacity, const char* format, const Arguments& arguments)
{
    va_list copy;
    CopyFormatArguments(copy, arguments);
#ifdef _WIN32
    const int result = _vsnprintf(buffer, capacity - 1, format, copy);
#else
    const int result = std::vsnprintf(buffer, capacity, format, copy);
#endif
    va_end(copy);
    return result >= 0 && static_cast<std::size_t>(result) > capacity - 1 ? -1 : result;
}

#ifndef _WIN32
inline std::wstring LegacyWideFormat(const wchar_t* format)
{
    const std::wstring input(format);
    std::wstring result;
    for (std::size_t i = 0; i < input.size();) {
        if (input[i] != L'%') { result.push_back(input[i++]); continue; }
        result.push_back(input[i++]);
        if (i < input.size() && input[i] == L'%') { result.push_back(input[i++]); continue; }
        // Keep flags, width and precision (including '*' arguments) intact.
        while (i < input.size() && std::wcschr(L"-+ #0", input[i]))
            result.push_back(input[i++]);
        if (i < input.size() && input[i] == L'*') result.push_back(input[i++]);
        else while (i < input.size() && input[i] >= L'0' && input[i] <= L'9') result.push_back(input[i++]);
        if (i < input.size() && input[i] == L'.') {
            result.push_back(input[i++]);
            if (i < input.size() && input[i] == L'*') result.push_back(input[i++]);
            else while (i < input.size() && input[i] >= L'0' && input[i] <= L'9') result.push_back(input[i++]);
        }
        std::wstring length;
        if (input.compare(i, 3, L"I64") == 0) { length = L"ll"; i += 3; }
        else if (input.compare(i, 3, L"I32") == 0) i += 3;
        else if (i < input.size() && input[i] == L'I') { length = L"t"; ++i; }
        else if (i < input.size() && std::wcschr(L"hlwztjL", input[i])) {
            length.push_back(input[i++]);
            if (i < input.size() && (length == L"h" || length == L"l") && input[i] == length[0])
                length.push_back(input[i++]);
        }
        if (i == input.size()) { result += length; break; }
        wchar_t conversion = input[i++];
        if (conversion == L's' || conversion == L'S' || conversion == L'c' || conversion == L'C') {
            const bool wide = length == L"l" || length == L"w" ||
                (length.empty() && (conversion == L's' || conversion == L'c'));
            length = wide ? L"l" : L"";
            conversion = (conversion == L's' || conversion == L'S') ? L's' : L'c';
        }
        result += length;
        result.push_back(conversion);
    }
    return result;
}
#endif

template<class Arguments>
inline int FormatText(wchar_t* buffer, std::size_t capacity, const wchar_t* format, const Arguments& arguments)
{
    va_list copy;
    CopyFormatArguments(copy, arguments);
#ifdef _WIN32
    const int result = _vsnwprintf(buffer, capacity - 1, format, copy);
#else
    const auto native = LegacyWideFormat(format);
    const int result = std::vswprintf(buffer, capacity, native.c_str(), copy);
#endif
    va_end(copy);
    return result >= 0 && static_cast<std::size_t>(result) > capacity - 1 ? -1 : result;
}
}
