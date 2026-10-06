// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

void InitializeNativeApplicationDiagnostics();
bool HandleNativeApplicationCommandLine(int argc, char* argv[]);
void EnableNativeApplicationHeapTracking();
void InitializeNativeWindowFeatures();
