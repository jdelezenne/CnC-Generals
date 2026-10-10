// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <string>
#include <vector>

namespace Platform {
struct DirectoryEntry { std::string name, path; };
std::vector<DirectoryEntry> ListFiles(const char* directory, const char* pattern = "*");
bool PathExists(const char* path);
}
