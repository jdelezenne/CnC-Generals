// SPDX-License-Identifier: GPL-3.0-or-later
// Native TCP table query, based on Renegade's committed platform implementation.
#include "Platform/Sockets.h"
#include <charconv>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace {
bool TcpAddress(const std::string& text, std::uint32_t& address, std::uint16_t& port)
{
    const auto colon = text.find(':');
    if (colon == std::string::npos) return false;
    std::uint32_t value;
    const auto ip = std::from_chars(text.data(), text.data() + colon, address, 16);
    const auto number = std::from_chars(text.data() + colon + 1, text.data() + text.size(), value, 16);
    if (ip.ec != std::errc{} || ip.ptr != text.data() + colon || number.ec != std::errc{} ||
        number.ptr != text.data() + text.size() || value > 65535) return false;
    port = static_cast<std::uint16_t>(value);
    return true;
}
}

bool Platform::LocalTcpEndpoint(const char* hostname, std::uint16_t remotePort, std::uint32_t& address)
{
    if (!hostname || !*hostname) return false;
    const auto* host = gethostbyname(hostname);
    if (!host || host->h_addrtype != AF_INET || host->h_length != sizeof(std::uint32_t) || !host->h_addr_list[0]) return false;
    std::uint32_t remoteAddress;
    std::memcpy(&remoteAddress, host->h_addr_list[0], sizeof(remoteAddress));
    std::ifstream table("/proc/net/tcp");
    std::string line;
    std::getline(table, line);
    bool found = false;
    std::tuple<std::uint32_t, std::uint16_t, std::uint16_t> first;
    while (std::getline(table, line)) {
        std::istringstream row(line);
        std::string index, local, remote, state;
        if (!(row >> index >> local >> remote >> state) || state != "01") continue;
        std::uint32_t peerAddress, localAddress;
        std::uint16_t peerPort, localPort;
        if (!TcpAddress(remote, peerAddress, peerPort) || peerAddress != remoteAddress ||
            (remotePort && peerPort != remotePort) || !TcpAddress(local, localAddress, localPort)) continue;
        const auto order = std::make_tuple(ntohl(localAddress), localPort, peerPort);
        // Match the old MIB-II traversal's address/port ordering.
        if (!found || order < first) { first = order; address = localAddress; found = true; }
    }
    return found;
}
