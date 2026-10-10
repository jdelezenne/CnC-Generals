// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Settings.h"
#include <SDL3/SDL.h>
#include <charconv>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <string>
#include <mutex>
#include <vector>

namespace {
std::string Trim(std::string text)
{
    const auto first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return {};
    return text.substr(first, text.find_last_not_of(" \t\r\n") - first + 1);
}
std::string SectionName(const char* section)
{
    std::string target = section;
    for (char& c : target) if (c == '\\') c = '/';
    if (target.empty()) target = "Installation";
    else if (target.front() == '/') target.erase(0, 1);
    return target;
}

bool Read(Platform::GameTitle game, const char* section, const char* name, std::string& value)
{
    std::ifstream file(std::filesystem::path(Platform::UserPath("Settings.ini", game)));
    std::string active, line;
    const std::string target = SectionName(section);
    while (std::getline(file, line)) {
        line = Trim(line);
        if (line.empty() || line.front() == ';' || line.front() == '#') continue;
        if (line.front() == '[' && line.back() == ']') {
            active = Trim(line.substr(1, line.size() - 2));
            continue;
        }
        const auto equal = line.find('=');
        if (equal == std::string::npos || SDL_strcasecmp(active.c_str(), target.c_str())) continue;
        const auto key = Trim(line.substr(0, equal));
        if (SDL_strcasecmp(key.c_str(), name)) continue;
        value = Trim(line.substr(equal + 1));
        if (value.size() >= 2 && value.front() == '"' && value.back() == '"')
            value = value.substr(1, value.size() - 2);
        return true;
    }
    return false;
}

}

bool Platform::ReadInstallationString(GameTitle game, const char* section, const char* name,
    char* value, std::size_t capacity)
{
    std::string text;
    if (!Read(game, section, name, text)) return false;
    if (text.size() >= capacity) return false;
    std::memcpy(value, text.c_str(), text.size() + 1);
    return true;
}

bool Platform::ReadInstallationUnsigned(GameTitle game, const char* section, const char* name,
    unsigned int& value)
{
    std::string text;
    if (!Read(game, section, name, text)) return false;
    unsigned int parsed;
    const auto result = std::from_chars(text.data(), text.data() + text.size(), parsed);
    if (result.ec != std::errc{} || result.ptr != text.data() + text.size()) return false;
    value = parsed;
    return true;
}

bool Platform::WriteInstallationString(GameTitle game, const char* section, const char* name,
    const char* value)
{
    if (std::strpbrk(section, "\r\n]") || std::strpbrk(name, "\r\n=") ||
        std::strpbrk(value, "\r\n")) return false;
    static std::mutex mutex;
    const std::lock_guard<std::mutex> lock(mutex);
    const std::string path = UserPath("Settings.ini", game);
    std::ifstream input{std::filesystem::path(path)};
    std::vector<std::string> lines;
    const std::string target = SectionName(section);
    std::string active, line;
    bool foundSection = false, written = false;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        const std::string trimmed = Trim(line);
        if (!trimmed.empty() && trimmed.front() == '[' && trimmed.back() == ']') {
            if (!written && !SDL_strcasecmp(active.c_str(), target.c_str())) {
                lines.push_back(std::string(name) + "=" + value);
                written = true;
            }
            active = Trim(trimmed.substr(1, trimmed.size() - 2));
            foundSection |= !SDL_strcasecmp(active.c_str(), target.c_str());
        } else if (!trimmed.empty() && trimmed.front() != '#' && trimmed.front() != ';') {
            const auto equal = trimmed.find('=');
            if (equal != std::string::npos && !SDL_strcasecmp(active.c_str(), target.c_str()) &&
                !SDL_strcasecmp(Trim(trimmed.substr(0, equal)).c_str(), name)) {
                if (!written) lines.push_back(std::string(name) + "=" + value);
                written = true;
                continue;
            }
        }
        lines.push_back(line);
    }
    if (input.bad()) return false;
    input.close();
    if (!written) {
        if (!foundSection) lines.push_back("[" + target + "]");
        lines.push_back(std::string(name) + "=" + value);
    }
    const std::string temporary = path + ".tmp";
    std::ofstream output{std::filesystem::path(temporary), std::ios::trunc};
    for (const auto& entry : lines) output << entry << '\n';
    output.close();
    if (output.fail()) { SDL_RemovePath(temporary.c_str()); return false; }
    if (SDL_RenamePath(temporary.c_str(), path.c_str())) return true;
    SDL_RemovePath(temporary.c_str());
    return false;
}

bool Platform::WriteInstallationUnsigned(GameTitle game, const char* section, const char* name,
    unsigned int value)
{
    return WriteInstallationString(game, section, name, std::to_string(value).c_str());
}
