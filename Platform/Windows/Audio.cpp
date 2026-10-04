// SPDX-License-Identifier: GPL-3.0-or-later
#include <Mss.h>

extern "C" S32 AILCALL AIL_waveOutOpen(HDIGDRIVER* driver, LPHWAVEOUT*, S32, LPWAVEFORMAT format)
{
    if (!format) return 1;
    return Audio_OpenDigitalDriver(driver, format->nSamplesPerSec, format->nChannels);
}
