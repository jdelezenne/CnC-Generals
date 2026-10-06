// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstdint>

struct _EXCEPTION_POINTERS;
namespace Platform {
using ThreadExceptionHandler = int (*)(int, _EXCEPTION_POINTERS*);
std::uint32_t CurrentThreadId();
void RunThreadFunction(void (*function)(void*), void* context, ThreadExceptionHandler handler,
    const char* name, std::uint32_t identifier);
}
