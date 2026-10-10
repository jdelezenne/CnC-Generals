// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Descriptor.h"
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>

int Platform::OpenDescriptor(const char* path, FileOpenOptions options)
{
    int flags = options.text ? _O_TEXT : _O_BINARY;
    flags |= options.write ? (options.read ? _O_RDWR : _O_WRONLY) : _O_RDONLY;
    if (options.create) flags |= _O_CREAT;
    if (options.truncate) flags |= _O_TRUNC;
    if (options.append) flags |= _O_APPEND;
    return _open(path, flags, _S_IREAD | _S_IWRITE);
}
int Platform::CloseDescriptor(int descriptor) { return _close(descriptor); }
int Platform::ReadDescriptor(int descriptor, void* buffer, unsigned int bytes) { return _read(descriptor, buffer, bytes); }
int Platform::WriteDescriptor(int descriptor, const void* buffer, unsigned int bytes) { return _write(descriptor, buffer, bytes); }
int Platform::SeekDescriptor(int descriptor, int offset, int origin) { return _lseek(descriptor, offset, origin); }
