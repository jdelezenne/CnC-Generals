// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

namespace Platform {
// The caller owns the returned process-lifetime handle. A second instance is
// reported separately from a native acquisition error.
enum class ApplicationInstanceResult { Acquired, AlreadyRunning, Error };
ApplicationInstanceResult AcquireApplicationInstance(const char* identifier, void*& handle);
void ReleaseApplicationInstance(void* handle);
}
