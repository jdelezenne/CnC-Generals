// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// The Bink API subset used by the original game, backed by libbinkdec.
struct BINK {
    unsigned Width, Height, Frames, FrameNum;
    void* state;
};
typedef BINK* HBINK;
typedef unsigned int u32;
enum {
    BINKPRELOADALL = 1,
    BINKSURFACE32 = 2,
    BINKSURFACE24 = 3,
    BINKSURFACE565 = 4,
    BINKSURFACE555 = 5
};
extern "C" {
HBINK BinkOpen(const char* filename, unsigned flags);
void BinkClose(HBINK handle);
int BinkWait(HBINK handle);
void BinkDoFrame(HBINK handle);
void BinkCopyToBuffer(HBINK handle, void* destination, int pitch, unsigned height,
    unsigned x, unsigned y, unsigned flags);
void BinkNextFrame(HBINK handle);
void BinkGoto(HBINK handle, unsigned frame, unsigned flags);
void BinkSetVolume(HBINK handle, unsigned track, int volume);
int BinkSoundUseDirectSound(void* driver);
void BinkSetSoundTrack(unsigned count, const unsigned* tracks);
}
