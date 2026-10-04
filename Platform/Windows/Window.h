// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <windows.h>
namespace Platform {
HWND NativeGameWindow();
typedef bool (*NativeWindowMessageHandler)(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result);
void SetNativeWindowMessageHandler(NativeWindowMessageHandler handler);
}
