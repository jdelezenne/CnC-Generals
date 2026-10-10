// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/FloatingPoint.h"
#include <float.h>
void Platform::SetGameFloatingPointMode()
{
    _fpreset();
    unsigned value = _statusfp();
    value = (value & ~_MCW_RC) | (_RC_NEAR & _MCW_RC);
    value = (value & ~_MCW_PC) | (_PC_24 & _MCW_PC);
    _controlfp(value, _MCW_PC | _MCW_RC);
}
