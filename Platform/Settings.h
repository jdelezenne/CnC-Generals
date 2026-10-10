// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "Paths.h"

namespace Platform {
bool ReadInstallationString(GameTitle game, const char* section, const char* name,
    char* value, std::size_t capacity);
bool ReadInstallationUnsigned(GameTitle game, const char* section, const char* name,
    unsigned int& value);
bool WriteInstallationString(GameTitle game, const char* section, const char* name,
    const char* value);
bool WriteInstallationUnsigned(GameTitle game, const char* section, const char* name,
    unsigned int value);
}
