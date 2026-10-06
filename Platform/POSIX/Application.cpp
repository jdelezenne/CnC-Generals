// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Application.h"
#include "Platform/Paths.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_stdinc.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>

namespace {
struct ApplicationInstance { int descriptor; };
}

Platform::ApplicationInstanceResult Platform::AcquireApplicationInstance(const char* identifier, void*& handle)
{
    handle = nullptr;
    // Both titles use the original Generals identifier and share one lock.
    const std::string path = UserPath((std::string(identifier) + ".lock").c_str(), GameTitle::Generals);
    const int descriptor = open(path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0600);
    if (descriptor < 0) {
        SDL_SetError("Could not open application lock: %s", std::strerror(errno));
        return ApplicationInstanceResult::Error;
    }
    if (flock(descriptor, LOCK_EX | LOCK_NB) != 0) {
        const int error = errno;
        close(descriptor);
        if (error == EWOULDBLOCK || error == EAGAIN) return ApplicationInstanceResult::AlreadyRunning;
        SDL_SetError("Could not acquire application lock: %s", std::strerror(error));
        return ApplicationInstanceResult::Error;
    }
    auto* instance = static_cast<ApplicationInstance*>(SDL_malloc(sizeof(ApplicationInstance)));
    if (!instance) {
        close(descriptor);
        return ApplicationInstanceResult::Error;
    }
    instance->descriptor = descriptor;
    handle = instance;
    return ApplicationInstanceResult::Acquired;
}

void Platform::ReleaseApplicationInstance(void* handle)
{
    auto* instance = static_cast<ApplicationInstance*>(handle);
    if (!instance) return;
    close(instance->descriptor);
    SDL_free(instance);
}
