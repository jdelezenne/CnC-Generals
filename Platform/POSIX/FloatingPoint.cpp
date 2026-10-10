// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/FloatingPoint.h"
#include <cfenv>
void Platform::SetGameFloatingPointMode()
{
    std::fesetenv(FE_DFL_ENV);
    std::fesetround(FE_TONEAREST);
#if defined(__i386__) || defined(__x86_64__)
    // The game requests 24-bit x87 precision and nearest rounding.
    unsigned short control;
    __asm__ volatile("fnstcw %0" : "=m"(control));
    control &= static_cast<unsigned short>(~0x0f00u);
    __asm__ volatile("fldcw %0" : : "m"(control));
#endif
}
