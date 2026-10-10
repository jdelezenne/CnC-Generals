// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/ByteOrder.h"
#include <SDL3/SDL_endian.h>
std::uint32_t Platform::NetworkOrder(std::uint32_t value) { return SDL_Swap32BE(value); }
