// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Settings.h"
#include <SDL3/SDL.h>
#include <charconv>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <string>

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
