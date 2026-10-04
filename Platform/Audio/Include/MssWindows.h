// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>

extern "C" S32 AILCALL AIL_waveOutOpen(HDIGDRIVER* driver, LPHWAVEOUT* waveOut,
    S32 deviceID, LPWAVEFORMAT format);
