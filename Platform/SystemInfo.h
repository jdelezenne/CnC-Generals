// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <string>
#include <cstdint>
#include <limits>
namespace Platform {
constexpr unsigned MemorySize(std::uint64_t bytes)
{
    constexpr auto maximum = static_cast<unsigned>(std::numeric_limits<int>::max());
    return static_cast<unsigned>(bytes > maximum ? maximum : bytes);
}
struct MemoryInfo {
    unsigned physical, availablePhysical, page, availablePage, virtualSize, availableVirtual;
};
struct OSInfo {
    unsigned major, minor, build, platform;
    std::string extra;
};
bool QueryMemoryInfo(MemoryInfo& info);
bool QueryOSInfo(OSInfo& info);
const char* OperatingSystemName(unsigned platform);
int TimeZoneBiasMinutes();
bool HasProcessorRegisters();
void ProcessorRegisters(unsigned& a, unsigned& b, unsigned& c, unsigned& d, unsigned leaf);
}
