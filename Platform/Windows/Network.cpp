// SPDX-License-Identifier: GPL-3.0-or-later
// Native TCP table query, based on Renegade's committed platform implementation.
#include "Platform/Sockets.h"
#include <iphlpapi.h>
#include <cstring>
#include <vector>

bool Platform::LocalTcpEndpoint(const char* hostname, std::uint16_t remotePort, std::uint32_t& address)
{
    if (!hostname || !*hostname) return false;
    WSADATA data;
    if (WSAStartup(MAKEWORD(1, 1), &data) != 0) return false;
    struct SocketSession { ~SocketSession() { WSACleanup(); } } session;
    const auto* host = gethostbyname(hostname);
    if (!host || host->h_addrtype != AF_INET || host->h_length != sizeof(std::uint32_t) || !host->h_addr_list[0]) return false;
    std::uint32_t remoteAddress;
    std::memcpy(&remoteAddress, host->h_addr_list[0], sizeof(remoteAddress));
    DWORD size = 0;
    if (GetTcpTable(nullptr, &size, TRUE) != ERROR_INSUFFICIENT_BUFFER) return false;
    std::vector<DWORD> storage;
    DWORD result;
    do {
        storage.resize((static_cast<std::size_t>(size) + sizeof(DWORD) - 1) / sizeof(DWORD));
        result = GetTcpTable(reinterpret_cast<MIB_TCPTABLE*>(storage.data()), &size, TRUE);
    } while (result == ERROR_INSUFFICIENT_BUFFER);
    if (result != NO_ERROR) return false;
    const auto* table = reinterpret_cast<const MIB_TCPTABLE*>(storage.data());
    for (DWORD i = 0; i < table->dwNumEntries; ++i) {
        const auto& row = table->table[i];
        if (row.dwState == MIB_TCP_STATE_ESTAB && row.dwRemoteAddr == remoteAddress &&
            (!remotePort || ntohs(static_cast<u_short>(row.dwRemotePort)) == remotePort)) {
            address = row.dwLocalAddr;
            return true;
        }
    }
    return false;
}
