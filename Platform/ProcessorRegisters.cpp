// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/SystemInfo.h"
#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86))
#include <intrin.h>
#elif defined(__x86_64__) || defined(__i386__)
#include <cpuid.h>
#endif
bool Platform::HasProcessorRegisters()
{
#if defined(_M_X64) || defined(_M_IX86) || defined(__x86_64__)
    return true;
#elif defined(__i386__)
    unsigned signature;
    return __get_cpuid_max(0, &signature) != 0;
#else
    return false;
#endif
}
void Platform::ProcessorRegisters(unsigned& a, unsigned& b, unsigned& c, unsigned& d, unsigned leaf)
{
#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86))
    int registers[4];
    __cpuidex(registers, static_cast<int>(leaf), 0);
    a = registers[0]; b = registers[1]; c = registers[2]; d = registers[3];
#elif defined(__x86_64__) || defined(__i386__)
    __cpuid_count(leaf, 0, a, b, c, d);
#else
    a = b = c = d = 0;
#endif
}
