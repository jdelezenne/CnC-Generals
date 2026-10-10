// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/System.h"
#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86))
#include <intrin.h>
#elif defined(__x86_64__) || defined(__i386__)
#include <x86intrin.h>
#else
#include <SDL3/SDL_timer.h>
#endif
std::uint64_t Platform::ProcessorTicks()
{
#if defined(_M_X64) || defined(_M_IX86) || defined(__x86_64__) || defined(__i386__)
    return __rdtsc();
#else
    return SDL_GetPerformanceCounter();
#endif
}
