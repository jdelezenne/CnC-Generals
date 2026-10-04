// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstddef>

namespace Platform {
enum class GameTitle { Generals, ZeroHour };
const char* PreferenceDirectory(GameTitle game);
bool UserDataDirectory(GameTitle game, const char* legacyLeafName, char* directory, std::size_t capacity);
}
