#include "GeneralsMD/Code/Libraries/Source/debug/_pch.h"
#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <exception>
#include <new>
#include <poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <cxxabi.h>

DebugStackwalk::DebugStackwalk() {}
DebugStackwalk::~DebugStackwalk() {}
DebugStackwalk::Signature::Signature(const Signature& source) { *this = source; }
DebugStackwalk::Signature& DebugStackwalk::Signature::operator=(const Signature& source)
{
    m_numAddr = source.m_numAddr;
    std::copy_n(source.m_addr, m_numAddr, m_addr);
    return *this;
}
std::uintptr_t DebugStackwalk::Signature::GetAddress(int index) const
{
    return index >= 0 && static_cast<unsigned>(index) < m_numAddr ? m_addr[index] : 0;
}
int DebugStackwalk::StackWalk(Signature& signature, _CONTEXT*)
{
    void* addresses[Signature::MAX_ADDR + 1];
    const int count = backtrace(addresses, Signature::MAX_ADDR + 1);
    signature.m_numAddr = count > 1 ? count - 1 : 0;
    for (unsigned i = 0; i < signature.m_numAddr; ++i)
        signature.m_addr[i] = reinterpret_cast<std::uintptr_t>(addresses[i + 1]);
    return signature.m_numAddr;
}
void DebugStackwalk::Signature::GetSymbol(std::uintptr_t address,
    char* module, unsigned moduleSize, std::uintptr_t* moduleOffset,
    char* symbol, unsigned symbolSize, std::uintptr_t* symbolOffset,
    char* file, unsigned fileSize, unsigned* line, unsigned* lineOffset)
{
    if (module && moduleSize) *module = 0;
    if (symbol && symbolSize) *symbol = 0;
    if (file && fileSize) *file = 0;
    if (moduleOffset) *moduleOffset = 0;
    if (symbolOffset) *symbolOffset = 0;
    if (line) *line = 0;
    if (lineOffset) *lineOffset = 0;
    Dl_info info{};
    if (!dladdr(reinterpret_cast<void*>(address), &info)) return;
    if (module && moduleSize && info.dli_fname) Platform::CopyString(module, info.dli_fname, moduleSize);
    if (moduleOffset) *moduleOffset = address - reinterpret_cast<std::uintptr_t>(info.dli_fbase);
    if (info.dli_sname) {
        int status;
        char* name = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
        if (symbol && symbolSize) Platform::CopyString(symbol, status == 0 ? name : info.dli_sname, symbolSize);
        std::free(name);
        if (symbolOffset) *symbolOffset = address - reinterpret_cast<std::uintptr_t>(info.dli_saddr);
    }
}
void DebugStackwalk::Signature::GetSymbol(std::uintptr_t address, char* buffer, unsigned size)
{
    char module[512], symbol[512];
    std::uintptr_t moduleOffset, symbolOffset;
    GetSymbol(address, module, sizeof(module), &moduleOffset, symbol, sizeof(symbol), &symbolOffset, nullptr, 0, nullptr, nullptr);
    std::snprintf(buffer, size, "%p %s+0x%zx %s+0x%zx", reinterpret_cast<void*>(address), module, moduleOffset, symbol, symbolOffset);
}
void* DebugStackwalk::GetDbghelpHandle() { return nullptr; }
bool DebugStackwalk::IsOldDbghelp() { return false; }
Debug& operator<<(Debug& debug, const DebugStackwalk::Signature& signature)
{
    for (unsigned i = 0; i < signature.Size(); ++i) {
        char symbol[1024];
        DebugStackwalk::Signature::GetSymbol(signature.GetAddress(i), symbol, sizeof(symbol));
        debug << symbol << "\n";
    }
    return debug;
}

void Debug::InstallExceptionHandler()
{
    std::set_terminate([] {
        std::fputs("Unhandled C++ exception\n", stderr);
        if (auto exception = std::current_exception()) {
            try { std::rethrow_exception(exception); }
            catch (const std::exception& error) { std::fprintf(stderr, "%s\n", error.what()); }
            catch (...) { std::fputs("Unknown exception type\n", stderr); }
        }
        void* addresses[256];
        const int count = backtrace(addresses, 256);
        backtrace_symbols_fd(addresses, count, STDERR_FILENO);
        std::abort();
    });
}

DebugIOCon::DebugIOCon() : m_allocatedConsole(false), m_inputUsed(0), m_inputRead(0) {}
DebugIOCon::~DebugIOCon() {}
int DebugIOCon::Read(char* buffer, int capacity)
{
    if (capacity <= 1) return 0;
    pollfd input{STDIN_FILENO, POLLIN, 0};
    if (poll(&input, 1, 0) <= 0 || !(input.revents & POLLIN)) return 0;
    const auto count = read(STDIN_FILENO, buffer, capacity - 1);
    return count > 0 ? static_cast<int>(count) : 0;
}
void DebugIOCon::Write(StringType type, const char*, const char* text)
{
    if (type != StringType::StructuredCmdReply && text) { std::fputs(text, stdout); std::fflush(stdout); }
}
void DebugIOCon::Execute(Debug& debug, const char* command, bool, unsigned, const char* const*)
{
    if (!command || !std::strcmp(command, "help"))
        debug << "con I/O help:\n  add\n    use the current terminal for debug commands and output\n";
}
DebugIOInterface* DebugIOCon::Create() { return new (DebugAllocMemory(sizeof(DebugIOCon))) DebugIOCon; }
void DebugIOCon::Delete() { this->~DebugIOCon(); DebugFreeMemory(this); }

DebugIONet::DebugIONet() : m_pipe(-1) {}
DebugIONet::~DebugIONet() { if (m_pipe >= 0) close(m_pipe); }
int DebugIONet::Read(char* buffer, int capacity)
{
    if (m_pipe < 0 || capacity <= 1) return 0;
    const auto count = recv(m_pipe, buffer, capacity - 1, MSG_DONTWAIT);
    return count > 0 ? static_cast<int>(count) : 0;
}
void DebugIONet::Write(StringType type, const char* source, const char* text)
{
    if (m_pipe < 0 || !text) return;
    const auto sendAll = [this](const void* data, std::size_t size) {
        auto* bytes = static_cast<const char*>(data);
        while (size) {
#ifdef MSG_NOSIGNAL
            const auto count = send(m_pipe, bytes, size, MSG_NOSIGNAL);
#else
            const auto count = send(m_pipe, bytes, size, 0);
#endif
            if (count < 0 && errno == EINTR) continue;
            if (count <= 0) return false;
            bytes += count;
            size -= count;
        }
        return true;
    };
    const unsigned char category = static_cast<unsigned char>(type);
    if (!sendAll(&category, 1)) return;
    for (const char* item : {source ? source : "", text}) {
        const std::uint32_t length = std::strlen(item);
        const unsigned char bytes[] = {static_cast<unsigned char>(length), static_cast<unsigned char>(length >> 8), static_cast<unsigned char>(length >> 16), static_cast<unsigned char>(length >> 24)};
        if (!sendAll(bytes, sizeof(bytes)) || !sendAll(item, length)) return;
    }
}
void DebugIONet::EmergencyFlush() {}
void DebugIONet::Execute(Debug& debug, const char* command, bool, unsigned count, const char* const* arguments)
{
    if (!command || !std::strcmp(command, "help")) {
        debug << "net I/O help:\n  add [ <socket path> ]\n    connect to a local ea_debug_v1 Unix socket\n";
    } else if (!std::strcmp(command, "add")) {
        const auto path = count ? std::string(arguments[0]) : Platform::UserPath("ea_debug_v1.sock");
        sockaddr_un address{};
        address.sun_family = AF_UNIX;
        if (path.size() >= sizeof(address.sun_path)) { debug << "Debug socket path is too long.\n"; return; }
        std::memcpy(address.sun_path, path.c_str(), path.size() + 1);
        const int descriptor = socket(AF_UNIX, SOCK_STREAM, 0);
        if (descriptor < 0 || connect(descriptor, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0) {
            if (descriptor >= 0) close(descriptor);
            debug << "Could not connect to debug socket.\n";
            return;
        }
        if (m_pipe >= 0) close(m_pipe);
        m_pipe = descriptor;
    }
}
DebugIOInterface* DebugIONet::Create() { return new (DebugAllocMemory(sizeof(DebugIONet))) DebugIONet; }
void DebugIONet::Delete() { this->~DebugIONet(); DebugFreeMemory(this); }
