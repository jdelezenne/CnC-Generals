// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

namespace Platform {
void* CreateNamedMutex(const char* name);
void DestroyNamedMutex(void* mutex);
bool LockNamedMutex(void* mutex, int timeoutMilliseconds);
bool UnlockNamedMutex(void* mutex);
}
