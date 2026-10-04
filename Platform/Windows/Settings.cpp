// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Settings.h"
#include <windows.h>
#include <string>

namespace {
std::string RegistryPath(Platform::GameTitle game, const char* section)
{
    std::string path = "SOFTWARE\\Electronic Arts\\EA Games\\";
    path += game == Platform::GameTitle::Generals ? "Generals" : "Command and Conquer Generals Zero Hour";
    return path + section;
}
bool ReadValue(HKEY root, const char* path, const char* name, void* value, DWORD& size)
{
    HKEY handle;
    if (RegOpenKeyExA(root, path, 0, KEY_READ, &handle) != ERROR_SUCCESS) return false;
    const LONG status = RegQueryValueExA(handle, name, nullptr, nullptr, static_cast<BYTE*>(value), &size);
    RegCloseKey(handle);
    return status == ERROR_SUCCESS;
}
}

bool Platform::ReadInstallationString(GameTitle game, const char* section, const char* name,
    char* value, std::size_t capacity)
{
    if (!capacity || capacity > MAXDWORD) return false;
    const auto path = RegistryPath(game, section);
    for (HKEY root : {HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER}) {
        DWORD size = static_cast<DWORD>(capacity);
        if (ReadValue(root, path.c_str(), name, value, size)) {
            value[capacity - 1] = '\0';
            return true;
        }
    }
    return false;
}

bool Platform::ReadInstallationUnsigned(GameTitle game, const char* section, const char* name,
    unsigned int& value)
{
    const auto path = RegistryPath(game, section);
    for (HKEY root : {HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER}) {
        DWORD data = 0, size = sizeof(data);
        if (ReadValue(root, path.c_str(), name, &data, size) && size == sizeof(data)) {
            value = data;
            return true;
        }
    }
    return false;
}
