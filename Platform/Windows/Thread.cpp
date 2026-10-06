// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Threads.h"
#include <SDL3/SDL_thread.h>
#include <windows.h>
#ifdef GEN_GAME_ZERO_HOUR
#include "except.h"
#endif

std::uint32_t Platform::CurrentThreadId() { return static_cast<std::uint32_t>(SDL_GetCurrentThreadID()); }
void Platform::RunThreadFunction(void (*function)(void*), void* context, ThreadExceptionHandler handler,
    const char* name, std::uint32_t identifier)
{
#ifdef GEN_GAME_ZERO_HOUR
    Register_Thread_ID(identifier, const_cast<char*>(name));
#endif
    if (handler) {
        __try { function(context); }
        __except(handler(GetExceptionCode(), GetExceptionInformation())) {}
    } else {
        function(context);
    }
#ifdef GEN_GAME_ZERO_HOUR
    Unregister_Thread_ID(identifier, const_cast<char*>(name));
#endif
}
