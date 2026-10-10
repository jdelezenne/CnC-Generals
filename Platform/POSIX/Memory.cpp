// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Memory.h"
#include <cstdlib>
#include <algorithm>
#include <cstdint>
#include <unistd.h>
#ifdef __APPLE__
#include <malloc/malloc.h>
#include <mach/mach.h>
#else
#include <malloc.h>
#include <sys/uio.h>
#endif

bool Platform::SystemMemoryReadable(const void* memory, std::size_t bytes)
{
    if (!bytes) return true;
    if (!memory) return false;
    unsigned char buffer[256];
    auto address = reinterpret_cast<std::uintptr_t>(memory);
    while (bytes) {
        const auto count = std::min(bytes, sizeof(buffer));
#ifdef __APPLE__
        mach_vm_size_t read = 0;
        if (mach_vm_read_overwrite(mach_task_self(), address, count,
                reinterpret_cast<mach_vm_address_t>(buffer), &read) != KERN_SUCCESS || read != count) return false;
#else
        iovec local{buffer, count}, remote{reinterpret_cast<void*>(address), count};
        if (process_vm_readv(getpid(), &local, 1, &remote, 1, 0) != static_cast<ssize_t>(count)) return false;
#endif
        address += count;
        bytes -= count;
    }
    return true;
}

void* Platform::AllocateSystemMemory(std::size_t bytes, bool zero)
{
    return zero ? std::calloc(1, bytes) : std::malloc(bytes);
}
void Platform::FreeSystemMemory(void* memory) { std::free(memory); }
std::size_t Platform::SystemMemorySize(void* memory)
{
    if (!memory) return 0;
#ifdef __APPLE__
    return malloc_size(memory);
#else
    return malloc_usable_size(memory);
#endif
}
