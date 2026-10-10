// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstddef>
#include <cstdio>
#include <string>
#include <vector>

namespace Platform {
// Capacities of existing legacy buffers, rather than native filesystem limits.
inline constexpr int LegacyPathCapacity = 260;
inline constexpr int LegacyFileNameCapacity = 256;
inline constexpr int LegacyExtensionCapacity = 256;
std::string WorkingDirectory();
std::string DesktopDirectory();
enum class GameTitle { Generals, ZeroHour };
const char* PreferenceDirectory(GameTitle game);
GameTitle CurrentGame();
const char* ExecutableDirectory();
std::string UserPath(const char* relativePath, GameTitle game = CurrentGame());
std::string ReadPath(const char* path);
std::string WritePath(const char* path);
std::vector<std::string> SearchDirectories(const char* path);
bool HasRootPath(const char* path);
std::string FileName(const char* path);
std::string FileStem(const char* path);
std::FILE* OpenStream(const char* path, const char* mode);
bool CreateUserDirectory(const char* path);
bool RemoveUserFile(const char* path);
bool RenameUserFile(const char* from, const char* to);
bool CopyUserFile(const char* from, const char* to, bool failIfExists);
}
