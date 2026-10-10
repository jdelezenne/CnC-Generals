// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Clock.h"
#include <time.h>
int Platform::ConfigureMillisecondTiming(bool)
{
    // POSIX high-resolution timers have no process-wide period to acquire/release.
    timespec resolution{};
    if (clock_getres(CLOCK_MONOTONIC, &resolution) != 0) return -1;
    return resolution.tv_sec == 0 && resolution.tv_nsec <= 1000000 ? 0 : -1;
}
