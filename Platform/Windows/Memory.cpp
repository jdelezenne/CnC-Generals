// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Memory.h"
#include <windows.h>
#include <algorithm>
#include <cstdint>

bool Platform::SystemMemoryReadable(const void* memory, std::size_t bytes)
{
    unsigned char buffer[256];
    auto address = reinterpret_cast<std::uintptr_t>(memory);
    while (bytes) {
        const auto count = (std::min)(bytes, sizeof(buffer));
        SIZE_T read = 0;
        if (!ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<const void*>(address), buffer, count, &read) || read != count) return false;
        address += count;
        bytes -= count;
    }
    return true;
}

void* Platform::AllocateSystemMemory(std::size_t bytes, bool zero)
{
    return GlobalAlloc(GMEM_FIXED | (zero ? GMEM_ZEROINIT : 0), bytes);
}
void Platform::FreeSystemMemory(void* memory) { GlobalFree(memory); }
std::size_t Platform::SystemMemorySize(void* memory) { return GlobalSize(memory); }
