#pragma once
#include <d3d8.h>
#include <string>
#include <string_view>
#include <vector>
namespace Platform::GPU {
bool AssembleShader(std::string_view text,std::vector<DWORD>& code,std::string& error);
}
