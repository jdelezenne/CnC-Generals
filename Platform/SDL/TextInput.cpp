// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/TextInput.h"
#include "InputInternal.h"
namespace { Platform::TextInputHandler Handler = nullptr; }
void Platform::SetTextInputHandler(TextInputHandler handler) { Handler = handler; }
void Platform::DispatchTextInput(const TextInputEvent& event) { if (Handler) Handler(event); }
bool Platform::EnableTextInput(bool enabled)
{
    auto* window = GetGameWindow();
    if (!window) return !enabled;
    if (!enabled) return SDL_StopTextInput(window);
    const auto properties = SDL_CreateProperties();
    if (!properties) return false;
    SDL_SetBooleanProperty(properties, SDL_PROP_TEXTINPUT_AUTOCORRECT_BOOLEAN, false);
    SDL_SetBooleanProperty(properties, SDL_PROP_TEXTINPUT_MULTILINE_BOOLEAN, false);
    const bool started = SDL_StartTextInputWithProperties(window, properties);
    SDL_DestroyProperties(properties);
    return started;
}
void Platform::SetTextInputArea(int x, int y, int width, int height, int cursor)
{
    if (auto* window = GetGameWindow()) {
        const SDL_Rect area{x, y, width, height};
        SDL_SetTextInputArea(window, &area, cursor);
    }
}
