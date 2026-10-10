// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/System.h"
#include "Platform/Text.h"
#include <cwchar>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <cstdio>
#include <system_error>
#include <vector>
#include <stdexcept>
#include <pwd.h>
#include <unistd.h>
#ifdef __APPLE__
#include <pthread.h>
#include <mach-o/dyld.h>
#else
#include <sys/syscall.h>
#include <unistd.h>
#endif
int Platform::LastSystemError() { return errno; }
std::string Platform::UserName()
{
    long capacity = sysconf(_SC_GETPW_R_SIZE_MAX);
    std::vector<char> buffer(capacity > 0 ? static_cast<std::size_t>(capacity) : 16384);
    passwd entry{};
    passwd* result = nullptr;
    int error;
    while ((error = getpwuid_r(geteuid(), &entry, buffer.data(), buffer.size(), &result)) == ERANGE)
        buffer.resize(buffer.size() * 2);
    return !error && result && result->pw_name ? result->pw_name : "unknown";
}
void Platform::FormatSystemError(int error, char* buffer, int length)
{
    if (length > 0) std::snprintf(buffer, length, "%s", std::system_category().message(error).c_str());
}
void Platform::FormatSystemError(int error, wchar_t* buffer, int length)
{
    if (length <= 0) return;
    buffer[0] = 0;
    std::wstring message;
    if (!WideFromNarrow(std::system_category().message(error).c_str(), message)) return;
    std::wcsncpy(buffer, message.c_str(), static_cast<std::size_t>(length - 1));
    buffer[length - 1] = 0;
}
std::string Platform::ExecutablePath()
{
    std::vector<char> path(512);
#ifdef __APPLE__
    std::uint32_t length = static_cast<std::uint32_t>(path.size());
    if (_NSGetExecutablePath(path.data(), &length)) {
        path.resize(length);
        if (_NSGetExecutablePath(path.data(), &length)) throw std::runtime_error("Cannot read executable path");
    }
    return path.data();
#else
    for (;;) {
        const auto length = readlink("/proc/self/exe", path.data(), path.size());
        if (length < 0) throw std::system_error(errno, std::generic_category());
        if (static_cast<std::size_t>(length) < path.size()) return {path.data(), static_cast<std::size_t>(length)};
        path.resize(path.size() * 2);
    }
#endif
}
void Platform::DebugMonitorOutput(const char* text) { std::fputs(text, stderr); }
std::uint32_t Platform::CurrentThreadIdentifier()
{
#ifdef __APPLE__
    std::uint64_t identifier;
    pthread_threadid_np(nullptr, &identifier);
    return static_cast<std::uint32_t>(identifier);
#else
    return static_cast<std::uint32_t>(syscall(SYS_gettid));
#endif
}
bool Platform::FormatSystemErrorMessage(int error, char* buffer, int length)
{
    // Failed HRESULTs are not POSIX error numbers; let the graphics API format them.
    if (error < 0 || length <= 0) return false;
    std::snprintf(buffer, static_cast<std::size_t>(length), "%s", std::strerror(error));
    return true;
}
