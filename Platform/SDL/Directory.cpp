// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Directory.h"
#include "Platform/Paths.h"
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_stdinc.h>
#include <filesystem>
#include <memory>

std::vector<Platform::DirectoryEntry> Platform::ListFiles(const char* directory, const char* pattern)
{
    const std::string resolved = ReadPath(directory && *directory ? directory : ".");
    int count = 0;
    std::unique_ptr<char*, decltype(&SDL_free)> names(
        SDL_GlobDirectory(resolved.c_str(), pattern, SDL_GLOB_CASEINSENSITIVE, &count), SDL_free);
    std::vector<DirectoryEntry> entries;
    for (int i = 0; names && i < count; ++i) {
        const auto path = (std::filesystem::path(resolved) / names.get()[i]).string();
        SDL_PathInfo info{};
        if (!SDL_GetPathInfo(path.c_str(), &info) || info.type != SDL_PATHTYPE_FILE) continue;
        entries.push_back({names.get()[i], path});
    }
    return entries;
}

bool Platform::PathExists(const char* path)
{
    SDL_PathInfo info{};
    return SDL_GetPathInfo(ReadPath(path).c_str(), &info);
}
