// SPDX-License-Identifier: GPL-3.0-or-later
#include "PreRTS.h"
#include "Common/StackDump.h"
#include "Common/Debug.h"
#include "Platform/Dialogs.h"
#include <algorithm>
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <memory>
#include <stdexcept>
#include <vector>

AsciiString g_LastErrorDump;

void FillStackAddresses(void** addresses, unsigned int count, unsigned int skip)
{
    if (!count) return;
    const std::size_t capacity = static_cast<std::size_t>(count) + skip + 1;
    if (capacity > static_cast<std::size_t>(std::numeric_limits<int>::max()))
        throw std::length_error("Stack capture is too large");
    std::vector<void*> captured(capacity);
    const int frames = backtrace(captured.data(), static_cast<int>(capacity));
    std::fill_n(addresses, count, nullptr);
    const std::size_t first = static_cast<std::size_t>(skip) + 1;
    if (frames > 0 && static_cast<std::size_t>(frames) > first)
        std::copy_n(captured.data() + first,
            std::min<std::size_t>(count, static_cast<std::size_t>(frames) - first), addresses);
}

void GetFunctionDetails(void* pointer, char* name, char* filename,
    unsigned int* lineNumber, unsigned int* address)
{
    if (name) std::strcpy(name, "<Unknown>");
    if (filename) std::strcpy(filename, "<Unknown>");
    // dladdr identifies images and symbols, but does not expose source lines.
    if (lineNumber) *lineNumber = 0xffffffff;
    if (address) *address = 0xffffffff;
    Dl_info info{};
    if (!dladdr(pointer, &info)) return;
    if (filename && info.dli_fname) std::strcpy(filename, info.dli_fname);
    if (name && info.dli_sname) {
        int status = 0;
        std::unique_ptr<char, decltype(&std::free)> demangled(
            abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status), std::free);
        std::strcpy(name, status == 0 ? demangled.get() : info.dli_sname);
        std::strcat(name, "();");
    }
}

void StackDumpFromAddresses(void** addresses, unsigned int count, void (*callback)(const char*))
{
    if (!callback) callback = Platform::DebuggerOutput;
    for (unsigned int i = 0; i < count && addresses[i]; ++i) {
        char* symbol = nullptr;
        // backtrace_symbols retains the image offset when a symbol is unavailable.
        std::unique_ptr<char*, decltype(&std::free)> symbols(
            backtrace_symbols(addresses + i, 1), std::free);
        if (symbols) symbol = symbols.get()[0];
        AsciiString line;
        if (symbol) line.format("  %s\n", symbol);
        else line.format("  %p\n", addresses[i]);
        if (g_LastErrorDump.isNotEmpty()) g_LastErrorDump.concat(line);
        callback(line.str());
    }
}

void StackDump(void (*callback)(const char*))
{
    void* addresses[256];
    FillStackAddresses(addresses, 256, 1);
    StackDumpFromAddresses(addresses, 256, callback);
}
