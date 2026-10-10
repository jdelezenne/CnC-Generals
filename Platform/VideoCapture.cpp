// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/VideoCapture.h"
#include "Platform/Paths.h"
#include <SDL3/SDL_iostream.h>
#include <cstdint>
#include <limits>
#include <memory>
#include <vector>

namespace Platform {
struct VideoCapture {
    SDL_IOStream* file = nullptr;
    std::uint32_t frameBytes = 0;
    Sint64 totalFrames = 0, streamFrames = 0, movieSize = 0, movieStart = 0;
    std::vector<std::uint32_t> offsets;
    bool failed = false;
    void bytes(const void* data, std::size_t size) {
        if (SDL_WriteIO(file, data, size) != size) failed = true;
    }
    void tag(const char* text) { bytes(text, 4); }
    void number(std::uint32_t value) {
        const unsigned char encoded[] = {static_cast<unsigned char>(value), static_cast<unsigned char>(value >> 8),
            static_cast<unsigned char>(value >> 16), static_cast<unsigned char>(value >> 24)};
        bytes(encoded, sizeof(encoded));
    }
    void patch(Sint64 position, std::uint32_t value) {
        if (SDL_SeekIO(file, position, SDL_IO_SEEK_SET) != position) failed = true;
        number(value);
    }
};
}

Platform::VideoCapture* Platform::OpenVideoCapture(const char* filename, int width, int height, int bits, int rate)
{
    if (width <= 0 || height <= 0 || rate <= 0 || (bits != 16 && bits != 24 && bits != 32)) return nullptr;
    const std::uint64_t size = ((static_cast<std::uint64_t>(width) * bits + 31) / 32) * 4 * height;
    if (size > std::numeric_limits<std::uint32_t>::max()) return nullptr;
    auto capture = std::make_unique<VideoCapture>();
    const std::string path = WritePath(filename);
    capture->file = SDL_IOFromFile(path.c_str(), "wb");
    if (!capture->file) return nullptr;
    capture->frameBytes = static_cast<std::uint32_t>(size);
    auto& output = *capture;
    output.tag("RIFF"); output.number(0); output.tag("AVI ");
    output.tag("LIST"); output.number(200); output.tag("hdrl");
    output.tag("avih"); output.number(56);
    output.number(1000000 / rate); output.number(0); output.number(0); output.number(0x10); // AVIF_HASINDEX
    output.totalFrames = SDL_TellIO(output.file);
    output.number(0); output.number(0); output.number(1); output.number(output.frameBytes);
    output.number(width); output.number(height);
    for (int i = 0; i < 4; ++i) output.number(0);
    output.tag("LIST"); output.number(124); output.tag("strl");
    output.tag("strh"); output.number(64);
    output.tag("vids"); output.tag("DIB ");
    output.number(0); output.number(0); output.number(0); // flags, priority/language, initial frames
    output.number(1); output.number(rate); output.number(0);
    output.streamFrames = SDL_TellIO(output.file);
    output.number(0); output.number(output.frameBytes); output.number(0); output.number(0);
    output.number(0); output.number(0); output.number(width); output.number(height);
    output.tag("strf"); output.number(40);
    output.number(40); output.number(width); output.number(height);
    output.number(1 | (static_cast<std::uint32_t>(bits) << 16)); // planes/bit count
    output.number(0); output.number(output.frameBytes); // BI_RGB
    output.number(1); output.number(1); output.number(0); output.number(0);
    output.tag("LIST"); output.movieSize = SDL_TellIO(output.file); output.number(0);
    output.movieStart = SDL_TellIO(output.file); output.tag("movi");
    if (output.failed) { SDL_CloseIO(output.file); return nullptr; }
    return capture.release();
}

std::size_t Platform::VideoCaptureFrameSize(const VideoCapture* capture)
{
    return capture ? capture->frameBytes : 0;
}

bool Platform::WriteVideoCapture(VideoCapture* capture, const void* pixels)
{
    if (!capture || capture->failed || !pixels) return false;
    const Sint64 position = SDL_TellIO(capture->file);
    // Classic AVI has 32-bit chunk lengths and an index. Reserve space for finalization.
    const std::uint64_t end = static_cast<std::uint64_t>(position) + 8 + capture->frameBytes +
        8 + (static_cast<std::uint64_t>(capture->offsets.size()) + 1) * 16;
    if (position < 0 || end > std::numeric_limits<std::uint32_t>::max()) { capture->failed = true; return false; }
    capture->offsets.push_back(static_cast<std::uint32_t>(position - capture->movieStart));
    capture->tag("00db"); capture->number(capture->frameBytes);
    capture->bytes(pixels, capture->frameBytes);
    return !capture->failed;
}

void Platform::CloseVideoCapture(VideoCapture* capture)
{
    if (!capture) return;
    const Sint64 movieEnd = SDL_TellIO(capture->file);
    capture->tag("idx1"); capture->number(static_cast<std::uint32_t>(capture->offsets.size() * 16));
    for (const auto offset : capture->offsets) {
        capture->tag("00db"); capture->number(0x10); capture->number(offset); capture->number(capture->frameBytes);
    }
    const Sint64 end = SDL_TellIO(capture->file);
    capture->patch(4, static_cast<std::uint32_t>(end - 8));
    capture->patch(capture->movieSize, static_cast<std::uint32_t>(movieEnd - capture->movieStart));
    capture->patch(capture->totalFrames, static_cast<std::uint32_t>(capture->offsets.size()));
    capture->patch(capture->streamFrames, static_cast<std::uint32_t>(capture->offsets.size()));
    SDL_CloseIO(capture->file);
    delete capture;
}
