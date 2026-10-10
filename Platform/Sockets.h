// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstdint>
#include <string>
#include <vector>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
// The legacy simulation owns an enum named AI_PASSIVE. Keep the POSIX
// address-resolution flag out of the engine's public namespace.
#undef AI_PASSIVE
#include <unistd.h>
#endif
namespace Platform {
bool InitializeSockets();
void ShutdownSockets();
bool LocalIPv4Addresses(std::vector<std::uint32_t>& addresses);
std::string HostName();
int CloseSocket(int descriptor);
int LastSocketError();
bool SocketWouldBlock(int error);
bool SocketConnectionPending(int error);
bool SocketAlreadyConnected(int error);
bool SocketConnectionLost(int error);
int SetSocketNonBlocking(int descriptor, bool enabled);
bool LocalTcpEndpoint(const char* hostname, std::uint16_t remotePort, std::uint32_t& address);
int SocketAddress(int descriptor, sockaddr* address, int& length);
int ReadDatagram(int descriptor, void* buffer, unsigned size, sockaddr* address, int* length);
int SocketOption(int descriptor, int level, int option, void* value, int& length);
#ifndef _WIN32
int PingIPv4(std::uint32_t address, int timeoutMilliseconds);
#endif
}
