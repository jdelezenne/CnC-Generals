// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <charconv>
#include <cstdlib>
#include <cwchar>
#include <cstring>
#include <cerrno>
#include <limits>
#include <type_traits>
namespace Platform {
template<class Integer>
inline char* IntegerTextValue(Integer value, char* text, int radix)
{
    using Number = std::conditional_t<(sizeof(Integer) < sizeof(int)), int, Integer>;
    const Number number = value;
    char digits[sizeof(Number) * 8 + 2];
    const auto result = radix == 10 ? std::to_chars(digits, digits + sizeof(digits), number, radix)
        : std::to_chars(digits, digits + sizeof(digits), static_cast<std::make_unsigned_t<Number>>(number), radix);
    const auto count = result.ptr - digits;
    std::memcpy(text, digits, count);
    text[count] = 0;
    return text;
}
inline char* IntegerText(int value, char* text, int radix)
{
#ifdef _WIN32
    return _itoa(value, text, radix);
#else
    char digits[33];
    const auto result = radix == 10 ? std::to_chars(digits, digits + sizeof(digits), value, radix)
        : std::to_chars(digits, digits + sizeof(digits), static_cast<unsigned>(value), radix);
    const auto count = result.ptr - digits;
    std::memcpy(text, digits, count);
    text[count] = 0;
    return text;
#endif
}
inline int WideInteger(const wchar_t* text)
{
#ifdef _WIN32
    return _wtoi(text);
#else
    const auto value = std::wcstol(text, nullptr, 10);
    if (value > std::numeric_limits<int>::max()) { errno = ERANGE; return std::numeric_limits<int>::max(); }
    if (value < std::numeric_limits<int>::min()) { errno = ERANGE; return std::numeric_limits<int>::min(); }
    return static_cast<int>(value);
#endif
}
}
