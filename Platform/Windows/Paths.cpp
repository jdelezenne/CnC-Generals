// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Paths.h"
#include <windows.h>
#include <shlobj.h>
#include <cstring>
#include <string>

bool Platform::UserDataDirectory(GameTitle, const char* legacyLeafName, char* directory, std::size_t capacity)
{
    char documents[MAX_PATH];
    if (!SHGetSpecialFolderPathA(nullptr, documents, CSIDL_PERSONAL, true)) return false;
    std::string path = documents;
    if (path.back() != '\\') path += '\\';
    path += legacyLeafName;
    if (path.back() != '\\') path += '\\';
    if (path.size() >= capacity) return false;
    CreateDirectoryA(path.c_str(), nullptr);
    std::memcpy(directory, path.c_str(), path.size() + 1);
    return true;
}
