// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace Platform {
struct FileMetadata { std::uint64_t size, lastWriteTime; bool directory; };
struct FileSystemEntry { std::string name; FileMetadata info; };
bool ReadFileMetadata(const char* path, FileMetadata& info);
std::vector<FileSystemEntry> ReadDirectory(const char* directory, const char* pattern);
}
