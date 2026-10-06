// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Application.h"
#include <windows.h>

Platform::ApplicationInstanceResult Platform::AcquireApplicationInstance(const char* identifier, void*& handle)
{
    handle = CreateMutexA(nullptr, FALSE, identifier);
    const DWORD error = GetLastError();
    if (!handle) return ApplicationInstanceResult::Error;
    if (error != ERROR_ALREADY_EXISTS) return ApplicationInstanceResult::Acquired;

    if (HWND window = FindWindowA(identifier, nullptr)) {
        SetForegroundWindow(window);
        ShowWindow(window, SW_RESTORE);
    }
    ReleaseApplicationInstance(handle);
    handle = nullptr;
    return ApplicationInstanceResult::AlreadyRunning;
}

void Platform::ReleaseApplicationInstance(void* handle)
{
    if (handle) CloseHandle(handle);
}
