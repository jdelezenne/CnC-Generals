// SPDX-License-Identifier: GPL-3.0-or-later
#include "Bink.h"
#include "BinkDecoder.h"
#include <windows.h>
#include <mmsystem.h>
#include <algorithm>
#include <array>
#include <vector>

struct BinkStreamState
{
    struct AudioBuffer {
        WAVEHDR header{};
        std::vector<int16_t> samples;
        bool prepared = false;
    };
    BinkDecoder decoder;
    YUVbuffer planes{};
    int decodedFrame = -1;
    ULONGLONG startTime = GetTickCount64();
    ULONGLONG pauseTime = 0;
    HWAVEOUT audio = nullptr;
    unsigned audioBytes = 0;
    unsigned blockAlign = 0;
    std::array<AudioBuffer, 16> buffers;

    ~BinkStreamState() { closeAudio(); }

    void closeAudio()
    {
        if (!audio) return;
        waveOutReset(audio);
        for (auto &buffer : buffers) {
            if (buffer.prepared) waveOutUnprepareHeader(audio, &buffer.header, sizeof(WAVEHDR));
            buffer.prepared = false;
        }
        waveOutClose(audio);
        audio = nullptr;
    }

    AudioBuffer *freeAudioBuffer()
    {
        for (auto &buffer : buffers) {
            if (!buffer.prepared) return &buffer;
            if ((buffer.header.dwFlags & WHDR_DONE) &&
                waveOutUnprepareHeader(audio, &buffer.header, sizeof(WAVEHDR)) == MMSYSERR_NOERROR) {
                buffer.prepared = false;
                return &buffer;
            }
        }
        return nullptr;
    }

    void openAudio()
    {
        if (!decoder.GetNumAudioTracks()) return;
        const AudioInfo info = decoder.GetAudioTrackDetails(0);
        if (!info.sampleRate || !info.idealBufferSize || (info.nChannels != 1 && info.nChannels != 2)) return;
        WAVEFORMATEX format{};
        format.wFormatTag = WAVE_FORMAT_PCM;
        format.nChannels = static_cast<WORD>(info.nChannels);
        format.nSamplesPerSec = info.sampleRate;
        format.wBitsPerSample = 16;
        format.nBlockAlign = format.nChannels * sizeof(int16_t);
        format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
        if (waveOutOpen(&audio, WAVE_MAPPER, &format, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
            audio = nullptr;
            return;
        }
        audioBytes = info.idealBufferSize;
        blockAlign = format.nBlockAlign;
        for (auto &buffer : buffers) buffer.samples.resize((audioBytes + 1) / 2);
    }

    void queueAudio()
    {
        if (!audio) return;
        AudioBuffer *buffer = freeAudioBuffer();
        if (!buffer) return;
        unsigned bytes = std::min(decoder.GetAudioData(0, buffer->samples.data()), audioBytes);
        bytes -= bytes % blockAlign;
        if (!bytes) return;
        buffer->header = {};
        buffer->header.lpData = reinterpret_cast<LPSTR>(buffer->samples.data());
        buffer->header.dwBufferLength = bytes;
        if (waveOutPrepareHeader(audio, &buffer->header, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
            closeAudio();
            return;
        }
        buffer->prepared = true;
        if (waveOutWrite(audio, &buffer->header, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) closeAudio();
    }
};

static bool soundEnabled = true;
static BinkStreamState& state(HBINK handle)
{
    return *static_cast<BinkStreamState*>(handle->state);
}

HBINK BinkOpen(const char* filename, unsigned)
{
    auto stream = new BinkStreamState;
    if (!stream->decoder.Open(filename) || !stream->decoder.GetNumFrames() ||
        stream->decoder.GetFrameRate() <= 0) {
        delete stream;
        return nullptr;
    }
    if (soundEnabled) stream->openAudio();
    stream->startTime = GetTickCount64();
    return new BINK{stream->decoder.frameWidth, stream->decoder.frameHeight,
        stream->decoder.GetNumFrames(), 1, stream};
}

void BinkClose(HBINK handle)
{
    if (!handle) return;
    delete &state(handle);
    delete handle;
}

int BinkWait(HBINK handle)
{
    if (!handle) return 1;
    auto& stream = state(handle);
    return (handle->FrameNum - 1) / stream.decoder.GetFrameRate() >
        (GetTickCount64() - stream.startTime) / 1000.0 ||
        (stream.audio && !stream.freeAudioBuffer());
}

void BinkDoFrame(HBINK handle)
{
    if (!handle) return;
    auto& stream = state(handle);
    const unsigned frame = handle->FrameNum - 1;
    if (stream.decodedFrame == static_cast<int>(frame)) return;
    while (stream.decoder.GetCurrentFrameNum() <= frame)
        stream.decoder.GetNextFrame(stream.planes);
    stream.decodedFrame = static_cast<int>(frame);
    stream.queueAudio();
}

void BinkCopyToBuffer(HBINK handle, void* destination, int pitch, unsigned height,
    unsigned xOffset, unsigned yOffset, unsigned flags)
{
    if (!handle || !destination || state(handle).decodedFrame < 0 || pitch <= 0) return;
    const unsigned bytes = flags == BINKSURFACE32 ? 4 : flags == BINKSURFACE24 ? 3 :
        flags == BINKSURFACE565 || flags == BINKSURFACE555 ? 2 : 0;
    if (!bytes || yOffset >= height || xOffset >= static_cast<unsigned>(pitch) / bytes) return;
    auto& stream = state(handle);
    const unsigned rows = std::min(handle->Height, height - yOffset);
    const unsigned columns = std::min(handle->Width, static_cast<unsigned>(pitch) / bytes - xOffset);
    for (unsigned y = 0; y < rows; ++y) {
        auto dest = static_cast<uint8_t*>(destination) + (y + yOffset) * pitch + xOffset * bytes;
        const auto luma = stream.planes[0].data + y * stream.planes[0].pitch;
        const auto u = stream.planes[1].data + (y / 2) * stream.planes[1].pitch;
        const auto v = stream.planes[2].data + (y / 2) * stream.planes[2].pitch;
        for (unsigned x = 0; x < columns; ++x) {
            const int c = 298 * (static_cast<int>(luma[x]) - 16);
            const int d = static_cast<int>(u[x / 2]) - 128;
            const int e = static_cast<int>(v[x / 2]) - 128;
            const unsigned r = std::clamp((c + 409 * e + 128) >> 8, 0, 255);
            const unsigned g = std::clamp((c - 100 * d - 208 * e + 128) >> 8, 0, 255);
            const unsigned b = std::clamp((c + 516 * d + 128) >> 8, 0, 255);
            if (bytes >= 3) {
                dest[0] = static_cast<uint8_t>(b);
                dest[1] = static_cast<uint8_t>(g);
                dest[2] = static_cast<uint8_t>(r);
                if (bytes == 4) dest[3] = 255;
            } else {
                const uint16_t pixel = flags == BINKSURFACE565 ?
                    static_cast<uint16_t>(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)) :
                    static_cast<uint16_t>(0x8000 | ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3));
                dest[0] = static_cast<uint8_t>(pixel);
                dest[1] = static_cast<uint8_t>(pixel >> 8);
            }
            dest += bytes;
        }
    }
}

void BinkNextFrame(HBINK handle)
{
    if (!handle) return;
    if (handle->FrameNum == handle->Frames) BinkGoto(handle, 1, 0);
    else ++handle->FrameNum;
}

void BinkGoto(HBINK handle, unsigned frame, unsigned)
{
    if (!handle) return;
    auto& stream = state(handle);
    // Bink uses one-based frame numbers; the original game also passes zero.
    frame = std::clamp(frame, 1u, handle->Frames) - 1;
    if (frame < stream.decoder.GetCurrentFrameNum()) stream.decoder.GotoFrame(0);
    handle->FrameNum = frame + 1;
    stream.decodedFrame = -1;
    if (stream.audio) waveOutReset(stream.audio);
    stream.startTime = GetTickCount64() -
        static_cast<ULONGLONG>(frame * 1000.0 / stream.decoder.GetFrameRate());
}

void BinkSetVolume(HBINK handle, unsigned, int volume)
{
    if (!handle || !state(handle).audio) return;
    const DWORD level = static_cast<DWORD>(std::clamp(volume, 0, 32768) * 65535LL / 32768);
    waveOutSetVolume(state(handle).audio, level | (level << 16));
}

int BinkSoundUseDirectSound(void* driver)
{
    soundEnabled = driver != nullptr;
    return soundEnabled;
}

void BinkSetSoundTrack(unsigned count, const unsigned*)
{
    soundEnabled = count != 0;
}
