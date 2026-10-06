#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace Platform::GPU {
bool CompileHLSL(const std::string& source, bool vertex, bool spirv,
    std::vector<std::uint8_t>& bytecode, std::string& error);
}
