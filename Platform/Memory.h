// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstddef>
namespace Platform {
void* AllocateSystemMemory(std::size_t bytes, bool zero);
void FreeSystemMemory(void* memory);
std::size_t SystemMemorySize(void* memory);
bool SystemMemoryReadable(const void* memory, std::size_t bytes);
}
