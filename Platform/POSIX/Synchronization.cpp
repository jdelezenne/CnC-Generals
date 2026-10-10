// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/NativeSynchronization.h"
#include <SDL3/SDL.h>
#include <atomic>
#include <cerrno>
#include <chrono>
#include <fcntl.h>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <sys/file.h>
#include <unistd.h>

namespace {
struct NamedMutex {
    int descriptor;
    std::recursive_timed_mutex local;
    std::atomic<SDL_ThreadID> owner{0};
    unsigned depth = 0;
    explicit NamedMutex(int file) : descriptor(file) {}
    ~NamedMutex() { close(descriptor); }
};
using Handle = std::shared_ptr<NamedMutex>;
std::mutex CacheMutex;
std::map<std::string, std::weak_ptr<NamedMutex>> Cache;
int FileLock(int descriptor, int operation)
{
    int result;
    do { result = flock(descriptor, operation); } while (result != 0 && errno == EINTR);
    return result;
}
}

void* Platform::CreateNamedMutex(const char* name)
{
    if (!name || !*name) return nullptr;
    const std::lock_guard<std::mutex> lock(CacheMutex);
    if (auto mutex = Cache[name].lock()) return new Handle(std::move(mutex));
    char* directory = SDL_GetPrefPath("Electronic Arts", "Synchronization");
    if (!directory) return nullptr;
    std::string path(directory);
    SDL_free(directory);
    constexpr char digits[] = "0123456789abcdef";
    for (const auto* c = reinterpret_cast<const unsigned char*>(name); *c; ++c) {
        path += digits[*c >> 4]; path += digits[*c & 15];
    }
    path += ".lock";
    const int file = open(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0600);
    if (file == -1) return nullptr;
    auto mutex = std::make_shared<NamedMutex>(file);
    Cache[name] = mutex;
    return new Handle(std::move(mutex));
}

void Platform::DestroyNamedMutex(void* mutex) { delete static_cast<Handle*>(mutex); }

bool Platform::LockNamedMutex(void* handle, int timeoutMilliseconds)
{
    if (!handle) return false;
    auto& mutex = **static_cast<Handle*>(handle);
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMilliseconds);
    if (timeoutMilliseconds == -1) mutex.local.lock();
    else if (!mutex.local.try_lock_until(deadline)) return false;
    const auto current = SDL_GetCurrentThreadID();
    if (mutex.owner == current) { ++mutex.depth; return true; }
    bool acquired;
    if (timeoutMilliseconds == -1) acquired = FileLock(mutex.descriptor, LOCK_EX) == 0;
    else {
        for (;;) {
            acquired = FileLock(mutex.descriptor, LOCK_EX | LOCK_NB) == 0;
            if (acquired || (errno != EWOULDBLOCK && errno != EAGAIN) || std::chrono::steady_clock::now() >= deadline) break;
            SDL_Delay(1);
        }
    }
    if (!acquired) { mutex.local.unlock(); return false; }
    mutex.depth = 1;
    mutex.owner = current;
    return true;
}

bool Platform::UnlockNamedMutex(void* handle)
{
    if (!handle) return false;
    auto& mutex = **static_cast<Handle*>(handle);
    if (mutex.owner != SDL_GetCurrentThreadID()) return false;
    if (mutex.depth == 1) {
        if (FileLock(mutex.descriptor, LOCK_UN) != 0) return false;
        mutex.depth = 0;
        mutex.owner = 0;
    } else --mutex.depth;
    mutex.local.unlock();
    return true;
}
