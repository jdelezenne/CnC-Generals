// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstdint>

namespace Platform {
void* CreateRecursiveMutex(const char* name = nullptr);
void DestroyRecursiveMutex(void* mutex);
bool LockRecursiveMutex(void* mutex, int timeoutMilliseconds);
bool UnlockRecursiveMutex(void* mutex);
void* CreateCriticalSection();
void DestroyCriticalSection(void* section);
void EnterCriticalSection(void* section);
void LeaveCriticalSection(void* section);
bool TryAcquireSpinFlag(unsigned int& flag);
void ReleaseSpinFlag(unsigned int& flag);
}
