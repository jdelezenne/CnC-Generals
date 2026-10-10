// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
namespace Platform {
struct Cursor;
Cursor* LoadCursorFile(const char* path);
void DestroyCursor(Cursor* cursor);
void SetCursor(Cursor* cursor);
void UpdateCursor();
void MousePosition(int& x, int& y);
}
