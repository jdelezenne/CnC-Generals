// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <string>
#include <vector>
namespace Platform {
enum class TextInputKind { Commit, Composition, Candidates, Return };
struct TextInputEvent {
    TextInputKind kind;
    std::string text;
    int start = 0, length = 0, selected = -1;
    std::vector<std::string> candidates;
};
using TextInputHandler = void (*)(const TextInputEvent&);
void SetTextInputHandler(TextInputHandler handler);
bool EnableTextInput(bool enabled);
void SetTextInputArea(int x, int y, int width, int height, int cursor);
void DispatchTextInput(const TextInputEvent& event);
}
