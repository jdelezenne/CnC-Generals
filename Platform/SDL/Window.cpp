// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Window.h"
#include "Platform/Input.h"
#include "InputInternal.h"
#ifdef _WIN32
#include "Platform/Windows/Window.h"
#endif

namespace {
SDL_Window* GameWindow = nullptr;
Platform::WindowEventHandler EventHandler = nullptr;
bool SplashActive = false;
}
SDL_Window* Platform::GetGameWindow() { return GameWindow; }
void Platform::SetWindowEventHandler(WindowEventHandler handler) { EventHandler = handler; }
void Platform::DispatchWindowEvent(WindowEvent event) { if (EventHandler) EventHandler(event); }
bool Platform::WindowMinimized() { return GameWindow && (SDL_GetWindowFlags(GameWindow) & SDL_WINDOW_MINIMIZED); }
bool Platform::WindowHasFocus() { return GameWindow && (SDL_GetWindowFlags(GameWindow) & SDL_WINDOW_INPUT_FOCUS); }

bool Platform::CreateGameWindow(const char* title, int width, int height, bool windowed)
{
    if (GameWindow) return false;
#ifdef _WIN32
    ConfigureNativeInput();
#endif
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) return false;
    GameWindow = SDL_CreateWindow(title, width, height, SDL_WINDOW_HIDDEN | (windowed ? 0 : SDL_WINDOW_BORDERLESS));
    if (!GameWindow) { SDL_QuitSubSystem(SDL_INIT_VIDEO); return false; }
#ifdef _WIN32
    const unsigned timeOffset = NativeInputTimeOffset();
#else
    const unsigned timeOffset = 0;
#endif
    if (!InitializeInput(GameWindow, timeOffset)) { DestroyGameWindow(); return false; }
    SDL_SetWindowPosition(GameWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(GameWindow);
    return true;
}

bool Platform::ConfigureRenderWindow(int width, int height, bool windowed)
{
    if (!GameWindow) return false;
    SplashActive = false;
    if (SDL_WindowHasSurface(GameWindow) && !SDL_DestroyWindowSurface(GameWindow)) return false;
    if (!windowed) {
        SDL_DisplayMode mode{};
        if (!SDL_GetClosestFullscreenDisplayMode(SDL_GetDisplayForWindow(GameWindow), width, height, 0, true, &mode) ||
            mode.w != width || mode.h != height || !SDL_SetWindowFullscreenMode(GameWindow, &mode)) return false;
    }
    if (!SDL_SetWindowFullscreen(GameWindow, !windowed)) return false;
    if (!SDL_SetWindowBordered(GameWindow, windowed) ||
        !SDL_SetWindowSize(GameWindow, width, height) ||
        !SDL_SetWindowAlwaysOnTop(GameWindow, !windowed)) return false;
    return windowed || SDL_SetWindowPosition(GameWindow, 0, 0);
}

bool Platform::ShowStartupSplash(const char* filename)
{
    if (!GameWindow) return false;
    SDL_Surface* bitmap = SDL_LoadBMP(filename);
    if (!bitmap) return false;
    SDL_Surface* surface = SDL_GetWindowSurface(GameWindow);
    const bool shown = surface && SDL_BlitSurface(bitmap, nullptr, surface, nullptr) && SDL_UpdateWindowSurface(GameWindow);
    SDL_DestroySurface(bitmap);
    SplashActive = shown;
    return shown;
}

void Platform::RefreshStartupSplash()
{
    if (GameWindow && SplashActive) SDL_UpdateWindowSurface(GameWindow);
}

void Platform::DestroyGameWindow()
{
    if (!GameWindow) return;
    ShutdownInput();
#ifdef _WIN32
    SetNativeWindowMessageHandler(nullptr);
#endif
    SDL_DestroyWindow(GameWindow);
    GameWindow = nullptr;
    SplashActive = false;
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}
