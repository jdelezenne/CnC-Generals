// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Input.h"
#include "Platform/Cursor.h"
#include "Platform/TextInput.h"
#include "Platform/Window.h"
#include "InputInternal.h"
#include "KeyMapping.h"
#include <array>
#include <deque>

namespace {
SDL_Window* Window = nullptr;
std::deque<Platform::KeyEvent> Keys;
std::deque<Platform::MouseEvent> Mouse;
std::array<bool, 256> Down{};
std::array<bool, SDL_SCANCODE_COUNT> ScanDown{}, ScanPressed{};
unsigned int Sequence = 0;
std::array<bool, 4> MouseDown{};
int MouseX = 0, MouseY = 0;
void QueueKey(unsigned key, bool down)
{
    if (!key || key >= Down.size() || Down[key] == down) return;
    Down[key] = down;
    if (Keys.size() == 256) Keys.pop_front();
    Keys.push_back({key, ++Sequence, down});
}
void QueueMouse(const Platform::MouseEvent& event)
{
    MouseX = event.x; MouseY = event.y;
    if (event.type == Platform::MouseEventType::Button && event.button < MouseDown.size())
        MouseDown[event.button] = event.down;
    if (Mouse.size() == 256) Mouse.pop_front();
    Mouse.push_back(event);
}
}

bool Platform::InitializeInput(SDL_Window* window)
{
    if (!window || Window) return false;
    Window = window;
    ResetKeyboardInput();
    ResetMouseInput();
    return true;
}

void Platform::ShutdownInput()
{
    if (!Window) return;
    Window = nullptr;
    ResetKeyboardInput();
    ResetMouseInput();
}

void Platform::ResetKeyboardInput() { Keys.clear(); Down.fill(false); ScanDown.fill(false); ScanPressed.fill(false); }
void Platform::ResetMouseInput() { Mouse.clear(); MouseDown.fill(false); }
bool Platform::CapsLockEnabled() { return (SDL_GetModState() & SDL_KMOD_CAPS) != 0; }
bool Platform::KeyDownOrPressed(int scancode)
{
    if (scancode <= 0 || static_cast<std::size_t>(scancode) >= ScanDown.size()) return false;
    const bool result = ScanDown[scancode] || ScanPressed[scancode];
    ScanPressed[scancode] = false;
    return result;
}
#ifndef _WIN32
bool Platform::FrenchKeyboardLayout()
{
    return SDL_GetKeyFromScancode(SDL_SCANCODE_A, SDL_KMOD_NONE, false) == SDLK_Q &&
        SDL_GetKeyFromScancode(SDL_SCANCODE_Q, SDL_KMOD_NONE, false) == SDLK_A;
}
unsigned int Platform::DoubleClickTime()
{
    const char* hint = SDL_GetHint(SDL_HINT_MOUSE_DOUBLE_CLICK_TIME);
    const long value = hint ? SDL_strtol(hint, nullptr, 10) : 500;
    return value > 0 ? static_cast<unsigned int>(value) : 500;
}
#endif

bool Platform::ReadKeyEvent(KeyEvent& event)
{
    if (Keys.empty()) return false;
    event = Keys.front(); Keys.pop_front(); return true;
}
bool Platform::ReadMouseEvent(MouseEvent& event)
{
    if (Mouse.empty()) return false;
    event = Mouse.front(); Mouse.pop_front(); return true;
}

void Platform::PumpInput()
{
    if (!Window) return;
    UpdateCursor();
    const auto windowID = SDL_GetWindowID(Window);
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_WINDOW_EXPOSED:
            if (event.window.windowID == windowID) RefreshStartupSplash();
            break;
        case SDL_EVENT_QUIT:
            DispatchWindowEvent(WindowEvent::Close);
            break;
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            if (event.window.windowID == windowID) DispatchWindowEvent(WindowEvent::Close);
            break;
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
            if (event.window.windowID == windowID) DispatchWindowEvent(WindowEvent::FocusGained);
            break;
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
            if (event.key.windowID == windowID && !event.key.repeat) {
                if (event.key.scancode > 0 && event.key.scancode < SDL_SCANCODE_COUNT) {
                    ScanDown[event.key.scancode] = event.key.down;
                    if (event.key.down) ScanPressed[event.key.scancode] = true;
                }
                QueueKey(Legacy_Key_ID(event.key.scancode), event.key.down);
                if (event.key.down && (event.key.scancode == SDL_SCANCODE_RETURN || event.key.scancode == SDL_SCANCODE_KP_ENTER))
                    DispatchTextInput({TextInputKind::Return});
            }
            break;
        case SDL_EVENT_TEXT_INPUT:
            if (event.text.windowID == windowID) DispatchTextInput({TextInputKind::Commit, event.text.text});
            break;
        case SDL_EVENT_TEXT_EDITING:
            if (event.edit.windowID == windowID)
                DispatchTextInput({TextInputKind::Composition, event.edit.text, event.edit.start, event.edit.length});
            break;
        case SDL_EVENT_TEXT_EDITING_CANDIDATES:
            if (event.edit_candidates.windowID == windowID) {
                TextInputEvent candidates{TextInputKind::Candidates};
                candidates.selected = event.edit_candidates.selected_candidate;
                for (int i = 0; i < event.edit_candidates.num_candidates; ++i)
                    candidates.candidates.emplace_back(event.edit_candidates.candidates[i]);
                DispatchTextInput(candidates);
            }
            break;
        case SDL_EVENT_WINDOW_FOCUS_LOST:
            if (event.window.windowID == windowID) {
                ScanDown.fill(false); ScanPressed.fill(false);
                for (unsigned key = 1; key < Down.size(); ++key) QueueKey(key, false);
                Mouse.clear();
                for (unsigned button = 1; button < MouseDown.size(); ++button)
                    if (MouseDown[button]) QueueMouse({MouseEventType::Button, MouseX, MouseY, 0,
                        static_cast<unsigned>(event.window.timestamp / 1000000), button, 0, false});
                DispatchWindowEvent(WindowEvent::FocusLost);
            }
            break;
        case SDL_EVENT_MOUSE_MOTION:
            if (event.motion.windowID == windowID)
                QueueMouse({MouseEventType::Move, static_cast<int>(event.motion.x), static_cast<int>(event.motion.y),
                    0, static_cast<unsigned>(event.motion.timestamp / 1000000), 0, 0, false});
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (event.button.windowID == windowID && event.button.button <= SDL_BUTTON_RIGHT)
                QueueMouse({MouseEventType::Button, static_cast<int>(event.button.x), static_cast<int>(event.button.y),
                    0, static_cast<unsigned>(event.button.timestamp / 1000000), event.button.button,
                    event.button.clicks, event.button.down});
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            if (event.wheel.windowID == windowID) {
                const float sign = event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1.0f : 1.0f;
                QueueMouse({MouseEventType::Wheel, static_cast<int>(event.wheel.mouse_x), static_cast<int>(event.wheel.mouse_y),
                    static_cast<int>(event.wheel.y * sign * 120), static_cast<unsigned>(event.wheel.timestamp / 1000000), 0, 0, false});
            }
            break;
        default: break;
        }
    }
}
