// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Sockets.h"
#include <cstring>
bool Platform::InitializeSockets()
{
    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) return false;
    if (data.wVersion == MAKEWORD(2, 2)) return true;
    WSACleanup();
    return false;
}
void Platform::ShutdownSockets() { WSACleanup(); }
std::string Platform::HostName()
{
    char name[256];
    return gethostname(name, sizeof(name)) == 0 ? std::string(name) : std::string();
}
bool Platform::LocalIPv4Addresses(std::vector<std::uint32_t>& addresses)
{
    addresses.clear();
    const auto name = HostName();
    if (name.empty()) return false;
    const auto* host = gethostbyname(name.c_str());
    if (!host || host->h_length != sizeof(std::uint32_t) || host->h_addrtype != AF_INET) return false;
    for (auto entry = host->h_addr_list; *entry; ++entry) {
        std::uint32_t address;
        std::memcpy(&address, *entry, sizeof(address));
        addresses.push_back(address);
    }
    return true;
}
int Platform::CloseSocket(int descriptor) { return closesocket(descriptor); }
int Platform::LastSocketError() { return WSAGetLastError(); }
bool Platform::SocketWouldBlock(int error) { return error == WSAEWOULDBLOCK; }
bool Platform::SocketConnectionPending(int error) { return SocketWouldBlock(error) || error == WSAEINVAL || error == WSAEALREADY; }
bool Platform::SocketAlreadyConnected(int error) { return error == WSAEISCONN; }
bool Platform::SocketConnectionLost(int error) { return error == WSAECONNRESET || error == WSAENOTCONN; }
int Platform::SetSocketNonBlocking(int descriptor, bool enabled)
{
    unsigned long mode = enabled ? 1 : 0;
    return ioctlsocket(descriptor, FIONBIO, &mode);
}
int Platform::SocketAddress(int descriptor, sockaddr* address, int& length) { return getsockname(descriptor, address, &length); }
int Platform::ReadDatagram(int descriptor, void* buffer, unsigned size, sockaddr* address, int* length)
{
    return recvfrom(descriptor, static_cast<char*>(buffer), size, 0, address, length);
}
int Platform::SocketOption(int descriptor, int level, int option, void* value, int& length)
{
    return getsockopt(descriptor, level, option, static_cast<char*>(value), &length);
}
