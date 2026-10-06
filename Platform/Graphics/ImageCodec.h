#pragma once
#include <DirectXTex.h>
#include <filesystem>

namespace Platform::GPU {
HRESULT LoadPlatformImage(const std::filesystem::path& path,
    DirectX::TexMetadata& metadata, DirectX::ScratchImage& image);
}
