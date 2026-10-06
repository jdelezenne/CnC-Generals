// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Paths.h"
#include <SDL3/SDL.h>
#include <cstring>
#include <filesystem>
#include <map>
#include <mutex>
#include <stdexcept>
#include <unordered_map>

namespace fs = std::filesystem;

Platform::GameTitle Platform::CurrentGame()
{
    return std::strcmp(GEN_GAME_NAME, "Generals") == 0 ? GameTitle::Generals : GameTitle::ZeroHour;
}

const char* Platform::PreferenceDirectory(GameTitle game)
{
    static std::string directories[2];
    static std::once_flag initialized[2];
    const unsigned index = game == GameTitle::Generals ? 0 : 1;
    std::call_once(initialized[index], [index] {
        char* path = SDL_GetPrefPath("Electronic Arts", index == 0 ? "Generals" : "ZeroHour");
        if (!path) throw std::runtime_error(SDL_GetError());
        const fs::path native(std::u8string(path, path + std::strlen(path)));
        SDL_free(path);
        directories[index] = native.string();
    });
    return directories[index].c_str();
}

namespace {
fs::path NativePath(const char* name)
{
    std::string text(name);
    for (char& c : text) if (c == '\\') c = '/';
    return fs::path(text).lexically_normal();
}

struct CaseInsensitiveLess {
    bool operator()(const std::string& left, const std::string& right) const
    {
        return SDL_strcasecmp(left.c_str(), right.c_str()) < 0;
    }
};
struct FileNameEntry { fs::path Name; bool Ambiguous = false; };
struct DirectoryIndex {
    fs::file_time_type Modified{};
    bool Ready = false;
    std::map<std::string, FileNameEntry, CaseInsensitiveLess> Names;
};
struct DirectoryCache {
    std::mutex Mutex;
    std::unordered_map<fs::path, DirectoryIndex> Directories;
};
DirectoryCache& PathCache()
{
    static DirectoryCache cache;
    return cache;
}
std::string Utf8Name(const fs::path& path)
{
    const auto name = path.u8string();
    return {reinterpret_cast<const char*>(name.data()), name.size()};
}
fs::path ResolveAssetCase(const fs::path& path)
{
    if (path.empty()) return path;
    std::error_code error;
    if (fs::exists(path, error)) return path;
    const auto parent = path.parent_path();
    if (parent.empty()) return ResolveAssetCase(fs::absolute(path));
    if (parent == path) return path;
    const auto directory = ResolveAssetCase(parent);
    const auto requested = directory / path.filename();
    if (!fs::is_directory(directory, error)) return requested;
    const auto modified = fs::last_write_time(directory, error);
    if (error) return requested;
    auto& cache = PathCache();
    std::lock_guard lock(cache.Mutex);
    auto& index = cache.Directories[directory];
    if (!index.Ready || index.Modified != modified) {
        decltype(index.Names) names;
        for (fs::directory_iterator entry(directory, error), end; !error && entry != end; entry.increment(error)) {
            const auto name = entry->path().filename();
            auto [item, inserted] = names.try_emplace(Utf8Name(name), FileNameEntry{name});
            if (!inserted) item->second.Ambiguous = true;
        }
        if (error) return requested;
        index.Names = std::move(names);
        index.Modified = modified;
        index.Ready = true;
    }
    const auto item = index.Names.find(Utf8Name(path.filename()));
    return item == index.Names.end() || item->second.Ambiguous ? requested : directory / item->second.Name;
}

bool RelativeTo(const fs::path& path, const fs::path& base, fs::path& relative)
{
    auto item = path.begin();
    for (auto root = base.begin(); root != base.end(); ++root) {
        if (root->empty()) continue;
        if (item == path.end()) return false;
#ifdef _WIN32
        if (_wcsicmp(item->c_str(), root->c_str()) != 0) return false;
#else
        if (*item != *root) return false;
#endif
        ++item;
    }
    relative.clear();
    for (; item != path.end(); ++item) relative /= *item;
    return true;
}

fs::path UserRelativePath(const char* name)
{
    const fs::path path = NativePath(name);
    if (!path.is_absolute()) return path;
    fs::path relative;
    if (RelativeTo(path, fs::current_path().lexically_normal(), relative)) return relative;
    const char* base = SDL_GetBasePath();
    if (base && RelativeTo(path, fs::path(std::u8string(base, base + std::strlen(base))).lexically_normal(), relative))
        return relative;
    return path;
}

fs::path PreferencePath(const fs::path& relative)
{
    if (relative.has_root_path() || (!relative.empty() && *relative.begin() == ".."))
        throw std::invalid_argument("User paths must stay inside the preferences folder");
    return fs::path(Platform::PreferenceDirectory(Platform::CurrentGame())) / relative;
}
}

std::string Platform::UserPath(const char* name, GameTitle game)
{
    const fs::path relative = NativePath(name);
    if (relative.has_root_path() || (!relative.empty() && *relative.begin() == ".."))
        throw std::invalid_argument("User paths must stay inside the preferences folder");
    const fs::path path = ResolveAssetCase(fs::path(PreferenceDirectory(game)) / relative);
    fs::create_directories(path.parent_path());
    return path.string();
}

std::string Platform::WritePath(const char* name)
{
    const fs::path relative = UserRelativePath(name);
    if (relative.is_absolute()) return ResolveAssetCase(relative).string();
    return UserPath(relative.string().c_str());
}

std::string Platform::ReadPath(const char* name)
{
    const fs::path relative = UserRelativePath(name);
    if (!relative.is_absolute() && !relative.has_root_path() && (relative.empty() || *relative.begin() != "..")) {
        const fs::path user = ResolveAssetCase(PreferencePath(relative));
        std::error_code error;
        if (fs::is_regular_file(user, error)) return user.string();
    }
    return ResolveAssetCase(NativePath(name)).string();
}

std::vector<std::string> Platform::SearchDirectories(const char* name)
{
    std::vector<std::string> result{ResolveAssetCase(NativePath(name)).string()};
    const fs::path relative = UserRelativePath(name);
    if (!relative.is_absolute() && !relative.has_root_path() && (relative.empty() || *relative.begin() != "..")) {
        const fs::path user = ResolveAssetCase(PreferencePath(relative));
        std::error_code error;
        if (fs::is_directory(user, error)) result.push_back(user.string());
    }
    return result;
}

std::FILE* Platform::OpenStream(const char* path, const char* mode)
{
    const std::string resolved = std::strpbrk(mode, "wa+") ? WritePath(path) : ReadPath(path);
    return std::fopen(resolved.c_str(), mode);
}

bool Platform::HasRootPath(const char* path)
{
    return path && *path && NativePath(path).has_root_path();
}

std::string Platform::FileName(const char* path)
{
    const std::string name(path);
    const auto separator = name.find_last_of("/\\");
    return separator == std::string::npos ? name : name.substr(separator + 1);
}

bool Platform::CreateUserDirectory(const char* name)
{
    std::error_code error;
    const fs::path path(WritePath(name));
    fs::create_directories(path, error);
    return !error;
}

bool Platform::RemoveUserFile(const char* name)
{
    std::error_code error;
    return fs::remove(fs::path(WritePath(name)), error);
}

bool Platform::RenameUserFile(const char* from, const char* to)
{
    std::error_code error;
    fs::rename(fs::path(WritePath(from)), fs::path(WritePath(to)), error);
    return !error;
}

bool Platform::CopyUserFile(const char* from, const char* to, bool failIfExists)
{
    std::error_code error;
    return fs::copy_file(fs::path(ReadPath(from)), fs::path(WritePath(to)),
        failIfExists ? fs::copy_options::none : fs::copy_options::overwrite_existing, error);
}
