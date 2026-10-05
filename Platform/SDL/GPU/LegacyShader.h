#pragma once
#include <d3d8.h>
#include <SDL3/SDL_gpu.h>
#include <string>
#include <vector>

namespace Platform::GPU {
bool ReadShader(const DWORD* code, std::vector<DWORD>& result, std::string& error);
bool TranslateShader(const std::vector<DWORD>& code, bool vertex, std::string& source, std::string& error);
SDL_GPUShader* CompileShader(SDL_GPUDevice* device, const std::string& source, bool vertex, bool spirv);
}
