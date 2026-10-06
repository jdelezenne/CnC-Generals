#include "Platform/Graphics/ImageCodec.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <cstring>
#include <memory>

namespace {
bool IsSRGBPNG(SDL_IOStream* file)
{
    const unsigned char signature[8] = {137,80,78,71,13,10,26,10};
    unsigned char header[8];
    if (SDL_SeekIO(file, 0, SDL_IO_SEEK_SET) < 0 ||
        SDL_ReadIO(file, header, sizeof(header)) != sizeof(header) ||
        std::memcmp(header, signature, sizeof(header))) return false;

    const auto size = SDL_GetIOSize(file);
    bool srgbGamma = false;
    while (SDL_ReadIO(file, header, sizeof(header)) == sizeof(header)) {
        const Uint32 length = (Uint32(header[0])<<24) | (Uint32(header[1])<<16) |
            (Uint32(header[2])<<8) | header[3];
        const auto payload = SDL_TellIO(file);
        if (payload < 0 || size < payload || Uint64(length)+4 > Uint64(size-payload)) return false;
        if (!std::memcmp(header+4, "sRGB", 4) && length == 1) return true;
        if (!std::memcmp(header+4, "gAMA", 4) && length == 4) {
            unsigned char gamma[4];
            if (SDL_ReadIO(file, gamma, sizeof(gamma)) != sizeof(gamma)) return false;
            const Uint32 value = (Uint32(gamma[0])<<24) | (Uint32(gamma[1])<<16) |
                (Uint32(gamma[2])<<8) | gamma[3];
            srgbGamma = value == 45455;
        }
        if (!std::memcmp(header+4, "IEND", 4)) return srgbGamma;
        if (SDL_SeekIO(file, payload+length+4, SDL_IO_SEEK_SET) < 0) return false;
    }
    return false;
}
}

HRESULT Platform::GPU::LoadPlatformImage(const std::filesystem::path& path,
    DirectX::TexMetadata& metadata, DirectX::ScratchImage& image)
{
    using Surface = std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)>;
    using File = std::unique_ptr<SDL_IOStream, decltype(&SDL_CloseIO)>;
    const auto filename = path.u8string();
    File file(SDL_IOFromFile(reinterpret_cast<const char*>(filename.c_str()), "rb"), SDL_CloseIO);
    if (!file) return E_FAIL;
    Surface source(IMG_Load_IO(file.get(), false), SDL_DestroySurface);
    if (!source) return E_FAIL;
    Surface pixels(SDL_ConvertSurface(source.get(), SDL_PIXELFORMAT_BGRA32), SDL_DestroySurface);
    if (!pixels) return E_OUTOFMEMORY;
    const auto format = IsSRGBPNG(file.get()) ? DXGI_FORMAT_B8G8R8A8_UNORM_SRGB : DXGI_FORMAT_B8G8R8A8_UNORM;
    HRESULT result = image.Initialize2D(format, pixels->w, pixels->h, 1, 1);
    if (FAILED(result)) return result;
    const auto* output = image.GetImage(0, 0, 0);
    for (int y = 0; y < pixels->h; ++y) {
        std::memcpy(output->pixels + y*output->rowPitch,
            static_cast<const unsigned char*>(pixels->pixels) + y*pixels->pitch,
            static_cast<std::size_t>(pixels->w)*4);
    }
    metadata = image.GetMetadata();
    return S_OK;
}
