// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Threads.h"
#include <atomic>

std::uint32_t Platform::CurrentThreadId()
{
    // Native pthread identities can exceed the legacy engine's unsigned ID field.
    static std::atomic<std::uint32_t> next{1};
    thread_local const std::uint32_t identifier = next.fetch_add(1, std::memory_order_relaxed);
    return identifier;
}
void Platform::RunThreadFunction(void (*function)(void*), void* context, ThreadExceptionHandler,
    const char*, std::uint32_t)
{
    function(context);
}
