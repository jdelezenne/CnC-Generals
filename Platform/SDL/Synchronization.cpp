// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Synchronization.h"
#include "Platform/NativeSynchronization.h"
#include <SDL3/SDL.h>
#include <chrono>
#include <mutex>
#include <new>
#include <stdexcept>

namespace {
struct RecursiveMutex {
    std::recursive_timed_mutex local;
    void* named = nullptr;
};
}

void* Platform::CreateRecursiveMutex(const char* name)
{
    auto* mutex = new (std::nothrow) RecursiveMutex;
    if (mutex && name && *name) {
        mutex->named = CreateNamedMutex(name);
        if (!mutex->named) { delete mutex; return nullptr; }
    }
    return mutex;
}
void Platform::DestroyRecursiveMutex(void* handle)
{
    auto* mutex = static_cast<RecursiveMutex*>(handle);
    if (!mutex) return;
    if (mutex->named) DestroyNamedMutex(mutex->named);
    delete mutex;
}
bool Platform::LockRecursiveMutex(void* handle, int timeoutMilliseconds)
{
    auto* mutex = static_cast<RecursiveMutex*>(handle);
    if (!mutex) return false;
    if (mutex->named) return LockNamedMutex(mutex->named, timeoutMilliseconds);
    if (timeoutMilliseconds == -1) { mutex->local.lock(); return true; }
    if (timeoutMilliseconds == 0) return mutex->local.try_lock();
    return mutex->local.try_lock_for(std::chrono::milliseconds(timeoutMilliseconds));
}
bool Platform::UnlockRecursiveMutex(void* handle)
{
    auto* mutex = static_cast<RecursiveMutex*>(handle);
    if (!mutex) return false;
    if (mutex->named) return UnlockNamedMutex(mutex->named);
    mutex->local.unlock();
    return true;
}
void* Platform::CreateCriticalSection()
{
    auto* section = SDL_CreateMutex();
    if (!section) throw std::runtime_error(SDL_GetError());
    return section;
}
void Platform::DestroyCriticalSection(void* section) { SDL_DestroyMutex(static_cast<SDL_Mutex*>(section)); }
void Platform::EnterCriticalSection(void* section) { SDL_LockMutex(static_cast<SDL_Mutex*>(section)); }
void Platform::LeaveCriticalSection(void* section) { SDL_UnlockMutex(static_cast<SDL_Mutex*>(section)); }
