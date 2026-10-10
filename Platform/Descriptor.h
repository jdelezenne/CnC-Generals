// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
namespace Platform {
struct FileOpenOptions {
    bool read, write, create, truncate, append, text;
};
int OpenDescriptor(const char* path, FileOpenOptions options);
int CloseDescriptor(int descriptor);
int ReadDescriptor(int descriptor, void* buffer, unsigned int bytes);
int WriteDescriptor(int descriptor, const void* buffer, unsigned int bytes);
int SeekDescriptor(int descriptor, int offset, int origin);
}
