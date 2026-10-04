// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Paths.h"
#include <SDL3/SDL.h>
#include <cstring>
#include <mutex>
#include <string>

const char* Platform::PreferenceDirectory(GameTitle game)
{
    static std::string directories[2];
    static std::once_flag initialized[2];
    const unsigned index = game == GameTitle::Generals ? 0 : 1;
    std::call_once(initialized[index], [index] {
        char* path = SDL_GetPrefPath("Electronic Arts", index == 0 ? "Generals" : "ZeroHour");
        if (path) {
            directories[index] = path;
            SDL_free(path);
        }
    });
    return directories[index].c_str();
}

#ifndef _WIN32
bool Platform::UserDataDirectory(GameTitle game, const char*, char* directory, std::size_t capacity)
{
    const char* path = PreferenceDirectory(game);
    const auto size = std::strlen(path);
    if (!size || size >= capacity) return false;
    std::memcpy(directory, path, size + 1);
    return true;
}
#endif
