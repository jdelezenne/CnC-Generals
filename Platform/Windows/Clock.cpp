// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Clock.h"
#include <windows.h>
#include <mmsystem.h>
int Platform::ConfigureMillisecondTiming(bool enabled)
{
    return enabled ? timeBeginPeriod(1) : timeEndPeriod(1);
}
