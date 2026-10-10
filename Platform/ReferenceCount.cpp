// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/ReferenceCount.h"
#include <atomic>
#include "Platform/Synchronization.h"

void Platform::IncrementReferenceCount(std::uint16_t& count)
{
    std::atomic_ref<std::uint16_t>(count).fetch_add(1, std::memory_order_seq_cst);
}
void Platform::DecrementReferenceCount(std::uint16_t& count)
{
    std::atomic_ref<std::uint16_t>(count).fetch_sub(1, std::memory_order_seq_cst);
}

bool Platform::TryAcquireSpinFlag(unsigned int& flag)
{
    return (std::atomic_ref<unsigned int>(flag).fetch_or(1, std::memory_order_acquire) & 1) == 0;
}
void Platform::ReleaseSpinFlag(unsigned int& flag)
{
    std::atomic_ref<unsigned int>(flag).store(0, std::memory_order_release);
}
