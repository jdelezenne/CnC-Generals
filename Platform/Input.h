// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
namespace Platform {
struct KeyEvent { unsigned int key, sequence; bool down; };
enum class MouseEventType { Move, Button, Wheel };
struct MouseEvent {
    MouseEventType type;
    int x, y, wheel;
    unsigned int time, button, clicks;
    bool down;
};
void ShutdownInput();
void PumpInput();
bool ReadKeyEvent(KeyEvent& event);
bool ReadMouseEvent(MouseEvent& event);
bool CapsLockEnabled();
bool FrenchKeyboardLayout();
unsigned int DoubleClickTime();
void ResetKeyboardInput();
void ResetMouseInput();
}
