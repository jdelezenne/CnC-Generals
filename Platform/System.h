// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstdint>
#include <string>
namespace Platform {
int LastSystemError();
void FormatSystemError(int error, char* buffer, int length);
void FormatSystemError(int error, wchar_t* buffer, int length);
bool FormatSystemErrorMessage(int error, char* buffer, int length);
void DebugMonitorOutput(const char* text);
std::uint64_t ProcessorTicks();
std::uint32_t CurrentThreadIdentifier();
std::string ExecutablePath();
std::string UserName();
}
