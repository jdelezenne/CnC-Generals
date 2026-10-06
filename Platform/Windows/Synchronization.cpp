// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/NativeSynchronization.h"
#include <windows.h>

void* Platform::CreateNamedMutex(const char* name) { return CreateMutexA(nullptr, FALSE, name); }
void Platform::DestroyNamedMutex(void* mutex) { CloseHandle(mutex); }
bool Platform::LockNamedMutex(void* mutex, int timeoutMilliseconds)
{
    return WaitForSingleObject(mutex, timeoutMilliseconds == -1 ? INFINITE : DWORD(timeoutMilliseconds)) == WAIT_OBJECT_0;
}
bool Platform::UnlockNamedMutex(void* mutex) { return ReleaseMutex(mutex) != FALSE; }
