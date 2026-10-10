// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Sockets.h"
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <net/if.h>
#include "Platform/Clock.h"
#include <array>
#include <algorithm>
#include <poll.h>
// POSIX sockets have no process-wide startup or cleanup operation.
bool Platform::InitializeSockets() { return true; }
void Platform::ShutdownSockets() {}
int Platform::PingIPv4(std::uint32_t address, int timeoutMilliseconds)
{
    if (timeoutMilliseconds < 0) return -1;
    const int descriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
    if (descriptor < 0) return -1;
    sockaddr_in remote{};
    remote.sin_family = AF_INET;
    remote.sin_addr.s_addr = address;
    std::array<unsigned char, 40> packet{};
    packet[0] = 8;
    packet[7] = 1;
    for (unsigned i = 8; i < packet.size(); ++i) packet[i] = static_cast<unsigned char>(32 + i - 8);
    unsigned checksum = 0;
    for (unsigned i = 0; i < packet.size(); i += 2) checksum += (packet[i] << 8) | packet[i + 1];
    while (checksum >> 16) checksum = (checksum & 0xffff) + (checksum >> 16);
    checksum = ~checksum;
    packet[2] = static_cast<unsigned char>(checksum >> 8);
    packet[3] = static_cast<unsigned char>(checksum);
    const auto start = Milliseconds();
    int result = -1;
    if (sendto(descriptor, packet.data(), packet.size(), 0, reinterpret_cast<sockaddr*>(&remote), sizeof(remote)) == packet.size()) {
        pollfd event{descriptor, POLLIN, 0};
        for (;;) {
            const auto elapsed = Milliseconds() - start;
            const int remaining = timeoutMilliseconds - static_cast<int>(std::min<std::uint32_t>(elapsed, timeoutMilliseconds));
            const int ready = poll(&event, 1, remaining);
            if (ready < 0 && errno == EINTR && remaining) continue;
            if (ready <= 0) break;
            std::array<unsigned char, 512> response{};
            sockaddr_in source{};
            socklen_t length = sizeof(source);
            const auto size = recvfrom(descriptor, response.data(), response.size(), 0, reinterpret_cast<sockaddr*>(&source), &length);
            if (size >= 8 && source.sin_addr.s_addr == address && response[0] == 0 && response[1] == 0 && response[7] == 1) {
                result = static_cast<int>(std::min<std::uint32_t>(Milliseconds() - start, timeoutMilliseconds));
                break;
            }
            if (!remaining) break;
        }
    }
    close(descriptor);
    return result;
}
std::string Platform::HostName()
{
    char name[256];
    return gethostname(name, sizeof(name)) == 0 ? std::string(name) : std::string();
}
bool Platform::LocalIPv4Addresses(std::vector<std::uint32_t>& addresses)
{
    addresses.clear();
    ifaddrs* list = nullptr;
    if (getifaddrs(&list) != 0) return false;
    for (auto* entry = list; entry; entry = entry->ifa_next) {
        if (!entry->ifa_addr || entry->ifa_addr->sa_family != AF_INET || !(entry->ifa_flags & IFF_UP)) continue;
        addresses.push_back(reinterpret_cast<const sockaddr_in*>(entry->ifa_addr)->sin_addr.s_addr);
    }
    freeifaddrs(list);
    return true;
}
int Platform::CloseSocket(int descriptor) { return close(descriptor); }
int Platform::LastSocketError() { return errno; }
bool Platform::SocketWouldBlock(int error) { return error == EWOULDBLOCK || error == EAGAIN; }
bool Platform::SocketConnectionPending(int error) { return SocketWouldBlock(error) || error == EINPROGRESS || error == EINVAL || error == EALREADY; }
bool Platform::SocketAlreadyConnected(int error) { return error == EISCONN; }
bool Platform::SocketConnectionLost(int error) { return error == ECONNRESET || error == ENOTCONN; }
int Platform::SetSocketNonBlocking(int descriptor, bool enabled)
{
    const int flags = fcntl(descriptor, F_GETFL);
    if (flags == -1) return -1;
    return fcntl(descriptor, F_SETFL, enabled ? flags | O_NONBLOCK : flags & ~O_NONBLOCK);
}
int Platform::SocketAddress(int descriptor, sockaddr* address, int& length)
{
    socklen_t nativeLength = length;
    const int result = getsockname(descriptor, address, &nativeLength);
    length = static_cast<int>(nativeLength);
    return result;
}
int Platform::ReadDatagram(int descriptor, void* buffer, unsigned size, sockaddr* address, int* length)
{
    socklen_t nativeLength = length ? *length : 0;
    const auto result = recvfrom(descriptor, buffer, size, 0, address, length ? &nativeLength : nullptr);
    if (length) *length = static_cast<int>(nativeLength);
    return static_cast<int>(result);
}
int Platform::SocketOption(int descriptor, int level, int option, void* value, int& length)
{
    socklen_t nativeLength = length;
    const int result = getsockopt(descriptor, level, option, value, &nativeLength);
    length = static_cast<int>(nativeLength);
    return result;
}
