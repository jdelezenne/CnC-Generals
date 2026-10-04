// SPDX-License-Identifier: GPL-3.0-or-later
#include "Bink.h"
#include "BinkDecoder.h"
#include <SDL3/SDL.h>
#include <algorithm>
#include <array>
#include <vector>
#include <atomic>
#include <mutex>

struct BinkStreamState
{
    BinkDecoder decoder;
    BinkDecoder audioDecoder;
    YUVbuffer planes{};
    int decodedFrame = -1;
    Uint64 startTime = SDL_GetTicks();
    SDL_AudioStream* audio = nullptr;
    bool audioInitialized = false;
    unsigned audioBytes = 0;
    unsigned blockAlign = 0;
    std::vector<int16_t> samples;
    SDL_Thread* audioThread = nullptr;
    std::atomic<bool> audioRunning{false};
    std::mutex audioMutex;
    unsigned audioQueueLimit = 0;

    ~BinkStreamState() { closeAudio(); }

    void closeAudio()
    {
        audioRunning = false;
        if (audioThread) SDL_WaitThread(audioThread, nullptr);
        audioThread = nullptr;
        if (audio) SDL_DestroyAudioStream(audio);
        audio = nullptr;
        if (audioInitialized) SDL_QuitSubSystem(SDL_INIT_AUDIO);
        audioInitialized = false;
    }

    bool queueAudioFrame()
    {
        audioDecoder.GetNextAudioFrame();
        unsigned bytes = std::min(audioDecoder.GetAudioData(0, samples.data()), audioBytes);
        bytes -= bytes % blockAlign;
        return !bytes || SDL_PutAudioStreamData(audio, samples.data(), static_cast<int>(bytes));
    }

    static int mixAudio(void* context)
    {
        auto& stream = *static_cast<BinkStreamState*>(context);
        SDL_SetCurrentThreadPriority(SDL_THREAD_PRIORITY_HIGH);
        while (stream.audioRunning) {
            {
                std::lock_guard<std::mutex> lock(stream.audioMutex);
                if (stream.audioDecoder.GetCurrentFrameNum() < stream.audioDecoder.GetNumFrames() &&
                    SDL_GetAudioStreamQueued(stream.audio) < static_cast<int>(stream.audioQueueLimit)) {
                    if (!stream.queueAudioFrame()) stream.audioRunning = false;
                    continue;
                }
            }
            SDL_Delay(1);
        }
        return 0;
    }

    void openAudio(const char* filename)
    {
        if (!decoder.GetNumAudioTracks()) return;
        if (!audioDecoder.Open(filename)) return;
        const AudioInfo info = decoder.GetAudioTrackDetails(0);
        if (!info.sampleRate || !info.idealBufferSize || (info.nChannels != 1 && info.nChannels != 2)) return;
        audioInitialized = SDL_InitSubSystem(SDL_INIT_AUDIO);
        if (!audioInitialized) return;
        SDL_AudioSpec format{SDL_AUDIO_S16, static_cast<int>(info.nChannels), static_cast<int>(info.sampleRate)};
        audio = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &format, nullptr, nullptr);
        if (!audio) {
            closeAudio();
            return;
        }
        audioBytes = info.idealBufferSize;
        blockAlign = info.nChannels * sizeof(int16_t);
        samples.resize((audioBytes + 1) / 2);
        audioQueueLimit = info.sampleRate * blockAlign;
        // Prime the paused device before playback; subsequent packets are decoded
        // independently of the game's video rendering thread.
        if (!queueAudioFrame() || !SDL_ResumeAudioStreamDevice(audio)) {
            closeAudio();
            return;
        }
        audioRunning = true;
        audioThread = SDL_CreateThread(mixAudio, "Bink audio", this);
        if (!audioThread) closeAudio();
    }

    void seekAudio(unsigned frame)
    {
        if (!audio) return;
        std::lock_guard<std::mutex> lock(audioMutex);
        SDL_PauseAudioStreamDevice(audio);
        SDL_ClearAudioStream(audio);
        audioDecoder.GotoFrame(0);
        // Decode overlap history, but do not play packets before the seek target.
        while (audioDecoder.GetCurrentFrameNum() < frame)
            audioDecoder.GetNextAudioFrame();
        if (audioDecoder.GetCurrentFrameNum() < audioDecoder.GetNumFrames()) queueAudioFrame();
        SDL_ResumeAudioStreamDevice(audio);
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
    if (soundEnabled) stream->openAudio(filename);
    stream->startTime = SDL_GetTicks();
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
        (SDL_GetTicks() - stream.startTime) / 1000.0;
}

void BinkDoFrame(HBINK handle)
{
    if (!handle) return;
    auto& stream = state(handle);
    const unsigned frame = handle->FrameNum - 1;
    if (stream.decodedFrame == static_cast<int>(frame)) return;
    while (stream.decoder.GetCurrentFrameNum() <= frame)
        stream.decoder.GetNextFrame(stream.planes, false);
    stream.decodedFrame = static_cast<int>(frame);
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
    stream.seekAudio(frame);
    stream.startTime = SDL_GetTicks() -
        static_cast<Uint64>(frame * 1000.0 / stream.decoder.GetFrameRate());
}

void BinkSetVolume(HBINK handle, unsigned, int volume)
{
    if (!handle || !state(handle).audio) return;
    SDL_SetAudioStreamGain(state(handle).audio, std::clamp(volume, 0, 32768) / 32768.0f);
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
