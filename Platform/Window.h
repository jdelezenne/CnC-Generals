// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
namespace Platform {
enum class WindowEvent { Close, FocusGained, FocusLost };
typedef void (*WindowEventHandler)(WindowEvent event);
bool CreateGameWindow(const char* title, int width, int height, bool windowed);
void DestroyGameWindow();
bool ConfigureRenderWindow(int width, int height, bool windowed);
bool ShowStartupSplash(const char* filename);
bool WindowMinimized();
bool WindowHasFocus();
void* RenderWindowHandle();
bool GameWindowIsWindowed();
void SetWindowTitle(const char* title);
void SetWindowEventHandler(WindowEventHandler handler);
void DispatchWindowEvent(WindowEvent event);
}
