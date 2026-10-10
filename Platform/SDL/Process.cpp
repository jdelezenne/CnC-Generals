// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Application.h"
#include "Platform/Paths.h"
#include <SDL3/SDL.h>
#include <string>

bool Platform::LaunchDetachedTool(const char* basename)
{
    std::string filename = basename;
#ifdef _WIN32
    filename += ".exe";
#endif
    filename = ReadPath(filename.c_str());
    const char* arguments[] = {filename.c_str(), nullptr};
    const auto properties = SDL_CreateProperties();
    if (!properties) return false;
    SDL_SetPointerProperty(properties, SDL_PROP_PROCESS_CREATE_ARGS_POINTER, const_cast<const char**>(arguments));
    SDL_SetBooleanProperty(properties, SDL_PROP_PROCESS_CREATE_BACKGROUND_BOOLEAN, true);
    auto* process = SDL_CreateProcessWithProperties(properties);
    SDL_DestroyProperties(properties);
    if (!process) return false;
    SDL_DestroyProcess(process);
    return true;
}
