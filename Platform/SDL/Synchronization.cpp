// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Synchronization.h"
#ifdef _WIN32
#include "Platform/NativeSynchronization.h"
#endif
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
#ifdef _WIN32
        mutex->named = CreateNamedMutex(name);
        if (!mutex->named) { delete mutex; return nullptr; }
#else
        delete mutex;
        // All compiled game mutexes are unnamed. Do not silently lose process isolation.
        throw std::runtime_error("Named interprocess mutexes require a native implementation");
#endif
    }
    return mutex;
}
void Platform::DestroyRecursiveMutex(void* handle)
{
    auto* mutex = static_cast<RecursiveMutex*>(handle);
    if (!mutex) return;
#ifdef _WIN32
    if (mutex->named) DestroyNamedMutex(mutex->named);
#endif
    delete mutex;
}
bool Platform::LockRecursiveMutex(void* handle, int timeoutMilliseconds)
{
    auto* mutex = static_cast<RecursiveMutex*>(handle);
    if (!mutex) return false;
#ifdef _WIN32
    if (mutex->named) return LockNamedMutex(mutex->named, timeoutMilliseconds);
#endif
    if (timeoutMilliseconds == -1) { mutex->local.lock(); return true; }
    if (timeoutMilliseconds == 0) return mutex->local.try_lock();
    return mutex->local.try_lock_for(std::chrono::milliseconds(timeoutMilliseconds));
}
bool Platform::UnlockRecursiveMutex(void* handle)
{
    auto* mutex = static_cast<RecursiveMutex*>(handle);
    if (!mutex) return false;
#ifdef _WIN32
    if (mutex->named) return UnlockNamedMutex(mutex->named);
#endif
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
