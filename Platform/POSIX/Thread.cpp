// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Threads.h"
#include "Platform/System.h"

std::uint32_t Platform::CurrentThreadId()
{
    return Platform::CurrentThreadIdentifier();
}
void Platform::RunThreadFunction(void (*function)(void*), void* context, ThreadExceptionHandler,
    const char*, std::uint32_t)
{
    function(context);
}
