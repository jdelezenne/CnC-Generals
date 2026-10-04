// SPDX-License-Identifier: GPL-3.0-or-later
#include "Window.h"
#include "Platform/SDL/InputInternal.h"
#include <commctrl.h>
namespace {
Platform::NativeWindowMessageHandler MessageHandler = nullptr;
LRESULT CALLBACK NativeFeatureProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR)
{
    LRESULT result = 0;
    if (MessageHandler && MessageHandler(window, message, wParam, lParam, result)) return result;
    return DefSubclassProc(window, message, wParam, lParam);
}
}
HWND Platform::NativeGameWindow()
{
    SDL_Window* window = GetGameWindow();
    return window ? static_cast<HWND>(SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr)) : nullptr;
}
void Platform::SetNativeWindowMessageHandler(NativeWindowMessageHandler handler)
{
    HWND window = NativeGameWindow();
    if (window && MessageHandler) RemoveWindowSubclass(window, NativeFeatureProc, 1);
    MessageHandler = handler;
    if (window && handler) SetWindowSubclass(window, NativeFeatureProc, 1, 0);
}
