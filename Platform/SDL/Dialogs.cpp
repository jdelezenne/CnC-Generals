// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Dialogs.h"
#include "InputInternal.h"
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <thread>

namespace {
std::thread::id GameThread;
std::string ConvertText(const char* encoding, const char* text, std::size_t bytes)
{
    std::unique_ptr<char, decltype(&SDL_free)> converted(
        SDL_iconv_string("UTF-8", encoding, text, bytes), SDL_free);
    if (!converted) throw std::runtime_error("Cannot convert dialog text to UTF-8");
    return converted.get();
}
}

void Platform::SetGameThread() { GameThread = std::this_thread::get_id(); }
bool Platform::IsGameThread() { return GameThread == std::this_thread::get_id(); }
bool Platform::HasGameWindow() { return GetGameWindow() != nullptr; }
void Platform::HideGameWindow() { if (auto* window = GetGameWindow()) SDL_HideWindow(window); }
void Platform::BreakDebugger() { SDL_TriggerBreakpoint(); }

Platform::DialogResult Platform::ShowDialog(const char* text, const char* caption,
    DialogButtons buttons, DialogIcon icon, DialogResult defaultButton, bool parentToGameWindow)
{
    SDL_MessageBoxButtonData choices[3];
    int count = 0;
    const auto add = [&](DialogResult result, const char* label, bool escape) {
        choices[count++] = {
            static_cast<SDL_MessageBoxButtonFlags>((result == defaultButton ? SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT : 0) |
                (escape ? SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT : 0)), static_cast<int>(result), label};
    };
    if (buttons == DialogButtons::AbortRetryIgnore) {
        add(DialogResult::Abort, "Abort", false);
        add(DialogResult::Retry, "Retry", false);
        add(DialogResult::Ignore, "Ignore", true);
    } else if (buttons == DialogButtons::YesNo) {
        add(DialogResult::Yes, "Yes", false);
        add(DialogResult::No, "No", true);
    } else if (buttons == DialogButtons::OKCancel) {
        add(DialogResult::OK, "OK", false);
        add(DialogResult::Cancel, "Cancel", true);
    } else {
        add(DialogResult::OK, "OK", true);
    }
    const SDL_MessageBoxFlags flags = icon == DialogIcon::Error ? SDL_MESSAGEBOX_ERROR :
        icon == DialogIcon::Warning ? SDL_MESSAGEBOX_WARNING : SDL_MESSAGEBOX_INFORMATION;
    const SDL_MessageBoxData dialog{flags, parentToGameWindow ? GetGameWindow() : nullptr,
        caption, text, count, choices, nullptr};
    int selected = -1;
    if (!SDL_ShowMessageBox(&dialog, &selected)) {
        std::fprintf(stderr, "%s: %s\nSDL message box failed: %s\n", caption, text, SDL_GetError());
        std::abort();
    }
    // Closing a choice dialog corresponds to its Escape button.
    if (selected == -1) return buttons == DialogButtons::AbortRetryIgnore ? DialogResult::Ignore :
        buttons == DialogButtons::YesNo ? DialogResult::No :
        buttons == DialogButtons::OKCancel ? DialogResult::Cancel : DialogResult::OK;
    return static_cast<DialogResult>(selected);
}

std::string Platform::UTF16ToUTF8(const std::vector<unsigned char>& text)
{
    auto terminated = text;
    terminated.push_back(0);
    terminated.push_back(0);
    return ConvertText("UTF-16LE", reinterpret_cast<const char*>(terminated.data()), terminated.size());
}

#ifndef _WIN32
void Platform::DebuggerOutput(const char* text) { std::fputs(text, stderr); }
std::string Platform::LegacyTextToUTF8(const char* text)
{
    // Legacy narrow game strings and source literals use Windows-1252.
    return ConvertText("WINDOWS-1252", text, SDL_strlen(text) + 1);
}
#endif
