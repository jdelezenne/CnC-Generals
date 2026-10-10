// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/FileSystem.h"
#include "Platform/Paths.h"
#include <SDL3/SDL.h>
#include <filesystem>
#include <string_view>
namespace {
bool Match(std::string_view name, std::string_view pattern)
{
    if (pattern == "*.*") pattern = "*";
    if (pattern == "*.") return name.find('.') == std::string_view::npos;
    if (pattern.empty()) return name.empty();
    if (pattern[0] == '*') {
        if (pattern.size() == 1) return true;
        for (std::size_t i = 0; i <= name.size(); ++i)
            if (Match(name.substr(i), pattern.substr(1))) return true;
        return false;
    }
    if (pattern[0] == '?') {
        if (name.empty() || name[0] == '.') return Match(name, pattern.substr(1));
        return Match(name.substr(1), pattern.substr(1));
    }
    if (name.empty()) return pattern == "." || pattern == ".*";
    if (SDL_tolower(static_cast<unsigned char>(name[0])) != SDL_tolower(static_cast<unsigned char>(pattern[0]))) return false;
    return Match(name.substr(1), pattern.substr(1));
}
Platform::FileMetadata Metadata(const SDL_PathInfo& info)
{
    return {info.size, static_cast<std::uint64_t>(info.modify_time / 100 + 116444736000000000LL),
        info.type == SDL_PATHTYPE_DIRECTORY};
}
struct Enumeration { std::vector<Platform::FileSystemEntry> entries; const char* pattern; };
SDL_EnumerationResult SDLCALL Enumerate(void* userdata, const char* directory, const char* name)
{
    auto& context = *static_cast<Enumeration*>(userdata);
    if (!Match(name, context.pattern)) return SDL_ENUM_CONTINUE;
    SDL_PathInfo info{};
    const auto path = (std::filesystem::path(directory) / name).string();
    if (SDL_GetPathInfo(path.c_str(), &info)) context.entries.push_back({name, Metadata(info)});
    return SDL_ENUM_CONTINUE;
}
}
bool Platform::ReadFileMetadata(const char* path, FileMetadata& info)
{
    SDL_PathInfo native{};
    if (!SDL_GetPathInfo(ReadPath(path).c_str(), &native)) return false;
    info = Metadata(native);
    return true;
}
std::vector<Platform::FileSystemEntry> Platform::ReadDirectory(const char* directory, const char* pattern)
{
    Enumeration context{{}, pattern};
    SDL_EnumerateDirectory(ReadPath(directory && *directory ? directory : ".").c_str(), Enumerate, &context);
    return context.entries;
}
