// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <SDL3/SDL.h>
namespace Platform {
bool InitializeInput(SDL_Window* window);
SDL_Window* GetGameWindow();
void RefreshStartupSplash();
#ifdef _WIN32
void ConfigureNativeInput();
#endif
}
