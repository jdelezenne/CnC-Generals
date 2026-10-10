// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <string>
#include <vector>

namespace Platform {
enum class DialogButtons { OK, AbortRetryIgnore, YesNo, OKCancel };
enum class DialogResult { OK, Abort, Retry, Ignore, Yes, No, Cancel };
enum class DialogIcon { None, Warning, Error };
DialogResult ShowDialog(const char* text, const char* caption, DialogButtons buttons,
    DialogIcon icon, DialogResult defaultButton, bool parentToGameWindow);
bool IsGameThread();
void SetGameThread();
bool HasGameWindow();
void HideGameWindow();
void BreakDebugger();
void DebuggerOutput(const char* text);
std::string LegacyTextToUTF8(const char* text);
std::string UTF16ToUTF8(const std::vector<unsigned char>& text);
}
