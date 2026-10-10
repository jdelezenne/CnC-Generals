// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Descriptor.h"
#include <cerrno>
#include <climits>
#include <cstdio>
#include <fcntl.h>
#include <memory>
#include <mutex>
#include <sys/stat.h>
#include <unistd.h>
#include <unordered_map>

namespace {
struct TextState { std::mutex mutex; bool eof = false; };
std::mutex TextMutex;
std::unordered_map<int, std::shared_ptr<TextState>> TextFiles;
std::shared_ptr<TextState> FindTextState(int descriptor)
{
    std::lock_guard<std::mutex> lock(TextMutex);
    const auto found = TextFiles.find(descriptor);
    return found == TextFiles.end() ? nullptr : found->second;
}
}

int Platform::OpenDescriptor(const char* path, FileOpenOptions options)
{
    int flags = options.write ? (options.read ? O_RDWR : O_WRONLY) : O_RDONLY;
    if (options.create) flags |= O_CREAT;
    if (options.truncate) flags |= O_TRUNC;
    if (options.append) flags |= O_APPEND;
    const int descriptor = open(path, flags, S_IRUSR | S_IWUSR);
    if (descriptor == -1) return -1;
    struct stat info{};
    if (fstat(descriptor, &info) == -1 || !S_ISREG(info.st_mode)) {
        const int error = errno;
        close(descriptor);
        errno = S_ISDIR(info.st_mode) ? EISDIR : (error ? error : EACCES);
        return -1;
    }
    if (options.text) {
        // Match the CRT's read/write text-file handling of a final Ctrl-Z.
        if (options.read && options.write && info.st_size > 0) {
            unsigned char last = 0;
            if (pread(descriptor, &last, 1, info.st_size - 1) != 1 ||
                (last == 0x1a && ftruncate(descriptor, info.st_size - 1) == -1)) {
                const int error = errno;
                close(descriptor);
                errno = error;
                return -1;
            }
        }
        std::lock_guard<std::mutex> lock(TextMutex);
        try { TextFiles.emplace(descriptor, std::make_shared<TextState>()); }
        catch (...) { close(descriptor); throw; }
    }
    return descriptor;
}

int Platform::CloseDescriptor(int descriptor)
{
    const auto text = FindTextState(descriptor);
    std::unique_lock<std::mutex> operation;
    if (text) operation = std::unique_lock<std::mutex>(text->mutex);
    {
        std::lock_guard<std::mutex> lock(TextMutex);
        TextFiles.erase(descriptor);
    }
    return close(descriptor);
}

int Platform::ReadDescriptor(int descriptor, void* buffer, unsigned int bytes)
{
    if (bytes > INT_MAX || (bytes && !buffer)) { errno = EINVAL; return -1; }
    const auto text = FindTextState(descriptor);
    std::unique_lock<std::mutex> operation;
    if (text) operation = std::unique_lock<std::mutex>(text->mutex);
    if (text && text->eof) return 0;
    const int count = static_cast<int>(read(descriptor, buffer, bytes));
    if (count <= 0 || !text) return count;
    auto* data = static_cast<unsigned char*>(buffer);
    int output = 0;
    for (int i = 0; i < count; ++i) {
        if (data[i] == 0x1a) { text->eof = true; break; }
        if (data[i] != '\r') { data[output++] = data[i]; continue; }
        if (i + 1 < count) {
            if (data[i + 1] == '\n') { ++i; data[output++] = '\n'; }
            else data[output++] = '\r';
            continue;
        }
        unsigned char peek;
        if (read(descriptor, &peek, 1) != 1) { data[output++] = '\r'; continue; }
        // Preserve physical offsets and the CRT's one-byte CRLF read behavior.
        if (peek == '\n' && output == 0) data[output++] = '\n';
        else {
            if (lseek(descriptor, -1, SEEK_CUR) == -1) return -1;
            if (peek != '\n') data[output++] = '\r';
        }
    }
    return output;
}

int Platform::WriteDescriptor(int descriptor, const void* buffer, unsigned int bytes)
{
    if (bytes > INT_MAX || (bytes && !buffer)) { errno = EINVAL; return -1; }
    const auto text = FindTextState(descriptor);
    if (!text) return static_cast<int>(write(descriptor, buffer, bytes));
    std::lock_guard<std::mutex> operation(text->mutex);
    const auto* data = static_cast<const unsigned char*>(buffer);
    unsigned int consumed = 0, physical = 0, inserted = 0;
    while (consumed < bytes) {
        unsigned char translated[4096];
        unsigned int count = 0;
        while (consumed < bytes && count < sizeof(translated) - 1) {
            const auto value = data[consumed++];
            if (value == '\n') { translated[count++] = '\r'; ++inserted; }
            translated[count++] = value;
        }
        const auto written = write(descriptor, translated, count);
        if (written < 0) return physical ? static_cast<int>(physical - inserted) : -1;
        physical += static_cast<unsigned int>(written);
        if (static_cast<unsigned int>(written) < count) break;
    }
    return static_cast<int>(physical - inserted);
}

int Platform::SeekDescriptor(int descriptor, int offset, int origin)
{
    const auto text = FindTextState(descriptor);
    std::unique_lock<std::mutex> operation;
    if (text) operation = std::unique_lock<std::mutex>(text->mutex);
    const auto saved = lseek(descriptor, 0, SEEK_CUR);
    if (saved == -1) return -1;
    const auto position = lseek(descriptor, offset, origin);
    if (position == -1) return -1;
    // The existing LocalFile interface uses the CRT's signed 32-bit seek range.
    if (position > INT_MAX) {
        lseek(descriptor, saved, SEEK_SET);
        errno = EINVAL;
        return -1;
    }
    if (text) text->eof = false;
    return static_cast<int>(position);
}
